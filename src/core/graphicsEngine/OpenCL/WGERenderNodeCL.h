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

#ifndef WGERENDERNODECL_H
#define WGERENDERNODECL_H

#include <vector>

#include <OpenThreads/Atomic>       // NOLINT - he thinks this is a c system header
#include <osg/Array>
#include <osg/buffered_value>
#include <osg/Node>
#include <osg/StateSet>
#include <osg/Texture2D>
#include <osgUtil/RenderBin>
#include <osgUtil/RenderStage>

#include "WGEModuleCL.h"

#include "../WExportWGE.h"

/**
 * WGERenderNodeCL allows for OpenCL rendering. It has to be connected to a WGEModuleCL object
 * that implements the render functionality.
 *
 * \ingroup ge
 */
class WGE_EXPORT WGERenderNodeCL: public osg::Node
{
public:

    /**
     * Standard constructor.
     *
     * \param deactivated Set whether the node should be set to deactivated state.
     */
    WGERenderNodeCL( bool deactivated = false );

    /**
     * Copy construcor.
     *
     * \param node The node to copy.
     * \param copyop The optional OSG copy operator.
     */
    WGERenderNodeCL( const WGERenderNodeCL& node, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY );

    /**
     * Overrides osg::Referenced::setThreadSafeRefUnref().
     *
     * \param threadSafe The thread safe state.
     */
    virtual void setThreadSafeRefUnref( bool threadSafe );

    /**
     * Overrides osg::Object::cloneType().
     *
     * \return The cloned node.
     */
    virtual osg::Object* cloneType() const;

    /**
     * Overrides osg::Object::clone().
     *
     * \param copyop The optional OSG copy operator.
     *
     * \return The cloned node.
     */
    virtual osg::Object* clone( const osg::CopyOp& copyop ) const;

    /**
     * Overrides osg::Object::isSameKindAs().
     *
     * \param object The object to compare with.
     *
     * \return States whether this node and object are of same type.
     */
    virtual bool isSameKindAs( const osg::Object* object ) const;

    /**
     * Overrides osg::Object::libraryName().
     *
     * \return Gives the node's library name.
     */
    virtual const char* libraryName() const;

    /**
     * Overrides osg::Object::className().
     *
     * \return Gives the node's class name.
     */
    virtual const char* className() const;

    /**
     * Overrides osg::Node::accept().
     *
     * \param nv The traversal's NodeVisitor.
     */
    virtual void accept( osg::NodeVisitor& nv );  // NOLINT - non const ref. Needed since derived from OSG

    /**
     * Overrides osg::Node::traverse().
     *
     * \param nv The traversal's NodeVisitor.
     */
    virtual void traverse( osg::NodeVisitor& nv );   // NOLINT - non const ref. Needed since derived from OSG

    /**
     * Overrides osg::Node::computeBound().
     *
     * \return A bounding sphere enclosing the node.
     */
    virtual osg::BoundingSphere computeBound() const;

    /**
     * Overrides osg::Object::resizeGLObjectBuffers().
     *
     * \param maxSize The new buffer size.
     */
    virtual void resizeGLObjectBuffers( unsigned int maxSize );

    /**
     * Overrides osg::Object::releaseGLObjects().
     *
     * \param state The state for which to release all GL objects.
     */
    virtual void releaseGLObjects( osg::State* state = 0 );

    /**
     * Check whether the node is deactivated.
     *
     * \return The activation state.
     */
    bool isDeactivated() const;

    /**
     * Set the activation state of the node. Deactivating the node allows for changing the node's and
     * connected module's data without using callbacks. This method is thread safe and will return after
     * successfully setting the state.
     *
     * \param deactivated The activation state.
     */
    void setDeactivated( bool deactivated ) const;

    /**
     * Get the connected module.
     *
     * \return The connected module or 0.
     */
    WGEModuleCL* getModule();

    /**
     * Get the connected module.
     *
     * \return The connected module or 0.
     */
    const WGEModuleCL* getModule() const;

    /**
     * Connect a module to the node. If the module is already connected to another node it will get disconnected.
     *
     * \param module The module to connect.
     */
    void setModule( WGEModuleCL* module );

    /**
     * Remove the connected module.
     */
    void disconnectModule();

protected:

    /**
     * Destructor.
     */
    virtual ~WGERenderNodeCL();

private:

    class PerContextInformation;
    class CLRenderBin;
    class CLDrawBin;
    class DrawQuad;

    /**
     * Initializes basic CL objects.
     *
     * \param perContextInfo The PerContextInformation object to initialize.
     *
     * \return True if an error occurred, false otherwise.
     */
    bool initCL( PerContextInformation& perContextInfo ) const;     // NOLINT - non const ref. Needed since derived from OSG

    /**
     * Initializes color buffer and depth buffer.
     *
     * \param perContextInfo The PerContextInformation object containing the color buffer and depth buffer.
     * \param state The corresponding GL state.
     *
     * \return True if an error occurred, false otherwise.
     */
    bool initBuffers( PerContextInformation& perContextInfo, osg::State& state ) const;   // NOLINT - non const ref. Needed since derived from OSG

    /**
     * Starts the rendering process.
     *
     * \param state The state.
     * \param mvm The model view matrix.
     * \param pm The projection matrix.
     */
    void render( osg::State& state, osg::RefMatrix* mvm, osg::RefMatrix* pm ) const;   // NOLINT - non const ref. Needed since derived from OSG

    /**
     * Draws the rendered scene.
     *
     * \param state The state.
     */
    void draw( osg::State& state ) const;       // NOLINT - non const ref. Needed since derived from OSG

    /**
     * GL context specific data.
     */
    mutable osg::buffered_object< PerContextInformation > m_perContextInformation;

    /**
     * The color buffer texture.
     */
    mutable osg::ref_ptr< osg::Texture2D > m_colorBufferTex;

    /**
     * The depth buffer texture.
     */
    mutable osg::ref_ptr< osg::Texture2D > m_depthBufferTex;

    /**
     * The activation state.
     */
    mutable bool m_deactivated;

    /**
     * The synch counter.
     */
    mutable OpenThreads::Atomic m_count;

    /**
     * The WGEModuleCL object this node is connected to.
     */
    mutable osg::ref_ptr< WGEModuleCL > m_module;

    /**
     * The draw quad.
     */
    static DrawQuad m_drawQuad;

friend class WGEModuleCL;
};

/**
 * Contains the data that have to be stored for every GL context.
 */
class WGERenderNodeCL::PerContextInformation
{
public:

    /**
     * Constructor.
     */
    PerContextInformation();

    /**
     * Destructor.
     */
    ~PerContextInformation();

    /**
     * Release all data.
     */
    void reset();

    /**
     * The view information and basic CL objects.
     */
    WGEModuleCL::CLViewData m_clViewData;

    /**
     * Module data.
     */
    WGEModuleCL::CLData* m_clData;

    /**
     * Render step invalid.
     */
    bool m_invalid;
};

/**
 * RenderBin rendering the scene.
 */
class WGERenderNodeCL::CLRenderBin: public osgUtil::RenderBin
{
public:

    /**
     * Overrides osgUtil::RenderBin::draw().
     *
     * \param renderInfo The current render information.
     * \param previous The previously rendered RenderLeaf.
     */
    virtual void draw( osg::RenderInfo& renderInfo, osgUtil::RenderLeaf*& previous );  // NOLINT - non const ref. Needed since derived from OSG

    /**
     * Overrides osgUtil::RenderBin::computeNumberOfDynamicRenderLeaves().
     *
     * \return 0.
     */
    virtual unsigned int computeNumberOfDynamicRenderLeaves() const;

    /**
     * Get the current stage's CLRenderBin object.
     *
     * \param stage The current RenderStage.
     *
     * \return The CLRenderBin object.
     */
    static CLRenderBin* getOrCreateRenderBin( osgUtil::RenderStage* stage );

    /**
     * The nodes to draw.
     */
    std::vector< osg::ref_ptr< WGERenderNodeCL > > m_nodes;

    /**
     * The  model view matrices to use.
     */
    std::vector< osg::ref_ptr< osg::RefMatrix > > m_modelViews;

    /**
     * The projection matrices to use.
     */
    std::vector< osg::ref_ptr< osg::RefMatrix > > m_projections;
};

/**
 * RenderBin drawing the rendered scene.
 */
class WGERenderNodeCL::CLDrawBin: public osgUtil::RenderBin
{
public:

    /**
     * Overrides osgUtil::RenderBin::draw().
     *
     * \param renderInfo The current render information.
     * \param previous The previously rendered RenderLeaf.
     */
    virtual void draw( osg::RenderInfo& renderInfo, osgUtil::RenderLeaf*& previous );  // NOLINT - non const ref. Needed since derived from OSG

    /**
     * Overrides osgUtil::RenderBin::computeNumberOfDynamicRenderLeaves().
     *
     * \return The number of dynamic nodes.
     */
    virtual unsigned int computeNumberOfDynamicRenderLeaves() const;

    /**
     * Get the current stage's CLDrawBin object.
     *
     * \param stage The current RenderStage.
     *
     * \return The CLDrawBin object.
     */
    static CLDrawBin* getOrCreateDrawBin( osgUtil::RenderStage* stage );

    /**
     * The nodes to draw.
     */
    std::vector< osg::ref_ptr< WGERenderNodeCL > > m_nodes;
};

/**
 * The draw quad's properties.
 */
class WGERenderNodeCL::DrawQuad
{
public:

    /**
     * Constructor.
     */
    DrawQuad();

    /**
     * The state settings needed for drawing.
     */
    osg::ref_ptr< osg::StateSet > m_drawState;

    /**
     * The quad vertices.
     */
    osg::ref_ptr< osg::Vec2Array > m_vertices;

    /**
     * The quad texture coordinates.
     */
    osg::ref_ptr< osg::Vec2Array > m_coordinates;
};

inline bool WGERenderNodeCL::isDeactivated() const
{
    return m_deactivated;
}

inline WGEModuleCL* WGERenderNodeCL::getModule()
{
    return m_module.get();
}

inline const WGEModuleCL* WGERenderNodeCL::getModule() const
{
    return m_module.get();
}

inline void WGERenderNodeCL::setModule( WGEModuleCL* module )
{
    if ( ( module != 0 ) && ( module != m_module.get() ) )
    {
        if ( module->m_node != 0 )
        {
            module->m_node->disconnectModule();
        }

        if ( m_module.valid() )
        {
            disconnectModule();
        }

        m_module = module;
        m_module->m_node = this;

        dirtyBound();
    }
}

inline void WGERenderNodeCL::disconnectModule()
{
    if ( m_module.valid() )
    {
        m_module->m_node = 0;
        m_module = 0;
    }
}

#endif  // WGERENDERNODECL_H
