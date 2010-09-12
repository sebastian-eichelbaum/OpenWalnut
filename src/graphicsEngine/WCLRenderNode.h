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

#ifndef CLRENDER_H
#define CLRENDER_H

#include <string>
#include <vector>

#include <osg/Array>
#include <osg/BoundingBox>
#include <osg/buffered_value>
#include <osg/Node>
#include <osg/State>
#include <osg/StateSet>
#include <osg/Texture2D>
#include <osgUtil/RenderStage>

#include "WExportWGE.h"

#if defined (__APPLE__) || defined (MACOSX)
    #include <OpenCL/opencl.h>
	#include <OpenGL/gl.h>
#else
    #include <CL/opencl.h>
	#include <GL/gl.h>
#endif

/**
*	Abstract base class for OpenCL rendering and integration into OpenSceneGraph.
*
*	Derive from this class to implement your render functionality. You just have to write your color and depth
*	data into colorBuffer and depthBuffer respectively, see CLViewInformation.
*
*	You have to implement:
*
*	cloneType()
*	clone()
*	initProgram()
*	setBuffers()
*	render()
*
*	It is also recommended to implement:
*
*	isSameKindAs()
*	libraryName()
*	className()
*	computeBoundingBox()
*/
class WGE_EXPORT WCLRenderNode: public osg::Node
{
	public:

		/**
		*	Standard constructor.
		*/
		WCLRenderNode();

		/**
		*	Copy construcor.
		*/
		WCLRenderNode(const WCLRenderNode& node,const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

		/**
		*	Overrides Object::cloneType.
		*/
		virtual osg::Object* cloneType() const = 0;

		/**
		*	Overrides Object::clone.
		*/
		virtual osg::Object* clone(const osg::CopyOp& copyop) const = 0;

		/**
		*	Overrides Object::isSameKindAs.
		*/
		virtual bool isSameKindAs(const osg::Object* obj) const;

		/**
		*	Overrides Object::libraryName.
		*/
        virtual const char* libraryName() const;

		/**
		*	Overrides Object::className.
		*/
        virtual const char* className() const;

		/**
		*	Overrides Node::traverse.
		*/
		virtual void traverse(osg::NodeVisitor& nv);

		/**
		*	Overrides Node::computeBound.
		*/
		virtual osg::BoundingSphere computeBound() const;

		/**
		*	Overrides Object::resizeGLObjectBuffers.
		*/
		virtual void resizeGLObjectBuffers(unsigned int maxSize);

		/**
		*	Overrides Object::releaseGLObjects.
		*/
		virtual void releaseGLObjects(osg::State* state = 0);

		/**
		*	Override this method to create a bounding box around your renderable objects.
		*	You should do this to ensure that nothing is being clipped away.
		*/
		virtual osg::BoundingBox computeBoundingBox() const;

		/**
		*	Check whether the initialization for any context failed. If so, you have to invoke reset()
		*	to start a new attempt.
		*/
		bool initializationFailed() const;

		/**
		*	Resets all CL objects of the WCLRenderNode. After calling this method all CL objects will be
		*	reinitialized before the next render stage.
		*/
		void reset() const;

		/**
		*	Returns the string form of a specific CL error. If the error is undefined getCLError returns "".
		*/
		static std::string getCLError(cl_int clError);

	protected:

		/**
		*	Destructor.
		*/
		virtual ~WCLRenderNode();

		/**
		*	The type of a projection, see CLViewInformation::getViewProperties.
		*/
		enum ProjectionType
		{
			PERSPECTIVE = 0,
			ORTHOGRAPHIC = 1
		};

		/**
		*	Contains view properties, see CLViewInformation::getViewProperties.
		*/
		class WGE_EXPORT ViewProperties
		{
			public:

				osg::Vec3f origin;
				osg::Vec3f origin2LowerLeft;
				osg::Vec3f edgeX;
				osg::Vec3f edgeY;

				ProjectionType type;

				float planeNear;
				float planeFar;
		};

		/**
		*	Contains the viewport resolution and the CL context, device and command queue, as well as the
		*	colorBuffer and depthBuffer. colorBuffer and depthBuffer are 2D images in CL_RGBA and CL_R format
		*	with CL_FLOAT components. Hence depth data are stored within the depthBuffer's Red component.
		*/
		class WGE_EXPORT CLViewInformation
		{
			public:

				/**
				*	Constructor.
				*/
				CLViewInformation();

				/**
				*	The CL context.
				*/
				cl_context clContext;

				/**
				*	The CL device.
				*/
				cl_device_id clDevice;

				/**
				*	The CL command queue.
				*/
				cl_command_queue clCommQueue;

				/**
				*	The color buffer.
				*/
				cl_mem colorBuffer;

				/**
				*	The depth buffer.
				*/
				cl_mem depthBuffer;

				/**
				*	The view's width.
				*/
				unsigned int width;

				/**
				*	The view's height.
				*/
				unsigned int height;

				/**
				*	This method gives the following view properties:
				*
				*		type:		the projection's type, either PERSPECTIVE or ORTHOGRAPHIC
				*		edgeX:		a vector along the horizontal edge of the view's near plane in model space
				*		edgeY:		a vector along the vertical edge of the view's near plane in model space
				*		planeNear:	the distance from the eye point to the near plane
				*		planeNear:	the distance from the eye point to the far plane
				*
				*		perspective projections:
				*
				*		origin:				the eye point in model space
				*		origin2LowerLeft:	a vector from the eye point to the lower left corner of the near
				*							plane in model space
				*
				*		orthographic projections:
				*
				*		origin:				the eye point in model space moved parallel along the near plane to
				*							its lower left corner
				*		origin2LowerLeft:	a vector from the origin to the lower left corner of the near plane
				*							in model space
				*
				*	A ray's direction vector and initial point are given as:
				*
				*		perspective projections:
				*
				*		direction = origin2LowerLeft + edgeX * (pixel.x / width) + edgeY * (pixel.y / height)
				*		inititialPoint = origin
				*
				*		orthographic projections:
				*
				*		direction = origin2LowerLeft
				*		inititialPoint = origin + edgeX * (pixel.x / width) + edgeY * (pixel.y / height)
				*
				*	Let p be a point to render and t be a float, so that p = direction * t + inititialPoint.
				*	Then the depth value of p is calculated as follows:
				*
				*	perspective projections:	depth = (t - 1) / (planeFar - planeNear) * planeFar / t
				*	orthographic projections:	depth = (t - 1) / (planeFar - planeNear) * planeNear
				*
				*	If depth < 0 or depth > 1, then the point is outside of the view volume and should be
				*	discarded by setting depth to 1.
				*/
				void getViewProperties(ViewProperties& properties) const;

			private:

				/**
				*	The current model view matrix.
				*/
				const osg::Matrix* m_modelViewMatrix;

				/**
				*	The current projection matrix.
				*/
				const osg::Matrix* m_projectionMatrix;

			friend class WCLRenderNode;
		};

	private:

		class WGE_EXPORT PerContextInformation;

	protected:

		/**
		*	Base class to store your CL objects. Derive from it and add your data.
		*	Also add a destructor that releases all CL objects if they have been successfully created.
		*/
		class WGE_EXPORT CLProgramDataSet
		{
			protected:

				/**
				*	Fool protector.
				*/
				virtual ~CLProgramDataSet();

			friend class PerContextInformation;
		};

		/**
		*	Abstract Callback you have to implement, if you want to change your CL objects.
		*	It has to be used in conjuntion with changeDataSet().
		*/
		class WGE_EXPORT CLDataChangeCallback
		{
			public:

				/**
				*	Change operations.
				*/
				virtual void change(CLProgramDataSet* clProgramDataSet) const = 0;
		};

		/**
		*	Use this method with an appropriate DataChangeCallback, if you want to change data concerning
		*	your CL objects. Your callback is automatically applied to every per context instance.
		*/
		void changeDataSet(const CLDataChangeCallback& callback);
		
	private:

		/**
		*	Override this method to initialize your CL program(s) and kernel(s).
		*	You may also create CL memory objects or set static kernel arguments. 
		*	Return your CL objects in a new CLProgramDataSet.
		*	Return 0 if the creation of your CL objects fails.
		*/
		virtual CLProgramDataSet* initProgram(const CLViewInformation& clViewInfo) const = 0;

		/**
		*	Override this method to set the colorBuffer and depthbuffer kernel arguments.
		*	You may also set width and height arguments. Do not use this method to set any other kernel
		*	arguments or change CL objects.
		*/
		virtual void setBuffers(const CLViewInformation& clViewInfo,CLProgramDataSet* clProgramDataSet) const = 0;

		/**
		*	Override this method to execute your kernel(s), set kernel arguments (except colorBuffer
		*	and depthBuffer) and create or change CL memory objects.
		*	ColorBuffer and depthBuffer will always have the approriate size for the rendering cycle.
		*	You may use getViewProperties to aquire the necessary view information.
		*/
		virtual void render(const CLViewInformation& clViewInfo,CLProgramDataSet* clProgramDataSet) const = 0;

		/**
		*	Contains data per GL context.
		*/
		class WGE_EXPORT PerContextInformation
		{
			public:

				/**
				*	Constructor.
				*/
				PerContextInformation();

				/**
				*	Destructor.
				*/
				~PerContextInformation();

				/**
				*	Release all CL objets.
				*/
				void reset();

				/**
				*	Release clProgramDataSet.
				*/
				void resetProgramDataSet();

				/**
				*	The view information and basic CL objects.
				*/
				CLViewInformation clViewInfo;

				/**
				*	Custom CL objects.
				*/
				CLProgramDataSet* clProgramDataSet;

				/**
				*	Use CL-GL context sharing.
				*/
				bool contextSharing;

				/**
				*	CL initialized.
				*/
				bool clInitialized;

				/**
				*	Buffers initialized.
				*/
				bool buffersInitialized;

				/**
				*	Initialization failed.
				*/
				bool initializationError;
		};

		/**
		*	The draw quad's properties.
		*/
		class DrawQuad
		{
			public:

				/**
				*	Initialize the quad.
				*/
				static void initQuad();

				/**
				*	Release the quad data.
				*/
				static void releaseQuad();

				/**
				*	The state settings needed for drawing.
				*/
				osg::ref_ptr<osg::StateSet> drawState;

				/**
				*	The quad vertices.
				*/
				osg::ref_ptr<osg::Vec2Array> vertices;

				/**
				*	The quad texture coordinates.
				*/
				osg::ref_ptr<osg::Vec2Array> coordinates;
		};

		/**
		*	RenderBin rendering the scene.
		*/
		class CLRenderBin: public osgUtil::RenderBin
		{
			public:

				/**
				*	Constructor.
				*/
				CLRenderBin(osg::RefMatrix* modelView,osg::RefMatrix* projection);

				/**
				*	Overrides RenderBin::draw.
				*/
				virtual void draw(osg::RenderInfo& renderInfo,osgUtil::RenderLeaf*& previous);

				/**
				*	Overrides RenderBin::computeNumberOfDynamicRenderLeaves.
				*/
				virtual unsigned int computeNumberOfDynamicRenderLeaves() const;

				/**
				*	Get the current CLRenderBin of the stage.
				*/
				static CLRenderBin* getOrCreateRenderBin(osgUtil::RenderStage* stage,
														 osg::RefMatrix* modelView,
														 osg::RefMatrix* projection);

				/**
				*	The WCLRenderNodes to draw.
				*/
				std::vector<osg::ref_ptr<WCLRenderNode>> nodes;

				/**
				*	The model view matrix to use.
				*/
				osg::ref_ptr<osg::RefMatrix> modelView;

				/**
				*	The projection matrix to use.
				*/
				osg::ref_ptr<osg::RefMatrix> projection;
		};

		/**
		*	RenderBin drawing the rendered scene.
		*/
		class CLDrawBin: public osgUtil::RenderBin
		{
			public:

				/**
				*	Constructor.
				*/
				CLDrawBin();

				/**
				*	Overrides RenderStage::draw.
				*/
				virtual void draw(osg::RenderInfo& renderInfo,osgUtil::RenderLeaf*& previous);

				/**
				*	Overrides RenderBin::computeNumberOfDynamicRenderLeaves.
				*/
				virtual unsigned int computeNumberOfDynamicRenderLeaves() const;

				/**
				*	Get the current CLDrawBin of the stage.
				*/
				static CLDrawBin* getOrCreateDrawBin(osgUtil::RenderStage* stage);

				/**
				*	The WCLRenderNodes to draw.
				*/
				std::vector<osg::ref_ptr<WCLRenderNode>> nodes;

				/**
				*	The Number of dynamic WCLRenderNodes.
				*/
				unsigned int dynamicNodes;
		};

		/**
		*	Starts the CL rendering process.
		*/
		void renderStart(osg::State& state) const;

		/**
		*	Draws the rendered scene.
		*/
		void draw(osg::State& state) const;

		/**
		*	Initializes necessary CL objects.
		*/
		bool initCL(PerContextInformation& perContextInfo) const;

		/**
		*	Initializes colorBuffer and depthBuffer.
		*/
		bool initBuffers(PerContextInformation& perContextInfo,osg::State& state) const;

		/**
		*	Bounding box surrounding renderable objects.
		*/
		mutable osg::BoundingBox m_box;

		/**
		*	Per context data.
		*/
		mutable osg::buffered_object<PerContextInformation> m_perContextInformation;

		/**
		*	The colorbuffer texture.
		*/
		mutable osg::ref_ptr<osg::Texture2D> m_colorBuffer;

		/**
		*	The depthbuffer texture.
		*/
		mutable osg::ref_ptr<osg::Texture2D> m_depthBuffer;

		/**
		*	The draw quad.
		*/
		static DrawQuad m_drawQuad;
};

/*-------------------------------------------------------------------------------------------------------------------*/

inline bool WCLRenderNode::isSameKindAs(const osg::Object* obj) const
{
	return (dynamic_cast<const WCLRenderNode*>(obj) != 0);
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline const char* WCLRenderNode::libraryName() const
{
	return "osgCL";
}

/*-------------------------------------------------------------------------------------------------------------------*/

inline const char* WCLRenderNode::className() const
{
	return "WCLRenderNode";
}

/*-------------------------------------------------------------------------------------------------------------------*/

#endif