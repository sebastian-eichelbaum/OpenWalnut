#ifndef OPENCLRENDER_H
#define OPENCLRENDER_H

#include <osg/buffered_value>
#include <osg/Drawable>
#include <string>

#if defined (__APPLE__) || defined (MACOSX)
    #include <OpenCL/opencl.h>
	#include <OpenGL/gl.h>
#else
    #include <CL/opencl.h>
	#include <GL/gl.h>
#endif

class PerContextGLObjects;

/**
*	Abstract base class for OpenCL rendering and integration into OpenSceneGraph. OpenCLRender diplays objects, rendered by OpenCL,
*	in an OSG scene. Derive from this class to implement your render functionality. You just have to care to write your color and depth
*	data into colorBuffer and depthBuffer respectively. Since this is a Drawable, it has to be attached to a Geode.
*/
class OpenCLRender: public osg::Drawable
{
	public:

		/**
		*	Standard constructor.
		*/
		OpenCLRender();

		/**
		*	Copy construcor.
		*/
		OpenCLRender(const OpenCLRender& openCLRender,const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

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
		*	Overrides Drawable::drawImplementation.
		*/
		void drawImplementation(osg::RenderInfo& renderInfo) const;

		/**
		*	Overrides Object::resizeGLObjectBuffers.
		*/
		virtual void resizeGLObjectBuffers(unsigned int maxSize);

		/**
		*	Resets GL and CL objects of the OpenCLRender object. This method has to be called whenever you destroy a GL context,
		*	that has been used to render an OpenCLRender object and you plan to reuse it with one or more new contexts.
		*	After calling this method all CL objects will be reinitialized during the first render cycle.
		*	Recompiling a CL kernel will always take some time dependig on its code size.
		*/
		void reset() const;

		/**
		*	Check whether the initialization for any context failed. If so, you have to invoke reset() to start a new attempt.
		*/
		bool initializationFailed() const;

	protected:

		/**
		*	Destructor.
		*/
		virtual ~OpenCLRender();

		/**
		*	The type of a projection, see getViewProperties.
		*/
		enum ProjectionType
		{
			PERSPECTIVE = 0,
			ORTHOGRAPHIC = 1
		};

		/**
		*	Contains view properties, see getViewProperties.
		*/
		class ViewProperties
		{
			public:

				osg::Vec3 origin;
				osg::Vec3 origin2LowerLeft;
				osg::Vec3 edgeX;
				osg::Vec3 edgeY;
				ProjectionType type;
				float planeNear;
				float planeFar;
		};

		/**
		*	Contains the viewport resolution and the CL context, device and command queue, as well as the colorBuffer
		*	and depthBuffer. colorBuffer and depthBuffer both are 2D images in CL_RGBA format with CL_FLOAT components.
		*	Depth data are stored within the depthBuffer's Alpha component.
		*/
		class CLViewInformation
		{
			public:

				CLViewInformation();

				cl_context clContext;
				cl_device_id clDevice;
				cl_command_queue clCommQueue;

				cl_mem colorBuffer;
				cl_mem depthBuffer;

				unsigned int width;
				unsigned int height;
		};

		/**
		*	Basic class to store your CL objects. Derive from it and add your data. Also add a destructor that releases all
		*	CL objects if they have been successfully created.
		*/
		class CLProgramDataSet {};

		/**
		*	This Function gives the following view properties:
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
		*		origin2LowerLeft:	a vector from the eye point to the lower left corner of the near plane
		*							in model space
		*
		*		orthographic projections:
		*
		*		origin:				the eye point in model space moved parallel along the near plane to its
		*							lower left corner
		*		origin2LowerLeft:	a vector from the origin to the lower left corner of the near plane
		*							in model space
		*
		*	A ray's direction vector and initial point are given as:
		*
		*		perspective projections:
		*
		*		direction = origin2LowerLeft + edgeX * (get_global_id(0) / width) + edgeY * (get_global_id(1) / height)
		*		inititialPoint = origin
		*
		*		orthographic projections:
		*
		*		direction = origin2LowerLeft
		*		inititialPoint = origin + edgeX * (get_global_id(0) / width) + edgeY * (get_global_id(1) / height)
		*
		*	Let p be a point to render and t be a float, so that p = ray(t) = direction * t + inititialPoint.
		*	Then the depth value of p is calculated as follows:
		*
		*	perspective projections:	depth = (t - 1) / (planeFar - planeNear) * planeFar / t
		*	orthographic projections:	depth = (t - 1) / (planeFar - planeNear) * planeNear
		*
		*	If depth < 0 or depth > 1, then the point is outside of the view volume and should be discarded
		*	by setting depth to 1.
		*/
		void getViewProperties(ViewProperties& properties,const osg::State& state) const;
		
	private:

		/**
		*	Override this method to initialize your CL program(s) and kernel(s). You may also create static CL memory objects or set
		*	static kernel arguments. 
		*	Return your Cl objects in a new CLProgramDataSet. Return 0 if the creation of your CL objects fails.
		*/
		virtual CLProgramDataSet* initProgram(const CLViewInformation& clViewInfo) const = 0;

		/**
		*	Override this method to set the colorBuffer and depthbuffer kernel arguments. You should not use this method to set any
		*	other kernel arguments.
		*/
		virtual void setBuffers(const CLViewInformation& clViewInfo,CLProgramDataSet* clProgramDataSet) const = 0;

		/**
		*	Override this method to execute your kernel(s), set kernel arguments (except colorBuffer and depthBuffer) and create or
		*	change CL memory objects. colorBuffer and depthBuffer will always have the approriate size for the rendering cycle.
		*	You may use getViewProperties to aquire the necessary view information.
		*/
		virtual void render(const CLViewInformation& clViewInfo,CLProgramDataSet* clProgramDataSet,const osg::State& state) const = 0;

		/**
		*	Contains per GL context data.
		*/
		class PerContextInformation
		{
			public:

				PerContextInformation();

				virtual ~PerContextInformation();

				void reset();

				CLViewInformation clViewInfo;

				CLProgramDataSet* clProgramDataSet;

				bool contextSharing;
				bool cglInitialized;
				bool buffersInitialized;
				bool initializationError;

				GLuint colorTexture;
				GLuint depthTexture;
		};

		/**
		*	Initializes necessary GL objects.
		*/
		bool initGL(PerContextInformation& perContextInfo,PerContextGLObjects& perContextGLObjects) const;

		/**
		*	Initializes necessary CL objects.
		*/
		bool initCL(PerContextInformation& perContextInfo) const;

		/**
		*	Initializes colorBuffer and depthBuffer.
		*/
		void initBuffers(PerContextInformation& perContextInfo) const;

		mutable osg::buffered_object<PerContextInformation> perContextInformation;
};

inline bool OpenCLRender::isSameKindAs(const osg::Object* obj) const
{
	return dynamic_cast<const OpenCLRender*>(obj) != 0;
}

inline const char* OpenCLRender::libraryName() const
{
	return "osgOpenCLRendering";
}

inline const char* OpenCLRender::className() const
{
	return "OpenCLRender";
}

/**
*	Returns the string form of a specific CL error. If the error is unknown getCLError returns "".
*/
std::string getCLError(cl_int clError);

#endif