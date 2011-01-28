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

#include "../../dataHandler/WGridRegular3D.h"
#include "../../dataHandler/WValueSet.h"

#include "WGlyphRenderNode.h"

/*-------------------------------------------------------------------------------------------------------------------*/

size_t WGlyphRenderNode::m_lws[ 2 ] = { 4, 32 };

/*-------------------------------------------------------------------------------------------------------------------*/

inline bool readKernelSource( std::string& kernelSource, const std::string& filePath )
{
    std::ifstream sourceFile( filePath.c_str() );

    if ( !sourceFile.is_open() )
    {
        osg::notify( osg::FATAL ) << "Can not open kernel file \"" + filePath + "\"." << std::endl;

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

WGlyphRenderNode::ReloadCallback::ReloadCallback( int dim ): m_dimension( dim )
{}

/*-------------------------------------------------------------------------------------------------------------------*/

void WGlyphRenderNode::ReloadCallback::change( CLProgramDataSet* clProgramDataSet ) const
{
    CLObjects& clObjects = *static_cast< CLObjects* >( clProgramDataSet );

    switch ( m_dimension )
    {
        case 0: clObjects.m_reloadX = true;
                break;
        case 1: clObjects.m_reloadY = true;
                break;
        case 2: clObjects.m_reloadZ = true;
                break;
    }
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WGlyphRenderNode::ChangeCallback::change( CLProgramDataSet* clProgramDataSet ) const
{
    CLObjects& clObjects = *static_cast< CLObjects* >( clProgramDataSet );

    clObjects.m_setVisibility = true;
}

/*-------------------------------------------------------------------------------------------------------------------*/

WGlyphRenderNode::CLObjects::CLObjects():
    m_dataLoaded( false ),
    m_reloadX( true ),
    m_reloadY( true ),
    m_reloadZ( true ),
    m_setVisibility( true )
{}

/*-------------------------------------------------------------------------------------------------------------------*/

WGlyphRenderNode::CLObjects::~CLObjects()
{
    if ( m_dataLoaded )
    {
        if ( !m_reloadX )
        {
            clReleaseMemObject( m_sliceX );
        }

        if ( !m_reloadY )
        {
            clReleaseMemObject( m_sliceY );
        }

        if ( !m_reloadZ )
        {
            clReleaseMemObject( m_sliceZ );
        }
    }

    clReleaseKernel( m_scaleKernel );
    clReleaseKernel( m_renderKernel );
    clReleaseProgram( m_program );
}

/*-------------------------------------------------------------------------------------------------------------------*/

WGlyphRenderNode::WGlyphRenderNode( const boost::filesystem::path& search, bool deactivated ):
    WCLRenderNode( deactivated ),
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

    // for thread safety during data changes

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

    m_slicePosition[ 0 ] = wGlyphRenderNode.m_slicePosition[ 0 ];
    m_slicePosition[ 1 ] = wGlyphRenderNode.m_slicePosition[ 1 ];
    m_slicePosition[ 2 ] = wGlyphRenderNode.m_slicePosition[ 2 ];

    m_sliceVisibility[ 0 ] = wGlyphRenderNode.m_sliceVisibility[ 0 ];
    m_sliceVisibility[ 1 ] = wGlyphRenderNode.m_sliceVisibility[ 1 ];
    m_sliceVisibility[ 2 ] = wGlyphRenderNode.m_sliceVisibility[ 2 ];
}

/*-------------------------------------------------------------------------------------------------------------------*/

WGlyphRenderNode::~WGlyphRenderNode()
{}

/*-------------------------------------------------------------------------------------------------------------------*/

osg::BoundingBox WGlyphRenderNode::computeBoundingBox() const
{
    int numHalf[ 3 ] = { m_numOfTensors[0] / 2, m_numOfTensors[1] / 2, m_numOfTensors[2] / 2 };

    osg::BoundingBox box
    (
        osg::Vec3( -numHalf[ 0 ], -numHalf[ 1 ], -numHalf[ 2 ] ),
        osg::Vec3( m_numOfTensors[ 0 ] - numHalf[ 0 ], m_numOfTensors[ 1 ] - numHalf[ 1 ], m_numOfTensors[ 2 ] - numHalf[ 2 ] )
    );

    return box;
}

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::CLProgramDataSet* WGlyphRenderNode::initProgram( const CLViewInformation& clViewInfo ) const
{
    if ( !m_sourceRead )
    {
        return 0;
    }

    cl_int clError;

    const char* source = m_kernelSource.c_str();
    size_t sourceLength = m_kernelSource.length();

    cl_program program;
    cl_kernel scaleKernel;
    cl_kernel renderKernel;

    // create CL program

    program = clCreateProgramWithSource( clViewInfo.getContext(),1, &source, &sourceLength, &clError );

    if ( clError != CL_SUCCESS )
    {
        osg::notify( osg::FATAL ) << "Could not create the program with source: " << getCLError( clError ) << std::endl;

        return 0;
    }

    // build CL program

    std::string options = std::string( "-D Order=" ) + boost::lexical_cast< std::string, int >( m_order );

    clError = clBuildProgram( program, 0, 0, options.c_str(), 0, 0 );

    if ( clError != CL_SUCCESS )
    {
        osg::notify( osg::FATAL ) << "Could not build the program: " << getCLError( clError ) << std::endl;

        if ( clError == CL_BUILD_PROGRAM_FAILURE )
        {
            size_t logLength;

            clGetProgramBuildInfo( program, clViewInfo.getDevice(), CL_PROGRAM_BUILD_LOG, 0, 0, &logLength );

            char* buildLog = new char[ logLength ];

            clGetProgramBuildInfo( program, clViewInfo.getDevice(), CL_PROGRAM_BUILD_LOG, logLength, buildLog, 0 );

            osg::notify( osg::FATAL ) << buildLog << std::endl;

            delete[] buildLog;
        }

        clReleaseProgram( program );

        return 0;
    }

    // create scaling kernel

    scaleKernel = clCreateKernel( program, "scaleGlyphs", &clError );

    if ( clError != CL_SUCCESS )
    {
        osg::notify( osg::FATAL ) << "Could not create the scaling kernel: " << getCLError( clError ) << std::endl;

        clReleaseProgram( program );

        return 0;
    }

    // create rendering kernel

    renderKernel = clCreateKernel( program, "renderGlyphs", &clError );

    if ( clError != CL_SUCCESS )
    {
        osg::notify( osg::FATAL ) << "Could not create the rendering kernel: " << getCLError( clError ) << std::endl;

        clReleaseKernel( scaleKernel );
        clReleaseProgram( program );

        return 0;
    }

    // assemble

    CLObjects* clObjects = new CLObjects();

    clObjects->m_program = program;
    clObjects->m_scaleKernel = scaleKernel;
    clObjects->m_renderKernel = renderKernel;

    return clObjects;
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WGlyphRenderNode::setBuffers( const CLViewInformation& clViewInfo, CLProgramDataSet* clProgramDataSet ) const
{
    CLObjects& clObjects = *static_cast< CLObjects* >( clProgramDataSet );

    // set static kernel arguments

    clSetKernelArg( clObjects.m_renderKernel, 6, sizeof( unsigned int ), &clViewInfo.getWidth() );
    clSetKernelArg( clObjects.m_renderKernel, 7, sizeof( unsigned int ), &clViewInfo.getHeight() );
    clSetKernelArg( clObjects.m_renderKernel, 13, sizeof( cl_mem), &clViewInfo.getColorBuffer() );
    clSetKernelArg( clObjects.m_renderKernel, 14, sizeof( cl_mem), &clViewInfo.getDepthBuffer() );

    // calculate global work size which has to be a multiple of local work size

    clObjects.m_gws[ 0 ] = clViewInfo.getWidth() / m_lws[ 0 ];
    clObjects.m_gws[ 1 ] = clViewInfo.getHeight() / m_lws[ 1 ];

    if ( ( clObjects.m_gws[ 0 ] * m_lws[ 0 ] ) != clViewInfo.getWidth() )
    {
        clObjects.m_gws[ 0 ]++;
    }

    if ( ( clObjects.m_gws[ 1 ] * m_lws[ 1 ] ) != clViewInfo.getWidth() )
    {
        clObjects.m_gws[ 1 ]++;
    }

    clObjects.m_gws[ 0 ] *= m_lws[ 0 ];
    clObjects.m_gws[ 1 ] *= m_lws[ 1 ];
}

void WGlyphRenderNode::render( const CLViewInformation& clViewInfo, CLProgramDataSet* clProgramDataSet) const
{
    CLObjects& clObjects = *static_cast< CLObjects* >( clProgramDataSet );

    // check for new data and load them

    if ( clObjects.m_reloadX || clObjects.m_reloadY || clObjects.m_reloadZ || clObjects.m_setVisibility )
    {
        if ( updateData( clViewInfo, clObjects ) )
        {
            return;
        }
    }

    // get view properties

    ViewProperties props;

    clViewInfo.getViewProperties( props );

    // set camera position relative to the center of the data set beginning at (0,0,0)

    osg::Vec3f origin = props.getOrigin() + osg::Vec3f
    (
        m_numOfTensors[ 0 ] / 2,
        m_numOfTensors[ 1 ] / 2,
        m_numOfTensors[ 2 ] / 2
    );

    // set kernel view arguments

    clSetKernelArg( clObjects.m_renderKernel, 0, ( 4 * sizeof( float ) ), osg::Vec4f( origin, 0.0f ).ptr() );
    clSetKernelArg( clObjects.m_renderKernel, 1, ( 4 * sizeof( float ) ), osg::Vec4f( props.getOriginToLowerLeft(), 0.0f ).ptr() );
    clSetKernelArg( clObjects.m_renderKernel, 2, ( 4 * sizeof( float ) ), osg::Vec4f( props.getEdgeX(), 0.0f ).ptr() );
    clSetKernelArg( clObjects.m_renderKernel, 3, ( 4 * sizeof( float ) ), osg::Vec4f( props.getEdgeY(), 0.0f ).ptr() );
    clSetKernelArg( clObjects.m_renderKernel, 4, sizeof( float ), &props.getNearPlane() );
    clSetKernelArg( clObjects.m_renderKernel, 5, sizeof( float ), &props.getFarPlane() );

    // run kernel

    cl_int clError;

    clError = clEnqueueNDRangeKernel( clViewInfo.getCommQueue(), clObjects.m_renderKernel, 2, 0, clObjects.m_gws, m_lws, 0, 0, 0 );

    if ( clError != CL_SUCCESS )
    {
        osg::notify( osg::FATAL ) << "Could not run the kernel: " << getCLError( clError ) << std::endl;
    }
}

/*-------------------------------------------------------------------------------------------------------------------*/

bool WGlyphRenderNode::loadSlice( const CLViewInformation& clViewInfo, CLObjects& clObjects, cl_mem& slice, int dim ) const
{
    cl_int clError;

    if ( clObjects.m_dataLoaded )
    {
        clReleaseMemObject( slice );
    }

    int dim2 = ( dim + 1 ) % 3;
    int dim3 = ( dim + 2 ) % 3;

    // load slice

    slice = clCreateBuffer
    (
        clViewInfo.getContext(),
        CL_MEM_READ_ONLY|CL_MEM_ALLOC_HOST_PTR,
        m_numOfTensors[ dim2 ] * m_numOfTensors[ dim3 ],
        0, &clError
    );

    if ( clError != CL_SUCCESS )
    {
        osg::notify( osg::FATAL ) << "Could not load the data to GPU memory: " << getCLError( clError ) << std::endl;

        return true;
    }

    float* data = static_cast< float* >
    (
        clEnqueueMapBuffer
        (
            clViewInfo.getCommQueue(), slice,
            CL_TRUE, CL_MAP_WRITE, 0, m_numOfTensors[ dim2 ] * m_numOfTensors[ dim3 ],
            0, 0, 0, &clError
        )
    );

    if ( clError != CL_SUCCESS )
    {
        osg::notify( osg::FATAL ) << "Could not load the data to GPU memory: " << getCLError( clError ) << std::endl;

        clReleaseMemObject( slice );

        return true;
    }

    int numOfCoeffs = ( m_order + 1 ) * ( m_order + 1 ) / 2;
    int factors[ 3 ] = { 1, m_numOfTensors[ 0 ], m_numOfTensors[ 0 ] * m_numOfTensors[ 1 ] };

    for ( int idx1 = 0, i = 0; idx1 < m_numOfTensors[ dim2 ]; idx1++ )
    {
        for ( int idx2 = 0; idx2 < m_numOfTensors[ dim3 ]; idx2++, i++ )
        {
            int mapCoord = numOfCoeffs * i;
            int dataCoord = numOfCoeffs * ( m_slicePosition[ dim ] * factors[ dim ] + idx1 * factors[ dim2 ] + idx2 * factors[ dim3 ] );

            for ( int j = 0; j < numOfCoeffs; j++ )
            {
                data[ mapCoord + j ] = static_cast< float >( m_tensorData->getValueSet()->getScalarDouble( dataCoord + j ) );
            }
        }
    }

    clError = clEnqueueUnmapMemObject
    (
        clViewInfo.getCommQueue(), slice,
        data, 0, 0, 0
    );

    if ( clError != CL_SUCCESS )
    {
        osg::notify( osg::FATAL ) << "Could not load the data to GPU memory: " << getCLError( clError ) << std::endl;

        clReleaseMemObject( slice );

        return true;
    }

    // scale tensors

    size_t gws[ 2 ];
    size_t lws[ 2 ] = { 16, 16 };
    float dimension[ 2 ] = { m_numOfTensors[ dim2 ], m_numOfTensors[ dim3 ] };

    gws[ 0 ] = dimension[ 0 ] / lws[ 0 ];
    gws[ 1 ] = dimension[ 1 ] / lws[ 1 ];

    if ( ( gws[ 0 ] * lws[ 0 ] ) != dimension[ 0 ] )
    {
        gws[ 0 ]++;
    }

    if ( ( gws[ 1 ] * lws[ 1 ] ) != dimension[ 1 ] )
    {
        gws[ 1 ]++;
    }

    gws[ 0 ] *= lws[ 0 ];
    gws[ 1 ] *= lws[ 1 ];
    
    clSetKernelArg( clObjects.m_scaleKernel, 0, sizeof( cl_mem ), &slice);
    clSetKernelArg( clObjects.m_scaleKernel, 1, 2 * sizeof( float ), dimension);

    clError = clEnqueueNDRangeKernel( clViewInfo.getCommQueue(), clObjects.m_scaleKernel, 2, 0, gws, lws, 0, 0, 0 );

    if ( clError != CL_SUCCESS )
    {
        osg::notify( osg::FATAL ) << "Could not run the scaling kernel: " << getCLError( clError ) << std::endl;
    }

    return false;
}

bool WGlyphRenderNode::updateData( const CLViewInformation& clViewInfo, CLObjects& clObjects ) const
{
    if ( clObjects.m_reloadX )
    {
        if ( loadSlice( clViewInfo, clObjects, clObjects.m_sliceX, 0 ) )
        {
            return true;
        }

        clObjects.m_reloadX = false;
    }

    if ( clObjects.m_reloadY )
    {
        if ( loadSlice( clViewInfo, clObjects, clObjects.m_sliceY, 1 ) )
        {
            return true;
        }

        clObjects.m_reloadY = false;
    }

    if ( clObjects.m_reloadZ )
    {
        if ( loadSlice( clViewInfo, clObjects, clObjects.m_sliceZ, 2 ) )
        {
            return true;
        }

        clObjects.m_reloadZ = false;
    }

    clObjects.m_dataLoaded = true;

    // set new kernel arguments

    int numOfTensors[ 4 ] = { m_numOfTensors[ 0 ], m_numOfTensors[ 1 ], m_numOfTensors[ 2 ], 0 };

    clSetKernelArg( clObjects.m_renderKernel, 9, ( 4 * sizeof( int ) ), numOfTensors );
    clSetKernelArg( clObjects.m_renderKernel, 10, sizeof( cl_mem ), &clObjects.m_sliceX );
    clSetKernelArg( clObjects.m_renderKernel, 11, sizeof( cl_mem ), &clObjects.m_sliceY );
    clSetKernelArg( clObjects.m_renderKernel, 12, sizeof( cl_mem ), &clObjects.m_sliceZ );

    return true;
}

/*-------------------------------------------------------------------------------------------------------------------*/
