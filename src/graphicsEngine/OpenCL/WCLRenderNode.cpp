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

#include <osg/BufferObject>
#include <osg/Matrix>
#include <osg/Notify>
#include <osg/Program>
#include <osg/Shader>
#include <osg/Uniform>
#include <osgUtil/CullVisitor>

#include "WCLRenderNode.h"

#if defined (__APPLE__) || defined (MACOSX)
    #include <Carbon/Carbon.h>
#else
    #if defined (__linux__)
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

#define GL_R32F 0x822E
#define GL_RGBA32F 0x8814

/*-------------------------------------------------------------------------------------------------------------------*/

static const char* vertexShaderSource =
"void main()"
"{"
"   gl_TexCoord[0] = gl_MultiTexCoord0;"
"   gl_Position = gl_Vertex;"
"}";

static const char* fragmentShaderSource =
"uniform sampler2D colorBuffer;"
"uniform sampler2D depthBuffer;"
""
"void main()"
"{"
"   gl_FragColor = texture2D(colorBuffer,gl_TexCoord[0].st);"
"   gl_FragDepth = texture2D(depthBuffer,gl_TexCoord[0].st).r;"
"}";

static unsigned int instances = 0;

WCLRenderNode::DrawQuad WCLRenderNode::m_drawQuad;

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::CLViewInformation::CLViewInformation(): width(0),height(0)
{}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::CLViewInformation::getViewProperties(ViewProperties& properties) const
{
    const osg::Matrix& PMatrix = *m_projectionMatrix;

    double pNear,pFar,pLeft,pRight,pTop,pBottom;

    if (PMatrix(3,3) == 0.0)
    {
        // perspective projection

        //  PMatrix =
        //  (
        //      2 * near / (right - left)       |0                                  |0                              |0
        //      0                               |2 * near / (top - bottom)          |0                              |0
        //      (right + left) / (right - left) |(top + bottom) / (top - bottom)    |-(far + near) / (far - near)   |-1
        //      0                               |0                                  |-2 * far * near / (far - near) |0
        //  )

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
        // orthographic projection

        //  PMatrix =
        //  (
        //      2 / (right - left)              |0                                  |0                              |0
        //      0                               |2 / (top - bottom)                 |0                              |0
        //      0                               |0                                  |-2 / (far - near)              |0
        //      -(right + left) / (right - left)|-(top + bottom) / (top - bottom)   |-(far + near) / (far - near)   |1
        //  )

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

    const osg::Matrix& MVMatrix = *m_modelViewMatrix;

    //  MVMatrix =
    //  (
    //      R(0,0)  R(0,1)  R(0,2)  0
    //      R(1,0)  R(1,1)  R(1,2)  0
    //      R(2,0)  R(2,2)  R(2,2)  0
    //      tx      ty      tz      1
    //  )

    // (txNew,tyNew,tzNew) = -(tx,ty,tz) * Inverse[R] = -(tx,ty,tz) * Transpose[R]

    double txNew = -(MVMatrix(0,0) * MVMatrix(3,0) + MVMatrix(0,1) * MVMatrix(3,1) + MVMatrix(0,2) * MVMatrix(3,2));
    double tyNew = -(MVMatrix(1,0) * MVMatrix(3,0) + MVMatrix(1,1) * MVMatrix(3,1) + MVMatrix(1,2) * MVMatrix(3,2));
    double tzNew = -(MVMatrix(2,0) * MVMatrix(3,0) + MVMatrix(2,1) * MVMatrix(3,1) + MVMatrix(2,2) * MVMatrix(3,2));

    //  ModelViewMatrixInverse =
    //  (
    //      R(0,0)  R(1,0)  R(2,0)  0
    //      R(0,1)  R(1,1)  R(2,1)  0
    //      R(0,2)  R(1,2)  R(2,2)  0
    //      txNew   tyNew   tzNew   1
    //  )
    //
    //  Inverse[ModelViewMatrix] =
    //  (
    //      MVMatrix(0,0)   MVMatrix(1,0)   MVMatrix(2,0)   0
    //      MVMatrix(0,1)   MVMatrix(1,1)   MVMatrix(2,1)   0
    //      MVMatrix(0,2)   MVMatrix(1,2)   MVMatrix(2,2)   0
    //      txNew           tyNew           tzNew           1
    //  )

    if (properties.type == PERSPECTIVE)
    {
        // origin = (0,0,0,1) * Inverse[ModelViewMatrix]

        properties.origin.set(txNew,tyNew,tzNew);

        // origin2LowerLeft = (left,bottom,-near,0) * Inverse[ModelViewMatrix]

        properties.origin2LowerLeft.set
        (
            MVMatrix(0,0) * pLeft + MVMatrix(0,1) * pBottom - MVMatrix(0,2) * pNear,
            MVMatrix(1,0) * pLeft + MVMatrix(1,1) * pBottom - MVMatrix(1,2) * pNear,
            MVMatrix(2,0) * pLeft + MVMatrix(2,1) * pBottom - MVMatrix(2,2) * pNear
        );
    }
    else
    {
        // origin = (left,bottom,0,1) * Inverse[ModelViewMatrix]

        properties.origin.set
        (
            MVMatrix(0,0) * pLeft + MVMatrix(0,1) * pBottom + txNew,
            MVMatrix(1,0) * pLeft + MVMatrix(1,1) * pBottom + tyNew,
            MVMatrix(2,0) * pLeft + MVMatrix(2,1) * pBottom + tzNew
        );


        // origin2LowerLeft = (0,0,-near,0) * Inverse[ModelViewMatrix]

        properties.origin2LowerLeft.set
        (
            -MVMatrix(0,2) * pNear,
            -MVMatrix(1,2) * pNear,
            -MVMatrix(2,2) * pNear
        );
    }

    // edgeX = ((right - left),0,0,0) * Inverse[ModelViewMatrix]

    properties.edgeX.set
    (
	    MVMatrix(0,0) * (pRight - pLeft),
	    MVMatrix(1,0) * (pRight - pLeft),
	    MVMatrix(2,0) * (pRight - pLeft)
    );

    // edgeY = (0,(top - bottom),0,0) * Inverse[ModelViewMatrix]

    properties.edgeY.set
    (
	    MVMatrix(0,1) * (pTop - pBottom),
	    MVMatrix(1,1) * (pTop - pBottom),
	    MVMatrix(2,1) * (pTop - pBottom)
    );
}

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::CLProgramDataSet::~CLProgramDataSet()
{}

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::PerContextInformation::PerContextInformation():
    contextSharing(true),
    clInitialized(false),
    buffersInitialized(false),
    initializationError(false)
{}

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::PerContextInformation::~PerContextInformation()
{
    reset();
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::PerContextInformation::reset()
{
    if (buffersInitialized)
    {
        clReleaseMemObject(clViewInfo.colorBuffer);
        clReleaseMemObject(clViewInfo.depthBuffer);

        buffersInitialized = false;
    }

    if (clInitialized)
    {
        delete clProgramDataSet;

        clReleaseCommandQueue(clViewInfo.clCommQueue);
        clReleaseContext(clViewInfo.clContext);

        clInitialized = false;
    }

    initializationError = false;
    contextSharing = true;
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::DrawQuad::initQuad()
{
    // create state

    osg::StateSet* state = new osg::StateSet();
    osg::Program* program = new osg::Program();
    osg::Shader* vertexShader = new osg::Shader(osg::Shader::VERTEX);
    osg::Shader* fragmentShader = new osg::Shader(osg::Shader::FRAGMENT);

    vertexShader->setShaderSource(vertexShaderSource);
    fragmentShader->setShaderSource(fragmentShaderSource);

    program->addShader(vertexShader);
    program->addShader(fragmentShader);

    state->setAttributeAndModes(program,osg::StateAttribute::ON);
    state->addUniform(new osg::Uniform("colorBuffer",0),osg::StateAttribute::ON);
    state->addUniform(new osg::Uniform("depthBuffer",1),osg::StateAttribute::ON);

    // create vertices

    osg::Vec2Array* vertices = new osg::Vec2Array();

    vertices->push_back(osg::Vec2(-1.0f,-1.0f));
    vertices->push_back(osg::Vec2(-1.0f,1.0f));
    vertices->push_back(osg::Vec2(1.0f,1.0f));
    vertices->push_back(osg::Vec2(1.0f,-1.0f));

    // create coordinates

    osg::Vec2Array* coordinates = new osg::Vec2Array();

    coordinates->push_back(osg::Vec2(0.0f,0.0f));
    coordinates->push_back(osg::Vec2(0.0f,1.0f));
    coordinates->push_back(osg::Vec2(1.0f,1.0f));
    coordinates->push_back(osg::Vec2(1.0f,0.0f));

    // create VBO

    osg::VertexBufferObject* vbo = new osg::VertexBufferObject();

    vbo->setUsage(GL_STATIC_DRAW);

    vertices->setVertexBufferObject(vbo);
    coordinates->setVertexBufferObject(vbo);

    // assemble

    m_drawQuad.drawState = state;
    m_drawQuad.vertices = vertices;
    m_drawQuad.coordinates = coordinates;
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::DrawQuad::releaseQuad()
{
    m_drawQuad.drawState.release();
    m_drawQuad.vertices.release();
    m_drawQuad.coordinates.release();
}

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::CLRenderBin::CLRenderBin(osg::RefMatrix* modelView,osg::RefMatrix* projection):
    RenderBin(),
    modelView(modelView),
    projection(projection)
{}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::CLRenderBin::draw(osg::RenderInfo& renderInfo,osgUtil::RenderLeaf*& previous)
{
    osg::State& state = *renderInfo.getState();

    // apply matrices to state

    state.applyModelViewMatrix(modelView.get());
    state.applyProjectionMatrix(projection.get());

    // initialize rendering

    for (std::vector< osg::ref_ptr< WCLRenderNode > >::iterator it = nodes.begin(); it != nodes.end(); it++)
    {
        (*it)->renderStart(state);
    }
}

/*-------------------------------------------------------------------------------------------------------------------*/

unsigned int WCLRenderNode::CLRenderBin::computeNumberOfDynamicRenderLeaves() const
{
    return 0;
}

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::CLRenderBin* WCLRenderNode::CLRenderBin::getOrCreateRenderBin(osgUtil::RenderStage* stage,
																		   osg::RefMatrix* modelView,
																		   osg::RefMatrix* projection)
{
    RenderBinList& binList = stage->getRenderBinList();

    // look for existing CLRenderBin

    for (RenderBinList::iterator it = binList.begin(); (it != binList.end()) && (it->first < 0); it++)
    {
        WCLRenderNode::CLRenderBin* bin = dynamic_cast< WCLRenderNode::CLRenderBin* >(it->second.get());

        if (bin != 0)
        {
            return bin;
        }
    }

    // determine bin number

    RenderBinList::iterator it = binList.begin();

    int binNum;

    if((it != binList.end()) && (it->first < 0))
    {
        binNum = it->first - 1;
    }
    else
    {
        binNum = -1;
    }

    // create CLRenderBin

    WCLRenderNode::CLRenderBin* bin = new CLRenderBin(modelView,projection);

    bin->_binNum = binNum;
    bin->_parent = stage;
    bin->_stage = stage;

    binList[binNum] = bin;

    return bin;
}

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::CLDrawBin::CLDrawBin(): RenderBin(),dynamicNodes(0)
{}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::CLDrawBin::draw(osg::RenderInfo& renderInfo,osgUtil::RenderLeaf*& previous)
{
    osg::State& state = *renderInfo.getState();

    if (previous == 0)
    {
        state.pushStateSet(renderInfo.getCurrentCamera()->getStateSet());
    }

    state.apply(WCLRenderNode::m_drawQuad.drawState);
    state.setVertexPointer(WCLRenderNode::m_drawQuad.vertices);
    state.setTexCoordPointer(0,WCLRenderNode::m_drawQuad.coordinates);

    for (std::vector< osg::ref_ptr< WCLRenderNode > >::iterator it = nodes.begin(); it != nodes.end(); it++)
    {
        WCLRenderNode* node = *it;

        state.applyTextureAttribute(0,node->m_colorBuffer);
        state.applyTextureAttribute(1,node->m_depthBuffer);

        node->draw(state);

        if (node->getDataVariance() == DYNAMIC)
        {
            state.decrementDynamicObjectCount();
        }
    }
}

/*-------------------------------------------------------------------------------------------------------------------*/

unsigned int WCLRenderNode::CLDrawBin::computeNumberOfDynamicRenderLeaves() const
{
    return dynamicNodes;
}

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::CLDrawBin* WCLRenderNode::CLDrawBin::getOrCreateDrawBin(osgUtil::RenderStage* stage)
{
    RenderBinList& binList = stage->getRenderBinList();

    // look for existing CLDrawBin

    for (RenderBinList::reverse_iterator it = binList.rbegin(); (it != binList.rend()) && (it->first > 0); it++)
    {
        WCLRenderNode::CLDrawBin* bin = dynamic_cast< WCLRenderNode::CLDrawBin* >(it->second.get());

        if (bin != 0)
        {
            return bin;
        }
    }

    // determine bin number

    RenderBinList::reverse_iterator it = binList.rbegin();

    int binNum;

    if((it != binList.rend()) && (it->first > 0))
    {
        binNum = it->first + 1;
    }
    else
    {
        binNum = 1;
    }

    // create CLDrawBin

    WCLRenderNode::CLDrawBin* bin = new CLDrawBin();

    bin->_binNum = binNum;
    bin->_parent = stage;
    bin->_stage = stage;

    binList[binNum] = bin;

    return bin;
}

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::WCLRenderNode(): Node()
{
    // create dummy callback to avoid unnecessary texture loads

    class NoOpCallback: public osg::Texture2D::SubloadCallback
    {

    public:

        virtual void load(const osg::Texture2D& texture,osg::State& state) const {};
        virtual void subload(const osg::Texture2D& texture,osg::State& state) const {};
    };

    NoOpCallback* callback = new NoOpCallback();

    // create color buffer

    m_colorBuffer = new osg::Texture2D();

    m_colorBuffer->setInternalFormat(GL_RGBA32F);
    m_colorBuffer->setFilter(osg::Texture::MIN_FILTER,osg::Texture::NEAREST);
    m_colorBuffer->setFilter(osg::Texture::MAG_FILTER,osg::Texture::NEAREST);
    m_colorBuffer->setSubloadCallback(callback);

    // create depth buffer

    m_depthBuffer = new osg::Texture2D();

    m_depthBuffer->setInternalFormat(GL_R32F);
    m_depthBuffer->setFilter(osg::Texture::MIN_FILTER,osg::Texture::NEAREST);
    m_depthBuffer->setFilter(osg::Texture::MAG_FILTER,osg::Texture::NEAREST);
    m_depthBuffer->setSubloadCallback(callback);

    // create the draw quad if necessary

    if (instances == 0)
    {
        DrawQuad::initQuad();
    }

    instances++;
}

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::WCLRenderNode(const WCLRenderNode& node,const osg::CopyOp& copyop):
    Node(node,copyop),
    m_box(node.m_box),
    m_colorBuffer(new osg::Texture2D(*node.m_colorBuffer,copyop)),
    m_depthBuffer(new osg::Texture2D(*node.m_depthBuffer,copyop))
{
    instances++;
}

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::~WCLRenderNode()
{
    // release CL objects

    reset();

    // release the draw quad if it is no longer needed

    instances--;

    if (instances == 0)
    {
        DrawQuad::releaseQuad();
    }
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::traverse(osg::NodeVisitor& nv)
{
    // check visitor type

    if (nv.getVisitorType() != osg::NodeVisitor::CULL_VISITOR)
    {
        return;
    }

    osgUtil::CullVisitor* cv = static_cast< osgUtil::CullVisitor* >(&nv);

    // cull operations

    if (!cv->getCurrentCullingSet().getFrustum().contains(m_box))
    {
        return;
    }

    // fit near and far planes if necessary

    if (cv->getComputeNearFarMode() != osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR)
    {
        cv->updateCalculatedNearFar(*cv->getModelViewMatrix(),m_box);
    }

    // create CLRenderBin

    osgUtil::RenderStage* stage = cv->getRenderStage();

    CLRenderBin* renderBin = CLRenderBin::getOrCreateRenderBin(stage,cv->getModelViewMatrix(),cv->getProjectionMatrix());

    renderBin->nodes.push_back(this);

    // create CLDrawBin

    CLDrawBin* drawBin = CLDrawBin::getOrCreateDrawBin(stage);

    drawBin->nodes.push_back(this);

    // check data variance

    if (getDataVariance() == DYNAMIC)
    {
        drawBin->dynamicNodes++;
    }
}

/*-------------------------------------------------------------------------------------------------------------------*/

osg::BoundingSphere WCLRenderNode::computeBound() const
{
    m_box = computeBoundingBox();

    return osg::BoundingSphere(m_box);
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::resizeGLObjectBuffers(unsigned int maxSize)
{
    m_perContextInformation.resize(maxSize);

    m_colorBuffer->resizeGLObjectBuffers(maxSize);
    m_depthBuffer->resizeGLObjectBuffers(maxSize);

    m_drawQuad.drawState->resizeGLObjectBuffers(maxSize);
    m_drawQuad.vertices->getVertexBufferObject()->resizeGLObjectBuffers(maxSize);

    Node::resizeGLObjectBuffers(maxSize);
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::releaseGLObjects(osg::State* state)
{
    // release CL objects

    if (state == 0)
    {
        reset();
    }
    else
    {
        m_perContextInformation[state->getContextID()].reset();
    }

    // release GL objects

    m_colorBuffer->releaseGLObjects(state);
    m_depthBuffer->releaseGLObjects(state);

    m_drawQuad.drawState->releaseGLObjects(state);
    m_drawQuad.vertices->getVertexBufferObject()->releaseGLObjects(state);

    Node::releaseGLObjects(state);
}

/*-------------------------------------------------------------------------------------------------------------------*/

osg::BoundingBox WCLRenderNode::computeBoundingBox() const
{
    return osg::BoundingBox();
}

/*-------------------------------------------------------------------------------------------------------------------*/

bool WCLRenderNode::initializationFailed() const
{
    unsigned int size = m_perContextInformation.size();

    for (unsigned int i = 0; i < size; i++)
    {
        if (m_perContextInformation[i].initializationError)
        {
            return true;
        }
    }

    return false;
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::reset() const
{
    unsigned int size = m_perContextInformation.size();

    for (unsigned int i = 0; i < size; i++)
    {
        m_perContextInformation[i].reset();
    }
}

/*-------------------------------------------------------------------------------------------------------------------*/

std::string WCLRenderNode::getCLError(cl_int clError)
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

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::changeDataSet(const CLDataChangeCallback& callback)
{
    unsigned int size = m_perContextInformation.size();

    for (unsigned int i = 0; i < size; i++)
    {
        PerContextInformation& perContextInfo = m_perContextInformation[i];

        if (perContextInfo.clInitialized)
        {
            callback.change(perContextInfo.clProgramDataSet);
        }
    }
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::renderStart(osg::State& state) const
{
    PerContextInformation& perContextInfo = m_perContextInformation[state.getContextID()];

    perContextInfo.clViewInfo.m_modelViewMatrix = &state.getModelViewMatrix();
    perContextInfo.clViewInfo.m_projectionMatrix = &state.getProjectionMatrix();

    // do nothing if an initialization attempt has failed already

    if (perContextInfo.initializationError)
    {
        return;
    }

    // initialize CL objects if required

    if (!perContextInfo.clInitialized)
    {
        if (!initCL(perContextInfo))
        {
            perContextInfo.initializationError = true;

            return;
        }

        perContextInfo.clProgramDataSet = initProgram(perContextInfo.clViewInfo);

        if (perContextInfo.clProgramDataSet == 0)
        {
            perContextInfo.initializationError = true;

            return;
        }

        perContextInfo.clInitialized = true;
    }

    // initialize or refit buffers if required

    const osg::Viewport* currentViewport = state.getCurrentViewport();

    unsigned int currentWidth = currentViewport->width();
    unsigned int currentHeight = currentViewport->height();

    if (!perContextInfo.buffersInitialized || 
        (currentWidth != perContextInfo.clViewInfo.width) || 
        (currentHeight != perContextInfo.clViewInfo.height))
    {
        perContextInfo.clViewInfo.width = currentWidth;
        perContextInfo.clViewInfo.height = currentHeight;

        if (!initBuffers(perContextInfo,state))
        {
            perContextInfo.initializationError = true;

            return;
        }

        perContextInfo.buffersInitialized = true;
    }

    // start rendering

    if (perContextInfo.contextSharing)
    {
        glFinish();

        cl_int clError;

        cl_mem glObjects[2] = {perContextInfo.clViewInfo.colorBuffer,perContextInfo.clViewInfo.depthBuffer};

        clError = clEnqueueAcquireGLObjects(perContextInfo.clViewInfo.clCommQueue,2,glObjects,0,0,0);

        if (clError != CL_SUCCESS)
        {
            osg::notify(osg::FATAL) << "Could not aquire the buffers: " << getCLError(clError) << std::endl;

            return;
        }

        render(perContextInfo.clViewInfo,perContextInfo.clProgramDataSet);

        clError = clEnqueueReleaseGLObjects(perContextInfo.clViewInfo.clCommQueue,2,glObjects,0,0,0);

        if (clError != CL_SUCCESS)
        {
            osg::notify(osg::FATAL) << "Could not release the buffers: " << getCLError(clError) << std::endl;

            return;
        }
    }
    else
    {
        render(perContextInfo.clViewInfo,perContextInfo.clProgramDataSet);
    }
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::draw(osg::State& state) const
{
    PerContextInformation& perContextInfo = m_perContextInformation[state.getContextID()];

    // do nothing if an initialization attempt has failed already

    if (perContextInfo.initializationError)
    {
        return;
    }

    // draw

    if (perContextInfo.contextSharing)
    {
        clFinish(perContextInfo.clViewInfo.clCommQueue);
    }
    else
    {
        // load rendered content to main memory

        cl_int clError;

        void* colorData = new float[perContextInfo.clViewInfo.width * perContextInfo.clViewInfo.height * 4];
        void* depthData = new float[perContextInfo.clViewInfo.width * perContextInfo.clViewInfo.height];

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

        // bind color and depth texture and load new content to texture memory

        state.setActiveTextureUnit(0);

        glTexSubImage2D
        (
            GL_TEXTURE_2D,0,0,0,
            perContextInfo.clViewInfo.width,perContextInfo.clViewInfo.height,
            GL_RGBA,GL_FLOAT,colorData
        );

        state.setActiveTextureUnit(1);

        glTexSubImage2D
        (
            GL_TEXTURE_2D,0,0,0,
            perContextInfo.clViewInfo.width,perContextInfo.clViewInfo.height,
            GL_RED,GL_FLOAT,depthData
        );

        delete[] colorData;
        delete[] depthData;
    }

    glDrawArrays(GL_QUADS,0,4);
}

/*-------------------------------------------------------------------------------------------------------------------*/

bool WCLRenderNode::initCL(PerContextInformation& perContextInfo) const
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

    // get available CL platforms

    clGetPlatformIDs(numOfPlatforms,platforms,0);

    cl_uint* numOfDevices = new cl_uint[numOfPlatforms];

    // get available CL devices

    for (unsigned int i = 0; i < numOfPlatforms; i++)
    {
        clGetDeviceIDs(platforms[i],CL_DEVICE_TYPE_GPU,0,0,&numOfDevices[i]);

        devices[i] = new cl_device_id[numOfDevices[i]];

        clGetDeviceIDs(platforms[i],CL_DEVICE_TYPE_GPU,numOfDevices[i],devices[i],0);
    }

    #if defined (__APPLE__) || defined (MACOSX)
        cl_context_properties properties[5];

        properties[2] = CL_CGL_SHAREGROUP_KHR;
        properties[3] = reinterpret_cast< cl_context_properties >(CGLGetCurrentContext());
        properties[4] = 0;
    #else
        #ifdef __linux__
            cl_context_properties properties[7];

            properties[2] = CL_GL_CONTEXT_KHR;
            properties[3] = reinterpret_cast< cl_context_properties >(glXGetCurrentContext());
            properties[4] = CL_GLX_DISPLAY_KHR;
            properties[5] = reinterpret_cast< cl_context_properties >(glXGetCurrentDisplay());
            properties[6] = 0;
        #else
            cl_context_properties properties[7];

            properties[2] = CL_GL_CONTEXT_KHR;
            properties[3] = reinterpret_cast< cl_context_properties >(wglGetCurrentContext());
            properties[4] = CL_WGL_HDC_KHR;
            properties[5] = reinterpret_cast< cl_context_properties >(wglGetCurrentDC());
            properties[6] = 0;
        #endif
    #endif

    properties[0] = CL_CONTEXT_PLATFORM;

    // create CL context with GL context sharing

    for (unsigned int i = 0; i < numOfPlatforms; i++)
    {
        for (unsigned int j = 0; j < numOfDevices[i]; j++)
        {
            properties[1] = reinterpret_cast< cl_context_properties >(platforms[i]);

            perContextInfo.clViewInfo.clContext = clCreateContext(properties,1,&devices[i][j],0,0,&clError);

            if (clError == CL_SUCCESS)
            {
                perContextInfo.clViewInfo.clDevice = devices[i][j];

                i = numOfPlatforms;

                break;
            }
        }
    }

    // create CL context without GL context sharing if context creation failed

    if (clError != CL_SUCCESS)
    {
        perContextInfo.contextSharing = false;

        properties[2] = 0;

        for (unsigned int i = 0; i < numOfPlatforms; i++)
        {
            for (unsigned int j = 0; j < numOfDevices[i]; j++)
            {
                properties[1] = reinterpret_cast< cl_context_properties >(platforms[i]);

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

    for (unsigned int i = 0; i < numOfPlatforms; i++)
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

    // create CL command queue

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

/*-------------------------------------------------------------------------------------------------------------------*/

bool WCLRenderNode::initBuffers(PerContextInformation& perContextInfo,osg::State& state) const
{
    // release existing buffers

    if (perContextInfo.buffersInitialized)
    {
        clReleaseMemObject(perContextInfo.clViewInfo.colorBuffer);
        clReleaseMemObject(perContextInfo.clViewInfo.depthBuffer);
    }

    // resize color and depth texture

    state.applyTextureAttribute(0,m_colorBuffer);

    glTexImage2D
    (
        GL_TEXTURE_2D,0,GL_RGBA32F,
        perContextInfo.clViewInfo.width,perContextInfo.clViewInfo.height,
        0,GL_RGBA,GL_FLOAT,0
    );

    state.applyTextureAttribute(1,m_depthBuffer);

    glTexImage2D
    (
        GL_TEXTURE_2D,0,GL_R32F,
        perContextInfo.clViewInfo.width,perContextInfo.clViewInfo.height,
        0,GL_RED,GL_FLOAT,0
    );

    // create colorBuffer and depthBuffer

    cl_int clError;

    if (perContextInfo.contextSharing)
    {
        unsigned int contextID = state.getContextID();

        perContextInfo.clViewInfo.colorBuffer = clCreateFromGLTexture2D
        (
            perContextInfo.clViewInfo.clContext,
            CL_MEM_READ_WRITE,GL_TEXTURE_2D,0,
            m_colorBuffer->getTextureObject(contextID)->_id,&clError
        );

        if (clError != CL_SUCCESS)
        {
            osg::notify(osg::FATAL) << "Could not create the color buffer: " << getCLError(clError) << std::endl;

            return false;
        }

        perContextInfo.clViewInfo.depthBuffer = clCreateFromGLTexture2D
        (
            perContextInfo.clViewInfo.clContext,
            CL_MEM_READ_WRITE,GL_TEXTURE_2D,0,
            m_depthBuffer->getTextureObject(contextID)->_id,&clError
        );

        if (clError != CL_SUCCESS)
        {
            osg::notify(osg::FATAL) << "Could not create the depth buffer: " << getCLError(clError) << std::endl;

            clReleaseMemObject(perContextInfo.clViewInfo.colorBuffer);

            return false;
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

            return false;
	    }

	    format.image_channel_order = CL_R;

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

            return false;
	    }
    }

    // set buffer kernel arguments

    setBuffers(perContextInfo.clViewInfo,perContextInfo.clProgramDataSet);

    return true;
}

/*-------------------------------------------------------------------------------------------------------------------*/
