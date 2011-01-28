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

#ifndef WGLYPHRENDERNODE_H
#define WGLYPHRENDERNODE_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "../../dataHandler/WDataSetSingle.h"
#include "../../graphicsEngine/OpenCL/WCLRenderNode.h"

/**
 * This node type renders high order tensor glyphs.
 *
 * @todo Replace 4-component data by 3-component when OpenCL 1.1 drivers are available. NVIDIA is dilly-dallying around...
 */
class WGlyphRenderNode: public WCLRenderNode
{

public:

    /**
     * Constructor.
     *
     * @param search The path of the kernel file.
     * @param deactivated Set whether the node should be set to deactivated state.
     */
    WGlyphRenderNode( const boost::filesystem::path& search, bool deactivated );

    /**
     * Copy constructor.
     */
    WGlyphRenderNode( const WGlyphRenderNode& wGlyphRenderNode, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY );

     /**
     * Overrides osg::Object::cloneType().
     *
     * @return The cloned node.
     */
    virtual osg::Object* cloneType() const;

    /**
     * Overrides osg::Object::clone().
     *
     * @param copyop The optional OSG copy operator.
     *
     * @return The cloned node.
     */
    virtual osg::Object* clone( const osg::CopyOp& copyop ) const;

    /**
     * Overrides osg::Object::isSameKindAs().
     *
     * @param obj The object to compare with.
     *
     * @return States whether this node and obj are of same type.
     */
    virtual bool isSameKindAs( const osg::Object* obj ) const;

    /**
     * Overrides osg::Object::libraryName().
     *
     * @return Gives this node's library name.
     */
    virtual const char* libraryName() const;

    /**
     * Overrides osg::Object::className().
     *
     * @return Gives this node's class name.
     */
    virtual const char* className() const;

    /**
     * Overrides WCLRenderNode::computeBoundingBox().
     *
     * @return The bounding box.
     */
    virtual osg::BoundingBox computeBoundingBox() const;

    /**
     * Has the kernel source code successfully been read?
     *
     * @return States whether the source file has been read.
     */
    bool isSourceRead() const;

    /**
     * Set the new tensor data to render. This method assumes data validity.
     *
     * @param data The tensor data.
     * @param order The tensor order.
     */
    void setTensorData( const boost::shared_ptr< WDataSetSingle >& data, const int& order );

    /**
     * Set the x-slice position.
     *
     * @param slice The position.
     */
    void setPositionX( const int& slice );

    /**
     * Set the y-slice position.
     *
     * @param slice The position.
     */
    void setPositionY( const int& slice );

    /**
     * Set the z-slice position.
     *
     * @param slice The position.
     */
    void setPositionZ( const int& slice );

    /**
     * Set the x-slice visibility.
     *
     * @param slice The visibility.
     */
    void setVisibilityX( const bool& slice );

    /**
     * Set the y-slice visibility.
     *
     * @param slice The visibility.
     */
    void setVisibilityY( const bool& slice );

    /**
     * Set the z-slice visibility.
     *
     * @param slice The visibility.
     */
    void setVisibilityZ( const bool& slice );

protected:

    /**
     * Destructor.
     */
    virtual ~WGlyphRenderNode();

private:

    /**
     * Callback to mark data for reload.
     */
    class ReloadCallback: public CLDataChangeCallback
    {

    public:

        /**
         * Constructor.
         *
         * @param dim The dimension to set.
         */
        ReloadCallback( int dim );

        /**
         * Overrides WCLRenderNode::CLDataChangeCallback::change().
         *
         * @param clProgramDataSet The CLProgramDataSet to be changed.
         */
        virtual void change( CLProgramDataSet* clProgramDataSet ) const;

        /**
         * The dimension. What else?
         */
        int m_dimension;

    };

    /**
     * Callback to set slice visibility.
     */
    class ChangeCallback: public CLDataChangeCallback
    {

    public:

        /**
         * Overrides WCLRenderNode::CLDataChangeCallback::change().
         *
         * @param clProgramDataSet The CLProgramDataSet to be changed.
         */
        virtual void change( CLProgramDataSet* clProgramDataSet ) const;

    };

    /**
     * Derives from WCLRenderNode::CLProgramDataSet.
     */
    class CLObjects: public CLProgramDataSet
    {

    public:

        /**
         * Constructor.
         */
        CLObjects();

        /**
         * Destructor.
         */
        ~CLObjects();

        /**
         * The CL program containing scaling and rendering kernel.
         */
        cl_program m_program;

        /**
         * The scaling kernel.
         */
        cl_kernel m_scaleKernel;

        /**
         * The rendering kernel.
         */
        cl_kernel m_renderKernel;

        /**
         * The x-slice tensor data.
         */
        cl_mem m_sliceX;

        /**
         * The y-slice tensor data.
         */
        cl_mem m_sliceY;

        /**
         * The z-slice tensor data.
         */
        cl_mem m_sliceZ;

        /**
         * Data have already been loaded.
         */
        bool m_dataLoaded;

        /**
         * X slice has to be reloaded.
         */
        bool m_reloadX;

        /**
         * Y slice has to be reloaded.
         */
        bool m_reloadY;

        /**
         * Z slice has to be reloaded.
         */
        bool m_reloadZ;

        /**
         * Slice visibility has to be set.
         */
        bool m_setVisibility;

        /**
         * Global work size.
         */
        size_t m_gws[ 2 ];

    };

    /**
     * Overrides WCLRenderNode::initProgram().
     *
     * @param clViewInfo The CLViewInformation.
     *
     * @return A CLProgramDataSet containing CL objects.
     */
    virtual CLProgramDataSet* initProgram( const CLViewInformation& clViewInfo ) const;

    /**
     * Overrides WCLRenderNode::setBuffers().
     *
     * @param clViewInfo The CLViewInformation.
     * @param clProgramDataSet The CLProgramDataSet.
     */
    virtual void setBuffers( const CLViewInformation& clViewInfo, CLProgramDataSet* clProgramDataSet ) const;

    /**
     * Overrides WCLRenderNode::render().
     *
     * @param clViewInfo The CLViewInformation.
     * @param clProgramDataSet The CLProgramDataSet.
     */
    virtual void render( const CLViewInformation& clViewInfo, CLProgramDataSet* clProgramDataSet ) const;

    /**
     * Loads slice data to GPU memory.
     *
     * @param clViewInfo The CLViewInformation.
     * @param clObjects The CL objects.
     * @param slice The slice object.
     * @param dim The slice dimension.
     *
     * @return States whether there have been errors.
     */
    bool loadSlice( const CLViewInformation& clViewInfo, CLObjects& objects, cl_mem& slice, int dim ) const;

    /**
     * Updates the data on GPU memory.
     *
     * @param clViewInfo The CLViewInformation.
     * @param clObjects The CL objects.
     *
     * @return States whether there have been errors.
     */
    bool updateData( const CLViewInformation& clViewInfo, CLObjects& objects ) const;

    /**
     * Tensor order.
     */
    int m_order;

    /**
     * Number of tensors in grid's x, y and z direction.
     */
    int m_numOfTensors[ 4 ];

    /**
     * Slice positions.
     */
    int m_slicePosition[ 4 ];

    /**
     * Slice visibility.
     */
    int m_sliceVisibility[ 4 ];

    /**
     * The tensor data set.
     */
    boost::shared_ptr< WDataSetSingle > m_tensorData;

    /**
     * The kernel source code.
     */
    std::string m_kernelSource;

    /**
     * Has the kernel source code successfully been read?
     */
    mutable bool m_sourceRead;

    /**
     * Local work size.
     */
    static size_t m_lws[ 2 ];

};

/*-------------------------------------------------------------------------------------------------------------------*/

inline osg::Object* WGlyphRenderNode::cloneType() const
{
    return 0;
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline osg::Object* WGlyphRenderNode::clone( const osg::CopyOp& copyop ) const
{
    return new WGlyphRenderNode( *this, copyop );
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline bool WGlyphRenderNode::isSameKindAs( const osg::Object* obj ) const
{
    return ( dynamic_cast< const WGlyphRenderNode* >( obj ) != 0 );
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline const char* WGlyphRenderNode::libraryName() const
{
    return "tensorGlyphs";
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline const char* WGlyphRenderNode::className() const
{
    return "WGlyphRenderNode";
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline bool WGlyphRenderNode::isSourceRead() const
{
    return m_sourceRead;
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline void WGlyphRenderNode::setTensorData( const boost::shared_ptr< WDataSetSingle >& data, const int& order )
{
    WGridRegular3D* grid = static_cast< WGridRegular3D* >( data->getGrid().get() );

    m_numOfTensors[ 0 ] = grid->getNbCoordsX();
    m_numOfTensors[ 1 ] = grid->getNbCoordsY();
    m_numOfTensors[ 2 ] = grid->getNbCoordsZ();

    m_tensorData = data;
    m_order = order;
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline void WGlyphRenderNode::setPositionX( const int& slice )
{
    m_slicePosition[ 0 ] = slice;

    changeDataSet( ReloadCallback( 0 ) );
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline void WGlyphRenderNode::setPositionY( const int& slice )
{
    m_slicePosition[ 1 ] = slice;

    changeDataSet( ReloadCallback( 1 ) );
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline void WGlyphRenderNode::setPositionZ( const int& slice )
{
    m_slicePosition[ 2 ] = slice;

    changeDataSet( ReloadCallback( 2 ) );
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline void WGlyphRenderNode::setVisibilityX( const bool& slice )
{
    m_sliceVisibility[ 0 ] = slice ? 1 : 0;

    changeDataSet( ChangeCallback() );
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline void WGlyphRenderNode::setVisibilityY( const bool& slice )
{
    m_sliceVisibility[ 1 ] = slice ? 1 : 0;

    changeDataSet( ChangeCallback() );
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline void WGlyphRenderNode::setVisibilityZ( const bool& slice )
{
    m_sliceVisibility[ 2 ] = slice ? 1 : 0;

    changeDataSet( ChangeCallback() );
}

/*-------------------------------------------------------------------------------------------------------------------*/

#endif  // WGLYPHRENDERNODE_H
