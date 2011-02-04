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

#ifndef WGEMODULECL_H
#define WGEMODULECL_H

#if defined( __APPLE__ )
    #include <OpenCL/cl.hpp>
#else
    #include <CL/cl.hpp>    // NOTE: this header can be grabbed from http://www.khronos.org/registry/cl/. Its the c++ binding header.
#endif

#include <vector>

#include <osg/BoundingBox>
#include <osg/Matrix>
#include <osg/Referenced>
#include <osg/State>
#include <osg/Vec3f>

#include "../WExportWGE.h"

class WGERenderNodeCL;

/**
 * WGEModuleCL is an abstract base class for OpenCL rendering. It implements the render functionality and has to be
 * connected to a WGERenderNodeCL object. A WGEModuleCL object may only be associated to a single WGERenderNodeCL
 * object at a time.
 *
 * Derive from this class to integrate your render operations. You just have to write your color and depth data into
 * the color buffer and depth buffer objects respectively, see ViewProperties and CLViewData::getViewProperties().
 *
 * You have to implement:
 *
 *  - initCLData()
 *  - setBuffers()
 *  - render()
 *
 * It is also recommended to reimplement:
 *
 *  - computeBoundingBox()
 *
 * \ingroup ge
 */
class WGE_EXPORT WGEModuleCL: public osg::Referenced
{
public:

    /**
     * Constructor.
     */
    WGEModuleCL();

    /**
     * Call this method after changing your renderable objects to force the recomputation of the bounding box.
     */
    void dirtyBound();

    /**
     * Get the bounding box.
     */
    const osg::BoundingBox& getBound() const;

    /**
     * Get the connected node.
     *
     * \return The node.
     */
    WGERenderNodeCL* getNode();

    /**
     * Get the connected node.
     *
     * \return The connected node or 0.
     */
    const WGERenderNodeCL* getNode() const;

protected:

    class ViewProperties;
    class CLViewData;
    class CLData;
    class CLDataChangeCallback;

    /**
     * Destructor.
     */
    virtual ~WGEModuleCL();

    /**
     * Override this method to initialize your CL program(s), kernel(s), memory objects and other data
     * that are associated with a particular CL context. You may also set static kernel arguments.
     *
     * Return your data in a new CLData object.
     * Return 0 if the creation of your data failed.
     *
     * \param viewData The CLViewData object you may need to initialize your data.
     *
     * \return A CLData object.
     */
    virtual CLData* initCLData( const CLViewData& viewData ) const = 0;

    /**
     * Override this method to set the color buffer and depth buffer kernel arguments.
     * You may also set width and height arguments or resize memory objects that depend
     * on the screen resolution.
     *
     * \param viewData The CLViewData object containing the color buffer and depth buffer.
     * \param data The CLData object containing your data.
     */
    virtual void setBuffers( const CLViewData& viewData, CLData& data ) const = 0;

    /**
     * Override this method to create or change CL memory objects, set kernel arguments (except the color buffer
     * and depth buffer) and execute your kernel(s).
     *
     * The color buffer and depth buffer will always have the approriate size for the rendering cycle.
     * You may use ViewProperties to aquire the necessary view information.
     *
     * \param viewData The CLViewData object needed for rendering.
     * \param data The CLData containing your data.
     *
     * \return True if a CL error occurred, false otherwise.
     */
    virtual bool render( const CLViewData& viewData, CLData& data ) const = 0;

    /**
     * Override this method to create a bounding box around your renderable objects.
     * You should do this to ensure that nothing is being clipped away.
     *
     * \return The bounding box.
     */
    virtual osg::BoundingBox computeBoundingBox() const;

    /**
     * Use this method with an appropriate CLDataChangeCallback if you want to change your CLData objects.
     * Your callback is automatically applied for every GL/CL context instance.
     *
     * \param callback The CLDataChangeCallback to apply.
     */
    void changeCLData( const CLDataChangeCallback& callback );

    /**
     * This method calculates the global work size given a local work size and the number of work items.
     * Assure that the local work size and the number of work items are compatible, otherwise computeGlobalWorkSize()
     * will return an NDRange object of dimension 0.
     *
     * \param localWorkSize The local work size.
     * \param workItems The number of work items.
     *
     * \return The global work size.
     */
    cl::NDRange computeGlobalWorkSize( const cl::NDRange& localWorkSize, const cl::NDRange& workItems ) const;

    /**
     * Projection types.
     */
    enum ProjectionType
    {
        PERSPECTIVE = 0,
        ORTHOGRAPHIC = 1
    };

private:

    /**
     * The WGERenderNodeCL object this module is connected to.
     */
    WGERenderNodeCL* m_node;

    /**
     * Bounding box computed.
     */
    mutable bool m_boundComputed;

    /**
     * Bounding box surrounding the renderable objects.
     */
    mutable osg::BoundingBox m_box;

friend class WGERenderNodeCL;
};

/**
 * Contains view properties. All vectors are given in model space.
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
 */
class WGE_EXPORT WGEModuleCL::ViewProperties
{
public:

    /**
     * Constructor.
     *
     * \param viewData The current CLViewData object.
     */
    explicit ViewProperties( const CLViewData& viewData );

    /**
     * Get the eye point.
     *
     * \return The eye point.
     */
    const cl_float4& getOrigin() const;

    /**
     * Get the vector from the eye point to the lower left corner of the near plane.
     *
     * \return The vector from the eye point to the lower left corner of the near plane.
     */
    const cl_float4& getOriginToLowerLeft() const;

    /**
     * Get the vector along the horizontal edge of the view's near plane.
     *
     * \return The vector along the horizontal edge of the view's near plane.
     */
    const cl_float4& getEdgeX() const;

    /**
     * Get the vector along the vertical edge of the view's near plane.
     *
     * \return The vector along the vertical edge of the view's near plane.
     */
    const cl_float4& getEdgeY() const;

    /**
     * Get the projection's type, either PERSPECTIVE or ORTHOGRAPHIC.
     *
     * \return The projection's type, either PERSPECTIVE or ORTHOGRAPHIC.
     */
    const ProjectionType& getProjectionType() const;

    /**
     * Get the distance from the eye point to the near plane.
     *
     * \return The distance from the eye point to the near plane.
     */
    const cl_float& getNearPlane() const;

    /**
     * Get the distance from the eye point to the far plane.
     *
     * \return The distance from the eye point to the far plane.
     */
    const cl_float& getFarPlane() const;

private:

    /**
     * The eye point.
     */
    cl_float4 m_origin;

    /**
     * The vector from the eye point to the lower left corner of the near plane.
     */
    cl_float4 m_originToLowerLeft;

    /**
     * The vector along the horizontal edge of the view's near plane.
     */
    cl_float4 m_edgeX;

    /**
     * The vector along the vertical edge of the view's near plane.
     */
    cl_float4 m_edgeY;

    /**
     * The projection's type, either PERSPECTIVE or ORTHOGRAPHIC.
     */
    ProjectionType m_type;

    /**
     * The distance from the eye point to the near plane.
     */
    cl_float m_planeNear;

    /**
     * The distance from the eye point to the far plane.
     */
    cl_float m_planeFar;
};

/**
 * Contains the viewport resolution and the CL context, device and command queue, as well as the
 * color buffer and depth buffer. The color buffer and depth buffer are 2D images in CL_RGBA and CL_R format
 * with CL_FLOAT components. Hence depth data are stored within the depth buffer's Red component.
 */
class WGE_EXPORT WGEModuleCL::CLViewData
{
public:

    /**
     * Get the CL context.
     *
     * \return The CL context.
     */
    const cl::Context& getContext() const;

    /**
     * Get the CL device.
     *
     * \return The CL device.
     */
    const cl::Device& getDevice() const;

    /**
     * Get the CL command queue.
     *
     * \return The CL command queue.
     */
    const cl::CommandQueue& getCommQueue() const;

    /**
     * Get the color buffer.
     *
     * \return The color buffer.
     */
    const cl::Image2DGL& getColorBuffer() const;

    /**
     * Get the depth buffer.
     *
     * \return The depth buffer.
     */
    const cl::Image2DGL& getDepthBuffer() const;

    /**
     * Get the viewport's width.
     *
     * \return The viewport's width.
     */
    const cl_uint& getWidth() const;

    /**
     * Get the viewport's height.
     *
     * \return The viewport's height.
     */
    const cl_uint& getHeight() const;

private:

    /**
     * Constructor.
     */
    CLViewData();

    /**
     * The CL context.
     */
    cl::Context m_context;

    /**
     * The CL device.
     */
    cl::Device m_device;

    /**
     * The CL command queue.
     */
    cl::CommandQueue m_commQueue;

    /**
     * The viewport's width.
     */
    cl_uint m_width;

    /**
     * The viewport's height.
     */
    cl_uint m_height;

    /**
     * The color and depth buffer.
     */
    std::vector< cl::Image2DGL > m_buffers;

    /**
     * The current model view matrix.
     */
    osg::ref_ptr< osg::RefMatrix > m_modelView;

    /**
     * The current projection matrix.
     */
    osg::ref_ptr< osg::RefMatrix > m_projection;

friend class ViewProperties;
friend class WGERenderNodeCL;
};

/**
 * Base class to store your CL objects or more precisely all CL context specific data. Derive from it and add your data.
 */
class WGE_EXPORT WGEModuleCL::CLData
{
    // intensionally empty
};

/**
 * Abstract Callback you have to implement if you want to make changes in your CLData objects.
 * It has to be used in conjuntion with changeCLData().
 */
class WGE_EXPORT WGEModuleCL::CLDataChangeCallback      // NOLINT
{
public:

    /**
     * Perform changes.
     *
     * \param viewData The CLViewData object.
     * \param data The CLData object to be changed.
     */
    virtual void change( const CLViewData& viewData, CLData& data ) const = 0;
};

inline const cl_float4& WGEModuleCL::ViewProperties::getOrigin() const
{
    return m_origin;
}

inline const cl_float4& WGEModuleCL::ViewProperties::getOriginToLowerLeft() const
{
    return m_originToLowerLeft;
}

inline const cl_float4& WGEModuleCL::ViewProperties::getEdgeX() const
{
    return m_edgeX;
}

inline const cl_float4& WGEModuleCL::ViewProperties::getEdgeY() const
{
    return m_edgeY;
}

inline const WGEModuleCL::ProjectionType& WGEModuleCL::ViewProperties::getProjectionType() const
{
    return m_type;
}

inline const cl_float& WGEModuleCL::ViewProperties::getNearPlane() const
{
    return m_planeNear;
}

inline const cl_float& WGEModuleCL::ViewProperties::getFarPlane() const
{
    return m_planeFar;
}

inline const cl::Context& WGEModuleCL::CLViewData::getContext() const
{
    return m_context;
}

inline const cl::Device& WGEModuleCL::CLViewData::getDevice() const
{
    return m_device;
}

inline const cl::CommandQueue& WGEModuleCL::CLViewData::getCommQueue() const
{
    return m_commQueue;
}

inline const cl::Image2DGL& WGEModuleCL::CLViewData::getColorBuffer() const
{
    return m_buffers[ 0 ];
}

inline const cl::Image2DGL& WGEModuleCL::CLViewData::getDepthBuffer() const
{
    return m_buffers[ 1 ];
}

inline const unsigned int& WGEModuleCL::CLViewData::getWidth() const
{
    return m_width;
}

inline const unsigned int& WGEModuleCL::CLViewData::getHeight() const
{
    return m_height;
}

inline const osg::BoundingBox& WGEModuleCL::getBound() const
{
    if ( !m_boundComputed )
    {
        m_box = computeBoundingBox();

        m_boundComputed = true;
    }

    return m_box;
}

inline WGERenderNodeCL* WGEModuleCL::getNode()
{
    return m_node;
}

inline const WGERenderNodeCL* WGEModuleCL::getNode() const
{
    return m_node;
}

#endif  // WGEMODULECL_H
