//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
// For more information see http://www.openwalnut.org/copying
//
// This file is part of OpenWalnut.
//
// OpenWalnut is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OpenWalnut is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with OpenWalnut. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

#include <fstream>

#include <boost/filesystem.hpp>

#include "../../graphicsEngine/WGraphicsEngine.h"
#include "../../common/WLogger.h"

#include "WGlyphRender.h"

WGlyphRender::CLObjects::CLObjects(): dataCreated(false)
{}

WGlyphRender::CLObjects::~CLObjects()
{
	if (dataCreated)
	{
		clReleaseMemObject(tensorData);
	}

	clReleaseKernel(clKernel);
	clReleaseProgram(clProgram);
}

WGlyphRender::WGlyphRender(): 
	OpenCLRender(),
	m_numOfCoeffs(0),
	m_numOfTensors(0),
	m_sourceRead(false),
	m_dataInitialized(false),
	m_dataChanged(false),
	m_tensorData(0)
{
	/* load kernel source code */

	std::string filePath = WGraphicsEngine::getGraphicsEngine()->getShaderPath() + "/GlyphKernel.cl";

	filePath = boost::filesystem::path(filePath).file_string();

	std::ifstream sourceFile(filePath.c_str());

	if (!sourceFile.is_open())
	{
		WLogger::getLogger()->addLogMessage
		(
			"Can not open kernel file \"" + filePath + "\".",
			"WGlyphRender",LL_ERROR
		);

		return;
	}

	std::string line;

	while (!sourceFile.eof())
	{
		std::getline(sourceFile,line);

		m_kernelSource += line + '\n';
	}

	sourceFile.close();

	m_sourceRead = true;
}

WGlyphRender::WGlyphRender(const WGlyphRender& wGlyphRender,const osg::CopyOp& copyop): 
	OpenCLRender(wGlyphRender,copyop),
	m_numOfCoeffs(wGlyphRender.m_numOfCoeffs),
	m_numOfTensors(wGlyphRender.m_numOfTensors),
	m_kernelSource(wGlyphRender.m_kernelSource),
	m_sourceRead(wGlyphRender.m_sourceRead),
	m_dataInitialized(false),
	m_dataChanged(false)
{
	unsigned int length = m_numOfCoeffs * m_numOfTensors;

	if (length == 0)
	{
		m_tensorData = 0;

		return;
	}

	m_tensorData = new float[length];

	for (unsigned int i = 0;i < length;i++)
	{
		m_tensorData[i] = wGlyphRender.m_tensorData[i];
	}
}

WGlyphRender::~WGlyphRender()
{
	if (m_dataInitialized)
	{
		delete[] m_tensorData;
	}
}

bool WGlyphRender::isSourceRead() const
{
	return m_sourceRead;
}

void WGlyphRender::setTensorData(WDataSetSingle* data)
{
	WValueSetBase* values = data->getValueSet().get();
	WGridRegular3D* grid = static_cast<WGridRegular3D*>(data->getGrid().get());

	m_numOfCoeffs = values->dimension();
	m_numOfTensors = values->size();

	unsigned int length = m_numOfTensors * m_numOfCoeffs;

	if (m_dataInitialized)
	{
		delete[] m_tensorData;
	}
	else
	{
		m_dataInitialized = true;
	}

	m_tensorData = new float[length];

	// etc. pp.

	for (unsigned int i = 0;i < length;i++)
	{
		m_tensorData[i] = values->getScalarDouble(i);
	}

	m_dataChanged = true;
}

OpenCLRender::CLProgramDataSet* WGlyphRender::initProgram(const OpenCLRender::CLViewInformation& clViewInfo) const
{
	if (!m_sourceRead)
	{
		return 0;
	}

	cl_int clError;

	const char* cSource = m_kernelSource.c_str();
	size_t sourceLength = m_kernelSource.length();

	cl_program clProgram;
	cl_kernel clKernel;

	const cl_context& clContext = clViewInfo.clContext;
	const cl_device_id& clDevice = clViewInfo.clDevice;

	/* create CL program */

	clProgram = clCreateProgramWithSource(clContext,1,&cSource,&sourceLength,&clError);

	if (clError != CL_SUCCESS)
	{
		osg::notify(osg::FATAL) << "Could not create the program with source: " << getCLError(clError) << std::endl;

		return 0;
	}

	/* build CL program */

	clError = clBuildProgram(clProgram,0,0,0,0,0);

	if (clError != CL_SUCCESS)
	{
		osg::notify(osg::FATAL) << "Could not build the program: " << getCLError(clError) << std::endl;

		if (clError == CL_BUILD_PROGRAM_FAILURE)
		{
			size_t logLength;

			clGetProgramBuildInfo(clProgram,clDevice,CL_PROGRAM_BUILD_LOG,0,0,&logLength);

			char* buildLog = new char[logLength];

			clGetProgramBuildInfo(clProgram,clDevice,CL_PROGRAM_BUILD_LOG,logLength,buildLog,0);

			osg::notify(osg::FATAL) << buildLog << std::endl;

			delete[] buildLog;
		}

		clReleaseProgram(clProgram);

		return 0;
	}

	/* create CL rendering kernel */

	clKernel = clCreateKernel(clProgram,"renderGlyphs",&clError);

	if (clError != CL_SUCCESS)
	{
		osg::notify(osg::FATAL) << "Could not create the kernel: " << getCLError(clError) << std::endl;

		clReleaseProgram(clProgram);

		return 0;
	}

	CLObjects* clObjects = new CLObjects();

	clObjects->clProgram = clProgram;
	clObjects->clKernel = clKernel;

	return clObjects;
}

void WGlyphRender::setBuffers(const OpenCLRender::CLViewInformation& clViewInfo,
							  OpenCLRender::CLProgramDataSet* clProgramDataSet) const
{
	cl_kernel& clKernel = static_cast<CLObjects*>(clProgramDataSet)->clKernel;

	clSetKernelArg(clKernel,19,sizeof(cl_mem),&clViewInfo.colorBuffer);
	clSetKernelArg(clKernel,20,sizeof(cl_mem),&clViewInfo.depthBuffer);
}

void WGlyphRender::render(const OpenCLRender::CLViewInformation& clViewInfo,
						  OpenCLRender::CLProgramDataSet* clProgramDataSet,
						  const osg::State& state) const
{
	CLObjects& clObjects = *static_cast<CLObjects*>(clProgramDataSet);

	/* check for non-existing data */

	if (!m_dataInitialized && !m_dataChanged)
	{
		return;
	}

	/* check for and load new data */

	if (m_dataChanged)
	{
		loadCLData(clViewInfo,clObjects);
	}

	/* get view properties */

	ViewProperties props;

	getViewProperties(props,state);

	/* set kernel view arguments */

	clSetKernelArg(clObjects.clKernel,0,sizeof(float),&props.origin.x());
	clSetKernelArg(clObjects.clKernel,1,sizeof(float),&props.origin.y());
	clSetKernelArg(clObjects.clKernel,2,sizeof(float),&props.origin.z());
	clSetKernelArg(clObjects.clKernel,3,sizeof(float),&props.origin2LowerLeft.x());
	clSetKernelArg(clObjects.clKernel,4,sizeof(float),&props.origin2LowerLeft.y());
	clSetKernelArg(clObjects.clKernel,5,sizeof(float),&props.origin2LowerLeft.z());
	clSetKernelArg(clObjects.clKernel,6,sizeof(float),&props.edgeX.x());
	clSetKernelArg(clObjects.clKernel,7,sizeof(float),&props.edgeX.y());
	clSetKernelArg(clObjects.clKernel,8,sizeof(float),&props.edgeX.z());
	clSetKernelArg(clObjects.clKernel,9,sizeof(float),&props.edgeY.x());
	clSetKernelArg(clObjects.clKernel,10,sizeof(float),&props.edgeY.y());
	clSetKernelArg(clObjects.clKernel,11,sizeof(float),&props.edgeY.z());
	clSetKernelArg(clObjects.clKernel,12,sizeof(float),&props.planeNear);
	clSetKernelArg(clObjects.clKernel,13,sizeof(float),&props.planeFar);
	clSetKernelArg(clObjects.clKernel,14,sizeof(unsigned int),&clViewInfo.width);
	clSetKernelArg(clObjects.clKernel,15,sizeof(unsigned int),&clViewInfo.height);

	/* run kernel */

	size_t gwsX = clViewInfo.width;
	size_t gwsY = clViewInfo.height;

	if ((gwsX % 16) != 0) gwsX += 16 - (gwsX % 16);
	if ((gwsY % 16) != 0) gwsY += 16 - (gwsY % 16);

	size_t gws[2] = {gwsX,gwsY};

	size_t lws[2] = {16,16};

	cl_int clError;

	clError = clEnqueueNDRangeKernel(clViewInfo.clCommQueue,clObjects.clKernel,2,0,gws,lws,0,0,0);

	if (clError != CL_SUCCESS)
	{
		osg::notify(osg::FATAL) << "Could not run the kernel: " << getCLError(clError) << std::endl;
	}
}

void WGlyphRender::loadCLData(const CLViewInformation& clViewInfo,CLObjects& clObjects) const
{
	/* release existing data if required */

	if (m_dataInitialized)
	{
		clReleaseMemObject(clObjects.tensorData);
	}

	cl_int clError;

	/* load new data */

	clObjects.tensorData = clCreateBuffer
	(
		clViewInfo.clContext,
		CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR|CL_MEM_ALLOC_HOST_PTR,
		m_numOfCoeffs * m_numOfTensors,
		m_tensorData,&clError
	);

	if (clError != CL_SUCCESS)
	{
		osg::notify(osg::FATAL) << "Could not load the data to GPU memory: " << getCLError(clError) << std::endl;
	}

	/* set new kernel arguments */

	clSetKernelArg(clObjects.clKernel,16,sizeof(cl_mem),&clObjects.tensorData);
	clSetKernelArg(clObjects.clKernel,17,sizeof(unsigned int),&m_numOfTensors);
	clSetKernelArg(clObjects.clKernel,18,sizeof(unsigned int),&m_numOfCoeffs);
}