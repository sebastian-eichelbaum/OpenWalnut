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

#include <string>

// Use filesystem version 2 for compatibility with newer boost versions.
#ifndef BOOST_FILESYSTEM_VERSION
    #define BOOST_FILESYSTEM_VERSION 2
#endif
#include <boost/filesystem.hpp>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>

#include "../../dataHandler/WDataSetSingle.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../graphicsEngine/OpenCL/WGEModuleCL.h"
#include "../../graphicsEngine/OpenCL/WGERenderNodeCL.h"

/**
 * WGlyphModule renders high order tensor glyphs.
 *
 * \todo Replace 4-component data by 3-component when OpenCL 1.1 drivers are available. NVIDIA is dilly-dallying around...
 */
class WGlyphModule: public WGEModuleCL
{
public:

    /**
     * Constructor.
     *
     * \param path The path of the kernel file.
     */
    explicit WGlyphModule( const boost::filesystem::path& path );

    /**
     * States whether the source file has been read.
     *
     * \return True if successful, false otherwise.
     */
    bool isSourceRead() const;

    /**
     * Set the new tensor data to render. This method assumes data validity.
     *
     * \param data The tensor data.
     * \param order The tensor order.
     * \param mode The coloring mode.
     */
    void setTensorData( const boost::shared_ptr< WDataSetSingle >& data, int order, int mode );

    /**
     * Set a slice position.
     *
     * \param slice The slice dimension.
     * \param coord The position.
     */
    void setPosition( int slice, int coord );

    /**
     * Set the slice visibility.
     *
     * \param sliceX The x-slice visibility.
     * \param sliceY The y-slice visibility.
     * \param sliceZ The z-slice visibility.
     */
    void setVisibility( bool sliceX, bool sliceY, bool sliceZ );

    /**
     * Set the coloring mode.
     *
     * \param mode The coloring mode.
     */
    void setColoring( int mode );

protected:

    /**
     * Destructor.
     */
    ~WGlyphModule();

    /**
     * Overrides WGlyphModule::initCLData().
     *
     * \param viewData The CLViewData object you may need to initialize your data.
     *
     * \return A CLData object.
     */
    virtual CLData* initCLData( const CLViewData& viewData ) const;

    /**
     * Overrides WGlyphModule::setBuffers().
     *
     * \param viewData The CLViewData object containing the color buffer and depth buffer.
     * \param data The CLData object containing your data.
     */
    virtual void setBuffers( const CLViewData& viewData, CLData& data ) const;

    /**
     * Overrides WGlyphModule::render().
     *
     * \param viewData The CLViewData object needed for rendering.
     * \param data The CLData containing your data.
     *
     * \return True if a CL error occurred, false otherwise.
     */
    virtual bool render( const CLViewData& viewData, CLData& data ) const;

    /**
     * Overrides WGlyphModule::computeBoundingBox().
     *
     * \return The bounding box.
     */
    virtual osg::BoundingBox computeBoundingBox() const;

private:

    class CLObjects;
    class ChangeCallback;

    /**
     * Extract a slice out of the complete data set.
     *
     * \param dim The slice dimension.
     *
     * \return The slice data.
     */
    boost::shared_array< cl_float > extractSlice( int dim );

    /**
     * Set the visibility kernel argument.
     *
     * \param objects The CL objects.
     *
     * \return True if a CL error occurred, false otherwise.
     */
    void loadVisibility( CLObjects* objects ) const;

    /**
     * Updates slice data on GPU memory.
     *
     * \param viewData The CLViewData object.
     * \param objects The CL objects.
     * \param dim The slice dimension.
     *
     * \return True if a CL error occurred, false otherwise.
     */
    bool updateSlice( const CLViewData& viewData, CLObjects& objects, int dim ) const;

    /**
     * Loads the tensor data set to GPU memory and updates kernels.
     *
     * \param viewData The CLViewData object.
     * \param objects The CL objects.
     *
     * \return True if a CL error occurred, false otherwise.
     */
    bool loadDataset( const CLViewData& viewData, CLObjects& objects ) const;

    /**
     * Tensor order.
     */
    int m_order;

    /**
     * The coloring mode.
     */
    int m_mode;

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
     * The slice data.
     */
    boost::shared_array< cl_float > m_sliceData[ 3 ];

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
     * The slice tensor data.
     */
    cl::Buffer m_slices[ 3 ];

    /**
     * Global work size.
     */
    cl::NDRange m_globalSize;
};

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
        UPDATE_SLICEX = 0,
        UPDATE_SLICEY = 1,
        UPDATE_SLICEZ = 2,
        VISIBILITY,
        SET_COLORING,
        LOAD_DATA_SET
    };

    /**
     * Constructor.
     *
     * \param module The module to update.
     * \param operation The operation to execute.
     */
    ChangeCallback( WGlyphModule* module, ChangeOperation operation );

    /**
     * Overrides WGEModuleCL::CLDataChangeCallback::change().
     *
     * \param viewData The CLViewData object.
     * \param data The CLData object to be changed.
     */
    virtual void change( const CLViewData& viewData, CLData& data ) const;

    /**
     * The module to update.
     */
    WGlyphModule* m_module;

    /**
     * The operation to execute.
     */
    ChangeOperation m_operation;
};

inline bool WGlyphModule::isSourceRead() const
{
    return ( m_source != "" );
}

#endif  // WGLYPHMODULE_H
