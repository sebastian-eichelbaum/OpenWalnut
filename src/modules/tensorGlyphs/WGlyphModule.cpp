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

#include <fstream>

#include <boost/lexical_cast.hpp>

#include <osg/Notify>

#include "../../dataHandler/WGridRegular3D.h"
#include "../../dataHandler/WValueSet.h"

#include "WGlyphModule.h"

//---------------------------------------------------------------------------------------------------------------------

WGlyphModule::ChangeCallback::ChangeCallback( WGlyphModule* module, ChangeOperation operation ):
    m_module( module ),
    m_operation( operation )
{
    // initialize
}

void WGlyphModule::ChangeCallback::change( const CLViewData &viewData, CLData &data ) const
{
    CLObjects& objects = static_cast< CLObjects& >( data );

    switch ( m_operation )
    {
        case UPDATE_SLICEX: m_module->updateSlice( viewData, objects, 0 );
                            m_module->loadVisibility( &objects );

                            return;

        case UPDATE_SLICEY: m_module->updateSlice( viewData, objects, 1 );
                            m_module->loadVisibility( &objects );

                            return;

        case UPDATE_SLICEZ: m_module->updateSlice( viewData, objects, 2 );
                            m_module->loadVisibility( &objects );

                            return;

        case VISIBILITY:    m_module->loadVisibility( &objects );

                            return;

        case SET_COLORING:  objects.m_renderKernel.setArg< cl_int >( 14, m_module->m_mode );

                            return;

        case LOAD_DATA_SET: objects.m_scaleKernel = cl::Kernel();
                            objects.m_renderKernel = cl::Kernel();

                            m_module->loadDataset( viewData, objects );

                            return;
    }
}

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

WGlyphModule::~WGlyphModule()
{
    // shutdown
}

void WGlyphModule::setTensorData( const boost::shared_ptr< WDataSetSingle >& data, int order, int mode )
{
    getNode()->setDeactivated( true );

    WGridRegular3D* grid = static_cast< WGridRegular3D* >( data->getGrid().get() );

    m_order = order;
    m_mode = mode;

    m_numOfTensors[ 0 ] = grid->getNbCoordsX();
    m_numOfTensors[ 1 ] = grid->getNbCoordsY();
    m_numOfTensors[ 2 ] = grid->getNbCoordsZ();

    m_slicePosition[ 0 ] = m_numOfTensors[ 0 ] / 2;
    m_slicePosition[ 1 ] = m_numOfTensors[ 1 ] / 2;
    m_slicePosition[ 2 ] = m_numOfTensors[ 2 ] / 2;

    m_sliceVisibility[ 0 ] = true;
    m_sliceVisibility[ 1 ] = true;
    m_sliceVisibility[ 2 ] = true;

    m_tensorData = data;

    m_sliceData[ 0 ] = extractSlice( 0 );
    m_sliceData[ 1 ] = extractSlice( 1 );
    m_sliceData[ 2 ] = extractSlice( 2 );

    dirtyBound();

    changeCLData( ChangeCallback( this, ChangeCallback::LOAD_DATA_SET ) );

    getNode()->setDeactivated( false );
}

void WGlyphModule::setPosition( int slice, int coord )
{
    boost::shared_array< cl_float > sliceData = extractSlice( slice );

    getNode()->setDeactivated( true );

    m_slicePosition[ slice ] = coord;

    m_sliceData[ slice ] = sliceData;

    changeCLData( ChangeCallback( this, static_cast< ChangeCallback::ChangeOperation >( slice ) ) );

    getNode()->setDeactivated( false );
}

void WGlyphModule::setVisibility( bool sliceX, bool sliceY, bool sliceZ )
{
    getNode()->setDeactivated( true );

    m_sliceVisibility[ 0 ] = sliceX;
    m_sliceVisibility[ 1 ] = sliceY;
    m_sliceVisibility[ 2 ] = sliceZ;

    changeCLData( ChangeCallback( this, ChangeCallback::VISIBILITY ) );

    getNode()->setDeactivated( false );
}

void WGlyphModule::setColoring( int mode )
{
    getNode()->setDeactivated( true );

    m_mode = mode;

    changeCLData( ChangeCallback( this, ChangeCallback::SET_COLORING ) );

    getNode()->setDeactivated( false );
}

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

    CLObjects* objects = new CLObjects();

    objects->m_program = program;

    // create kernels and load data to GPU memory ---------------------------------------------------------------------

    if ( loadDataset( viewData, *objects ) )
    {
        delete objects;

        return 0;
    }

    return objects;
}

void WGlyphModule::setBuffers( const CLViewData& viewData, CLData& data ) const
{
    CLObjects& objects = static_cast< CLObjects& >( data );

    // set static kernel arguments ------------------------------------------------------------------------------------

    objects.m_renderKernel.setArg< cl_uint >( 6, viewData.getWidth() );
    objects.m_renderKernel.setArg< cl_uint >( 7, viewData.getHeight() );
    objects.m_renderKernel.setArg< cl::Image2DGL >( 12, viewData.getColorBuffer() );
    objects.m_renderKernel.setArg< cl::Image2DGL >( 13, viewData.getDepthBuffer() );

    // calculate global work size which has to be a multiple of local work size ---------------------------------------

    objects.m_globalSize = computeGlobalWorkSize( m_localSize, cl::NDRange( viewData.getWidth(), viewData.getHeight() ) );
}

bool WGlyphModule::render( const CLViewData& viewData, CLData& data ) const
{
    CLObjects& objects = static_cast< CLObjects& >( data );

    // get view properties --------------------------------------------------------------------------------------------

    ViewProperties props( viewData );

    // set kernel view arguments --------------------------------------------------------------------------------------

    objects.m_renderKernel.setArg< cl_float4 >( 0, props.getOrigin() );
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

osg::BoundingBox WGlyphModule::computeBoundingBox() const
{
    osg::BoundingBox box
    (
        osg::Vec3( 0.0f, 0.0f, 0.0f ),
        osg::Vec3( m_numOfTensors[ 0 ], m_numOfTensors[ 1 ], m_numOfTensors[ 2 ] )
    );

    return box;
}

boost::shared_array< cl_float > WGlyphModule::extractSlice( int dim )
{
    int dim2 = ( dim + 1 ) % 3;
    int dim3 = ( dim + 2 ) % 3;

    int numOfCoeffs = ( m_order + 1 ) * ( m_order + 2 ) / 2;

    int factors[ 3 ] = { 1, m_numOfTensors[ 0 ], m_numOfTensors[ 0 ] * m_numOfTensors[ 1 ] };

    factors[ 0 ] *= numOfCoeffs;
    factors[ 1 ] *= numOfCoeffs;
    factors[ 2 ] *= numOfCoeffs;

    boost::shared_array< cl_float > slice( new cl_float[ numOfCoeffs * m_numOfTensors[ dim2 ] * m_numOfTensors[ dim3 ] ] );

    int i = 0;

    for ( int id3 = 0; id3 < m_numOfTensors[ dim3 ]; id3++ )
    {
        for ( int id2 = 0; id2 < m_numOfTensors[ dim2 ]; id2++ )
        {
            int dataCoord = m_slicePosition[ dim ] * factors[ dim ] + id2 * factors[ dim2 ] + id3 * factors[ dim3 ];

            for ( int j = 0; j < numOfCoeffs; j++ )
            {
                slice.get()[ i + j ] = static_cast< cl_float >( m_tensorData->getValueAt( dataCoord + j ) );
            }

            i += numOfCoeffs;
        }
    }

    return slice;
}

void WGlyphModule::loadVisibility( CLObjects* objects ) const
{
    cl_float4 slicePosition =
    {
        {
            m_sliceVisibility[ 0 ] ? m_slicePosition[ 0 ] : CL_INFINITY,
            m_sliceVisibility[ 1 ] ? m_slicePosition[ 1 ] : CL_INFINITY,
            m_sliceVisibility[ 2 ] ? m_slicePosition[ 2 ] : CL_INFINITY,
            0.0f
        }
    };

    objects->m_renderKernel.setArg< cl_float4 >( 8, slicePosition );
}

bool WGlyphModule::updateSlice( const CLViewData& viewData, CLObjects& objects, int dim ) const
{
    cl_int error;

    int dim2 = ( dim + 1 ) % 3;
    int dim3 = ( dim + 2 ) % 3;

    // load new tensor data -------------------------------------------------------------------------------------------

    int numOfCoeffs = ( m_order + 1 ) * ( m_order + 2 ) / 2;

    error = viewData.getCommQueue().enqueueWriteBuffer
    (
        objects.m_slices[ dim ], CL_TRUE, 0,
        numOfCoeffs * m_numOfTensors[ dim2 ] * m_numOfTensors[ dim3 ] * sizeof( cl_float ),
        m_sliceData[ dim ].get()
    );

    if ( error != CL_SUCCESS )
    {
        return true;
    }

    // scale new tensors ----------------------------------------------------------------------------------------------

    cl::NDRange localSize( 256 );
    cl::NDRange bufferSize( m_numOfTensors[ dim2 ] * m_numOfTensors[ dim3 ] );
    cl::NDRange globalSize = computeGlobalWorkSize( localSize, bufferSize );

    objects.m_scaleKernel.setArg< cl::Buffer >( 0, objects.m_slices[ dim ] );
    objects.m_scaleKernel.setArg< cl_uint >( 1, bufferSize[ 0 ] );

    error = viewData.getCommQueue().enqueueNDRangeKernel( objects.m_scaleKernel, cl::NDRange(), globalSize, localSize );

    if ( error != CL_SUCCESS )
    {
        return true;
    }

    return false;
}

bool WGlyphModule::loadDataset( const CLViewData& viewData, CLObjects& objects ) const
{
    cl_int error;

    // build program --------------------------------------------------------------------------------------------------

    std::string options = "-D Order=" + boost::lexical_cast< std::string, int >( m_order ) + " " +
                          "-D NumOfTensors=(float4)(" +
                          boost::lexical_cast< std::string, int >( m_numOfTensors[ 0 ] ) + "," +
                          boost::lexical_cast< std::string, int >( m_numOfTensors[ 1 ] ) + "," +
                          boost::lexical_cast< std::string, int >( m_numOfTensors[ 2 ] ) + "," +
                          "0)";

    error = objects.m_program.build( std::vector< cl::Device >( 1, viewData.getDevice() ), options.c_str() );

    if ( error != CL_SUCCESS )
    {
        #ifdef DEBUG

            if ( error == CL_BUILD_PROGRAM_FAILURE )
            {
                std::string log;

                objects.m_program.getBuildInfo< std::string >( viewData.getDevice(), CL_PROGRAM_BUILD_LOG, &log );

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

    // load data set --------------------------------------------------------------------------------------------------

    int numOfCoeffs = ( m_order + 1 ) * ( m_order + 2 ) / 2;

    for ( int dim = 0; dim < 3; dim++ )
    {
        int dim2 = ( dim + 1 ) % 3;
        int dim3 = ( dim + 2 ) % 3;

        // create new buffers -----------------------------------------------------------------------------------------

        objects.m_slices[ dim ] = cl::Buffer
        (
            viewData.getContext(),
            CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
            numOfCoeffs * m_numOfTensors[ dim2 ] * m_numOfTensors[ dim3 ] * sizeof( cl_float ),
            0, &error
        );

        if ( error != CL_SUCCESS )
        {
            return true;
        }

        // load tensors -----------------------------------------------------------------------------------------------

        if ( updateSlice( viewData, objects, dim ) )
        {
            return true;
        }
    }

    // set kernel arguments -------------------------------------------------------------------------------------------

    loadVisibility( &objects );

    objects.m_renderKernel.setArg< cl::Buffer >( 9, objects.m_slices[ 0 ] );
    objects.m_renderKernel.setArg< cl::Buffer >( 10, objects.m_slices[ 1 ] );
    objects.m_renderKernel.setArg< cl::Buffer >( 11, objects.m_slices[ 2 ] );
    objects.m_renderKernel.setArg< cl_int >( 14, m_mode );

    return false;
}

