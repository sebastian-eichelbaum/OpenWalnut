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

#ifndef WGlyphRenderNode_H
#define WGlyphRenderNode_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "../../dataHandler/WDataSetSingle.h"
#include "../../graphicsEngine/OpenCL/WCLRenderNode.h"

/**
 * A Node for rendering high order tensor glyphs with OpenCL.
 */
class WGlyphRenderNode: public WCLRenderNode
{

public:

    /**
     * Constructor.
     *
     * @param data The tensor data.
     * @param order The tensor order.
     * @param sliceX The x slice to display.
     * @param sliceY The y slice to display.
     * @param sliceZ The z slice to display.
     * @param enabledX States whether the x slice is to be drawn.
     * @param enabledY States whether the y slice is to be drawn.
     * @param enabledZ States whether the z slice is to be drawn.
     * @param path The path of the kernel file.
     */
    WGlyphRenderNode(const boost::shared_ptr< WDataSetSingle >& data,const int& order,
                     const int& sliceX, const int& sliceY, const int& sliceZ,
                     const bool& enabledX,const bool& enabledY,const bool& enabledZ,
                     const boost::filesystem::path& search);

    /**
     * Copy constructor.
     */
    WGlyphRenderNode(const WGlyphRenderNode& WGlyphRenderNode,const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

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
    virtual osg::Object* clone(const osg::CopyOp& copyop) const;

    /**
     * Overrides osg::Object::isSameKindAs().
     *
     * @param copyop The optional OSG copy operator.
     *
     * @return States whether this node and obj are of same type.
     */
    virtual bool isSameKindAs(const osg::Object* obj) const;

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
     * Overrides osg::Node::traverse().
     *
     * @param nv The traversal's node visitor.
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
     * @param sliceX The x slice to display.
     * @param sliceY The y slice to display.
     * @param sliceZ The z slice to display.
     * @param enabledX States whether the x slice is to be drawn.
     * @param enabledY States whether the y slice is to be drawn.
     * @param enabledZ States whether the z slice is to be drawn.
     */
    void setTensorData(const boost::shared_ptr< WDataSetSingle >& data,const int& order,
                       const int& sliceX, const int& sliceY, const int& sliceZ,
                       const bool& enabledX,const bool& enabledY,const bool& enabledZ);

    /**
     * Set the slice properties to render.
     *
     * @param sliceX The x slice to display.
     * @param sliceY The y slice to display.
     * @param sliceZ The z slice to display.
     * @param enabledX States whether the x slice is to be drawn.
     * @param enabledY States whether the y slice is to be drawn.
     * @param enabledZ States whether the z slice is to be drawn.
     */
    void setSlices(const int& sliceX, const int& sliceY, const int& sliceZ,
                   const bool& enabledX,const bool& enabledY,const bool& enabledZ);

protected:

    /**
     * Destructor.
     */
    virtual ~WGlyphRenderNode();

private:

    /**
     * Callback used to update the data thread-safe without removing and readding the glyph render
     * object from the scene graph.
     */
    class DataChangeCallback: public osg::NodeCallback
    {

    public:

        /**
         * Constructor.
         */
        DataChangeCallback();

        /**
         * Overrides osg::NodeCallback::operator().
         *
         * @param node The node to change.
         * @param nv The traversal's NodeVisitor.
         */
        virtual void operator()(osg::Node* node,osg::NodeVisitor* nv);

        /**
         * Data or slices changed.
         */
        bool m_changed;

        /**
         * Data changed.
         */
        bool m_dataChanged;

        /**
         * The new data's tensor order.
         */
        int m_order;

        /**
         * The new slice positions.
         */
        int m_slices[3];

        /**
         * The new slice status.
         */
        int m_sliceEnabled[3];

        /**
         * The new tensor data.
         */
        boost::shared_ptr< WDataSetSingle > m_tensorData;
    };

    /**
     * Callback used to update the CL objects.
     */
    class ReloadCallback: public CLDataChangeCallback
    {

    public:

        /**
         * Overrides WCLRenderNode::CLDataChangeCallback::change().
         *
         * @param clProgramDataSet The CLProgramDataSet to be changed.
         */
        virtual void change(CLProgramDataSet* clProgramDataSet) const;
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
        cl_program clProgram;

        /**
         * The scaling kernel.
         */
        cl_kernel clScaleKernel;

        /**
         * The rendering kernel.
         */
        cl_kernel clRenderKernel;

        /**
         * The tensor data.
         */
        cl_mem tensorData;

        /**
         * The multinomial factors.
         */
        cl_mem factors;

        /**
         * Data has to be reloaded.
         */
        bool reloadData;
    };

    /**
     * Overrides WCLRenderNode::initProgram().
     *
     * @param clViewInfo The CLViewInformation.
     *
     * @return A CLProgramDataSet containing CL objects.
     */
    virtual CLProgramDataSet* initProgram(const CLViewInformation& clViewInfo) const;

    /**
     * Overrides WCLRenderNode::setBuffers().
     *
     * @param clViewInfo The CLViewInformation.
     * @param clProgramDataSet The CLProgramDataSet.
     */
    virtual void setBuffers(const CLViewInformation& clViewInfo,CLProgramDataSet* clProgramDataSet) const;

    /**
     * Overrides WCLRenderNode::render().
     *
     * @param clViewInfo The CLViewInformation.
     * @param clProgramDataSet The CLProgramDataSet.
     */
    virtual void render(const CLViewInformation& clViewInfo,CLProgramDataSet* clProgramDataSet) const;

    /**
     * Loads new data to GPU memory.
     *
     * @param clViewInfo The CLViewInformation needed for data loading.
     * @param clObjects The CL objects.
     */
    bool loadCLData(const CLViewInformation& clViewInfo,CLObjects& clObjects) const;

    /**
     * Tensor order.
     */
    int m_order;

    /**
     * Number of tensors in grid's x, y and z direction.
     */
    int m_numOfTensors[3];

    /**
     * Slice positions.
     */
    int m_slices[3];

    /**
     * Show slices.
     */
    int m_sliceEnabled[3];

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
};

/*-------------------------------------------------------------------------------------------------------------------*/

inline osg::Object* WGlyphRenderNode::cloneType() const
{
    return 0;
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline osg::Object* WGlyphRenderNode::clone(const osg::CopyOp& copyop) const
{
    return new WGlyphRenderNode(*this,copyop);
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline bool WGlyphRenderNode::isSameKindAs(const osg::Object* obj) const
{
    return dynamic_cast< const WGlyphRenderNode* >(obj) != 0;
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

#endif
