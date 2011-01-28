//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
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

#include <boost/lexical_cast.hpp>

#include <osg/notify>

#include "../../dataHandler/WGridRegular3D.h"
#include "../../dataHandler/WValueSet.h"

#include "WGlyphModule.h"

//---------------------------------------------------------------------------------------------------------------------

WGlyphModule::ChangeCallback::ChangeCallback( WGlyphModule* module, ChangeOperation operation, int slice ):
    m_module( module ),
    m_operation( operation ),
    m_slice( slice )
{}

//---------------------------------------------------------------------------------------------------------------------

void WGlyphModule::ChangeCallback::change( const CLViewData &viewData, CLData &data ) const
{
    if ( m_operation == VISIBILITY )
    {
        m_module->loadVisibility( viewData, static_cast< CLObjects& >( data ) );

        return;
    }

    if ( m_operation == UPDATE_SLICE )
    {
        m_module->updateSlice( viewData, static_cast< CLObjects& >( data ), m_slice );
        
        return;
    }

    if ( m_operation == LOAD_DATA_SET )
    {
        CLObjects& objects = static_cast< CLObjects& >( data );

        objects.m_scaleKernel = cl::Kernel();
        objects.m_renderKernel = cl::Kernel();

        m_module->loadDataset( viewData, static_cast< CLObjects& >( data ) );
    }
}

//---------------------------------------------------------------------------------------------------------------------

WGlyphModule::WGlyphModule( const boost::filesystem::path& path ):
    WGEModuleCL(),
    m_localSize( 4, 32 ),
    m_source( "" )
{
    std::ifstream sourceFile( path.string().c_str() );

    if ( !sourceFile.is_open() )
    {
        return;
    }

    std::string line;

    while ( !sourceFile.eof() )
    {
        std::getline( sourceFile, line );

        m_source += line + '\n';
    }

    sourceFile.close();
}

//---------------------------------------------------------------------------------------------------------------------

WGlyphModule::~WGlyphModule()
{}

//---------------------------------------------------------------------------------------------------------------------

WGEModuleCL::CLData* WGlyphModule::initCLData( const CLViewData& viewData ) const
{
    if ( m_source == "" )
    {
        return 0;
    }

    cl_int error;

    // create program -------------------------------------------------------------------------------------------------

    cl::Program program
    (
        viewData.getContext(),
        cl::Program::Sources( 1, std::pair< const char*, size_t >( m_source.c_str(), m_source.length() ) ),
        &error
    );

    if ( error != CL_SUCCESS )
    {
        return 0;
    }

    // create data container ------------------------------------------------------------------------------------------

    boost::shared_ptr< CLObjects > objects( new CLObjects() );

    objects->m_program = program;

    // create kernels and load data to GPU memory ---------------------------------------------------------------------

    if ( loadDataset( viewData, *objects ) )
    {
        return 0;
    }

    if ( updateSlice( viewData, *objects, 0 ) )
    {
        return 0;
    }

    if ( updateSlice( viewData, *objects, 1 ) )
    {
        return 0;
    }

    if ( updateSlice( viewData, *objects, 2 ) )
    {
        return 0;
    }

    loadVisibility( viewData, *objects );

    return objects.get();
}

//---------------------------------------------------------------------------------------------------------------------

void WGlyphModule::setBuffers( const CLViewData& viewData, CLData& data ) const
{
    CLObjects& objects = static_cast< CLObjects& >( data );

    // set static kernel arguments ------------------------------------------------------------------------------------

    objects.m_renderKernel.setArg< cl_uint >( 6, viewData.getWidth() );
    objects.m_renderKernel.setArg< cl_uint >( 7, viewData.getHeight() );
    objects.m_renderKernel.setArg< cl::Image2DGL >( 13, viewData.getColorBuffer() );
    objects.m_renderKernel.setArg< cl::Image2DGL >( 14, viewData.getDepthBuffer() );

    // calculate global work size which has to be a multiple of local work size ---------------------------------------

    objects.m_globalSize = computeGlobalWorkSize( m_localSize, cl::NDRange( viewData.getWidth(), viewData.getHeight() ) );
}

//---------------------------------------------------------------------------------------------------------------------

bool WGlyphModule::render( const CLViewData& viewData, CLData& data ) const
{
    CLObjects& objects = static_cast< CLObjects& >( data );

    // get view properties --------------------------------------------------------------------------------------------

    ViewProperties props( viewData );

    // set camera position relative to the center of the data set beginning at (0,0,0) --------------------------------

    cl_float4 origin =
    {
        props.getOrigin().s[ 0 ] + m_numOfTensors[ 0 ] / 2,
        props.getOrigin().s[ 1 ] + m_numOfTensors[ 1 ] / 2,
        props.getOrigin().s[ 2 ] + m_numOfTensors[ 2 ] / 2,
        1.0f
    };

    // set kernel view arguments --------------------------------------------------------------------------------------

    objects.m_renderKernel.setArg< cl_float4 >( 0, origin );
    objects.m_renderKernel.setArg< cl_float4 >( 1, props.getOriginToLowerLeft() );
    objects.m_renderKernel.setArg< cl_float4 >( 2, props.getEdgeX() );
    objects.m_renderKernel.setArg< cl_float4 >( 3, props.getEdgeY() );
    objects.m_renderKernel.setArg< cl_float >( 4, props.getNearPlane() );
    objects.m_renderKernel.setArg< cl_float >( 5, props.getFarPlane() );

    // run kernel -----------------------------------------------------------------------------------------------------

    cl_int error = viewData.getCommQueue().enqueueNDRangeKernel
    (
        objects.m_renderKernel, cl::NDRange(), objects.m_globalSize, m_localSize
    );

    if ( error != CL_SUCCESS )
    {
        return true;
    }

    return false;
}

//---------------------------------------------------------------------------------------------------------------------

osg::BoundingBox WGlyphModule::computeBoundingBox() const
{
    int lowerHalf[ 3 ] =
    {
        -m_numOfTensors[0] / 2,
        -m_numOfTensors[1] / 2,
        -m_numOfTensors[2] / 2
    };

    int upperHalf[ 3 ] =
    {
        m_numOfTensors[ 0 ] + lowerHalf[ 0 ],
        m_numOfTensors[ 1 ] + lowerHalf[ 1 ],
        m_numOfTensors[ 2 ] + lowerHalf[ 2 ]
    };

    osg::BoundingBox box
    (
        osg::Vec3( lowerHalf[ 0 ], lowerHalf[ 1 ], lowerHalf[ 2 ] ),
        osg::Vec3( upperHalf[ 0 ], upperHalf[ 1 ], upperHalf[ 2 ] )
    );

    return box;
}

//---------------------------------------------------------------------------------------------------------------------

void WGlyphModule::loadVisibility( const CLViewData& viewData, CLObjects& objects ) const
{
    cl_float4 slicePosition =
    {
        m_sliceVisibility[0] ? m_slicePosition[0] : CL_INFINITY,
        m_sliceVisibility[1] ? m_slicePosition[1] : CL_INFINITY,
        m_sliceVisibility[2] ? m_slicePosition[2] : CL_INFINITY,
        0.0f
    };

    objects.m_renderKernel.setArg< cl_float4 >( 8, slicePosition );
}

//---------------------------------------------------------------------------------------------------------------------

bool WGlyphModule::updateSlice( const CLViewData& viewData, CLObjects& objects, int dim ) const
{
    cl::Buffer* slice;

    switch ( dim )
    {
        case 0: slice = &objects.m_sliceX;
                break;
        case 1: slice = &objects.m_sliceY;
                break;
        case 2: slice = &objects.m_sliceZ;
                break;
    }

    cl_int error;

    int dim2 = ( dim + 1 ) % 3;
    int dim3 = ( dim + 2 ) % 3;

    // load new tensor data -------------------------------------------------------------------------------------------

    int numOfCoeffs = ( m_order + 1 ) * ( m_order + 2 ) / 2;

    cl_float* data = static_cast< cl_float* >
    (
        viewData.getCommQueue().enqueueMapBuffer
        (
            *slice, CL_TRUE, CL_MAP_WRITE, 0,
            numOfCoeffs * m_numOfTensors[ dim2 ] * m_numOfTensors[ dim3 ] * sizeof( cl_float ),
            0, 0, &error
        )
    );

    if ( error != CL_SUCCESS )
    {
        return true;
    }

    int factors[ 3 ] = { 1, m_numOfTensors[ 0 ], m_numOfTensors[ 0 ] * m_numOfTensors[ 1 ] };

    for ( int idx1 = 0, i = 0; idx1 < m_numOfTensors[ dim2 ]; idx1++ )
    {
        for ( int idx2 = 0; idx2 < m_numOfTensors[ dim3 ]; idx2++, i += numOfCoeffs )
        {
            int dataCoord = m_slicePosition[ dim ] * factors[ dim ] + idx1 * factors[ dim2 ] + idx2 * factors[ dim3 ];

            dataCoord *= numOfCoeffs;

            for ( int j = 0; j < numOfCoeffs; j++ )
            {
                data[ i + j ] = static_cast< cl_float >( m_tensorData->getValueAt( dataCoord + j ) );
            }
        }
    }

    error = viewData.getCommQueue().enqueueUnmapMemObject( *slice, data );

    if ( error != CL_SUCCESS )
    {
        return true;
    }

    // scale new tensors ----------------------------------------------------------------------------------------------

    cl::NDRange localSize( 256 );
    cl::NDRange bufferSize( m_numOfTensors[ dim2 ] * m_numOfTensors[ dim3 ] );
    cl::NDRange globalSize = computeGlobalWorkSize( localSize, bufferSize );

    objects.m_scaleKernel.setArg< cl::Buffer >( 0, *slice );
    objects.m_scaleKernel.setArg< cl_int >( 1, bufferSize[ 0 ] );

    error = viewData.getCommQueue().enqueueNDRangeKernel( objects.m_scaleKernel, cl::NDRange(), globalSize, localSize );

    if ( error != CL_SUCCESS )
    {
        return true;
    }

    return false;
}

//---------------------------------------------------------------------------------------------------------------------

bool WGlyphModule::loadDataset( const CLViewData& viewData, CLObjects& objects ) const
{
    cl_int error;

    // build program --------------------------------------------------------------------------------------------------

    std::string options = std::string( "-D Order=" ) + boost::lexical_cast< std::string, int >( m_order );

    error = objects.m_program.build( std::vector< cl::Device >( 1, viewData.getDevice() ), options.c_str() );

    if ( error != CL_SUCCESS )
    {
        #ifdef DEBUG

            if ( error == CL_BUILD_PROGRAM_FAILURE )
            {
                std::string log;

                program.getBuildInfo< std::string >( viewData.getDevice(), CL_PROGRAM_BUILD_LOG, &log );

                osg::notify( osg::FATAL ) << log << std::endl;
            }

        #endif

        return true;
    }

    // create kernels -------------------------------------------------------------------------------------------------

    objects.m_scaleKernel = cl::Kernel( objects.m_program, "scaleGlyphs", &error );

    if ( error != CL_SUCCESS )
    {
        return true;
    }

    objects.m_renderKernel = cl::Kernel( objects.m_program, "renderGlyphs", &error );

    if ( error != CL_SUCCESS )
    {
        return true;
    }

    // create new buffers ---------------------------------------------------------------------------------------------

    int numOfCoeffs = ( m_order + 1 ) * ( m_order + 2 ) / 2;

    objects.m_sliceX = cl::Buffer
    (
        viewData.getContext(),
        CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
        numOfCoeffs * m_numOfTensors[ 1 ] * m_numOfTensors[ 2 ] * sizeof( cl_float ),
        0, &error
    );

    if ( error != CL_SUCCESS )
    {
        return true;
    }

    objects.m_sliceY = cl::Buffer
    (
        viewData.getContext(),
        CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
        numOfCoeffs * m_numOfTensors[ 0 ] * m_numOfTensors[ 2 ] * sizeof( cl_float ),
        0, &error
    );

    if ( error != CL_SUCCESS )
    {
        return true;
    }

    objects.m_sliceZ = cl::Buffer
    (
        viewData.getContext(),
        CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
        numOfCoeffs * m_numOfTensors[ 0 ] * m_numOfTensors[ 1 ] * sizeof( cl_float ),
        0, &error
    );

    if ( error != CL_SUCCESS )
    {
        return true;
    }

    // load tensors ---------------------------------------------------------------------------------------------------

    if ( updateSlice( viewData, objects, 0 ) )
    {
        return true;
    }

    if ( updateSlice( viewData, objects, 1 ) )
    {
        return true;
    }

    if ( updateSlice( viewData, objects, 2 ) )
    {
        return true;
    }

    // set kernel arguments -------------------------------------------------------------------------------------------

    loadVisibility( viewData, objects );

    cl_float4 numOfTensors = { m_numOfTensors[ 0 ], m_numOfTensors[ 1 ], m_numOfTensors[ 2 ], 0 };

    objects.m_renderKernel.setArg< cl_float4 >( 9, numOfTensors );
    objects.m_renderKernel.setArg< cl::Buffer >( 10, objects.m_sliceX );
    objects.m_renderKernel.setArg< cl::Buffer >( 11, objects.m_sliceY );
    objects.m_renderKernel.setArg< cl::Buffer >( 12, objects.m_sliceZ );

    return false;
}

//---------------------------------------------------------------------------------------------------------------------