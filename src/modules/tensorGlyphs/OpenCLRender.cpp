#include "OpenCLRender.h"

#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>
#include <osg/GLExtensions>
#include <osg/Matrix>
#include <osg/Notify>

#if defined (__APPLE__) || defined (MACOSX)
	#include <Carbon/Carbon.h>
#else
	#ifdef UNIX
		#include <GL/glx.h>
	#else
		#include <windows.h>
	#endif
#endif

#if defined (__APPLE__) || defined (MACOSX)
	#define GL_SHARING_EXTENSION "cl_APPLE_gl_sharing"
#else
	#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
#endif

#ifndef APIENTRY
	#define APIENTRY
#endif

#define GL_ARRAY_BUFFER 0x8892
#define GL_STATIC_DRAW 0x88E4

#define GL_ALPHA32F 0x8816
#define GL_RGBA32F 0x8814

typedef ptrdiff_t GLsizeiptr;

/**
*	Helper class to access necessary GL extensions.
*/
class GLRenderExtensions
{
	public:

		/**
		*	Standard constructor.
		*/
		GLRenderExtensions();

		/**
		*	Initialize the extensions.
		*/
		bool initExtensions();

		/**
		*	Check for Initialization.
		*/
		bool isInitialized();

		/**
		*	Reset the extensions.
		*/
		void reset();

		void genBuffers(GLsizei n,GLuint* buffers);
		void bindBuffer(GLenum target,GLuint buffer);
		void bufferData(GLenum target,GLsizeiptr size,const GLvoid* data,GLenum usage);
		void deleteBuffers(GLsizei n,const GLuint* buffers);

		void attachShader(GLuint program,GLuint shader);
		void compileShader(GLuint shader);
		GLuint createProgram();
		GLuint createShader(GLenum type);
		void deleteProgram(GLuint program);
		void deleteShader(GLuint shader);
		void detachShader(GLuint program,GLuint shader);
		GLint getUniformLocation(GLuint program,const GLchar* name);
		void linkProgram(GLuint program);
		void shaderSource(GLuint shader,GLsizei count,const GLchar** string,const GLint* length);
		void uniform1i(GLint location,GLint v0);
		void useProgram(GLuint program);

	private:

		typedef void (APIENTRY * GenBuffersProc)(GLsizei n,GLuint* buffers);
		typedef void (APIENTRY * BindBufferProc)(GLenum target,GLuint buffer);
		typedef void (APIENTRY * BufferDataProc)(GLenum target,GLsizeiptr size,const GLvoid* data,GLenum usage);
		typedef void (APIENTRY * DeleteBuffersProc)(GLsizei n,const GLuint* buffers);

		typedef void (APIENTRY * AttachShaderProc)(GLuint program,GLuint shader);
		typedef void (APIENTRY * CompileShaderProc)(GLuint shader);
		typedef GLuint (APIENTRY * CreateProgramProc)();
		typedef GLuint (APIENTRY * CreateShaderProc)(GLenum type);
		typedef void (APIENTRY * DeleteProgramProc)(GLuint program);
		typedef void (APIENTRY * DeleteShaderProc)(GLuint shader);
		typedef void (APIENTRY * DetachShaderProc)(GLuint program,GLuint shader);
		typedef GLint (APIENTRY * GetUniformLocationProc)(GLuint program,const GLchar* name);
		typedef void (APIENTRY * LinkProgramProc)(GLuint program);
		typedef void (APIENTRY * ShaderSourceProc)(GLuint shader,GLsizei count,const GLchar** string,const GLint* length);
		typedef void (APIENTRY * Uniform1iProc)(GLint location,GLint v0);
		typedef void (APIENTRY * UseProgramProc)(GLuint program);

		GenBuffersProc glGenBuffers;
		BindBufferProc glBindBuffer;
		BufferDataProc glBufferData;
		DeleteBuffersProc glDeleteBuffers;

		AttachShaderProc glAttachShader;
        CompileShaderProc glCompileShader;
        CreateProgramProc glCreateProgram;
        CreateShaderProc glCreateShader;
        DeleteProgramProc glDeleteProgram;
        DeleteShaderProc glDeleteShader;
        DetachShaderProc glDetachShader;
		GetUniformLocationProc glGetUniformLocation;
		LinkProgramProc glLinkProgram;
		ShaderSourceProc glShaderSource;
		Uniform1iProc glUniform1i;
		UseProgramProc glUseProgram;

		bool initialized;
};

GLRenderExtensions::GLRenderExtensions(): initialized(false) {}

bool GLRenderExtensions::initExtensions()
{
	if (initialized) return true;

	if (!osg::setGLExtensionFuncPtr(glGenBuffers,"glGenBuffers")) return false;
	if (!osg::setGLExtensionFuncPtr(glBindBuffer,"glBindBuffer")) return false;
	if (!osg::setGLExtensionFuncPtr(glBufferData,"glBufferData")) return false;
	if (!osg::setGLExtensionFuncPtr(glDeleteBuffers,"glDeleteBuffers")) return false;

	if (!osg::setGLExtensionFuncPtr(glAttachShader,"glAttachShader")) return false;
	if (!osg::setGLExtensionFuncPtr(glCompileShader,"glCompileShader")) return false;
	if (!osg::setGLExtensionFuncPtr(glCreateProgram,"glCreateProgram")) return false;
	if (!osg::setGLExtensionFuncPtr(glCreateShader,"glCreateShader")) return false;
	if (!osg::setGLExtensionFuncPtr(glDeleteProgram,"glDeleteProgram")) return false;
	if (!osg::setGLExtensionFuncPtr(glDeleteShader,"glDeleteShader")) return false;
	if (!osg::setGLExtensionFuncPtr(glDetachShader,"glDetachShader")) return false;
	if (!osg::setGLExtensionFuncPtr(glGetUniformLocation,"glGetUniformLocation")) return false;
	if (!osg::setGLExtensionFuncPtr(glLinkProgram,"glLinkProgram")) return false;
	if (!osg::setGLExtensionFuncPtr(glShaderSource,"glShaderSource")) return false;
	if (!osg::setGLExtensionFuncPtr(glUniform1i,"glUniform1i")) return false;
	if (!osg::setGLExtensionFuncPtr(glUseProgram,"glUseProgram")) return false;

	initialized = true;

	return true;
}

inline void GLRenderExtensions::genBuffers(GLsizei n,GLuint* buffers)
{
	glGenBuffers(n,buffers);
}

inline void GLRenderExtensions::bindBuffer(GLenum target,GLuint buffer)
{
	glBindBuffer(target,buffer);
}

inline void GLRenderExtensions::bufferData(GLenum target,GLsizeiptr size,const GLvoid* data,GLenum usage)
{
	glBufferData(target,size,data,usage);
}

inline void GLRenderExtensions::deleteBuffers(GLsizei n,const GLuint* buffers)
{
	glDeleteBuffers(n,buffers);
}

inline void GLRenderExtensions::attachShader(GLuint program,GLuint shader)
{
	glAttachShader(program,shader);
}

inline void GLRenderExtensions::compileShader(GLuint shader)
{
	glCompileShader(shader);
}

inline GLuint GLRenderExtensions::createProgram()
{
	return glCreateProgram();
}

inline GLuint GLRenderExtensions::createShader(GLenum type)
{
	return glCreateShader(type);
}

inline void GLRenderExtensions::deleteProgram(GLuint program)
{
	glDeleteProgram(program);
}

inline void GLRenderExtensions::deleteShader(GLuint shader)
{
	glDeleteShader(shader);
}

inline void GLRenderExtensions::detachShader(GLuint program,GLuint shader)
{
	glDetachShader(program,shader);
}

inline GLint GLRenderExtensions::getUniformLocation(GLuint program,const GLchar* name)
{
	return glGetUniformLocation(program,name);
}

inline void GLRenderExtensions::linkProgram(GLuint program)
{
	glLinkProgram(program);
}

inline void GLRenderExtensions::shaderSource(GLuint shader,GLsizei count,const GLchar** string,const GLint* length)
{
	glShaderSource(shader,count,string,length);
}

inline void GLRenderExtensions::uniform1i(GLint location,GLint v0)
{
	glUniform1i(location,v0);
}

inline void GLRenderExtensions::useProgram(GLuint program)
{
	glUseProgram(program);
}

inline bool GLRenderExtensions::isInitialized()
{
	return initialized;
}

inline void GLRenderExtensions::reset()
{
	initialized = false;
}

/**
*	Contains necessary GL objects per context.
*/
class PerContextGLObjects
{
	public:

		GLRenderExtensions renderExtensions;

		GLuint program;
		GLuint vertexShader;
		GLuint fragmentShader;
		GLuint quadDataVBO;
};

static OpenThreads::Mutex mutex;

static unsigned int glInitializedInstances = 0;

static osg::buffered_object<PerContextGLObjects> perContextGLObjects(0);

const char* vertexShaderSource = 
"void main()"
"{"
"	gl_TexCoord[0]= gl_MultiTexCoord0;"
"	gl_Position = gl_Vertex;"
"}";

const char* fragmentShaderSource = 
"uniform sampler2D texture0;"
"uniform sampler2D texture1;"
""
"void main()"
"{"
"	gl_FragColor = texture2D(texture0,gl_TexCoord[0].st);"
"	gl_FragDepth = texture2D(texture1,gl_TexCoord[0].st).a;"
"}";

OpenCLRender::CLViewInformation::CLViewInformation(): width(0),height(0)
{}

OpenCLRender::PerContextInformation::PerContextInformation(): 
	contextSharing(true),
	cglInitialized(false),
	buffersInitialized(false),
	initializationError(false),
	clProgramDataSet(0)
{}

OpenCLRender::PerContextInformation::~PerContextInformation()
{
	reset();
}

void OpenCLRender::PerContextInformation::reset()
{
	if (buffersInitialized)
	{
		clReleaseMemObject(clViewInfo.colorBuffer);
		clReleaseMemObject(clViewInfo.depthBuffer);

		buffersInitialized = false;
	}

	if (cglInitialized)
	{
		delete clProgramDataSet;

		clProgramDataSet = 0;

		clReleaseCommandQueue(clViewInfo.clCommQueue);
		clReleaseContext(clViewInfo.clContext);

		glInitializedInstances--;

		cglInitialized = false;
	}

	initializationError = false;
	contextSharing = true;
}

OpenCLRender::OpenCLRender(): Drawable()
{
	_supportsDisplayList = false;
	_useDisplayList = false;
}

OpenCLRender::OpenCLRender(const OpenCLRender& openCLRender,const osg::CopyOp& copyop): Drawable(openCLRender,copyop)
{}

OpenCLRender::~OpenCLRender()
{
	reset();
}

void OpenCLRender::drawImplementation(osg::RenderInfo& renderInfo) const
{
	osg::State& state = *renderInfo.getState();

	unsigned int contextID = state.getContextID();

	PerContextInformation& perContextInfo = perContextInformation[contextID];

	/* do nothing if an initialization attempt has already failed */

	if (perContextInfo.initializationError) return;

	/* initialize CL and GL objects of required */

	PerContextGLObjects& perContextGLObs = perContextGLObjects[contextID];

	if (!perContextInfo.cglInitialized)
	{
		/* initialize GL objects */

		if (!initGL(perContextInfo,perContextGLObs))
		{
			perContextInfo.initializationError = true;

			return;
		}

		/* initialize CL objects */

		if (!initCL(perContextInfo))
		{
			perContextInfo.initializationError = true;

			return;
		}

		/* initialize program(s) */

		perContextInfo.clProgramDataSet = initProgram(perContextInfo.clViewInfo);

		if (perContextInfo.clProgramDataSet == 0)
		{
			perContextInfo.initializationError = true;

			return;
		}

		/* set CL and GL initialized */

		perContextInfo.cglInitialized = true;
	}

	/* initialize or reset buffers if required */

	osg::Viewport* currentViewport = renderInfo.getCurrentCamera()->getViewport();

	unsigned int currentWidth = currentViewport->width();
	unsigned int currentHeight = currentViewport->height();

	if (!perContextInfo.buffersInitialized || 
		(currentWidth != perContextInfo.clViewInfo.width) || 
		(currentHeight != perContextInfo.clViewInfo.height))
	{
		perContextInfo.clViewInfo.width = currentWidth;
		perContextInfo.clViewInfo.height = currentHeight;

		initBuffers(perContextInfo);
	}

	if (!perContextInfo.buffersInitialized) return;

	state.setCurrentVertexBufferObject(0);

	/* start rendering */

	if (perContextInfo.contextSharing)
	{
		cl_int clError;

		glFinish();

		cl_mem glObjects[2] = {perContextInfo.clViewInfo.colorBuffer,perContextInfo.clViewInfo.depthBuffer};

		clError = clEnqueueAcquireGLObjects(perContextInfo.clViewInfo.clCommQueue,2,glObjects,0,0,0);

		if (clError != CL_SUCCESS)
		{
			osg::notify(osg::FATAL) << "Could not aquire the buffers: " << getCLError(clError) << std::endl;

			return;
		}

		render(perContextInfo.clViewInfo,perContextInfo.clProgramDataSet,state);

		clError = clEnqueueReleaseGLObjects(perContextInfo.clViewInfo.clCommQueue,2,glObjects,0,0,0);

		if (clError != CL_SUCCESS)
		{
			osg::notify(osg::FATAL) << "Could not release the buffers: " << getCLError(clError) << std::endl;

			return;
		}

		clFinish(perContextInfo.clViewInfo.clCommQueue);

		/* bind color and depth texture */

		state.setActiveTextureUnit(0);
		glBindTexture(GL_TEXTURE_2D,perContextInfo.colorTexture);

		state.setActiveTextureUnit(1);
		glBindTexture(GL_TEXTURE_2D,perContextInfo.depthTexture);
	}
	else
	{
		cl_int clError;

		render(perContextInfo.clViewInfo,perContextInfo.clProgramDataSet,state);

		/* load rendered content to main memory */

		void* colorData = new float[4*perContextInfo.clViewInfo.width*perContextInfo.clViewInfo.height];
		void* depthData = new float[perContextInfo.clViewInfo.width*perContextInfo.clViewInfo.height];

		size_t origin[3] = {0,0,0};
		size_t region[3] = {perContextInfo.clViewInfo.width,perContextInfo.clViewInfo.height,1};

		clError = clEnqueueReadImage
		(
			perContextInfo.clViewInfo.clCommQueue,
			perContextInfo.clViewInfo.colorBuffer,
			CL_TRUE,origin,region,0,0,colorData,0,0,0
		);

		if (clError != CL_SUCCESS)
		{
			osg::notify(osg::FATAL) << "Could not read color buffer: " << getCLError(clError) << std::endl;

			return;
		}

		clError = clEnqueueReadImage
		(
			perContextInfo.clViewInfo.clCommQueue,
			perContextInfo.clViewInfo.depthBuffer,
			CL_TRUE,origin,region,0,0,depthData,0,0,0
		);

		if (clError != CL_SUCCESS)
		{
			osg::notify(osg::FATAL) << "Could not read depth buffer: " << getCLError(clError) << std::endl;

			return;
		}

		/* bind color and depth texture and load new content to texture memory */

		state.setActiveTextureUnit(0);
		glBindTexture(GL_TEXTURE_2D,perContextInfo.colorTexture);
		glTexSubImage2D
		(
			GL_TEXTURE_2D,0,0,0,
			perContextInfo.clViewInfo.width,perContextInfo.clViewInfo.height,
			GL_RGBA,GL_FLOAT,colorData
		);
		delete[] colorData;

		state.setActiveTextureUnit(1);
		glBindTexture(GL_TEXTURE_2D,perContextInfo.depthTexture);
		glTexSubImage2D
		(
			GL_TEXTURE_2D,0,0,0,
			perContextInfo.clViewInfo.width,perContextInfo.clViewInfo.height,
			GL_ALPHA,GL_FLOAT,depthData
		);
		delete[] depthData;
	}

	/* bind shaders, the vertex buffer object and draw the render quad */

	GLRenderExtensions& gl = perContextGLObs.renderExtensions;

	gl.useProgram(perContextGLObs.program);
	gl.bindBuffer(GL_ARRAY_BUFFER,perContextGLObs.quadDataVBO);

	state.setVertexPointer(3,GL_FLOAT,5 * sizeof(float),0);
	state.setTexCoordPointer(0,2,GL_FLOAT,5 * sizeof(float),(GLvoid*)(3 * sizeof(float)));
	state.setTexCoordPointer(1,2,GL_FLOAT,5 * sizeof(float),(GLvoid*)(3 * sizeof(float)));

	glDrawArrays(GL_QUADS,0,4);

	glBindTexture(GL_TEXTURE_2D,0);
	state.setActiveTextureUnit(0);
	glBindTexture(GL_TEXTURE_2D,0);

	gl.bindBuffer(GL_ARRAY_BUFFER,0);
	gl.useProgram(0);
}

void OpenCLRender::resizeGLObjectBuffers(unsigned int maxSize)
{
	perContextInformation.resize(maxSize);

	Drawable::resizeGLObjectBuffers(maxSize);
}

void OpenCLRender::reset() const
{
	/* release CL objects */

	unsigned int size = perContextInformation.size();

	for (unsigned int i = 0;i < size;i++) perContextInformation[i].reset();

	/* release GL objects if required */

	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mutex);

	if (glInitializedInstances == 0)
	{
		size = perContextGLObjects.size();

		for (unsigned int i = 0;i < size;i++)
		{
			PerContextGLObjects& perContextGLObs = perContextGLObjects[i];

			GLRenderExtensions& gl = perContextGLObs.renderExtensions;

			if (gl.isInitialized())
			{
				gl.deleteProgram(perContextGLObs.program);
				gl.deleteShader(perContextGLObs.vertexShader);
				gl.deleteShader(perContextGLObs.fragmentShader);
				gl.deleteBuffers(1,&perContextGLObs.quadDataVBO);

				gl.reset();
			}
		}
	}
}

void OpenCLRender::getViewProperties(ViewProperties& properties,const osg::State& state) const
{
	const osg::Matrix& PMatrix = state.getProjectionMatrix();

	double pNear,pFar,pLeft,pRight,pTop,pBottom;

	if (PMatrix(3,3) == 0)
	{
		/* perspective projection */

		/*
		*	PMatrix = 
		*	(
		*		2*near/(right-left)			0							0						0
		*		0							2*near/(top-bottom)			0						0
		*		(right+left)/(right-left)	(top+bottom/(top-bottom)	-(far+near)/(far-near)	-1
		*		0							0							-2*far*near/(far-near)	0
		*	)
		*/

		properties.type = PERSPECTIVE;

		pNear = PMatrix(3,2) / (PMatrix(2,2) - 1.0);
		pFar = PMatrix(3,2) / (PMatrix(2,2) + 1.0);

		pLeft = pNear * (PMatrix(2,0) - 1.0) / PMatrix(0,0);
		pRight = pNear * (PMatrix(2,0) + 1.0) / PMatrix(0,0);

		pBottom = pNear * (PMatrix(2,1) - 1.0) / PMatrix(1,1);
		pTop = pNear * (PMatrix(2,1) + 1.0) / PMatrix(1,1);
	}
	else
	{
		/* orthographic projection */

		/*
		*	PMatrix = 
		*	(
		*		2/(right-left)				0							0						0
		*		0							2/(top-bottom)				0						0
		*		0							0							-2/(far-near)			0
		*		-(right+left)/(right-left)	-(top+bottom/(top-bottom)	-(far+near)/(far-near)	1
		*	)
		*/

		properties.type = ORTHOGRAPHIC;

		pNear = (PMatrix(3,2) + 1.0) / PMatrix(2,2);
		pFar = (PMatrix(3,2) - 1.0) / PMatrix(2,2);

		pLeft = -(1.0 + PMatrix(3,0)) / PMatrix(0,0);
		pRight = (1.0 - PMatrix(3,0)) / PMatrix(0,0);

		pBottom = -(1.0 + PMatrix(3,1)) / PMatrix(1,1);
		pTop = (1.0 - PMatrix(3,1)) / PMatrix(1,1);
	}

	properties.planeNear = pNear;
	properties.planeFar = pFar;

	const osg::Matrix& MVMatrix = state.getModelViewMatrix();

	/*
	*	MVMatrix = 
	*	(
	*		R(0,0)	R(0,1)	R(0,2)	0
	*		R(1,0)	R(1,1)	R(1,2)	0
	*		R(2,0)	R(2,2)	R(2,2)	0
	*		tx		ty		tz		1
	*	)
	*/

	/* (txNew,tyNew,tzNew) = -(tx,ty,tz) * Transpose[R] = -(tx,ty,tz) * Inverse[R] */

	double txNew = -(MVMatrix(0,0) * MVMatrix(3,0) + MVMatrix(0,1) * MVMatrix(3,1) + MVMatrix(0,2) * MVMatrix(3,2));
	double tyNew = -(MVMatrix(1,0) * MVMatrix(3,0) + MVMatrix(1,1) * MVMatrix(3,1) + MVMatrix(1,2) * MVMatrix(3,2));
	double tzNew = -(MVMatrix(2,0) * MVMatrix(3,0) + MVMatrix(2,1) * MVMatrix(3,1) + MVMatrix(2,2) * MVMatrix(3,2));

	/*
	*	ModelViewMatrixInverse = 
	*	(
	*		R(0,0)	R(1,0)	R(2,0)	0
	*		R(0,1)	R(1,1)	R(2,1)	0
	*		R(0,2)	R(1,2)	R(2,2)	0
	*		txNew	tyNew	tzNew	1
	*	)
	*
	*	ModelViewMatrixInverse = 
	*	(
	*		MVMatrix(0,0)	MVMatrix(1,0)	MVMatrix(2,0)	0
	*		MVMatrix(0,1)	MVMatrix(1,1)	MVMatrix(2,1)	0
	*		MVMatrix(0,2)	MVMatrix(1,2)	MVMatrix(2,2)	0
	*		txNew			tyNew			tzNew			1
	*	)
	*/

	if (properties.type == PERSPECTIVE)
	{
		/* origin = (0,0,0,1) * ModelViewMatrixInverse */

		properties.origin.set(txNew,tyNew,tzNew);

		/* origin2LowerLeft = (left,bottom,-near,0) * ModelViewMatrixInverse */

		properties.origin2LowerLeft.set
		(
			MVMatrix(0,0) * pLeft + MVMatrix(0,1) * pBottom - MVMatrix(0,2) * pNear,
			MVMatrix(1,0) * pLeft + MVMatrix(1,1) * pBottom - MVMatrix(1,2) * pNear,
			MVMatrix(2,0) * pLeft + MVMatrix(2,1) * pBottom - MVMatrix(2,2) * pNear
		);
	}
	else
	{
		/* origin = (left,bottom,0,1) * ModelViewMatrixInverse */

		properties.origin.set
		(
			MVMatrix(0,0) * pLeft + MVMatrix(0,1) * pBottom + txNew,
			MVMatrix(1,0) * pLeft + MVMatrix(1,1) * pBottom + tyNew,
			MVMatrix(2,0) * pLeft + MVMatrix(2,1) * pBottom + tzNew
		);


		/* origin2LowerLeft = (0,0,-near,0) * ModelViewMatrixInverse */

		properties.origin2LowerLeft.set
		(
			-MVMatrix(0,2) * pNear,
			-MVMatrix(1,2) * pNear,
			-MVMatrix(2,2) * pNear
		);
	}

	/* edgeX = ((right - left),0,0,0) * ModelViewMatrixInverse */

	properties.edgeX.set
	(
		MVMatrix(0,0) * (pRight - pLeft),
		MVMatrix(1,0) * (pRight - pLeft),
		MVMatrix(2,0) * (pRight - pLeft)
	);

	/* edgeY = (0,(top - bottom),0,0) * ModelViewMatrixInverse */

	properties.edgeY.set
	(
		MVMatrix(0,1) * (pTop - pBottom),
		MVMatrix(1,1) * (pTop - pBottom),
		MVMatrix(2,1) * (pTop - pBottom)
	);
}

bool OpenCLRender::initGL(PerContextInformation& perContextInfo,PerContextGLObjects& perContextGLObjects) const
{
	if (!perContextGLObjects.renderExtensions.isInitialized())
	{
		GLRenderExtensions& gl = perContextGLObjects.renderExtensions;

		if (!gl.initExtensions())
		{
			osg::notify(osg::FATAL) << "Could not initialize OpenGL extensions." << std::endl;

			return false;
		}

		/* create shaders and program */

		perContextGLObjects.vertexShader = gl.createShader(GL_VERTEX_SHADER);
		perContextGLObjects.fragmentShader = gl.createShader(GL_FRAGMENT_SHADER);

		gl.shaderSource(perContextGLObjects.vertexShader,1,&vertexShaderSource,0);
		gl.shaderSource(perContextGLObjects.fragmentShader,1,&fragmentShaderSource,0);

		gl.compileShader(perContextGLObjects.vertexShader);
		gl.compileShader(perContextGLObjects.fragmentShader);

		perContextGLObjects.program = gl.createProgram();

		gl.attachShader(perContextGLObjects.program,perContextGLObjects.vertexShader);
		gl.attachShader(perContextGLObjects.program,perContextGLObjects.fragmentShader);

		gl.linkProgram(perContextGLObjects.program);
		gl.useProgram(perContextGLObjects.program);

		GLint texSamplerUnit0 = gl.getUniformLocation(perContextGLObjects.program,"texture0");
		GLint texSamplerUnit1 = gl.getUniformLocation(perContextGLObjects.program,"texture1");

		gl.uniform1i(texSamplerUnit0,0);
		gl.uniform1i(texSamplerUnit1,1);

		/* create vertex buffer object with the render quad's vertex and texture coordinates */

		GLfloat quadData[] = 
		{
			-1.0f,-1.0f,0.0f,	// vertex 1
			0.0f,0.0f,			// texture coordinate 1
			-1.0f,1.0f,0.0f,	// vertex 2
			0.0f,1.0f,			// texture coordinate 2
			1.0f,1.0f,0.0f,		// vertex 3
			1.0f,1.0f,			// texture coordinate 3
			1.0f,-1.0f,0.0f,	// vertex 4
			1.0f,0.0f			// texture coordinate 4
		};

		gl.genBuffers(1,&perContextGLObjects.quadDataVBO);
		gl.bindBuffer(GL_ARRAY_BUFFER,perContextGLObjects.quadDataVBO);
		gl.bufferData(GL_ARRAY_BUFFER,sizeof(quadData),quadData,GL_STATIC_DRAW);

		glInitializedInstances++;
	}

	/* generate the textures containing color and depth information */

	glGenTextures(1,&perContextInfo.colorTexture);
	glBindTexture(GL_TEXTURE_2D,perContextInfo.colorTexture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	glGenTextures(1,&perContextInfo.depthTexture);
	glBindTexture(GL_TEXTURE_2D,perContextInfo.depthTexture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	return true;
}

bool OpenCLRender::initCL(PerContextInformation& perContextInfo) const
{
	cl_int clError;

	cl_uint numOfPlatforms;
	cl_platform_id* platforms;
	cl_device_id** devices;

	clGetPlatformIDs(0,0,&numOfPlatforms);

	if (numOfPlatforms == 0)
	{
		osg::notify(osg::FATAL) << "Could not find OpenCL platforms." << std::endl;

		return false;
	}

	platforms = new cl_platform_id[numOfPlatforms];
	devices = new cl_device_id*[numOfPlatforms];

	/* get available CL platforms */
	
	clGetPlatformIDs(numOfPlatforms,platforms,0);

	cl_uint* numOfDevices = new cl_uint[numOfPlatforms];

	/* get available CL devices */

	for (unsigned int i = 0;i < numOfPlatforms;i++)
	{
		clGetDeviceIDs(platforms[i],CL_DEVICE_TYPE_GPU,0,0,&numOfDevices[i]);

		devices[i] = new cl_device_id[numOfDevices[i]];

		clGetDeviceIDs(platforms[i],CL_DEVICE_TYPE_GPU,numOfDevices[i],devices[i],0);
	}

	#if defined (__APPLE__) || defined (MACOSX)
		cl_context_properties properties[5];
		
		properties[2] = CL_CGL_SHAREGROUP_KHR;
		properties[3] = reinterpret_cast<cl_context_properties>(CGLGetCurrentContext());
		properties[4] = 0;
	#else
		#ifdef UNIX
			cl_context_properties properties[7];
			
			properties[2] = CL_GL_CONTEXT_KHR;
			properties[3] = reinterpret_cast<cl_context_properties>(glXGetCurrentContext());
			properties[4] = CL_GLX_DISPLAY_KHR;
			properties[5] = reinterpret_cast<cl_context_properties>(glXGetCurrentDisplay());
			properties[6] = 0;
		#else
			cl_context_properties properties[7];

			properties[2] = CL_GL_CONTEXT_KHR;
			properties[3] = reinterpret_cast<cl_context_properties>(wglGetCurrentContext());
			properties[4] = CL_WGL_HDC_KHR;
			properties[5] = reinterpret_cast<cl_context_properties>(wglGetCurrentDC());
			properties[6] = 0;
		#endif
	#endif

	properties[0] = CL_CONTEXT_PLATFORM;

	/* create CL context sharing GL objects */
							
	for (unsigned int i = 0;i < numOfPlatforms;i++)
	{
		for (unsigned int j = 0;j < numOfDevices[i];j++)
		{
			properties[1] = reinterpret_cast<cl_context_properties>(platforms[i]);

			perContextInfo.clViewInfo.clContext = clCreateContext(properties,1,&devices[i][j],0,0,&clError);

			if (clError == CL_SUCCESS)
			{
				perContextInfo.clViewInfo.clDevice = devices[i][j];

				i = numOfPlatforms;

				break;
			}
		}
	}

	if (clError != CL_SUCCESS)
	{
		perContextInfo.contextSharing = false;

		properties[2] = 0;

		/* create CL context not sharing GL objects */
		
		for (unsigned int i = 0;i < numOfPlatforms;i++)
		{
			for (unsigned int j = 0;j < numOfDevices[i];j++)
			{
				properties[1] = reinterpret_cast<cl_context_properties>(platforms[i]);

				perContextInfo.clViewInfo.clContext = clCreateContext(properties,1,&devices[i][j],0,0,&clError);

				if (clError == CL_SUCCESS)
				{
					perContextInfo.clViewInfo.clDevice = devices[i][j];

					i = numOfPlatforms;

					break;
				}
			}
		}
	}
		
	for (unsigned int i = 0;i < numOfPlatforms;i++)
	{
		delete[] devices[i];
	}
	
	delete[] devices;
	delete[] platforms;
	delete[] numOfDevices;

	if (clError != CL_SUCCESS)
	{
		osg::notify(osg::FATAL) << "Could not create an OpenCL context: " << getCLError(clError) << std::endl;

		return false;
	}

	/* create CL command queue */

	perContextInfo.clViewInfo.clCommQueue = clCreateCommandQueue
	(
		perContextInfo.clViewInfo.clContext,
		perContextInfo.clViewInfo.clDevice,
		0,&clError
	);

	if (clError != CL_SUCCESS)
	{
		osg::notify(osg::FATAL) << "Could not create an OpenCL command-queue: " << getCLError(clError) << std::endl;

		clReleaseContext(perContextInfo.clViewInfo.clContext);

		return false;
	}

	return true;
}

void OpenCLRender::initBuffers(PerContextInformation& perContextInfo) const
{
	/* release existing buffers */

	if (perContextInfo.buffersInitialized)
	{
		clReleaseMemObject(perContextInfo.clViewInfo.colorBuffer);
		clReleaseMemObject(perContextInfo.clViewInfo.depthBuffer);

		perContextInfo.buffersInitialized = false;
	}

	/* resize color and depth texture */

	glBindTexture(GL_TEXTURE_2D,perContextInfo.colorTexture);
	glTexImage2D
	(
		GL_TEXTURE_2D,0,GL_RGBA32F,
		perContextInfo.clViewInfo.width,perContextInfo.clViewInfo.height,
		0,GL_RGBA,GL_FLOAT,0
	);

	glBindTexture(GL_TEXTURE_2D,perContextInfo.depthTexture);
	glTexImage2D
	(
		GL_TEXTURE_2D,0,GL_RGBA32F,
		perContextInfo.clViewInfo.width,perContextInfo.clViewInfo.height,
		0,GL_RGBA,GL_FLOAT,0
	);

	cl_int clError;

	/* create colorBuffer and depthBuffer */

	if (perContextInfo.contextSharing)
	{
		perContextInfo.clViewInfo.colorBuffer = clCreateFromGLTexture2D
		(
			perContextInfo.clViewInfo.clContext,
			CL_MEM_READ_WRITE,GL_TEXTURE_2D,0,
			perContextInfo.colorTexture,&clError
		);

		if (clError != CL_SUCCESS)
		{
			osg::notify(osg::FATAL) << "Could not create the color buffer: " << getCLError(clError) << std::endl;

			return;
		}

		perContextInfo.clViewInfo.depthBuffer = clCreateFromGLTexture2D
		(
			perContextInfo.clViewInfo.clContext,
			CL_MEM_READ_WRITE,GL_TEXTURE_2D,0,
			perContextInfo.depthTexture,&clError
		);

		if (clError != CL_SUCCESS)
		{
			osg::notify(osg::FATAL) << "Could not create the depth buffer: " << getCLError(clError) << std::endl;

			clReleaseMemObject(perContextInfo.clViewInfo.colorBuffer);

			return;
		}
	}
	else
	{
		cl_image_format format;

		format.image_channel_order = CL_RGBA;
		format.image_channel_data_type = CL_FLOAT;
		
		perContextInfo.clViewInfo.colorBuffer = clCreateImage2D
		(
			perContextInfo.clViewInfo.clContext,
			CL_MEM_READ_WRITE,&format,
			perContextInfo.clViewInfo.width,perContextInfo.clViewInfo.height,
			0,0,&clError
		);

		if (clError != CL_SUCCESS)
		{
			osg::notify(osg::FATAL) << "Could not create the color buffer: " << getCLError(clError) << std::endl;

			return;
		}

		format.image_channel_order = CL_A;

		perContextInfo.clViewInfo.depthBuffer = clCreateImage2D
		(
			perContextInfo.clViewInfo.clContext,
			CL_MEM_READ_WRITE,&format,
			perContextInfo.clViewInfo.width,perContextInfo.clViewInfo.height,
			0,0,&clError
		);

		if (clError != CL_SUCCESS)
		{
			osg::notify(osg::FATAL) << "Could not create the depth buffer: " << getCLError(clError) << std::endl;

			clReleaseMemObject(perContextInfo.clViewInfo.colorBuffer);

			return;
		}
	}

	/* set buffers in kernel */

	setBuffers(perContextInfo.clViewInfo,perContextInfo.clProgramDataSet);

	perContextInfo.buffersInitialized = true;
}

std::string getCLError(cl_int clError)
{
	switch (clError)
	{
		case CL_SUCCESS: return "CL_SUCCESS";
		case CL_DEVICE_NOT_FOUND: return "CL_DEVICE_NOT_FOUND";
		case CL_DEVICE_NOT_AVAILABLE: return "CL_DEVICE_NOT_AVAILABLE";
		case CL_COMPILER_NOT_AVAILABLE: return "CL_COMPILER_NOT_AVAILABLE";
		case CL_MEM_OBJECT_ALLOCATION_FAILURE: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
		case CL_OUT_OF_RESOURCES: return "CL_OUT_OF_RESOURCES";
		case CL_OUT_OF_HOST_MEMORY: return "CL_OUT_OF_HOST_MEMORY";
		case CL_PROFILING_INFO_NOT_AVAILABLE: return "CL_PROFILING_INFO_NOT_AVAILABLE";
		case CL_MEM_COPY_OVERLAP: return "CL_MEM_COPY_OVERLAP";
		case CL_IMAGE_FORMAT_MISMATCH: return "CL_IMAGE_FORMAT_MISMATCH";
		case CL_IMAGE_FORMAT_NOT_SUPPORTED: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
		case CL_BUILD_PROGRAM_FAILURE: return "CL_BUILD_PROGRAM_FAILURE";
		case CL_MAP_FAILURE: return "CL_MAP_FAILURE";

		case CL_INVALID_VALUE: return "CL_INVALID_VALUE";
		case CL_INVALID_DEVICE_TYPE: return "CL_INVALID_DEVICE_TYPE";
		case CL_INVALID_PLATFORM: return "CL_INVALID_PLATFORM";
		case CL_INVALID_DEVICE: return "CL_INVALID_DEVICE";
		case CL_INVALID_CONTEXT: return "CL_INVALID_CONTEXT";
		case CL_INVALID_QUEUE_PROPERTIES: return "CL_INVALID_QUEUE_PROPERTIES";
		case CL_INVALID_COMMAND_QUEUE: return "CL_INVALID_COMMAND_QUEUE";
		case CL_INVALID_HOST_PTR: return "CL_INVALID_HOST_PTR";
		case CL_INVALID_MEM_OBJECT: return "CL_INVALID_MEM_OBJECT";
		case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
		case CL_INVALID_IMAGE_SIZE: return "CL_INVALID_IMAGE_SIZE";
		case CL_INVALID_SAMPLER: return "CL_INVALID_SAMPLER";
		case CL_INVALID_BINARY: return "CL_INVALID_BINARY";
		case CL_INVALID_BUILD_OPTIONS: return "CL_INVALID_BUILD_OPTIONS";
		case CL_INVALID_PROGRAM: return "CL_INVALID_PROGRAM";
		case CL_INVALID_PROGRAM_EXECUTABLE: return "CL_INVALID_PROGRAM_EXECUTABLE";
		case CL_INVALID_KERNEL_NAME: return "CL_INVALID_KERNEL_NAME";
		case CL_INVALID_KERNEL_DEFINITION: return "CL_INVALID_KERNEL_DEFINITION";
		case CL_INVALID_KERNEL: return "CL_INVALID_KERNEL";
		case CL_INVALID_ARG_INDEX: return "CL_INVALID_ARG_INDEX";
		case CL_INVALID_ARG_VALUE: return "CL_INVALID_ARG_VALUE";
		case CL_INVALID_ARG_SIZE: return "CL_INVALID_ARG_SIZE";
		case CL_INVALID_KERNEL_ARGS: return "CL_INVALID_KERNEL_ARGS";
		case CL_INVALID_WORK_DIMENSION: return "CL_INVALID_WORK_DIMENSION";
		case CL_INVALID_WORK_GROUP_SIZE: return "CL_INVALID_WORK_GROUP_SIZE";
		case CL_INVALID_WORK_ITEM_SIZE: return "CL_INVALID_WORK_ITEM_SIZE";
		case CL_INVALID_GLOBAL_OFFSET: return "CL_INVALID_GLOBAL_OFFSET";
		case CL_INVALID_EVENT_WAIT_LIST: return "CL_INVALID_EVENT_WAIT_LIST";
		case CL_INVALID_EVENT: return "CL_INVALID_EVENT";
		case CL_INVALID_OPERATION: return "CL_INVALID_OPERATION";
		case CL_INVALID_GL_OBJECT: return "CL_INVALID_GL_OBJECT";
		case CL_INVALID_BUFFER_SIZE: return "CL_INVALID_BUFFER_SIZE";
		case CL_INVALID_MIP_LEVEL: return "CL_INVALID_MIP_LEVEL";
		case CL_INVALID_GLOBAL_WORK_SIZE: return "CL_INVALID_GLOBAL_WORK_SIZE";
	}

	return "";
}