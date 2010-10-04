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
#include <boost/lexical_cast.hpp>
#include <boost/shared_array.hpp>

#include "../../common/WLogger.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../dataHandler/WValueSet.h"

#include "WGlyphRenderNode.h"

static OpenThreads::Mutex mutex;

/**
 * Calculates a multinomial coefficient.
 */
inline unsigned int Multinomial( const unsigned int& n, unsigned int k2, unsigned int k3 )
{
    unsigned int k1 = n - k2 - k3;

    if ( k3 > k2 )
    {
        unsigned int tmp = k3;

        k3 = k2;
        k2 = tmp;
    }

    if ( k2 > k1 )
    {
        unsigned int tmp = k2;

        k2 = k1;
        k1 = tmp;
    }

    unsigned int value = 1;

    for ( unsigned int i = 1; i <= k2; i++ )
    {
        value = value * (n - i + 1) / i;
    }

    for ( unsigned int i = 1; i <= k3; i++ )
    {
        value = value * ( n - k2 - i + 1 ) / i;
    }

    return value;
}

/**
 * Calculates the monomial factors for glyph evaluation.
 */
inline boost::shared_array< float > calcFactors( const unsigned int& order, const unsigned int& numOfCoeffs )
{
    float* factorSet = new float[ numOfCoeffs ];

    unsigned int i = 0;

    for ( unsigned int z = 0; z <= order; z++ )
    {
        for ( unsigned int y = 0; y <= order - z; y++, i++ )
        {
            factorSet[ i ] = Multinomial( order, y, z );
        }
    }

    return boost::shared_array< float >( factorSet );
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline bool readKernelSource( std::string& kernelSource, const std::string& filePath )
{
    std::ifstream sourceFile( filePath.c_str() );

    if ( !sourceFile.is_open() )
    {
        WLogger::getLogger()->addLogMessage
        (
            "Can not open kernel file \"" + filePath + "\".", 
            "WGlyphRenderNode", LL_ERROR 
        );

        return false;
    }

    std::string line;

    while ( !sourceFile.eof() )
    {
        std::getline( sourceFile, line );

        kernelSource += line + '\n';
    }

    sourceFile.close();

    return true;
}

/*-------------------------------------------------------------------------------------------------------------------*/

WGlyphRenderNode::DataChangeCallback::DataChangeCallback(): m_dataChanged( false )
{}

/*-------------------------------------------------------------------------------------------------------------------*/

void WGlyphRenderNode::DataChangeCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    if ( m_changed )
    {
        OpenThreads::ScopedLock< OpenThreads::Mutex > lock( mutex );

        WGlyphRenderNode& renderNode = *static_cast< WGlyphRenderNode* >( node );

        if ( m_dataChanged )
        {
            if ( renderNode.m_order != m_order )
            {
                renderNode.reset();
            }
            else
            {
                renderNode.changeDataSet( ReloadCallback() );
            }

            WGridRegular3D* grid = static_cast< WGridRegular3D* >( m_tensorData->getGrid().get() );

            renderNode.m_numOfTensors[ 0 ] = grid->getNbCoordsX();
            renderNode.m_numOfTensors[ 1 ] = grid->getNbCoordsY();
            renderNode.m_numOfTensors[ 2 ] = grid->getNbCoordsZ();

            renderNode.m_order = m_order;
            renderNode.m_tensorData = m_tensorData;

            renderNode.dirtyBound();

            m_tensorData.reset();

            m_dataChanged = false;
        }

        renderNode.m_slices[ 0 ] = m_slices[ 0 ];
        renderNode.m_slices[ 1 ] = m_slices[ 1 ];
        renderNode.m_slices[ 2 ] = m_slices[ 2 ];

        renderNode.m_sliceEnabled[ 0 ] = m_sliceEnabled[ 0 ];
        renderNode.m_sliceEnabled[ 1 ] = m_sliceEnabled[ 1 ];
        renderNode.m_sliceEnabled[ 2 ] = m_sliceEnabled[ 2 ];

        m_changed = false;
    }

    traverse( node, nv );
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WGlyphRenderNode::ReloadCallback::change( CLProgramDataSet* clProgramDataSet ) const
{
    CLObjects& clObjects = *static_cast< CLObjects* >( clProgramDataSet );

    clObjects.reloadData = true;
}

/*-------------------------------------------------------------------------------------------------------------------*/

WGlyphRenderNode::CLObjects::CLObjects(): reloadData( false )
{}

/*-------------------------------------------------------------------------------------------------------------------*/

WGlyphRenderNode::CLObjects::~CLObjects()
{
    if ( !reloadData )
    {
        clReleaseMemObject( tensorData );
        clReleaseMemObject( factors );
    }

    clReleaseKernel( clScaleKernel );
    clReleaseKernel( clRenderKernel );
    clReleaseProgram( clProgram );
}

/*-------------------------------------------------------------------------------------------------------------------*/

WGlyphRenderNode::WGlyphRenderNode( const boost::shared_ptr< WDataSetSingle >& data, const int& order, 
                                    const int& sliceX, const int& sliceY, const int& sliceZ, 
                                    const bool& enabledX, const bool& enabledY, const bool& enabledZ, 
                                    const boost::filesystem::path& search ): 
    WCLRenderNode(),
    m_order( order ),
    m_tensorData( data ),
    m_sourceRead( false )
{
    // load kernel source code

    if ( readKernelSource( m_kernelSource,( search / "kernels" / "GlyphKernel.cl" ).file_string() ) )
    {
        m_sourceRead = true;
    }
    else
    {
        return;
    }

    // set tensor data

    WGridRegular3D* grid = static_cast< WGridRegular3D* >( data->getGrid().get() );

    m_numOfTensors[ 0 ] = grid->getNbCoordsX();
    m_numOfTensors[ 1 ] = grid->getNbCoordsY();
    m_numOfTensors[ 2 ] = grid->getNbCoordsZ();

    m_slices[ 0 ] = sliceX;
    m_slices[ 1 ] = sliceY;
    m_slices[ 2 ] = sliceZ;

    m_sliceEnabled[ 0 ] = enabledX;
    m_sliceEnabled[ 1 ] = enabledY;
    m_sliceEnabled[ 2 ] = enabledZ;

    // create callback for data changes

    setUpdateCallback( new DataChangeCallback() );
    setDataVariance( DYNAMIC );
}

/*-------------------------------------------------------------------------------------------------------------------*/

WGlyphRenderNode::WGlyphRenderNode( const WGlyphRenderNode& wGlyphRenderNode, const osg::CopyOp& copyop ): 
    WCLRenderNode( wGlyphRenderNode, copyop ),
    m_order( wGlyphRenderNode.m_order ),
    m_tensorData( wGlyphRenderNode.m_tensorData ),
    m_sourceRead( wGlyphRenderNode.m_sourceRead ),
    m_kernelSource( wGlyphRenderNode.m_kernelSource )
{
    m_numOfTensors[ 0 ] = wGlyphRenderNode.m_numOfTensors[ 0 ];
    m_numOfTensors[ 1 ] = wGlyphRenderNode.m_numOfTensors[ 1 ];
    m_numOfTensors[ 2 ] = wGlyphRenderNode.m_numOfTensors[ 2 ];

    m_slices[ 0 ] = wGlyphRenderNode.m_slices[ 0 ];
    m_slices[ 1 ] = wGlyphRenderNode.m_slices[ 1 ];
    m_slices[ 2 ] = wGlyphRenderNode.m_slices[ 2 ];

    m_sliceEnabled[ 0 ] = wGlyphRenderNode.m_sliceEnabled[ 0 ];
    m_sliceEnabled[ 1 ] = wGlyphRenderNode.m_sliceEnabled[ 1 ];
    m_sliceEnabled[ 2 ] = wGlyphRenderNode.m_sliceEnabled[ 2 ];
}

/*-------------------------------------------------------------------------------------------------------------------*/

WGlyphRenderNode::~WGlyphRenderNode()
{}

/*-------------------------------------------------------------------------------------------------------------------*/

osg::BoundingBox WGlyphRenderNode::computeBoundingBox() const
{
    int numHalf[ 3 ] = { ( m_numOfTensors[0] / 2 ), ( m_numOfTensors[1] / 2 ), ( m_numOfTensors[2] / 2 ) };

    osg::BoundingBox box
    (
        osg::Vec3( -numHalf[ 0 ], -numHalf[ 1 ], -numHalf[ 2 ] ), 
        osg::Vec3( ( m_numOfTensors[ 0 ] - numHalf[ 0 ] ), ( m_numOfTensors[ 1 ] - numHalf[ 1 ] ), ( m_numOfTensors[ 2 ] - numHalf[ 2 ] ) ) 
    );

    return box;
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WGlyphRenderNode::setTensorData( const boost::shared_ptr< WDataSetSingle >& data, const int& order, 
                                      const int& sliceX, const int& sliceY, const int& sliceZ, 
                                      const bool& enabledX, const bool& enabledY, const bool& enabledZ )
{
    OpenThreads::ScopedLock< OpenThreads::Mutex > lock( mutex );

    DataChangeCallback& changeCallback = *static_cast< DataChangeCallback* >( getUpdateCallback() );

    changeCallback.m_slices[ 0 ] = sliceX;
    changeCallback.m_slices[ 1 ] = sliceY;
    changeCallback.m_slices[ 2 ] = sliceZ;

    changeCallback.m_sliceEnabled[ 0 ] = enabledX;
    changeCallback.m_sliceEnabled[ 1 ] = enabledY;
    changeCallback.m_sliceEnabled[ 2 ] = enabledZ;

    changeCallback.m_order = order;
    changeCallback.m_tensorData = data;

    changeCallback.m_changed = true;
    changeCallback.m_dataChanged = true;
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WGlyphRenderNode::setSlices( const int& sliceX, const int& sliceY, const int& sliceZ, 
                                  const bool& enabledX, const bool& enabledY, const bool& enabledZ )
{
    OpenThreads::ScopedLock< OpenThreads::Mutex > lock( mutex );

    DataChangeCallback& changeCallback = *static_cast< DataChangeCallback* >( getUpdateCallback() );

    changeCallback.m_slices[ 0 ] = sliceX;
    changeCallback.m_slices[ 1 ] = sliceY;
    changeCallback.m_slices[ 2 ] = sliceZ;

    changeCallback.m_sliceEnabled[ 0 ] = enabledX;
    changeCallback.m_sliceEnabled[ 1 ] = enabledY;
    changeCallback.m_sliceEnabled[ 2 ] = enabledZ;

    changeCallback.m_changed = true;
}

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::CLProgramDataSet* WGlyphRenderNode::initProgram( const WCLRenderNode::CLViewInformation& clViewInfo ) const
{
    if ( !m_sourceRead )
    {
        return 0;
    }

    cl_int clError;

    const char* cSource = m_kernelSource.c_str();
    size_t sourceLength = m_kernelSource.length();

    cl_program clProgram;
    cl_kernel clScaleKernel;
    cl_kernel clRenderKernel;
    cl_mem tensorData;
    cl_mem factors;

    const cl_context& clContext = clViewInfo.clContext;
    const cl_device_id& clDevice = clViewInfo.clDevice;

    // create CL program

    clProgram = clCreateProgramWithSource( clContext,1, &cSource, &sourceLength, &clError );

    if ( clError != CL_SUCCESS )
    {
        osg::notify( osg::FATAL ) << "Could not create the program with source: " << getCLError( clError ) << std::endl;

        return 0;
    }

    // build CL program

    std::string options = std::string( "-D Order=" ) + boost::lexical_cast< std::string, unsigned int >( m_order );

    clError = clBuildProgram( clProgram, 0, 0, options.c_str(), 0, 0 );

    if ( clError != CL_SUCCESS )
    {
        osg::notify( osg::FATAL ) << "Could not build the program: " << getCLError( clError ) << std::endl;

        if ( clError == CL_BUILD_PROGRAM_FAILURE )
        {
            size_t logLength;

            clGetProgramBuildInfo( clProgram, clDevice, CL_PROGRAM_BUILD_LOG, 0, 0, &logLength );

            char* buildLog = new char[ logLength ];

            clGetProgramBuildInfo( clProgram, clDevice, CL_PROGRAM_BUILD_LOG, logLength, buildLog, 0 );

            osg::notify( osg::FATAL ) << buildLog << std::endl;

            delete[] buildLog;
        }

        clReleaseProgram( clProgram );

        return 0;
    }

    // create CL scaling kernel

    clScaleKernel = clCreateKernel( clProgram, "scaleGlyphs", &clError );

    if ( clError != CL_SUCCESS )
    {
        osg::notify( osg::FATAL ) << "Could not create the scaling kernel: " << getCLError( clError ) << std::endl;

        clReleaseProgram( clProgram );

        return 0;
    }

    // create CL rendering kernel

    clRenderKernel = clCreateKernel( clProgram, "renderGlyphs", &clError );

    if ( clError != CL_SUCCESS )
    {
        osg::notify( osg::FATAL ) << "Could not create the rendering kernel: " << getCLError( clError ) << std::endl;

        clReleaseKernel( clScaleKernel );
        clReleaseProgram( clProgram );

        return 0;
    }

    // load data set

    WValueSet< float >* valueSet = static_cast< WValueSet< float >* >( m_tensorData->getValueSet().get() );

    tensorData = clCreateBuffer
    (
        clViewInfo.clContext, 
        CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR|CL_MEM_ALLOC_HOST_PTR, 
        ( valueSet->rawSize() * sizeof( float ) ), const_cast< float* >( valueSet->rawData() ), &clError 
    );

    if ( clError != CL_SUCCESS )
    {
        osg::notify( osg::FATAL ) << "Could not load the data to GPU memory: " << getCLError( clError ) << std::endl;

        clReleaseKernel( clRenderKernel );
        clReleaseKernel( clScaleKernel );
        clReleaseProgram( clProgram );

        return 0;
    }

    // load factors

    unsigned int numOfCoeffs = valueSet->dimension();

    factors = clCreateBuffer
    (
        clViewInfo.clContext, 
        CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR|CL_MEM_ALLOC_HOST_PTR, 
        ( numOfCoeffs * sizeof( float ) ), calcFactors( m_order, numOfCoeffs ).get(), &clError 
    );

    if ( clError != CL_SUCCESS )
    {
        osg::notify( osg::FATAL ) << "Could not load the aux data to GPU memory: " << getCLError( clError ) << std::endl;

        clReleaseMemObject( tensorData );
        clReleaseKernel( clRenderKernel );
        clReleaseKernel( clScaleKernel );
        clReleaseProgram( clProgram );

        return 0;
    }

    // set kernel arguments

    int numOfTensors[ 4 ] = { m_numOfTensors[ 0 ], m_numOfTensors[ 1 ], m_numOfTensors[ 2 ], 0 };

    clSetKernelArg( clRenderKernel, 10, ( 4 * sizeof( int ) ), numOfTensors );
    clSetKernelArg( clRenderKernel, 11, sizeof( cl_mem ), &tensorData );

    // scale tensors

    clSetKernelArg( clScaleKernel, 0, sizeof( cl_mem ), &tensorData );
    clSetKernelArg( clScaleKernel, 1, sizeof( cl_mem ), &factors );
    clSetKernelArg( clScaleKernel, 2, ( 4 * sizeof( int ) ), numOfTensors );

    size_t gwsX = ( numOfTensors[ 0 ] / 16 ) * 16;
    size_t gwsY = ( numOfTensors[ 1 ] / 16 ) * 16;

    if ( gwsX != numOfTensors[ 0 ] )
    {
        gwsX += 16;
    }

    if ( gwsY != numOfTensors[ 1 ] )
    {
        gwsY += 16;
    }

    size_t gws[ 3 ] = { gwsX, gwsY, numOfTensors[ 2 ] };
    size_t lws[ 3 ] = { 16, 16, 1 };

    clError = clEnqueueNDRangeKernel( clViewInfo.clCommQueue, clScaleKernel, 3, 0, gws, lws, 0, 0, 0 );

    if ( clError != CL_SUCCESS )
    {
        osg::notify( osg::FATAL ) << "Could not run the scaling kernel: " << getCLError( clError ) << std::endl;

        clReleaseMemObject( factors );
        clReleaseMemObject( tensorData );
        clReleaseKernel( clRenderKernel );
        clReleaseKernel( clScaleKernel );
        clReleaseProgram( clProgram );

        return 0;
    }

    CLObjects* clObjects = new CLObjects();

    clObjects->clProgram = clProgram;
    clObjects->clScaleKernel = clScaleKernel;
    clObjects->clRenderKernel = clRenderKernel;
    clObjects->tensorData = tensorData;
    clObjects->factors = factors;

    return clObjects;
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WGlyphRenderNode::setBuffers( const WCLRenderNode::CLViewInformation& clViewInfo, 
                                   WCLRenderNode::CLProgramDataSet* clProgramDataSet ) const
{
    cl_kernel& clRenderKernel = static_cast< CLObjects* >(clProgramDataSet)->clRenderKernel;

    clSetKernelArg( clRenderKernel, 6, sizeof( unsigned int ), &clViewInfo.width );
    clSetKernelArg( clRenderKernel, 7, sizeof( unsigned int ), &clViewInfo.height );
    clSetKernelArg( clRenderKernel, 12, sizeof( cl_mem), &clViewInfo.colorBuffer );
    clSetKernelArg( clRenderKernel, 13, sizeof( cl_mem), &clViewInfo.depthBuffer );
}

void WGlyphRenderNode::render( const WCLRenderNode::CLViewInformation& clViewInfo, 
                               WCLRenderNode::CLProgramDataSet* clProgramDataSet) const
{
    CLObjects& clObjects = *static_cast< CLObjects* >( clProgramDataSet );

    // check for new data and load them

    if ( clObjects.reloadData )
    {
        if ( !loadCLData( clViewInfo, clObjects ) )
        {
            return;
        }

        clObjects.reloadData = false;
    }

    // get view properties

    ViewProperties props;

    clViewInfo.getViewProperties( props );

    // set camera position relative to the center of the data set beginning at (0,0,0)

    props.origin += osg::Vec3f
    (
        ( m_numOfTensors[ 0 ] / 2 ), 
        ( m_numOfTensors[ 1 ] / 2 ), 
        ( m_numOfTensors[ 2 ] / 2 ) 
    );

    // set kernel view arguments

    clSetKernelArg( clObjects.clRenderKernel, 0, ( 4 * sizeof( float ) ), osg::Vec4f( props.origin, 0.0f ).ptr() );
    clSetKernelArg( clObjects.clRenderKernel, 1, ( 4 * sizeof( float ) ), osg::Vec4f( props.origin2LowerLeft, 0.0f ).ptr() );
    clSetKernelArg( clObjects.clRenderKernel, 2, ( 4 * sizeof( float ) ), osg::Vec4f( props.edgeX, 0.0f ).ptr() );
    clSetKernelArg( clObjects.clRenderKernel, 3, ( 4 * sizeof( float ) ), osg::Vec4f( props.edgeY, 0.0f ).ptr() );
    clSetKernelArg( clObjects.clRenderKernel, 4, sizeof( float ), &props.planeNear );
    clSetKernelArg( clObjects.clRenderKernel, 5, sizeof( float ), &props.planeFar );

    int slices[ 4 ] = { m_slices[ 0 ], m_slices[ 1 ], m_slices[ 2 ], 0 };
    int sliceEnabled[ 4 ] = { m_sliceEnabled[ 0 ], m_sliceEnabled[ 1 ], m_sliceEnabled[ 2 ], 0 };

    clSetKernelArg( clObjects.clRenderKernel, 8, (4 * sizeof( int ) ), slices );
    clSetKernelArg( clObjects.clRenderKernel, 9, (4 * sizeof( int ) ), sliceEnabled );

    // global work size has to be a multiple of local work size

    size_t gwsX = ( clViewInfo.width / 16 ) * 16;
    size_t gwsY = ( clViewInfo.height / 16 ) * 16;

    if ( gwsX != clViewInfo.width )
    {
        gwsX += 16;
    }

    if ( gwsY != clViewInfo.height )
    {
        gwsY += 16;
    }

    size_t gws[ 2 ] = { gwsX,gwsY };
    size_t lws[ 2 ] = { 16,16 };

    // run kernel

    cl_int clError;

    clError = clEnqueueNDRangeKernel( clViewInfo.clCommQueue, clObjects.clRenderKernel, 2, 0, gws, lws, 0, 0, 0 );

    if ( clError != CL_SUCCESS )
    {
        osg::notify( osg::FATAL ) << "Could not run the kernel: " << getCLError( clError ) << std::endl;
    }
}

/*-------------------------------------------------------------------------------------------------------------------*/

bool WGlyphRenderNode::loadCLData( const CLViewInformation& clViewInfo, CLObjects& clObjects ) const
{
    cl_int clError;

    // release existing data

    clReleaseMemObject( clObjects.tensorData );

    // load new data set

    WValueSet< float >* valueSet = static_cast< WValueSet< float >* >( m_tensorData->getValueSet().get() );

    clObjects.tensorData = clCreateBuffer
    (
        clViewInfo.clContext, 
        CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR|CL_MEM_ALLOC_HOST_PTR, 
        ( valueSet->rawSize() * sizeof( float ) ), 
        const_cast< float* >( valueSet->rawData() ), &clError 
    );

    if ( clError != CL_SUCCESS )
    {
        osg::notify( osg::FATAL ) << "Could not load the data to GPU memory: " << getCLError( clError ) << std::endl;

        return false;
    }

    // set new kernel arguments

    int numOfTensors[ 4 ] = { m_numOfTensors[ 0 ], m_numOfTensors[ 1 ], m_numOfTensors[ 2 ], 0 };

    clSetKernelArg( clObjects.clRenderKernel, 10, (4 * sizeof( int ) ), numOfTensors );
    clSetKernelArg( clObjects.clRenderKernel, 11, sizeof( cl_mem ), &clObjects.tensorData );

    // scale tensors

    clSetKernelArg( clObjects.clScaleKernel, 0, sizeof( cl_mem ), &clObjects.tensorData );
    clSetKernelArg( clObjects.clScaleKernel, 2, (4 * sizeof( int ) ), numOfTensors );

    size_t gwsX = ( numOfTensors[ 0 ] / 16 ) * 16;
    size_t gwsY = ( numOfTensors[ 1 ] / 16 ) * 16;

    if ( gwsX != numOfTensors[ 0 ] )
    {
        gwsX += 16;
    }

    if ( gwsY != numOfTensors[ 1 ] )
    {
        gwsY += 16;
    }

    size_t gws[ 3 ] = { gwsX, gwsY, numOfTensors[ 2 ] };
    size_t lws[ 3 ] = { 16, 16, 1 };

    clError = clEnqueueNDRangeKernel( clViewInfo.clCommQueue, clObjects.clScaleKernel, 3, 0, gws, lws, 0, 0, 0 );

    if ( clError != CL_SUCCESS )
    {
        osg::notify( osg::FATAL ) << "Could not run the scaling kernel: " << getCLError( clError ) << std::endl;

        clReleaseMemObject( clObjects.tensorData );

        return false;
    }

    return true;
}

/*-------------------------------------------------------------------------------------------------------------------*/
