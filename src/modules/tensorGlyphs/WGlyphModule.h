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

#ifndef WGLYPHMODULE_H
#define WGLYPHMODULE_H

//---------------------------------------------------------------------------------------------------------------------

#include <string>

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

#include "../../dataHandler/WDataSetSingle.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../graphicsEngine/OpenCL/WGEModuleCL.h"

//---------------------------------------------------------------------------------------------------------------------

/**
 * WGlyphModule renders high order tensor glyphs.
 *
 * @todo Replace 4-component data by 3-component when OpenCL 1.1 drivers are available. NVIDIA is dilly-dallying around...
 */
class WGlyphModule: public WGEModuleCL
{

public:

    /**
     * Constructor.
     *
     * @param search The path of the kernel file.
     */
    WGlyphModule( const boost::filesystem::path& path );

    /**
     * States whether the source file has been read.
     *
     * @return True if successful, false otherwise.
     */
    bool isSourceRead() const;

    /**
     * Set the new tensor data to render. This method assumes data validity.
     *
     * @param data The tensor data.
     * @param order The tensor order.
     */
    void setTensorData( const boost::shared_ptr< WDataSetSingle >& data, int order );

    /**
     * Set the x-slice position.
     *
     * @param slice The position.
     */
    void setPositionX( int slice );

    /**
     * Set the y-slice position.
     *
     * @param slice The position.
     */
    void setPositionY( int slice );

    /**
     * Set the z-slice position.
     *
     * @param slice The position.
     */
    void setPositionZ( int slice );

    /**
     * Set the slice visibility.
     *
     * @param sliceX The x-slice visibility.
     * @param sliceY The y-slice visibility.
     * @param sliceZ The z-slice visibility.
     */
    void setVisibility( bool sliceX, bool sliceY, bool sliceZ );

protected:

    /**
     * Destructor.
     */
    ~WGlyphModule();

    /**
     * Overrides WGlyphModule::initCLData().
     *
     * @param viewData The CLViewData object you may need to initialize your data.
     *
     * @return A CLData object.
     */
    virtual CLData* initCLData( const CLViewData& viewData ) const;

    /**
     * Overrides WGlyphModule::setBuffers().
     *
     * @param viewData The CLViewData object containing the color buffer and depth buffer.
     * @param data The CLData object containing your data.
     */
    virtual void setBuffers( const CLViewData& viewData, CLData& data ) const;

    /**
     * Overrides WGlyphModule::render().
     *
     * @param viewData The CLViewData object needed for rendering.
     * @param data The CLData containing your data.
     *
     * @return True if a CL error occurred, false otherwise.
     */
    virtual bool render( const CLViewData& viewData, CLData& data ) const;

    /**
     * Overrides WGlyphModule::computeBoundingBox().
     *
     * @return The bounding box.
     */
    virtual osg::BoundingBox computeBoundingBox() const;

private:

    class CLObjects;
    class ChangeCallback;

    /**
     * Set the visibility kernel argument.
     *
     * @param viewData The CLViewData object.
     * @param objects The CL objects.
     *
     * @return True if a CL error occurred, false otherwise.
     */
    void loadVisibility( const CLViewData& viewData, CLObjects& objects ) const;

    /**
     * Updates slice data on GPU memory.
     *
     * @param viewData The CLViewData object.
     * @param objects The CL objects.
     * @param dim The slice dimension.
     *
     * @return True if a CL error occurred, false otherwise.
     */
    bool updateSlice( const CLViewData& viewData, CLObjects& objects, int dim ) const;

    /**
     * Loads the tensor data set to GPU memory and updates kernels.
     *
     * @param viewData The CLViewData object.
     * @param objects The CL objects.
     *
     * @return True if a CL error occurred, false otherwise.
     */
    bool loadDataset( const CLViewData& viewData, CLObjects& objects ) const;

    /**
     * Tensor order.
     */
    int m_order;

    /**
     * Number of tensors in grid's x, y and z direction.
     */
    int m_numOfTensors[ 3 ];

    /**
     * Slice positions.
     */
    int m_slicePosition[ 3 ];

    /**
     * Slice visibility.
     */
    bool m_sliceVisibility[ 3 ];

    /**
     * The tensor data set.
     */
    boost::shared_ptr< WDataSetSingle > m_tensorData;

    /**
     * Local work size.
     */
    cl::NDRange m_localSize;

    /**
     * The kernel source code.
     */
    std::string m_source;

};

//---------------------------------------------------------------------------------------------------------------------

/**
 * CLObjects stores all necessary CL data.
 */
class WGlyphModule::CLObjects: public WGEModuleCL::CLData
{

public:

    /**
     * The CL program containing scaling and rendering kernel.
     */
    cl::Program m_program;

    /**
     * The scaling kernel.
     */
    cl::Kernel m_scaleKernel;

    /**
     * The rendering kernel.
     */
    cl::Kernel m_renderKernel;

    /**
     * The x-slice tensor data.
     */
    cl::Buffer m_sliceX;

    /**
     * The y-slice tensor data.
     */
    cl::Buffer m_sliceY;

    /**
     * The z-slice tensor data.
     */
    cl::Buffer m_sliceZ;

    /**
     * Global work size.
     */
    cl::NDRange m_globalSize;

};

//---------------------------------------------------------------------------------------------------------------------

/**
 * Callback that handles data and visibility changes.
 */
class WGlyphModule::ChangeCallback: public CLDataChangeCallback
{

public:

    /**
     * ChangeOperation describes the sort of operation.
     */
    enum ChangeOperation
    {
        VISIBILITY,
        UPDATE_SLICE,
        LOAD_DATA_SET
    };

    /**
     * Constructor.
     *
     * @param module The module to update.
     * @param operation The operation to execute.
     * @param slice The optional slice dimension to update.
     */
    ChangeCallback( WGlyphModule* module, ChangeOperation operation, int slice = 0 );

    /**
     * Overrides WGEModuleCL::CLDataChangeCallback::change().
     *
     * @param viewData The CLViewData object.
     * @param data The CLData object to be changed.
     */
    virtual void change( const CLViewData& viewData, CLData& data ) const;

    /**
     * The module to update.
     */
    WGlyphModule* m_module;

    /**
     * The module to update.
     */
    ChangeOperation m_operation;

    /**
     * The slice dimension to update.
     */
    int m_slice;

};

//---------------------------------------------------------------------------------------------------------------------

inline bool WGlyphModule::isSourceRead() const
{
    return ( m_source != "" );
}

//---------------------------------------------------------------------------------------------------------------------

inline void WGlyphModule::setTensorData( const boost::shared_ptr< WDataSetSingle >& data, int order )
{
    WGridRegular3D* grid = static_cast< WGridRegular3D* >( data->getGrid().get() );

    m_numOfTensors[ 0 ] = static_cast< int >( grid->getNbCoordsX() );
    m_numOfTensors[ 1 ] = static_cast< int >( grid->getNbCoordsY() );
    m_numOfTensors[ 2 ] = static_cast< int >( grid->getNbCoordsZ() );

    m_slicePosition[ 0 ] = m_numOfTensors[ 0 ] / 2;
    m_slicePosition[ 1 ] = m_numOfTensors[ 1 ] / 2;
    m_slicePosition[ 2 ] = m_numOfTensors[ 2 ] / 2;

    m_sliceVisibility[ 0 ] = true;
    m_sliceVisibility[ 1 ] = true;
    m_sliceVisibility[ 2 ] = true;

    m_tensorData = data;
    m_order = order;

    changeCLData( ChangeCallback( this, ChangeCallback::LOAD_DATA_SET ) );
}

//---------------------------------------------------------------------------------------------------------------------

inline void WGlyphModule::setPositionX( int slice )
{
    m_slicePosition[ 0 ] = slice;

    changeCLData( ChangeCallback( this, ChangeCallback::UPDATE_SLICE, 0 ) );
}

//---------------------------------------------------------------------------------------------------------------------

inline void WGlyphModule::setPositionY( int slice )
{
    m_slicePosition[ 1 ] = slice;

    changeCLData( ChangeCallback( this, ChangeCallback::UPDATE_SLICE, 1 ) );
}

//---------------------------------------------------------------------------------------------------------------------

inline void WGlyphModule::setPositionZ( int slice )
{
    m_slicePosition[ 2 ] = slice;

    changeCLData( ChangeCallback( this, ChangeCallback::UPDATE_SLICE, 2 ) );
}

//---------------------------------------------------------------------------------------------------------------------

inline void WGlyphModule::setVisibility( bool sliceX, bool sliceY, bool sliceZ )
{
    m_sliceVisibility[ 0 ] = sliceX;
    m_sliceVisibility[ 1 ] = sliceY;
    m_sliceVisibility[ 2 ] = sliceZ;

    changeCLData( ChangeCallback( this, ChangeCallback::VISIBILITY ) );
}

//---------------------------------------------------------------------------------------------------------------------

#endif  // WGLYPHMODULE_H