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

#ifndef WCLRENDERNODE_H
#define WCLRENDERNODE_H

#include <string>
#include <vector>

#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>

#include <osg/Array>
#include <osg/BoundingBox>
#include <osg/buffered_value>
#include <osg/Node>
#include <osg/State>
#include <osg/StateSet>
#include <osg/Texture2D>
#include <osgUtil/RenderStage>

#include "../WExportWGE.h"

#if defined (__APPLE__) || defined (MACOSX)
    #include <OpenCL/opencl.h>
  	#include <OpenGL/gl.h>
#else
    #if defined (__linux__)
        #include <CL/cl.h>
        #include <CL/cl_gl.h>
        #include <GL/gl.h>
    #else
        #include <CL/opencl.h>
        #include <GL/gl.h>
    #endif
#endif

/**
 * Abstract base class for OpenCL rendering and integration into OpenSceneGraph.
 *
 * Derive from this class to implement your render functionality. You just have to write your color and depth
 * data into colorBuffer and depthBuffer respectively, see CLViewInformation.
 *
 * You have to implement:
 *
 *  - cloneType()
 *  - clone()
 *  - initProgram()
 *  - setBuffers()
 *  - render()
 *
 * It is recommended to reimplement:
 *
 *  - computeBoundingBox()
 *
 * You may also reimplement:
 *
 *  - isSameKindAs()
 *  - libraryName()
 *  - className()
 *
 * @ingroup ge
 */
class WGE_EXPORT WCLRenderNode: public osg::Node
{
	
public:

    /**
     * Standard constructor.
     *
     * @param deactivated Set whether the node should be set to deactivated state.
     */
    WCLRenderNode( bool deactivated = false );

    /**
     * Copy construcor.
     *
     * @param node The node to copy.
     * @param copyop The optional OSG copy operator.
     */
    WCLRenderNode( const WCLRenderNode& node, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY );

    /**
     * Overrides osg::Object::cloneType().
     *
     * @return The cloned node.
     */
    virtual osg::Object* cloneType() const = 0;

    /**
     * Overrides osg::Object::clone().
     *
     * @param copyop The optional OSG copy operator.
     *
     * @return The cloned node.
     */
    virtual osg::Object* clone( const osg::CopyOp& copyop ) const = 0;

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
     * Overrides osg::Node::traverse().
     *
     * @param nv The traversal's NodeVisitor.
     */
    virtual void traverse( osg::NodeVisitor& nv );

    /**
     * Overrides osg::Node::computeBound().
     *
     * @return A bounding sphere enclosing the node.
     */
    virtual osg::BoundingSphere computeBound() const;

    /**
     * Overrides osg::Object::resizeGLObjectBuffers().
     *
     * @param maxSize The new buffer size.
     */
    virtual void resizeGLObjectBuffers( unsigned int maxSize );

    /**
     * Overrides osg::Object::releaseGLObjects().
     *
     * @param state The state for which to release all GL objects.
     */
    virtual void releaseGLObjects( osg::State* state = 0 );

    /**
     * Override this method to create a bounding box around your renderable objects.
     * You should do this to ensure that nothing is being clipped away. The bounding
     * sphere will encompass this box.
     *
     * @return The bounding box.
     */
    virtual osg::BoundingBox computeBoundingBox() const;

    /**
     * Check whether the initialization for any context failed. If so, you have to invoke reset()
     * to start a new attempt.
     *
     * @return States whether the initialization failed.
     */
    bool initializationFailed() const;

    /**
     * Resets all CL objects of the WCLRenderNode. After calling this method all CL objects will be
     * reinitialized before the next render stage.
     */
    void reset() const;

    /**
     * Activates the node thread safely if it is deactivated.
     */
    void activate() const;

    /**
     * Deactivates the node thread safely, so its data can be changed after calling this method.
     * However the node's data variance still has to be set to DYNAMIC.
     *
     * @note This method may only be invoked if the node is already being rendered or it won't return. Use setDeactivated() otherwise.
     */
    void deactivate() const;

    /**
     * Check whether the node is deactivated.
     *
     * @return The state.
     */
    bool isDeactivated() const;

    /**
     * Set whether the node should be set to deactivated state.
     *
     * @param The state.
     *
     * @note This method mustn't be invoked if the node is already being rendered since it's not thread safe. Use activate() and deactivate() inatead.
     */
    void setDeactivated( bool deactivated ) const;

    /**
     * Returns the string form of a specific CL error. If the error is undefined getCLError returns "".
     *
     * @param clError The CL error.
     *
     * @return The string version of the error.
     */
    static std::string getCLError( cl_int clError );

protected:

    class WGE_EXPORT ViewProperties;
    class WGE_EXPORT CLViewInformation;
    class WGE_EXPORT CLProgramDataSet;
    class WGE_EXPORT CLDataChangeCallback;

private:

    class PerContextInformation;
    class DrawQuad;
    class CLRenderBin;
    class CLDrawBin;

protected:

    /**
     * Destructor.
     */
    virtual ~WCLRenderNode();

    /**
     * The type of a projection.
     */
    enum ProjectionType
    {
        PERSPECTIVE = 0,
        ORTHOGRAPHIC = 1
    };

    /**
     * Contains the view properties. All vectors are given in model space.
     *
     * A ray's direction vector and initial point are given as:
     *
     * Perspective projections:
     *
     *     - @f$ direction = originToLowerLeft + edgeX * (pixel.x / width) + edgeY * (pixel.y / height) @f$
     *     - @f$ inititialPoint = origin @f$
     *
     * Orthographic projections:
     *
     *     - @f$ direction = originToLowerLeft @f$
     *     - @f$ inititialPoint = origin + edgeX * (pixel.x / width) + edgeY * (pixel.y / height) @f$
     *
     * Let @f$ p @f$ be a point to render, @f$ t @f$ be a float and @f$ p = inititialPoint + t * direction @f$.
     * Then the depth value of @f$ p @f$ is calculated as follows:
     *
     * Perspective projections:     @f$ depth = (t - 1) / (planeFar - planeNear) * planeFar / t @f$
     *
     * Orthographic projections:    @f$ depth = (t - 1) / (planeFar - planeNear) * planeNear @f$
     *
     * If @f$ depth < 0 @f$ or @f$ depth > 1 @f$, then the point is outside of the view volume and should be
     * discarded by setting depth to 1.
     */
    class WGE_EXPORT ViewProperties
    {

    public:

        /**
         * Get the eye point.
         */
        const osg::Vec3f& getOrigin() const;

        /**
         * Get the vector from the eye point to the lower left corner of the near plane.
         */
        const osg::Vec3f& getOriginToLowerLeft() const;

        /**
         * Get the vector along the horizontal edge of the view's near plane.
         */
        const osg::Vec3f& getEdgeX() const;

        /**
         * Get the vector along the vertical edge of the view's near plane.
         */
        const osg::Vec3f& getEdgeY() const;

        /**
         * Get the projection's type, either PERSPECTIVE or ORTHOGRAPHIC.
         */
        const ProjectionType& getProjectionType() const;

        /**
         * Get the distance from the eye point to the near plane.
         */
        const float& getNearPlane() const;

        /**
         * Get the distance from the eye point to the far plane.
         */
        const float& getFarPlane() const;

    private:

        /**
         * The eye point.
         */
        osg::Vec3f m_origin;

        /**
         * The vector from the eye point to the lower left corner of the near plane.
         */
        osg::Vec3f m_originToLowerLeft;

        /**
         * The vector along the horizontal edge of the view's near plane.
         */
        osg::Vec3f m_edgeX;

        /**
         * The vector along the vertical edge of the view's near plane.
         */
        osg::Vec3f m_edgeY;

        /**
         * The projection's type, either PERSPECTIVE or ORTHOGRAPHIC.
         */
        ProjectionType m_type;

        /**
         * The distance from the eye point to the near plane.
         */
        float m_planeNear;

        /**
         * The distance from the eye point to the far plane.
         */
        float m_planeFar;

    friend class CLViewInformation;

    };

    /**
     * Contains the viewport resolution and the CL context, device and command queue, as well as the
     * colorBuffer and depthBuffer. colorBuffer and depthBuffer are 2D images in CL_RGBA and CL_R format
     * with CL_FLOAT components. Hence depth data are stored within the depthBuffer's Red component.
     */
    class WGE_EXPORT CLViewInformation
    {

    public:

        /**
         * Constructor.
         */
        CLViewInformation();

        /**
         * The CL context.
         */
        const cl_context& getContext() const;

        /**
         * The CL device.
         */
        const cl_device_id& getDevice() const;

        /**
         * The CL command queue.
         */
        const cl_command_queue& getCommQueue() const;

        /**
         * The color buffer.
         */
        const cl_mem& getColorBuffer() const;

        /**
         * The depth buffer.
         */
        const cl_mem& getDepthBuffer() const;

        /**
         * The view's width.
         */
        const unsigned int& getWidth() const;

        /**
         * The view's height.
         */
        const unsigned int& getHeight() const;

        /**
         * This method gives the view properties.
         *
         * @param properties A ViewProperties object to save the view information.
         */
        void getViewProperties( ViewProperties& properties ) const;

    private:

        /**
         * The CL context.
         */
        cl_context m_context;

        /**
         * The CL device.
         */
        cl_device_id m_device;

        /**
         * The CL command queue.
         */
        cl_command_queue m_commQueue;

        /**
         * The color buffer.
         */
        cl_mem m_colorBuffer;

        /**
         * The depth buffer.
         */
        cl_mem m_depthBuffer;

        /**
         * The view's width.
         */
        unsigned int m_width;

        /**
         * The view's height.
         */
        unsigned int m_height;

        /**
         * The current GL state.
         */
        osg::ref_ptr< osg::State > m_state;

    friend class WCLRenderNode;

    };

    /**
     * Base class to store your CL objects. Derive from it and add your data.
     * Also add a destructor that releases all CL objects if they have been successfully created.
     */
    class WGE_EXPORT CLProgramDataSet
    {

    protected:

        /**
         * Fool protector.
         */
        virtual ~CLProgramDataSet();

    friend class PerContextInformation;

    };

    /**
     * Abstract Callback you have to implement, if you want to change your CL objects.
     * It has to be used in conjuntion with changeDataSet().
     */
    class WGE_EXPORT CLDataChangeCallback
    {

    public:

        /**
         * Change operations.
         *
         * @param clProgramDataSet The CLProgramDataSet to be changed.
         */
        virtual void change( CLProgramDataSet* clProgramDataSet ) const = 0;

    };

    /**
     * Use this method with an appropriate CLDataChangeCallback, if you want to change data concerning
     * your CL objects. Your callback is automatically applied to every per context instance.
     *
     * @param callback The CLDataChangeCallback to apply.
     */
    void changeDataSet( const CLDataChangeCallback& callback );

private:

    /**
     * Override this method to initialize your CL program(s) and kernel(s).
     * You may also create CL memory objects or set static kernel arguments.
     * Return your CL objects in a new CLProgramDataSet.
     * Return 0 if the creation of your CL objects fails.
     *
     * @param clViewInfo The CLViewInformation you may need to create your objects.
     *
     * @return A CLProgramDataSet containing CL objects.
     */
    virtual CLProgramDataSet* initProgram( const CLViewInformation& clViewInfo ) const = 0;

    /**
     * Override this method to set the colorBuffer and depthBuffer kernel arguments.
     * You may also set width and height arguments or resize memory objects that depend
     * on the screen resolution. You should not use this method to set any other kernel
     * arguments or change CL objects.
     *
     * @param clViewInfo The CLViewInformation containing colorBuffer and depthBuffer.
     * @param clProgramDataSet The CLProgramDataSet containing your custom CL objects.
     */
    virtual void setBuffers( const CLViewInformation& clViewInfo, CLProgramDataSet* clProgramDataSet ) const = 0;

    /**
     * Override this method to execute your kernel(s), set kernel arguments (except colorBuffer
     * and depthBuffer) and create or change CL memory objects.
     * ColorBuffer and depthBuffer will always have the approriate size for the rendering cycle.
     * You may use getViewProperties to aquire the necessary view information.
     *
     * @param clViewInfo The CLViewInformation needed for rendering.
     * @param clProgramDataSet The CLProgramDataSet containing your custom CL objects.
     */
    virtual void render( const CLViewInformation& clViewInfo, CLProgramDataSet* clProgramDataSet ) const = 0;

    /**
     * Contains data per GL context.
     */
    class PerContextInformation
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
         * Release all CL objets.
         */
        void reset();

        /**
         * The view information and basic CL objects.
         */
        CLViewInformation m_clViewInfo;

        /**
         * Custom CL objects.
         */
        CLProgramDataSet* m_clProgramDataSet;

        /**
         * Use CL-GL context sharing.
         */
        bool m_contextSharing;

        /**
         * CL initialized.
         */
        bool m_clInitialized;

        /**
         * Buffers initialized.
         */
        bool m_buffersInitialized;

        /**
         * Initialization failed.
         */
        bool m_initializationError;

    };

    /**
     * The draw quad's properties.
     */
    class DrawQuad
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

    /**
     * RenderBin rendering the scene.
     */
    class CLRenderBin: public osgUtil::RenderBin
    {

    public:

        /**
         * Constructor.
         *
         * @param modelView The current model view matrix.
         * @param projection The current projection matrix.
         */
        CLRenderBin( osg::RefMatrix* modelView, osg::RefMatrix* projection );

        /**
         * Overrides osgUtil::RenderBin::draw.
         *
         * @param renderInfo The current render information.
         * @param previous The previously rendered RenderLeaf.
         */
        virtual void draw( osg::RenderInfo& renderInfo, osgUtil::RenderLeaf*& previous );

        /**
         * Overrides osgUtil::RenderBin::computeNumberOfDynamicRenderLeaves.
         *
         * @return 0.
         */
        virtual unsigned int computeNumberOfDynamicRenderLeaves() const;

        /**
         * Get the current CLRenderBin of the stage.
         *
         * @param stage The current RenderStage.
         * @param modelView The current model view matrix.
         * @param projection The current projection matrix.
         */
        static CLRenderBin* getOrCreateRenderBin( osgUtil::RenderStage* stage,
                                                  osg::RefMatrix* modelView,
                                                  osg::RefMatrix* projection );

        /**
         * The WCLRenderNodes to draw.
         */
        std::vector< osg::ref_ptr< WCLRenderNode > > m_nodes;

        /**
         * The model view matrix to use.
         */
        osg::ref_ptr< osg::RefMatrix > m_modelView;

        /**
         * The projection matrix to use.
         */
        osg::ref_ptr< osg::RefMatrix > m_projection;

    };

    /**
     * RenderBin drawing the rendered scene.
     */
    class CLDrawBin: public osgUtil::RenderBin
    {

    public:

        /**
         * Constructor.
         */
        CLDrawBin();

        /**
         * Overrides osgUtil::RenderBin::draw.
         *
         * @param renderInfo The current render information.
         * @param previous The previously rendered RenderLeaf.
         */
        virtual void draw( osg::RenderInfo& renderInfo, osgUtil::RenderLeaf*& previous );

        /**
         * Overrides osgUtil::RenderBin::computeNumberOfDynamicRenderLeaves.
         *
         * @return The number of dynamic WCLRenderNodes.
         */
        virtual unsigned int computeNumberOfDynamicRenderLeaves() const;

        /**
         * Get the current CLDrawBin of the stage.
         *
         * @param stage The current RenderStage.
         */
        static CLDrawBin* getOrCreateDrawBin( osgUtil::RenderStage* stage );

        /**
         * The WCLRenderNodes to draw.
         */
        std::vector< osg::ref_ptr< WCLRenderNode > > m_nodes;

        /**
         * The Number of dynamic WCLRenderNodes.
         */
        unsigned int m_dynamicNodes;

    };

    /**
     * Starts the CL rendering process.
     *
     * @param state The current GL state.
     */
    void renderStart( osg::State& state ) const;

    /**
     * Draws the rendered scene.
     *
     * @param state The current GL state.
     */
    void draw( osg::State& state ) const;

    /**
     * Initializes necessary CL objects.
     *
     * @param perContextInfo The PerContextInformation to initialize.
     */
    bool initCL( PerContextInformation& perContextInfo ) const;

    /**
     * Initializes colorBuffer and depthBuffer.
     *
     * @param perContextInfo The PerContextInformation containing colorBuffer and depthBuffer.
     * @param state The corresponding GL state.
     */
    bool initBuffers( PerContextInformation& perContextInfo,osg::State& state ) const;

    /**
     * Bounding box surrounding renderable objects.
     */
    mutable osg::BoundingBox m_box;

    /**
     * Per context data.
     */
    mutable osg::buffered_object< PerContextInformation > m_perContextInformation;

    /**
     * The colorbuffer texture.
     */
    mutable osg::ref_ptr< osg::Texture2D > m_colorBuffer;

    /**
     * The depthbuffer texture.
     */
    mutable osg::ref_ptr< osg::Texture2D > m_depthBuffer;

    /**
     * Condition for thread safe deactivation.
     */
    mutable boost::condition_variable m_wait;

    /**
     * Mutex for thread safe activation and deactivation.
     */
    mutable boost::mutex m_mutex;

    /**
     * The deactivation state.
     */
    mutable bool m_deactivated;

    /**
     * The draw quad.
     */
    static DrawQuad m_drawQuad;

};

/*-------------------------------------------------------------------------------------------------------------------*/

inline bool WCLRenderNode::isSameKindAs( const osg::Object* obj ) const
{
	return ( dynamic_cast< const WCLRenderNode* >( obj ) != 0 );
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline const char* WCLRenderNode::libraryName() const
{
	return "OWge";
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline const char* WCLRenderNode::className() const
{
	return "WCLRenderNode";
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline const osg::Vec3f& WCLRenderNode::ViewProperties::getOrigin() const
{
    return m_origin;
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline const osg::Vec3f& WCLRenderNode::ViewProperties::getOriginToLowerLeft() const
{
    return m_originToLowerLeft;
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline const osg::Vec3f& WCLRenderNode::ViewProperties::getEdgeX() const
{
    return m_edgeX;
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline const osg::Vec3f& WCLRenderNode::ViewProperties::getEdgeY() const
{
    return m_edgeY;
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline const WCLRenderNode::ProjectionType& WCLRenderNode::ViewProperties::getProjectionType() const
{
    return m_type;
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline const float& WCLRenderNode::ViewProperties::getNearPlane() const
{
    return m_planeNear;
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline const float& WCLRenderNode::ViewProperties::getFarPlane() const
{
    return m_planeFar;
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline const cl_context& WCLRenderNode::CLViewInformation::getContext() const
{
    return m_context;
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline const cl_device_id& WCLRenderNode::CLViewInformation::getDevice() const
{
    return m_device;
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline const cl_command_queue& WCLRenderNode::CLViewInformation::getCommQueue() const
{
    return m_commQueue;
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline const cl_mem& WCLRenderNode::CLViewInformation::getColorBuffer() const
{
    return m_colorBuffer;
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline const cl_mem& WCLRenderNode::CLViewInformation::getDepthBuffer() const
{
    return m_depthBuffer;
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline const unsigned int& WCLRenderNode::CLViewInformation::getWidth() const
{
    return m_width;
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline const unsigned int& WCLRenderNode::CLViewInformation::getHeight() const
{
    return m_height;
}

/*-------------------------------------------------------------------------------------------------------------------*/

#endif  // WCLRENDERNODE_H
