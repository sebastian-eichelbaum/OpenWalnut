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

#include "../../common/WLogger.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../dataHandler/WValueSet.h"
#include "../../graphicsEngine/WGraphicsEngine.h"

#include "WGlyphRender.h"

static OpenThreads::Mutex mutex;

/**
*	Calculates a multinomial coefficient.
*/
inline unsigned int Multinomial(const unsigned int& n,unsigned int k2,unsigned int k3)
{
	unsigned int k1 = n - k2 - k3;

	if (k3 > k2)
	{
		unsigned int tmp = k3;

		k3 = k2;
		k2 = tmp;
	}

	if (k2 > k1)
	{
		unsigned int tmp = k2;

		k2 = k1;
		k1 = tmp;
	}

	unsigned int value = 1;

	for (unsigned int i = 1; i <= k2; i++)
	{
		value = value * (n - i + 1) / i;
	}

	for (unsigned int i = 1; i <= k3; i++)
	{
		value = value * (n - k2 - i + 1) / i;
	}

	return value;
}

/**
*	Calculates the monomial factors for glyph evaluation.
*/
inline float* calcFactors(const unsigned int& order,const unsigned int& numOfCoeffs)
{
	float* factorSet = new float[numOfCoeffs];

	unsigned int i = 0;

	float value = 0.0f;

	for (unsigned int z = 0; z <= order; z++)
	{
		for (unsigned int y = 0; y <= order - z; y++,i++)
		{
			factorSet[i] = Multinomial(order,y,z);
		}
	}

	return factorSet;
}

inline bool readKernelSource(std::string& kernelSource)
{
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

		return false;
	}

	std::string line;

	while (!sourceFile.eof())
	{
		std::getline(sourceFile,line);

		kernelSource += line + '\n';
	}

	sourceFile.close();

	return true;
}

WGlyphRender::DataChangeCallback::DataChangeCallback(): m_dataChanged(false)
{}

void WGlyphRender::DataChangeCallback::update(osg::NodeVisitor*,osg::Drawable* drawable)
{
	if (m_changed)
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mutex);

		WGlyphRender& wGlyphRender = *static_cast<WGlyphRender*>(drawable);

		if (m_dataChanged)
		{
			if (wGlyphRender.m_order != m_order)
			{
				wGlyphRender.changeDataSet(ReloadCallback(true));
			}
			else
			{
				wGlyphRender.changeDataSet(ReloadCallback(false));
			}

			WGridRegular3D* grid = static_cast<WGridRegular3D*>(m_tensorData->getGrid().get());

			wGlyphRender.m_numOfTensors[0] = grid->getNbCoordsX();
			wGlyphRender.m_numOfTensors[1] = grid->getNbCoordsY();
			wGlyphRender.m_numOfTensors[2] = grid->getNbCoordsZ();

			wGlyphRender.m_order = m_order;
			wGlyphRender.m_tensorData = m_tensorData;

			wGlyphRender.dirtyBound();

			m_tensorData.reset();

			m_dataChanged = false;
		}

		wGlyphRender.m_slices[0] = m_slices[0];
		wGlyphRender.m_slices[1] = m_slices[1];
		wGlyphRender.m_slices[2] = m_slices[2];

		wGlyphRender.m_sliceEnabled[0] = m_sliceEnabled[0];
		wGlyphRender.m_sliceEnabled[1] = m_sliceEnabled[1];
		wGlyphRender.m_sliceEnabled[2] = m_sliceEnabled[2];

		m_changed = false;
	}
}

WGlyphRender::ReloadCallback::ReloadCallback(bool reloadFactors): factors(reloadFactors)
{}

void WGlyphRender::ReloadCallback::change(CLProgramDataSet* clProgramDataSet) const
{
	CLObjects& clObjects = *static_cast<CLObjects*>(clProgramDataSet);

	clObjects.reloadData = true;
	clObjects.reloadAuxData = factors;
}

WGlyphRender::CLObjects::CLObjects(): dataCreated(false),reloadData(true),reloadAuxData(true)
{}

WGlyphRender::CLObjects::~CLObjects()
{
	if (dataCreated)
	{
		clReleaseMemObject(tensorData);
		clReleaseMemObject(factors);
	}

	clReleaseKernel(clScaleKernel);
	clReleaseKernel(clRenderKernel);
	clReleaseProgram(clProgram);
}

WGlyphRender::WGlyphRender(const boost::shared_ptr<WDataSetSingle>& data,const int& order,
						   const int& sliceX, const int& sliceY, const int& sliceZ,
						   const bool& enabledX,const bool& enabledY,const bool& enabledZ): 
	OpenCLRender(),
	m_order(order),
	m_tensorData(data),
	m_sourceRead(false)
{
	/* load kernel source code */

	if (readKernelSource(m_kernelSource))
	{
		m_sourceRead = true;
	}
	else
	{
		return;
	}

	/* set tensor data */

	WGridRegular3D* grid = static_cast<WGridRegular3D*>(data->getGrid().get());

	m_numOfTensors[0] = grid->getNbCoordsX();
	m_numOfTensors[1] = grid->getNbCoordsY();
	m_numOfTensors[2] = grid->getNbCoordsZ();

	m_slices[0] = sliceX;
	m_slices[1] = sliceY;
	m_slices[2] = sliceZ;

	m_sliceEnabled[0] = enabledX;
	m_sliceEnabled[1] = enabledY;
	m_sliceEnabled[2] = enabledZ;

	/* create callback for data changes */

	setUpdateCallback(new DataChangeCallback());
}

WGlyphRender::WGlyphRender(const WGlyphRender& wGlyphRender,const osg::CopyOp& copyop): 
	OpenCLRender(wGlyphRender,copyop),
	m_order(wGlyphRender.m_order),
	m_tensorData(wGlyphRender.m_tensorData),
	m_sourceRead(wGlyphRender.m_sourceRead),
	m_kernelSource(wGlyphRender.m_kernelSource)
{
	m_numOfTensors[0] = wGlyphRender.m_numOfTensors[0];
	m_numOfTensors[1] = wGlyphRender.m_numOfTensors[1];
	m_numOfTensors[2] = wGlyphRender.m_numOfTensors[2];

	m_slices[0] = wGlyphRender.m_slices[0];
	m_slices[1] = wGlyphRender.m_slices[1];
	m_slices[2] = wGlyphRender.m_slices[2];

	m_sliceEnabled[0] = wGlyphRender.m_sliceEnabled[0];
	m_sliceEnabled[1] = wGlyphRender.m_sliceEnabled[1];
	m_sliceEnabled[2] = wGlyphRender.m_sliceEnabled[2];
}

WGlyphRender::~WGlyphRender()
{}

osg::BoundingBox WGlyphRender::computeBound() const
{
	osg::BoundingBox box(osg::Vec3(0.0f,0.0f,0.0f),osg::Vec3(m_numOfTensors[0],m_numOfTensors[1],m_numOfTensors[2]));
/*
	int numHalf[3] = {m_numOfTensors[0] / 2,m_numOfTensors[1] / 2,m_numOfTensors[2] / 2};

	osg::BoundingBox box
	(
		osg::Vec3(-numHalf[0],-numHalf[1],-numHalf[2]),
		osg::Vec3(m_numOfTensors[0] - numHalf[0],m_numOfTensors[1] - numHalf[1],m_numOfTensors[2] - numHalf[2])
	);
*/
	return box;
}

void WGlyphRender::setTensorData(const boost::shared_ptr<WDataSetSingle>& data,const int& order,
								 const int& sliceX, const int& sliceY, const int& sliceZ,
								 const bool& enabledX,const bool& enabledY,const bool& enabledZ)
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mutex);

	DataChangeCallback& changeCallback = *static_cast<DataChangeCallback*>(getUpdateCallback());

	changeCallback.m_slices[0] = sliceX;
	changeCallback.m_slices[1] = sliceY;
	changeCallback.m_slices[2] = sliceZ;

	changeCallback.m_sliceEnabled[0] = enabledX;
	changeCallback.m_sliceEnabled[1] = enabledY;
	changeCallback.m_sliceEnabled[2] = enabledZ;

	changeCallback.m_order = order;
	changeCallback.m_tensorData = data;

	changeCallback.m_changed = true;
	changeCallback.m_dataChanged = true;
}

void WGlyphRender::setSlices(const int& sliceX, const int& sliceY, const int& sliceZ,
							 const bool& enabledX,const bool& enabledY,const bool& enabledZ)
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mutex);

	DataChangeCallback& changeCallback = *static_cast<DataChangeCallback*>(getUpdateCallback());

	changeCallback.m_slices[0] = sliceX;
	changeCallback.m_slices[1] = sliceY;
	changeCallback.m_slices[2] = sliceZ;

	changeCallback.m_sliceEnabled[0] = enabledX;
	changeCallback.m_sliceEnabled[1] = enabledY;
	changeCallback.m_sliceEnabled[2] = enabledZ;

	changeCallback.m_changed = true;
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
	cl_kernel clScaleKernel;
	cl_kernel clRenderKernel;

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

	/* create CL scaling kernel */

	clScaleKernel = clCreateKernel(clProgram,"scaleGlyphs",&clError);

	if (clError != CL_SUCCESS)
	{
		osg::notify(osg::FATAL) << "Could not create the scaling kernel: " << getCLError(clError) << std::endl;

		clReleaseProgram(clProgram);

		return 0;
	}

	/* create CL rendering kernel */

	clRenderKernel = clCreateKernel(clProgram,"renderGlyphs",&clError);

	if (clError != CL_SUCCESS)
	{
		osg::notify(osg::FATAL) << "Could not create the rendering kernel: " << getCLError(clError) << std::endl;

		clReleaseKernel(clScaleKernel);
		clReleaseProgram(clProgram);

		return 0;
	}

	CLObjects* clObjects = new CLObjects();

	clObjects->clProgram = clProgram;
	clObjects->clScaleKernel = clScaleKernel;
	clObjects->clRenderKernel = clRenderKernel;

	return clObjects;
}

void WGlyphRender::setBuffers(const OpenCLRender::CLViewInformation& clViewInfo,OpenCLRender::CLProgramDataSet* clProgramDataSet) const
{
	cl_kernel& clRenderKernel = static_cast<CLObjects*>(clProgramDataSet)->clRenderKernel;

	clSetKernelArg(clRenderKernel,6,sizeof(unsigned int),&clViewInfo.width);
	clSetKernelArg(clRenderKernel,7,sizeof(unsigned int),&clViewInfo.height);
	clSetKernelArg(clRenderKernel,12,sizeof(cl_mem),&clViewInfo.colorBuffer);
	clSetKernelArg(clRenderKernel,13,sizeof(cl_mem),&clViewInfo.depthBuffer);
}

void WGlyphRender::render(const OpenCLRender::CLViewInformation& clViewInfo,
						  OpenCLRender::CLProgramDataSet* clProgramDataSet,
						  const osg::State& state) const
{
	CLObjects& clObjects = *static_cast<CLObjects*>(clProgramDataSet);

	/* check for new data and load them */

	if (clObjects.reloadData)
	{
		loadCLData(clViewInfo,clObjects);
	}

	if (clObjects.reloadData)
	{
		return;
	}

	/* get view properties */

	ViewProperties props;

	getViewProperties(props,state);

	/* set camera position relative to the center of the data set beginning at (0,0,0) */
/*
	props.origin += osg::Vec3f
	(
		m_numOfTensors[0] / 2,
		m_numOfTensors[1] / 2,
		m_numOfTensors[2] / 2
	);
*/
	/* set kernel view arguments */

	clSetKernelArg(clObjects.clRenderKernel,0,4 * sizeof(float),osg::Vec4f(props.origin,0.0f).ptr());
	clSetKernelArg(clObjects.clRenderKernel,1,4 * sizeof(float),osg::Vec4f(props.origin2LowerLeft,0.0f).ptr());
	clSetKernelArg(clObjects.clRenderKernel,2,4 * sizeof(float),osg::Vec4f(props.edgeX,0.0f).ptr());
	clSetKernelArg(clObjects.clRenderKernel,3,4 * sizeof(float),osg::Vec4f(props.edgeY,0.0f).ptr());
	clSetKernelArg(clObjects.clRenderKernel,4,sizeof(float),&props.planeNear);
	clSetKernelArg(clObjects.clRenderKernel,5,sizeof(float),&props.planeFar);

	int slices[4] = {m_slices[0],m_slices[1],m_slices[2],0};
	int sliceEnabled[4] = {m_sliceEnabled[0],m_sliceEnabled[1],m_sliceEnabled[2],0};

	clSetKernelArg(clObjects.clRenderKernel,8,4 * sizeof(int),slices);
	clSetKernelArg(clObjects.clRenderKernel,9,4 * sizeof(int),sliceEnabled);

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

	clError = clEnqueueNDRangeKernel(clViewInfo.clCommQueue,clObjects.clRenderKernel,2,0,gws,lws,0,0,0);

	if (clError != CL_SUCCESS)
	{
		osg::notify(osg::FATAL) << "Could not run the kernel: " << getCLError(clError) << std::endl;
	}
}

void WGlyphRender::loadCLData(const CLViewInformation& clViewInfo,CLObjects& clObjects) const
{
	cl_int clError;

	/* load new data set */

	WValueSet<float>* valueSet = static_cast<WValueSet<float>*>(m_tensorData->getValueSet().get());

	cl_mem tensorData = clCreateBuffer
	(
		clViewInfo.clContext,
		CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR|CL_MEM_ALLOC_HOST_PTR,
		valueSet->rawSize() * sizeof(float),const_cast<float*>(valueSet->rawData()),&clError
	);

	if (clError != CL_SUCCESS)
	{
		osg::notify(osg::FATAL) << "Could not load the data to GPU memory: " << getCLError(clError) << std::endl;

		return;
	}

	/* load auxiliary data if required */

	if (clObjects.reloadAuxData)
	{
		unsigned int numOfCoeffs = valueSet->dimension();

		/* load new factors */

		float* auxData = calcFactors(m_order,numOfCoeffs);

		cl_mem factors = clCreateBuffer
		(
			clViewInfo.clContext,
			CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR|CL_MEM_ALLOC_HOST_PTR,
			numOfCoeffs * sizeof(float),auxData,&clError
		);

		delete[] auxData;

		if (clError != CL_SUCCESS)
		{
			osg::notify(osg::FATAL) << "Could not load the aux data to GPU memory: " << getCLError(clError) << std::endl;

			clReleaseMemObject(tensorData);

			return;
		}

		if (clError != CL_SUCCESS)
		{
			osg::notify(osg::FATAL) << "Could not load the aux data to GPU memory: " << getCLError(clError) << std::endl;

			clReleaseMemObject(tensorData);
			clReleaseMemObject(factors);

			return;
		}

		/* release existing auxiliary data if required */

		if (clObjects.dataCreated)
		{
			clReleaseMemObject(clObjects.factors);
		}

		clObjects.factors = factors;

		/* set new kernel arguments */

		clSetKernelArg(clObjects.clScaleKernel,1,sizeof(cl_mem),&clObjects.factors);

		clObjects.reloadAuxData = false;
	}

	/* release existing data if required */

	if (clObjects.dataCreated)
	{
		clReleaseMemObject(clObjects.tensorData);
	}
	else
	{
		clObjects.dataCreated = true;
	}

	clObjects.tensorData = tensorData;

	/* set new kernel arguments */

	int numOfTensors[4] = {m_numOfTensors[0],m_numOfTensors[1],m_numOfTensors[2],0};

	clSetKernelArg(clObjects.clRenderKernel,10,4 * sizeof(int),numOfTensors);
	clSetKernelArg(clObjects.clRenderKernel,11,sizeof(cl_mem),&clObjects.tensorData);

	clObjects.reloadData = false;

	/* scale tensors */

	clSetKernelArg(clObjects.clScaleKernel,0,sizeof(cl_mem),&clObjects.tensorData);
	clSetKernelArg(clObjects.clScaleKernel,2,4 * sizeof(int),numOfTensors);

	size_t gwsX = numOfTensors[0] / 16;
	size_t gwsY = numOfTensors[1] / 16;

	if ((gwsX * 16) != numOfTensors[0]) gwsX++;
	if ((gwsY * 16) != numOfTensors[1]) gwsY++;

	gwsX *= 16;
	gwsY *= 16;

	size_t gws[3] = {gwsX,gwsY,numOfTensors[2]};
	size_t lws[3] = {16,16,1};

	clError = clEnqueueNDRangeKernel(clViewInfo.clCommQueue,clObjects.clScaleKernel,3,0,gws,lws,0,0,0);

	if (clError != CL_SUCCESS)
	{
		osg::notify(osg::FATAL) << "Could not run the scaling kernel: " << getCLError(clError) << std::endl;
	}
}