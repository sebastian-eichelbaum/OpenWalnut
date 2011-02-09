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

#ifdef __linux__
    #include <GL/glx.h>
#endif

#include <osg/BufferObject>
#include <osg/Program>
#include <osg/Shader>
#include <osg/Uniform>
#include <osgUtil/CullVisitor>

#include "WGERenderNodeCL.h"

#define GL_R32F 0x822E
#define GL_RGBA32F 0x8814

static const char* vertexShaderSource =
"void main()"
"{"
"   gl_TexCoord[ 0 ] = gl_MultiTexCoord0;"
""
"   gl_Position = gl_Vertex;"
"}";

static const char* fragmentShaderSource =
"uniform sampler2D colorBuffer;"
"uniform sampler2D depthBuffer;"
""
"void main()"
"{"
"   gl_FragColor = texture2D( colorBuffer, gl_TexCoord[ 0 ].st );"
"   gl_FragDepth = texture2D( depthBuffer, gl_TexCoord[ 0 ].st ).r;"
"}";

WGERenderNodeCL::DrawQuad WGERenderNodeCL::m_drawQuad;


WGERenderNodeCL::PerContextInformation::PerContextInformation(): m_clData( 0 ), m_invalid( true )
{
    // initialize
}

WGERenderNodeCL::PerContextInformation::~PerContextInformation()
{
    delete m_clData;
}

void WGERenderNodeCL::PerContextInformation::reset()
{
    m_clViewData.m_context = cl::Context();
    m_clViewData.m_device = cl::Device();
    m_clViewData.m_commQueue = cl::CommandQueue();

    m_clViewData.m_width = 0;
    m_clViewData.m_height = 0;

    m_clViewData.m_buffers[ 0 ] = cl::Image2DGL();
    m_clViewData.m_buffers[ 1 ] = cl::Image2DGL();

    delete m_clData;

    m_clData = 0;

    m_invalid = true;
}

void WGERenderNodeCL::CLRenderBin::draw( osg::RenderInfo& renderInfo, osgUtil::RenderLeaf*& /*previous*/ )  // NOLINT - non const ref.
{
    osg::State& state = *renderInfo.getState();

    unsigned int size = m_nodes.size();

    glFinish();

    for ( unsigned int i = 0; i < size; i++ )
    {
        m_nodes[ i ]->render( state, m_modelViews[ i ].get(), m_projections[ i ].get() );
    }
}

unsigned int WGERenderNodeCL::CLRenderBin::computeNumberOfDynamicRenderLeaves() const
{
    return 0;
}

WGERenderNodeCL::CLRenderBin* WGERenderNodeCL::CLRenderBin::getOrCreateRenderBin( osgUtil::RenderStage* stage )
{
    RenderBinList& binList = stage->getRenderBinList();

    // look for existing CLRenderBin ----------------------------------------------------------------------------------

    for ( RenderBinList::iterator it = binList.begin(); ( it != binList.end() ) && ( it->first < 0 ); it++ )
    {
        CLRenderBin* bin = dynamic_cast< CLRenderBin* >( it->second.get() );

        if ( bin != 0 )
        {
            return bin;
        }
    }

    // determine bin number -------------------------------------------------------------------------------------------

    RenderBinList::iterator it = binList.begin();

    int binNum;

    if ( ( it != binList.end() ) && ( it->first < 0 ) )
    {
        binNum = it->first - 1;
    }
    else
    {
        binNum = -1;
    }

    // create CLRenderBin ---------------------------------------------------------------------------------------------

    CLRenderBin* bin = new CLRenderBin();

    bin->_binNum = binNum;
    bin->_parent = stage;
    bin->_stage = stage;

    binList[ binNum ] = bin;

    return bin;
}

void WGERenderNodeCL::CLDrawBin::draw( osg::RenderInfo& renderInfo, osgUtil::RenderLeaf*& previous )   // NOLINT - non const ref.
{
    osg::State& state = *renderInfo.getState();

    // add camera attributes to current GL state if not already done --------------------------------------------------

    if ( previous == 0 )
    {
        state.pushStateSet( renderInfo.getCurrentCamera()->getStateSet() );
    }

    // prepare the quad for drawing -----------------------------------------------------------------------------------

    state.apply( WGERenderNodeCL::m_drawQuad.m_drawState );
    state.setVertexPointer( WGERenderNodeCL::m_drawQuad.m_vertices );
    state.setTexCoordPointer( 0, WGERenderNodeCL::m_drawQuad.m_coordinates );

    // draw the rendered nodes ----------------------------------------------------------------------------------------

    unsigned int size = m_nodes.size();

    for ( unsigned int i = 0; i < size; i++ )
    {
        osg::ref_ptr< WGERenderNodeCL >& node = m_nodes[ i ];

        node->draw( state );

        // update the states dynamic object count if necessary --------------------------------------------------------

        if ( node->getDataVariance() == DYNAMIC )
        {
            state.decrementDynamicObjectCount();
        }
    }
}

unsigned int WGERenderNodeCL::CLDrawBin::computeNumberOfDynamicRenderLeaves() const
{
    unsigned int count = 0;
    unsigned int size = m_nodes.size();

    for ( unsigned int i = 0; i < size; i++ )
    {
        if ( m_nodes[ i ]->getDataVariance() == DYNAMIC )
        {
            count++;
        }
    }

    return count;
}

WGERenderNodeCL::CLDrawBin* WGERenderNodeCL::CLDrawBin::getOrCreateDrawBin( osgUtil::RenderStage* stage )
{
    RenderBinList& binList = stage->getRenderBinList();

    // look for existing CLDrawBin ------------------------------------------------------------------------------------

    for ( RenderBinList::reverse_iterator it = binList.rbegin(); ( it != binList.rend() ) && ( it->first > 0 ); it++ )
    {
        CLDrawBin* bin = dynamic_cast< CLDrawBin* >( it->second.get() );

        if ( bin != 0 )
        {
            return bin;
        }
    }

    // determine bin number -------------------------------------------------------------------------------------------

    RenderBinList::reverse_iterator it = binList.rbegin();

    int binNum;

    if ( ( it != binList.rend() ) && ( it->first > 0 ) )
    {
        binNum = it->first + 1;
    }
    else
    {
        binNum = 1;
    }

    // create CLDrawBin -----------------------------------------------------------------------------------------------

    CLDrawBin* bin = new CLDrawBin();

    bin->_binNum = binNum;
    bin->_parent = stage;
    bin->_stage = stage;

    binList[ binNum ] = bin;

    return bin;
}

WGERenderNodeCL::DrawQuad::DrawQuad()
{
    // create state ---------------------------------------------------------------------------------------------------

    m_drawState = new osg::StateSet();

    osg::Program* program = new osg::Program();
    osg::Shader* vertexShader = new osg::Shader( osg::Shader::VERTEX );
    osg::Shader* fragmentShader = new osg::Shader( osg::Shader::FRAGMENT );

    vertexShader->setShaderSource( vertexShaderSource );
    fragmentShader->setShaderSource( fragmentShaderSource );

    program->addShader( vertexShader );
    program->addShader( fragmentShader );

    m_drawState->setAttributeAndModes( program, osg::StateAttribute::ON );
    m_drawState->addUniform( new osg::Uniform( "colorBuffer", 0 ), osg::StateAttribute::ON );
    m_drawState->addUniform( new osg::Uniform( "depthBuffer", 1 ), osg::StateAttribute::ON );

    // create vertices ------------------------------------------------------------------------------------------------

    m_vertices = new osg::Vec2Array();

    m_vertices->push_back( osg::Vec2( -1.0f, -1.0f ) );
    m_vertices->push_back( osg::Vec2( -1.0f, 1.0f ) );
    m_vertices->push_back( osg::Vec2( 1.0f, 1.0f ) );
    m_vertices->push_back( osg::Vec2( 1.0f, -1.0f ) );

    // create coordinates ---------------------------------------------------------------------------------------------

    m_coordinates = new osg::Vec2Array();

    m_coordinates->push_back( osg::Vec2( 0.0f, 0.0f ) );
    m_coordinates->push_back( osg::Vec2( 0.0f, 1.0f ) );
    m_coordinates->push_back( osg::Vec2( 1.0f, 1.0f ) );
    m_coordinates->push_back( osg::Vec2( 1.0f, 0.0f ) );

    // create VBO -----------------------------------------------------------------------------------------------------

    osg::VertexBufferObject* vbo = new osg::VertexBufferObject();

    vbo->setUsage( GL_STATIC_DRAW );

    m_vertices->setVertexBufferObject( vbo );
    m_coordinates->setVertexBufferObject( vbo );
}

WGERenderNodeCL::WGERenderNodeCL( bool deactivated ):
    Node(),
    m_deactivated( deactivated ),
    m_count( 0 ),
    m_module( 0 )
{
    // create color buffer --------------------------------------------------------------------------------------------

    m_colorBufferTex = new osg::Texture2D();

    m_colorBufferTex->setInternalFormat( GL_RGBA32F );
    m_colorBufferTex->setSourceFormat( GL_RGBA );
    m_colorBufferTex->setSourceType( GL_FLOAT );
    m_colorBufferTex->setFilter( osg::Texture::MIN_FILTER, osg::Texture::NEAREST );
    m_colorBufferTex->setFilter( osg::Texture::MAG_FILTER, osg::Texture::NEAREST );

    // create depth buffer --------------------------------------------------------------------------------------------

    m_depthBufferTex = new osg::Texture2D();

    m_depthBufferTex->setInternalFormat( GL_R32F );
    m_depthBufferTex->setSourceFormat( GL_RED );
    m_depthBufferTex->setSourceType( GL_FLOAT );
    m_depthBufferTex->setFilter( osg::Texture::MIN_FILTER, osg::Texture::NEAREST );
    m_depthBufferTex->setFilter( osg::Texture::MAG_FILTER, osg::Texture::NEAREST );
}

WGERenderNodeCL::WGERenderNodeCL( const WGERenderNodeCL& node, const osg::CopyOp& copyop ): // NOLINT - non const ref.
    Node( node, copyop ),
    m_perContextInformation(),
    m_colorBufferTex( new osg::Texture2D( *node.m_colorBufferTex, copyop ) ),
    m_depthBufferTex( new osg::Texture2D( *node.m_depthBufferTex, copyop ) ),
    m_deactivated( node.m_deactivated ),
    m_count( 0 ),
    m_module( 0 )
{
    // initialize
}

WGERenderNodeCL::~WGERenderNodeCL()
{
    disconnectModule();
}

void WGERenderNodeCL::setThreadSafeRefUnref( bool threadSafe )
{
    m_colorBufferTex->setThreadSafeRefUnref( threadSafe );
    m_depthBufferTex->setThreadSafeRefUnref( threadSafe );

    if ( m_module.valid() )
    {
        m_module->setThreadSafeRefUnref( threadSafe );
    }

    m_drawQuad.m_drawState->setThreadSafeRefUnref( threadSafe );
    m_drawQuad.m_vertices->setThreadSafeRefUnref( threadSafe );
    m_drawQuad.m_coordinates->setThreadSafeRefUnref( threadSafe );

    Node::setThreadSafeRefUnref( threadSafe );
}

osg::Object* WGERenderNodeCL::cloneType() const
{
    return new WGERenderNodeCL();
}

osg::Object* WGERenderNodeCL::clone( const osg::CopyOp& copyop ) const
{
    return new WGERenderNodeCL( *this, copyop );
}

bool WGERenderNodeCL::isSameKindAs( const osg::Object* object ) const
{
    return ( dynamic_cast< const WGERenderNodeCL* >( object ) != 0 );
}

const char* WGERenderNodeCL::libraryName() const
{
    return "WGE";
}

const char* WGERenderNodeCL::className() const
{
    return "WGERenderNodeCL";
}

void WGERenderNodeCL::accept( osg::NodeVisitor& nv )  // NOLINT - non const ref.
{
    if ( m_deactivated || !m_module.valid() )
    {
        return;
    }

    ++m_count;

    if ( m_deactivated )
    {
        --m_count;

        return;
    }

    Node::accept( nv );

    --m_count;
}

void WGERenderNodeCL::traverse( osg::NodeVisitor& nv )         // NOLINT - non const ref.
{
    if ( nv.getVisitorType() == osg::NodeVisitor::CULL_VISITOR )
    {
        osgUtil::CullVisitor& cv = reinterpret_cast< osgUtil::CullVisitor& >( nv );

        // cull operations --------------------------------------------------------------------------------------------

        if ( !cv.getCurrentCullingSet().getFrustum().contains( m_module->getBound() ) )
        {
            return;
        }

        // fit near and far planes if necessary -----------------------------------------------------------------------

        if ( cv.getComputeNearFarMode() != osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR )
        {
            cv.updateCalculatedNearFar( *cv.getModelViewMatrix(), m_module->getBound() );
        }

        // create CLRenderBin -----------------------------------------------------------------------------------------

        osgUtil::RenderStage* stage = cv.getRenderStage();

        CLRenderBin* renderBin = CLRenderBin::getOrCreateRenderBin( stage );

        renderBin->m_nodes.push_back( this );
        renderBin->m_modelViews.push_back( cv.getModelViewMatrix() );
        renderBin->m_projections.push_back( cv.getProjectionMatrix() );

        // create CLDrawBin -------------------------------------------------------------------------------------------

        CLDrawBin* drawBin = CLDrawBin::getOrCreateDrawBin( stage );

        drawBin->m_nodes.push_back( this );

        ++m_count;
    }
}

osg::BoundingSphere WGERenderNodeCL::computeBound() const
{
    if ( m_module.valid() )
    {
        return osg::BoundingSphere( m_module->getBound() );
    }
    else
    {
        return osg::BoundingSphere();
    }
}

void WGERenderNodeCL::resizeGLObjectBuffers( unsigned int maxSize )
{
    m_perContextInformation.resize( maxSize );

    m_colorBufferTex->resizeGLObjectBuffers( maxSize );
    m_depthBufferTex->resizeGLObjectBuffers( maxSize );

    m_drawQuad.m_drawState->resizeGLObjectBuffers( maxSize );
    m_drawQuad.m_vertices->getVertexBufferObject()->resizeGLObjectBuffers( maxSize );

    Node::resizeGLObjectBuffers( maxSize );
}

void WGERenderNodeCL::releaseGLObjects( osg::State* state )
{
    if ( state == 0 )
    {
        unsigned int size = m_perContextInformation.size();

        for ( unsigned int i = 0; i < size; i++ )
        {
            m_perContextInformation[ i ].reset();
        }
    }
    else
    {
        m_perContextInformation[ state->getContextID() ].reset();
    }

    m_colorBufferTex->releaseGLObjects( state );
    m_depthBufferTex->releaseGLObjects( state );

    m_drawQuad.m_drawState->releaseGLObjects( state );
    m_drawQuad.m_vertices->getVertexBufferObject()->releaseGLObjects( state );

    Node::releaseGLObjects( state );
}

void WGERenderNodeCL::setDeactivated( bool deactivated ) const
{
    m_deactivated = deactivated;

    if ( m_deactivated )
    {
        while ( m_count != 0 );
    }
}

bool WGERenderNodeCL::initCL( PerContextInformation& perContextInfo ) const   // NOLINT - non const ref.
{
    #if defined( __APPLE__ )

        cl_context_properties properties[ 5 ] =
        {
            CL_CONTEXT_PLATFORM,
            0,
            CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
            reinterpret_cast< cl_context_properties >( CGLGetCurrentContext() ),
            0
        };

    #elif defined( __linux__ )

        cl_context_properties properties[ 7 ] =
        {
            CL_CONTEXT_PLATFORM,
            0,
            CL_GL_CONTEXT_KHR,
            reinterpret_cast< cl_context_properties >( glXGetCurrentContext() ),
            CL_GLX_DISPLAY_KHR,
            reinterpret_cast< cl_context_properties >( glXGetCurrentDisplay() ),
            0
        };

    #else

        cl_context_properties properties[ 7 ] =
        {
            CL_CONTEXT_PLATFORM,
            0,
            CL_GL_CONTEXT_KHR,
            reinterpret_cast< cl_context_properties >( wglGetCurrentContext() ),
            CL_WGL_HDC_KHR,
            reinterpret_cast< cl_context_properties >( wglGetCurrentDC() ),
            0
        };

    #endif

    cl_int error = 0;

    unsigned int sizePlatforms;
    unsigned int sizeDevices;

    std::vector< cl::Platform > platforms;
    std::vector< cl::Device > devices;

    cl::Device& device = perContextInfo.m_clViewData.m_device;
    cl::Context& context = perContextInfo.m_clViewData.m_context;
    cl::CommandQueue& commQueue = perContextInfo.m_clViewData.m_commQueue;

    cl::Platform::get( &platforms );

    sizePlatforms = platforms.size();

    // create context -------------------------------------------------------------------------------------------------

    for ( unsigned int i = 0; i < sizePlatforms; i++ )
    {
        platforms[ i ].getDevices( CL_DEVICE_TYPE_GPU, &devices );

        sizeDevices = devices.size();

        for ( unsigned int j = 0; j < sizeDevices; j++ )
        {
            properties[ 1 ] = reinterpret_cast< cl_context_properties >( platforms[ i ]() );

            context = cl::Context( std::vector< cl::Device >( 1, devices[ j ] ), properties, 0, 0, &error );

            if ( error == CL_SUCCESS )
            {
                device = devices[ j ];

                i = sizePlatforms;

                break;
            }
        }
    }

    if ( error != CL_SUCCESS )
    {
        return true;
    }

    // create command queue -------------------------------------------------------------------------------------------

    commQueue = cl::CommandQueue( context, device, 0, &error );

    if ( error != CL_SUCCESS )
    {
        return true;
    }

    return false;
}

bool WGERenderNodeCL::initBuffers( PerContextInformation& perContextInfo, osg::State& state ) const        // NOLINT - non const ref.
{
    // resize color and depth texture ---------------------------------------------------------------------------------

    m_colorBufferTex->setTextureSize
    (
        perContextInfo.m_clViewData.m_width,
        perContextInfo.m_clViewData.m_height
    );

    m_depthBufferTex->setTextureSize
    (
        perContextInfo.m_clViewData.m_width,
        perContextInfo.m_clViewData.m_height
    );

    m_colorBufferTex->dirtyTextureObject();
    m_depthBufferTex->dirtyTextureObject();

    state.applyTextureAttribute( 0, m_colorBufferTex );
    state.applyTextureAttribute( 1, m_depthBufferTex );

    // create colorBuffer and depthBuffer -----------------------------------------------------------------------------

    cl_int error;

    perContextInfo.m_clViewData.m_buffers[ 0 ] = cl::Image2DGL
    (
        perContextInfo.m_clViewData.m_context,
        CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0,
        m_colorBufferTex->getTextureObject( state.getContextID() )->_id,
        &error
    );

    if ( error != CL_SUCCESS )
    {
        return true;
    }

    perContextInfo.m_clViewData.m_buffers[ 1 ] = cl::Image2DGL
    (
        perContextInfo.m_clViewData.m_context,
        CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0,
        m_depthBufferTex->getTextureObject( state.getContextID() )->_id,
        &error
    );

    if ( error != CL_SUCCESS )
    {
        return true;
    }

    // set buffer kernel arguments ------------------------------------------------------------------------------------

    m_module->setBuffers( perContextInfo.m_clViewData, *perContextInfo.m_clData );

    return false;
}

void WGERenderNodeCL::render( osg::State& state, osg::RefMatrix* mvm, osg::RefMatrix* pm ) const // NOLINT - non const ref.
{
    PerContextInformation& perContextInfo = m_perContextInformation[ state.getContextID() ];

    // initialize CL objects if necessary -----------------------------------------------------------------------------

    if ( perContextInfo.m_invalid )
    {
        if ( initCL( perContextInfo ) )
        {
            return;
        }

        perContextInfo.m_clData = m_module->initCLData( perContextInfo.m_clViewData );

        if ( perContextInfo.m_clData == 0 )
        {
            return;
        }
    }

    // initialize or resize buffers if necessary ----------------------------------------------------------------------

    cl_uint& width = perContextInfo.m_clViewData.m_width;
    cl_uint& height = perContextInfo.m_clViewData.m_height;

    cl_uint currentWidth = static_cast< cl_uint >( state.getCurrentViewport()->width() );
    cl_uint currentHeight = static_cast< cl_uint >( state.getCurrentViewport()->height() );

    if ( perContextInfo.m_invalid || ( width != currentWidth ) || ( height != currentHeight ) )
    {
        width = currentWidth;
        height = currentHeight;

        if ( initBuffers( perContextInfo, state ) )
        {
            perContextInfo.m_invalid = true;

            return;
        }
    }

    // acquire buffers ------------------------------------------------------------------------------------------------

    cl_int error = perContextInfo.m_clViewData.m_commQueue.enqueueAcquireGLObjects
    (
        reinterpret_cast< std::vector< cl::Memory >* >( &perContextInfo.m_clViewData.m_buffers ), 0, 0
    );

    if ( error != CL_SUCCESS )
    {
        perContextInfo.m_invalid = true;

        return;
    }

    perContextInfo.m_clViewData.m_modelView = mvm;
    perContextInfo.m_clViewData.m_projection = pm;

    // render ---------------------------------------------------------------------------------------------------------

    bool renderError = m_module->render( perContextInfo.m_clViewData, *perContextInfo.m_clData );

    perContextInfo.m_clViewData.m_modelView = 0;
    perContextInfo.m_clViewData.m_projection = 0;

    // release buffers ------------------------------------------------------------------------------------------------

    error = perContextInfo.m_clViewData.m_commQueue.enqueueReleaseGLObjects
    (
        reinterpret_cast< std::vector< cl::Memory >* >( &perContextInfo.m_clViewData.m_buffers ), 0, 0
    );

    if ( ( error != CL_SUCCESS ) || renderError )
    {
        perContextInfo.m_invalid = true;

        return;
    }

    perContextInfo.m_invalid = false;
}

void WGERenderNodeCL::draw( osg::State& state ) const          // NOLINT - non const ref.
{
    PerContextInformation& perContextInfo = m_perContextInformation[ state.getContextID() ];

    if ( perContextInfo.m_invalid )
    {
        perContextInfo.reset();
    }
    else
    {
        perContextInfo.m_clViewData.m_commQueue.finish();

        state.applyTextureAttribute( 0, m_colorBufferTex );
        state.applyTextureAttribute( 1, m_depthBufferTex );

        glDrawArrays( GL_QUADS, 0, 4 );
    }

    --m_count;
}

