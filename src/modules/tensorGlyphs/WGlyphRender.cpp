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
	m_dataInitialized(false),
	m_sourceRead(false),
	m_dataChanged(false)
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
	m_tensorData(wGlyphRender.m_tensorData),
	m_dataInitialized(false),
	m_sourceRead(wGlyphRender.m_sourceRead),
	m_dataChanged(false),
	m_kernelSource(wGlyphRender.m_kernelSource)
{}

WGlyphRender::~WGlyphRender()
{}

bool WGlyphRender::isSourceRead() const
{
	return m_sourceRead;
}

void WGlyphRender::setTensorData(WDataSetSingle* data)
{
	m_tensorData = boost::static_pointer_cast<WValueSet<float>>(data->getValueSet());

	//WGridRegular3D* grid = static_cast<WGridRegular3D*>(data->getGrid().get());
	// read dimension sizes individually from grid instead

	m_numOfCoeffs = m_tensorData->dimension();
	m_numOfTensors = m_tensorData->size();
	m_dataInitialized = true;
	m_dataChanged = true;

	// reorder tensors for spatial locality ? O.o
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

		m_dataChanged = false;
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

	/* global work size has to be a multiple of local work size */

	size_t gwsX = clViewInfo.width / 16;
	size_t gwsY = clViewInfo.height / 16;

	if ((gwsX * 16) != clViewInfo.width) gwsX++;
	if ((gwsY * 16) != clViewInfo.height) gwsY++;

	gwsX *= 16;
	gwsY *= 16;

	size_t gws[2] = {gwsX,gwsY};

	size_t lws[2] = {16,16};

	/* run kernel */

	cl_int clError;

	clError = clEnqueueNDRangeKernel(clViewInfo.clCommQueue,clObjects.clKernel,2,0,gws,lws,0,0,0);

	if (clError != CL_SUCCESS)
	{
		osg::notify(osg::FATAL) << "Could not run the kernel: " << getCLError(clError) << std::endl;
	}
}

void WGlyphRender::loadCLData(const CLViewInformation& clViewInfo,CLObjects& clObjects) const
{
	cl_int clError;

	/* load new data */

	cl_mem tensorData = clCreateBuffer
	(
		clViewInfo.clContext,
		CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR|CL_MEM_ALLOC_HOST_PTR,
		m_tensorData->rawSize(),const_cast<float*>(m_tensorData->rawData()),&clError
	);

	if (clError != CL_SUCCESS)
	{
		osg::notify(osg::FATAL) << "Could not load the data to GPU memory: " << getCLError(clError) << std::endl;

		return;
	}

	/* release existing data if required */

	if (clObjects.dataCreated)
	{
		clReleaseMemObject(clObjects.tensorData);
	}

	clObjects.tensorData = tensorData;
	clObjects.dataCreated = true;

	/* set new kernel arguments */

	clSetKernelArg(clObjects.clKernel,16,sizeof(cl_mem),&clObjects.tensorData);
	clSetKernelArg(clObjects.clKernel,17,sizeof(unsigned int),&m_numOfTensors);
	clSetKernelArg(clObjects.clKernel,18,sizeof(unsigned int),&m_numOfCoeffs);
}