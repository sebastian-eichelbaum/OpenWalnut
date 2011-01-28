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

#include <boost/thread/locks.hpp>

#include <osg/BufferObject>
#include <osg/Matrix>
#include <osg/Notify>
#include <osg/Program>
#include <osg/Shader>
#include <osg/Uniform>
#include <osgUtil/CullVisitor>

#include "WCLRenderNode.h"

#if defined ( __APPLE__ ) || defined ( MACOSX )
    #include <Carbon/Carbon.h>
#else
    #if defined ( __linux__ )
        #include <GL/glx.h>
    #else
        #include <windows.h>
    #endif
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

WCLRenderNode::DrawQuad WCLRenderNode::m_drawQuad;

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::CLViewInformation::CLViewInformation(): m_width( 0 ), m_height( 0 )
{}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::CLViewInformation::getViewProperties( ViewProperties& properties ) const
{
    const osg::Matrix& pm = m_state->getProjectionMatrix();

    double pNear, pFar, pLeft, pRight, pTop, pBottom;

    if ( pm( 3, 3 ) == 0.0 )
    {
        // perspective projection
        //
        //  pm :
        //
        //  ( 2 * near / (right - left)         |0                                  |0                              |0  )
        //  ( 0                                 |2 * near / (top - bottom)          |0                              |0  )
        //  ( (right + left) / (right - left)   |(top + bottom) / (top - bottom)    |-(far + near) / (far - near)   |-1 )
        //  ( 0                                 |0                                  |-2 * far * near / (far - near) |0  )

        properties.m_type = PERSPECTIVE;

        pNear = pm( 3, 2 ) / ( pm( 2, 2 ) - 1.0 );
        pFar = pm( 3, 2 ) / ( pm( 2, 2 ) + 1.0 );

        pLeft = pNear * ( pm( 2, 0 ) - 1.0 ) / pm( 0, 0 );
        pRight = pNear * ( pm( 2, 0 ) + 1.0 ) / pm( 0, 0 );

        pBottom = pNear * ( pm( 2, 1 ) - 1.0 ) / pm( 1, 1 );
        pTop = pNear * ( pm( 2, 1 ) + 1.0 ) / pm( 1, 1 );
    }
    else
    {
        // orthographic projection
        //
        //  pm :
        //
        //  ( 2 / (right - left)                |0                                  |0                              |0 )
        //  ( 0                                 |2 / (top - bottom)                 |0                              |0 )
        //  ( 0                                 |0                                  |-2 / (far - near)              |0 )
        //  ( -(right + left) / (right - left)  |-(top + bottom) / (top - bottom)   |-(far + near) / (far - near)   |1 )

        properties.m_type = ORTHOGRAPHIC;

        pNear = ( pm( 3, 2 ) + 1.0 ) / pm( 2, 2 );
        pFar = ( pm( 3,2 ) - 1.0 ) / pm( 2, 2 );

        pLeft = -( 1.0 + pm( 3, 0 ) ) / pm( 0, 0 );
        pRight = ( 1.0 - pm( 3, 0 ) ) / pm( 0, 0 );

        pBottom = -( 1.0 + pm( 3, 1 ) ) / pm( 1, 1 );
        pTop = ( 1.0 - pm( 3, 1 ) ) / pm( 1, 1 );
    }

    properties.m_planeNear = pNear;
    properties.m_planeFar = pFar;

    const osg::Matrix& mvm = m_state->getModelViewMatrix();

    //  mvm :
    //
    //  ( RS[0][0]  |RS[0][1]   |RS[0][2]   |0 )
    //  ( RS[1][0]  |RS[1][1]   |RS[1][2]   |0 )
    //  ( RS[2][0]  |RS[2][2]   |RS[2][2]   |0 )
    //  ( tx        |ty         |tz         |1 )
    //
    //  Inverse(mvm) :
    //
    //  ( Inverse(RS)[0][0] Inverse(RS)[0][1]   Inverse(RS)[0][2]   0 )
    //  ( Inverse(RS)[1][0] Inverse(RS)[1][1]   Inverse(RS)[1][2]   0 )
    //  ( Inverse(RS)[2][0] Inverse(RS)[2][2]   Inverse(RS)[2][2]   0 )
    //  ( invTx             invTy               invTz               1 )

    double invMvm[ 4 ][ 3 ];
    double invDet;

    invMvm[ 0 ][ 0 ] = mvm( 1, 1 ) * mvm( 2, 2 ) - mvm( 1, 2 ) * mvm( 2, 1 );
    invMvm[ 0 ][ 1 ] = mvm( 0, 2 ) * mvm( 2, 1 ) - mvm( 0, 1 ) * mvm( 2, 2 );
    invMvm[ 0 ][ 2 ] = mvm( 0, 1 ) * mvm( 1, 2 ) - mvm( 0, 2 ) * mvm( 1, 1 );
    invMvm[ 1 ][ 0 ] = mvm( 1, 2 ) * mvm( 2, 0 ) - mvm( 1, 0 ) * mvm( 2, 2 );
    invMvm[ 1 ][ 1 ] = mvm( 0, 0 ) * mvm( 2, 2 ) - mvm( 0, 2 ) * mvm( 2, 0 );
    invMvm[ 1 ][ 2 ] = mvm( 0, 2 ) * mvm( 1, 0 ) - mvm( 0, 0 ) * mvm( 1, 2 );
    invMvm[ 2 ][ 0 ] = mvm( 1, 0 ) * mvm( 2, 1 ) - mvm( 1, 1 ) * mvm( 2, 0 );
    invMvm[ 2 ][ 1 ] = mvm( 0, 1 ) * mvm( 2, 0 ) - mvm( 0, 0 ) * mvm( 2, 1 );
    invMvm[ 2 ][ 2 ] = mvm( 0, 0 ) * mvm( 1, 1 ) - mvm( 0, 1 ) * mvm( 1, 0 );

    invDet = 1.0 / ( mvm( 0, 0 ) * invMvm[ 0 ][ 0 ] + mvm( 1, 0 ) * invMvm[ 0 ][ 1 ] + mvm( 2, 0 ) * invMvm[ 0 ][ 2 ] );

    invMvm[ 0 ][ 0 ] *= invDet;
    invMvm[ 0 ][ 1 ] *= invDet;
    invMvm[ 0 ][ 2 ] *= invDet;
    invMvm[ 1 ][ 0 ] *= invDet;
    invMvm[ 1 ][ 1 ] *= invDet;
    invMvm[ 1 ][ 2 ] *= invDet;
    invMvm[ 2 ][ 0 ] *= invDet;
    invMvm[ 2 ][ 1 ] *= invDet;
    invMvm[ 2 ][ 2 ] *= invDet;

    // (invTx,invTy,invTz) = -(tx,ty,tz) * Inverse(RS)

    invMvm[ 3 ][ 0 ] = -( mvm( 3, 0 ) * invMvm[ 0 ][ 0 ] + mvm( 3, 1 ) * invMvm[ 1 ][ 0 ] + mvm( 3, 2 ) * invMvm[ 2 ][ 0 ] );
    invMvm[ 3 ][ 1 ] = -( mvm( 3, 0 ) * invMvm[ 0 ][ 1 ] + mvm( 3, 1 ) * invMvm[ 1 ][ 1 ] + mvm( 3, 2 ) * invMvm[ 2 ][ 1 ] );
    invMvm[ 3 ][ 2 ] = -( mvm( 3, 0 ) * invMvm[ 0 ][ 2 ] + mvm( 3, 1 ) * invMvm[ 1 ][ 2 ] + mvm( 3, 2 ) * invMvm[ 2 ][ 2 ] );

    if ( properties.m_type == PERSPECTIVE )
    {
        // origin = (0,0,0,1) * Inverse(mvm)

        properties.m_origin.set( invMvm[ 3 ][ 0 ], invMvm[ 3 ][ 1 ], invMvm[ 3 ][ 2 ] );

        // originToLowerLeft = (left,bottom,-near,0) * Inverse(mvm)

        properties.m_originToLowerLeft.set
        (
            pLeft * invMvm[ 0 ][ 0 ] + pBottom * invMvm[ 1 ][ 0 ] - pNear * invMvm[ 2 ][ 0 ],
            pLeft * invMvm[ 0 ][ 1 ] + pBottom * invMvm[ 1 ][ 1 ] - pNear * invMvm[ 2 ][ 1 ],
            pLeft * invMvm[ 0 ][ 2 ] + pBottom * invMvm[ 1 ][ 2 ] - pNear * invMvm[ 2 ][ 2 ]
        );
    }
    else
    {
        // origin = (left,bottom,0,1) * Inverse(mvm)

        properties.m_origin.set
        (
            pLeft * invMvm[ 0 ][ 0 ] + pBottom * invMvm[ 1 ][ 0 ] + invMvm[ 3 ][ 0 ],
            pLeft * invMvm[ 0 ][ 1 ] + pBottom * invMvm[ 1 ][ 1 ] + invMvm[ 3 ][ 1 ],
            pLeft * invMvm[ 0 ][ 2 ] + pBottom * invMvm[ 1 ][ 2 ] + invMvm[ 3 ][ 2 ]
        );


        // originToLowerLeft = (0,0,-near,0) * Inverse(mvm)

        properties.m_originToLowerLeft.set
        (
            -pNear * invMvm[ 2 ][ 0 ],
            -pNear * invMvm[ 2 ][ 1 ],
            -pNear * invMvm[ 2 ][ 2 ]
        );
    }

    // edgeX = (right - left,0,0,0) * Inverse(mvm)

    properties.m_edgeX.set
    (
	    ( pRight - pLeft ) * invMvm[ 0 ][ 0 ],
	    ( pRight - pLeft ) * invMvm[ 0 ][ 1 ],
	    ( pRight - pLeft ) * invMvm[ 0 ][ 2 ]
    );

    // edgeY = (0,top - bottom,0,0) * Inverse(mvm)

    properties.m_edgeY.set
    (
	    ( pTop - pBottom ) * invMvm[ 1 ][ 0 ],
	    ( pTop - pBottom ) * invMvm[ 1 ][ 1 ],
	    ( pTop - pBottom ) * invMvm[ 1 ][ 2 ]
    );
}

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::CLProgramDataSet::~CLProgramDataSet()
{}

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::PerContextInformation::PerContextInformation():
    m_contextSharing( true ),
    m_clInitialized( false ),
    m_buffersInitialized( false ),
    m_initializationError( false )
{}

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::PerContextInformation::~PerContextInformation()
{
    reset();
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::PerContextInformation::reset()
{
    if ( m_buffersInitialized )
    {
        clReleaseMemObject( m_clViewInfo.m_colorBuffer );
        clReleaseMemObject( m_clViewInfo.m_depthBuffer );

        m_buffersInitialized = false;
    }

    if ( m_clInitialized )
    {
        delete m_clProgramDataSet;

        clReleaseCommandQueue( m_clViewInfo.m_commQueue );
        clReleaseContext( m_clViewInfo.m_context );

        m_clInitialized = false;
    }

    m_initializationError = false;
    m_contextSharing = true;
}

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::DrawQuad::DrawQuad()
{
    // create state

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

    // create vertices

    m_vertices = new osg::Vec2Array();

    m_vertices->push_back( osg::Vec2( -1.0f, -1.0f ) );
    m_vertices->push_back( osg::Vec2( -1.0f, 1.0f ) );
    m_vertices->push_back( osg::Vec2( 1.0f, 1.0f ) );
    m_vertices->push_back( osg::Vec2( 1.0f, -1.0f ) );

    // create coordinates

    m_coordinates = new osg::Vec2Array();

    m_coordinates->push_back( osg::Vec2( 0.0f, 0.0f ) );
    m_coordinates->push_back( osg::Vec2( 0.0f, 1.0f ) );
    m_coordinates->push_back( osg::Vec2( 1.0f, 1.0f ) );
    m_coordinates->push_back( osg::Vec2( 1.0f, 0.0f ) );

    // create VBO

    osg::VertexBufferObject* vbo = new osg::VertexBufferObject();

    vbo->setUsage( GL_STATIC_DRAW );

    m_vertices->setVertexBufferObject( vbo );
    m_coordinates->setVertexBufferObject( vbo );
}

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::CLRenderBin::CLRenderBin( osg::RefMatrix* modelView, osg::RefMatrix* projection ):
    RenderBin(),
    m_modelView( modelView ),
    m_projection( projection )
{}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::CLRenderBin::draw( osg::RenderInfo& renderInfo, osgUtil::RenderLeaf*& previous )
{
    osg::State& state = *renderInfo.getState();

    // apply matrices to state

    state.applyModelViewMatrix( m_modelView.get() );
    state.applyProjectionMatrix( m_projection.get() );

    // initialize rendering

    for ( std::vector< osg::ref_ptr< WCLRenderNode > >::iterator it = m_nodes.begin(); it != m_nodes.end(); it++ )
    {
        ( *it )->renderStart( state );
    }
}

/*-------------------------------------------------------------------------------------------------------------------*/

unsigned int WCLRenderNode::CLRenderBin::computeNumberOfDynamicRenderLeaves() const
{
    return 0;
}

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::CLRenderBin* WCLRenderNode::CLRenderBin::getOrCreateRenderBin( osgUtil::RenderStage* stage,
                                                                              osg::RefMatrix* modelView,
                                                                              osg::RefMatrix* projection )
{
    RenderBinList& binList = stage->getRenderBinList();

    // look for existing CLRenderBin

    for ( RenderBinList::iterator it = binList.begin(); ( it != binList.end() ) && ( it->first < 0 ); it++ )
    {
        WCLRenderNode::CLRenderBin* bin = dynamic_cast< WCLRenderNode::CLRenderBin* >( it->second.get() );

        if ( bin != 0 )
        {
            return bin;
        }
    }

    // determine bin number

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

    // create CLRenderBin

    WCLRenderNode::CLRenderBin* bin = new CLRenderBin( modelView, projection );

    bin->_binNum = binNum;
    bin->_parent = stage;
    bin->_stage = stage;

    binList[ binNum ] = bin;

    return bin;
}

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::CLDrawBin::CLDrawBin(): RenderBin(), m_dynamicNodes( 0 )
{}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::CLDrawBin::draw( osg::RenderInfo& renderInfo, osgUtil::RenderLeaf*& previous )
{
    osg::State& state = *renderInfo.getState();

    // add camera attributes to current GL state if not already done

    if ( previous == 0 )
    {
        state.pushStateSet( renderInfo.getCurrentCamera()->getStateSet() );
    }

    // prepare the quad for drawing

    state.apply( WCLRenderNode::m_drawQuad.m_drawState );
    state.setVertexPointer( WCLRenderNode::m_drawQuad.m_vertices );
    state.setTexCoordPointer( 0, WCLRenderNode::m_drawQuad.m_coordinates );

    // draw the rendered nodes

    for ( std::vector< osg::ref_ptr< WCLRenderNode > >::iterator it = m_nodes.begin(); it != m_nodes.end(); it++ )
    {
        WCLRenderNode* node = *it;

        state.applyTextureAttribute( 0, node->m_colorBuffer );
        state.applyTextureAttribute( 1, node->m_depthBuffer );

        node->draw( state );

        // update the states dynamic object count if necessary

        if ( node->getDataVariance() == DYNAMIC )
        {
            state.decrementDynamicObjectCount();
        }
    }
}

/*-------------------------------------------------------------------------------------------------------------------*/

unsigned int WCLRenderNode::CLDrawBin::computeNumberOfDynamicRenderLeaves() const
{
    return m_dynamicNodes;
}

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::CLDrawBin* WCLRenderNode::CLDrawBin::getOrCreateDrawBin( osgUtil::RenderStage* stage )
{
    RenderBinList& binList = stage->getRenderBinList();

    // look for existing CLDrawBin

    for ( RenderBinList::reverse_iterator it = binList.rbegin(); ( it != binList.rend() ) && ( it->first > 0 ); it++ )
    {
        WCLRenderNode::CLDrawBin* bin = dynamic_cast< WCLRenderNode::CLDrawBin* >( it->second.get() );

        if ( bin != 0 )
        {
            return bin;
        }
    }

    // determine bin number

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

    // create CLDrawBin

    WCLRenderNode::CLDrawBin* bin = new CLDrawBin();

    bin->_binNum = binNum;
    bin->_parent = stage;
    bin->_stage = stage;

    binList[ binNum ] = bin;

    return bin;
}

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::WCLRenderNode( bool deactivated ): Node(), m_deactivated( deactivated )
{
    // create dummy callback to avoid unnecessary texture loads

    class NoOpCallback: public osg::Texture2D::SubloadCallback
    {

    public:

        virtual void load( const osg::Texture2D& texture, osg::State& state ) const
        {};

        virtual void subload( const osg::Texture2D& texture, osg::State& state ) const
        {};
    };

    NoOpCallback* callback = new NoOpCallback();

    // create color buffer

    m_colorBuffer = new osg::Texture2D();

    m_colorBuffer->setInternalFormat( GL_RGBA32F );
    m_colorBuffer->setFilter( osg::Texture::MIN_FILTER, osg::Texture::NEAREST );
    m_colorBuffer->setFilter( osg::Texture::MAG_FILTER, osg::Texture::NEAREST );
    m_colorBuffer->setSubloadCallback( callback );

    // create depth buffer

    m_depthBuffer = new osg::Texture2D();

    m_depthBuffer->setInternalFormat( GL_R32F );
    m_depthBuffer->setFilter( osg::Texture::MIN_FILTER, osg::Texture::NEAREST );
    m_depthBuffer->setFilter( osg::Texture::MAG_FILTER, osg::Texture::NEAREST );
    m_depthBuffer->setSubloadCallback( callback );
}

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::WCLRenderNode( const WCLRenderNode& node, const osg::CopyOp& copyop ):
    Node( node, copyop ),
    m_box( node.m_box ),
    m_perContextInformation(),
    m_colorBuffer( new osg::Texture2D( *node.m_colorBuffer, copyop ) ),
    m_depthBuffer( new osg::Texture2D( *node.m_depthBuffer, copyop ) ),
    m_deactivated( node.m_deactivated ),
    m_wait(),
    m_mutex()
{}

/*-------------------------------------------------------------------------------------------------------------------*/

WCLRenderNode::~WCLRenderNode()
{
    reset();
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::traverse( osg::NodeVisitor& nv )
{
    if ( nv.getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR )
    {
        m_wait.notify_one();
    }

    if ( nv.getVisitorType() == osg::NodeVisitor::CULL_VISITOR )
    {
        // node mustn't be drawn if it's deactivated

        boost::unique_lock< boost::mutex > lock( m_mutex );

        if ( m_deactivated )
        {
            return;
        }

        lock.unlock();

        // cull operations

        osgUtil::CullVisitor* cv = static_cast< osgUtil::CullVisitor* >( &nv );

        if ( !cv->getCurrentCullingSet().getFrustum().contains( m_box ) )
        {
            return;
        }

        // fit near and far planes if necessary

        if ( cv->getComputeNearFarMode() != osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR )
        {
            cv->updateCalculatedNearFar( *cv->getModelViewMatrix(), m_box );
        }

        // create CLRenderBin

        osgUtil::RenderStage* stage = cv->getRenderStage();

        CLRenderBin* renderBin = CLRenderBin::getOrCreateRenderBin( stage, cv->getModelViewMatrix(), cv->getProjectionMatrix() );

        renderBin->m_nodes.push_back( this );

        // create CLDrawBin

        CLDrawBin* drawBin = CLDrawBin::getOrCreateDrawBin( stage );

        drawBin->m_nodes.push_back( this );

        // check data variance

        if ( getDataVariance() == DYNAMIC )
        {
            drawBin->m_dynamicNodes++;
        }
    }
}

/*-------------------------------------------------------------------------------------------------------------------*/

osg::BoundingSphere WCLRenderNode::computeBound() const
{
    m_box = computeBoundingBox();

    return osg::BoundingSphere( m_box );
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::resizeGLObjectBuffers( unsigned int maxSize )
{
    m_perContextInformation.resize( maxSize );

    m_colorBuffer->resizeGLObjectBuffers( maxSize );
    m_depthBuffer->resizeGLObjectBuffers( maxSize );

    m_drawQuad.m_drawState->resizeGLObjectBuffers( maxSize );
    m_drawQuad.m_vertices->getVertexBufferObject()->resizeGLObjectBuffers( maxSize );

    Node::resizeGLObjectBuffers( maxSize );
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::releaseGLObjects( osg::State* state )
{
    // release CL objects

    if ( state == 0 )
    {
        reset();
    }
    else
    {
        m_perContextInformation[ state->getContextID() ].reset();
    }

    // release GL objects

    m_colorBuffer->releaseGLObjects( state );
    m_depthBuffer->releaseGLObjects( state );

    m_drawQuad.m_drawState->releaseGLObjects( state );
    m_drawQuad.m_vertices->getVertexBufferObject()->releaseGLObjects( state );

    Node::releaseGLObjects( state );
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

    for ( unsigned int i = 0; i < size; i++ )
    {
        if ( m_perContextInformation[ i ].m_initializationError )
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

    for ( unsigned int i = 0; i < size; i++ )
    {
        m_perContextInformation[ i ].reset();
    }
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::activate() const
{
    boost::unique_lock< boost::mutex > lock( m_mutex );

    m_deactivated = false;
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::deactivate() const
{
    boost::unique_lock< boost::mutex > lock( m_mutex );

    m_deactivated = true;

    m_wait.wait( lock );
}

/*-------------------------------------------------------------------------------------------------------------------*/

bool WCLRenderNode::isDeactivated() const
{
    return m_deactivated;
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::setDeactivated( bool deactivated ) const
{
    m_deactivated = deactivated;
}

/*-------------------------------------------------------------------------------------------------------------------*/

std::string WCLRenderNode::getCLError( cl_int clError )
{
    switch ( clError )
    {
        case CL_SUCCESS:                            return "CL_SUCCESS";
        case CL_DEVICE_NOT_FOUND:                   return "CL_DEVICE_NOT_FOUND";
        case CL_DEVICE_NOT_AVAILABLE:               return "CL_DEVICE_NOT_AVAILABLE";
        case CL_COMPILER_NOT_AVAILABLE:             return "CL_COMPILER_NOT_AVAILABLE";
        case CL_MEM_OBJECT_ALLOCATION_FAILURE:      return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
        case CL_OUT_OF_RESOURCES:                   return "CL_OUT_OF_RESOURCES";
        case CL_OUT_OF_HOST_MEMORY:                 return "CL_OUT_OF_HOST_MEMORY";
        case CL_PROFILING_INFO_NOT_AVAILABLE:       return "CL_PROFILING_INFO_NOT_AVAILABLE";
        case CL_MEM_COPY_OVERLAP:                   return "CL_MEM_COPY_OVERLAP";
        case CL_IMAGE_FORMAT_MISMATCH:              return "CL_IMAGE_FORMAT_MISMATCH";
        case CL_IMAGE_FORMAT_NOT_SUPPORTED:         return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
        case CL_BUILD_PROGRAM_FAILURE:              return "CL_BUILD_PROGRAM_FAILURE";
        case CL_MAP_FAILURE:                        return "CL_MAP_FAILURE";

        case CL_INVALID_VALUE:                      return "CL_INVALID_VALUE";
        case CL_INVALID_DEVICE_TYPE:                return "CL_INVALID_DEVICE_TYPE";
        case CL_INVALID_PLATFORM:                   return "CL_INVALID_PLATFORM";
        case CL_INVALID_DEVICE:                     return "CL_INVALID_DEVICE";
        case CL_INVALID_CONTEXT:                    return "CL_INVALID_CONTEXT";
        case CL_INVALID_QUEUE_PROPERTIES:           return "CL_INVALID_QUEUE_PROPERTIES";
        case CL_INVALID_COMMAND_QUEUE:              return "CL_INVALID_COMMAND_QUEUE";
        case CL_INVALID_HOST_PTR:                   return "CL_INVALID_HOST_PTR";
        case CL_INVALID_MEM_OBJECT:                 return "CL_INVALID_MEM_OBJECT";
        case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:    return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
        case CL_INVALID_IMAGE_SIZE:                 return "CL_INVALID_IMAGE_SIZE";
        case CL_INVALID_SAMPLER:                    return "CL_INVALID_SAMPLER";
        case CL_INVALID_BINARY:                     return "CL_INVALID_BINARY";
        case CL_INVALID_BUILD_OPTIONS:              return "CL_INVALID_BUILD_OPTIONS";
        case CL_INVALID_PROGRAM:                    return "CL_INVALID_PROGRAM";
        case CL_INVALID_PROGRAM_EXECUTABLE:         return "CL_INVALID_PROGRAM_EXECUTABLE";
        case CL_INVALID_KERNEL_NAME:                return "CL_INVALID_KERNEL_NAME";
        case CL_INVALID_KERNEL_DEFINITION:          return "CL_INVALID_KERNEL_DEFINITION";
        case CL_INVALID_KERNEL:                     return "CL_INVALID_KERNEL";
        case CL_INVALID_ARG_INDEX:                  return "CL_INVALID_ARG_INDEX";
        case CL_INVALID_ARG_VALUE:                  return "CL_INVALID_ARG_VALUE";
        case CL_INVALID_ARG_SIZE:                   return "CL_INVALID_ARG_SIZE";
        case CL_INVALID_KERNEL_ARGS:                return "CL_INVALID_KERNEL_ARGS";
        case CL_INVALID_WORK_DIMENSION:             return "CL_INVALID_WORK_DIMENSION";
        case CL_INVALID_WORK_GROUP_SIZE:            return "CL_INVALID_WORK_GROUP_SIZE";
        case CL_INVALID_WORK_ITEM_SIZE:             return "CL_INVALID_WORK_ITEM_SIZE";
        case CL_INVALID_GLOBAL_OFFSET:              return "CL_INVALID_GLOBAL_OFFSET";
        case CL_INVALID_EVENT_WAIT_LIST:            return "CL_INVALID_EVENT_WAIT_LIST";
        case CL_INVALID_EVENT:                      return "CL_INVALID_EVENT";
        case CL_INVALID_OPERATION:                  return "CL_INVALID_OPERATION";
        case CL_INVALID_GL_OBJECT:                  return "CL_INVALID_GL_OBJECT";
        case CL_INVALID_BUFFER_SIZE:                return "CL_INVALID_BUFFER_SIZE";
        case CL_INVALID_MIP_LEVEL:                  return "CL_INVALID_MIP_LEVEL";
        case CL_INVALID_GLOBAL_WORK_SIZE:           return "CL_INVALID_GLOBAL_WORK_SIZE";
    }

    return "";
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::changeDataSet( const CLDataChangeCallback& callback )
{
    unsigned int size = m_perContextInformation.size();

    for ( unsigned int i = 0; i < size; i++ )
    {
        PerContextInformation& perContextInfo = m_perContextInformation[ i ];

        if ( perContextInfo.m_clInitialized )
        {
            callback.change( perContextInfo.m_clProgramDataSet );
        }
    }
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::renderStart( osg::State& state ) const
{
    PerContextInformation& perContextInfo = m_perContextInformation[ state.getContextID() ];

    // do nothing if an initialization attempt has failed already

    if ( perContextInfo.m_initializationError )
    {
        return;
    }

    // initialize CL objects if required

    if ( !perContextInfo.m_clInitialized )
    {
        if ( !initCL( perContextInfo ) )
        {
            perContextInfo.m_initializationError = true;

            return;
        }

        perContextInfo.m_clProgramDataSet = initProgram( perContextInfo.m_clViewInfo );

        if ( perContextInfo.m_clProgramDataSet == 0 )
        {
            perContextInfo.m_initializationError = true;

            return;
        }

        perContextInfo.m_clInitialized = true;
    }

    // initialize or resize buffers if required

    const osg::Viewport* currentViewport = state.getCurrentViewport();

    unsigned int currentWidth = currentViewport->width();
    unsigned int currentHeight = currentViewport->height();

    if ( !perContextInfo.m_buffersInitialized ||
         ( currentWidth != perContextInfo.m_clViewInfo.m_width ) ||
         ( currentHeight != perContextInfo.m_clViewInfo.m_height ) )
    {
        perContextInfo.m_clViewInfo.m_width = currentWidth;
        perContextInfo.m_clViewInfo.m_height = currentHeight;

        if ( !initBuffers( perContextInfo, state ) )
        {
            perContextInfo.m_initializationError = true;

            return;
        }

        perContextInfo.m_buffersInitialized = true;
    }

    // start rendering

    perContextInfo.m_clViewInfo.m_state = &state;

    if ( perContextInfo.m_contextSharing )
    {
        glFinish();

        cl_int clError;

        cl_mem glObjects[ 2 ] = { perContextInfo.m_clViewInfo.m_colorBuffer, perContextInfo.m_clViewInfo.m_depthBuffer };

        clError = clEnqueueAcquireGLObjects( perContextInfo.m_clViewInfo.m_commQueue, 2, glObjects, 0, 0, 0 );

        if ( clError != CL_SUCCESS )
        {
            osg::notify( osg::FATAL ) << "Could not aquire the buffers: " << getCLError( clError ) << std::endl;

            return;
        }

        render( perContextInfo.m_clViewInfo, perContextInfo.m_clProgramDataSet );

        clError = clEnqueueReleaseGLObjects( perContextInfo.m_clViewInfo.m_commQueue, 2, glObjects, 0, 0, 0 );

        if ( clError != CL_SUCCESS )
        {
            osg::notify( osg::FATAL ) << "Could not release the buffers: " << getCLError( clError ) << std::endl;

            return;
        }
    }
    else
    {
        render( perContextInfo.m_clViewInfo, perContextInfo.m_clProgramDataSet );
    }
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WCLRenderNode::draw( osg::State& state ) const
{
    PerContextInformation& perContextInfo = m_perContextInformation[ state.getContextID() ];

    // do nothing if an initialization attempt has failed already

    if ( perContextInfo.m_initializationError )
    {
        return;
    }

    // draw

    if ( perContextInfo.m_contextSharing )
    {
        clFinish( perContextInfo.m_clViewInfo.m_commQueue );
    }
    else
    {
        // load rendered content to main memory

        cl_int clError;

        float* colorData = new float[ perContextInfo.m_clViewInfo.m_width * perContextInfo.m_clViewInfo.m_height * 4 ];
        float* depthData = new float[ perContextInfo.m_clViewInfo.m_width * perContextInfo.m_clViewInfo.m_height ];

        size_t origin[ 3 ] = { 0, 0, 0 };
        size_t region[ 3 ] = { perContextInfo.m_clViewInfo.m_width, perContextInfo.m_clViewInfo.m_height, 1 };

        clError = clEnqueueReadImage
        (
            perContextInfo.m_clViewInfo.m_commQueue,
            perContextInfo.m_clViewInfo.m_colorBuffer,
            CL_TRUE, origin, region, 0, 0, colorData, 0, 0, 0
        );

        if ( clError != CL_SUCCESS )
        {
            osg::notify( osg::FATAL ) << "Could not read color buffer: " << getCLError( clError ) << std::endl;

            return;
        }

        clError = clEnqueueReadImage
        (
            perContextInfo.m_clViewInfo.m_commQueue,
            perContextInfo.m_clViewInfo.m_depthBuffer,
            CL_TRUE, origin, region, 0, 0, depthData, 0, 0, 0
        );

        if ( clError != CL_SUCCESS )
        {
            osg::notify( osg::FATAL ) << "Could not read depth buffer: " << getCLError( clError ) << std::endl;

            return;
        }

        // bind color and depth texture and load new content to texture memory

        state.setActiveTextureUnit( 0 );

        glTexSubImage2D
        (
            GL_TEXTURE_2D, 0, 0, 0,
            perContextInfo.m_clViewInfo.m_width, perContextInfo.m_clViewInfo.m_height,
            GL_RGBA, GL_FLOAT, colorData
        );

        state.setActiveTextureUnit( 1 );

        glTexSubImage2D
        (
            GL_TEXTURE_2D, 0, 0, 0,
            perContextInfo.m_clViewInfo.m_width, perContextInfo.m_clViewInfo.m_height,
            GL_RED, GL_FLOAT, depthData
        );

        delete[] colorData;
        delete[] depthData;
    }

    glDrawArrays( GL_QUADS, 0, 4 );
}

/*-------------------------------------------------------------------------------------------------------------------*/

bool WCLRenderNode::initCL( PerContextInformation& perContextInfo ) const
{
    cl_int clError;

    cl_uint numOfPlatforms;
    cl_platform_id* platforms;
    cl_device_id** devices;

    clGetPlatformIDs( 0, 0, &numOfPlatforms );

    if ( numOfPlatforms == 0 )
    {
        osg::notify( osg::FATAL ) << "Could not find OpenCL platforms." << std::endl;

        return false;
    }

    platforms = new cl_platform_id[ numOfPlatforms ];
    devices = new cl_device_id*[ numOfPlatforms ];

    // get available CL platforms

    clGetPlatformIDs( numOfPlatforms, platforms, 0 );

    cl_uint* numOfDevices = new cl_uint[ numOfPlatforms ];

    // get available CL devices

    for ( unsigned int i = 0; i < numOfPlatforms; i++ )
    {
        clGetDeviceIDs( platforms[ i ], CL_DEVICE_TYPE_GPU, 0, 0, &numOfDevices[ i ] );

        devices[ i ] = new cl_device_id[ numOfDevices[ i ] ];

        clGetDeviceIDs( platforms[ i ], CL_DEVICE_TYPE_GPU, numOfDevices[ i ], devices[ i ], 0 );
    }

    #if defined ( __APPLE__ ) || defined ( MACOSX )
        cl_context_properties properties[ 5 ];

        properties[ 2 ] = CL_CGL_SHAREGROUP_KHR;
        properties[ 3 ] = reinterpret_cast< cl_context_properties >( CGLGetCurrentContext() );
        properties[ 4 ] = 0;
    #else
        #if defined ( __linux__ )
            cl_context_properties properties[7];

            properties[ 2 ] = CL_GL_CONTEXT_KHR;
            properties[ 3 ] = reinterpret_cast< cl_context_properties >( glXGetCurrentContext() );
            properties[ 4 ] = CL_GLX_DISPLAY_KHR;
            properties[ 5 ] = reinterpret_cast< cl_context_properties >( glXGetCurrentDisplay() );
            properties[ 6 ] = 0;
        #else
            cl_context_properties properties[7];

            properties[ 2 ] = CL_GL_CONTEXT_KHR;
            properties[ 3 ] = reinterpret_cast< cl_context_properties >( wglGetCurrentContext() );
            properties[ 4 ] = CL_WGL_HDC_KHR;
            properties[ 5 ] = reinterpret_cast< cl_context_properties >( wglGetCurrentDC() );
            properties[ 6 ] = 0;
        #endif
    #endif

    properties[ 0 ] = CL_CONTEXT_PLATFORM;

    // create CL context with GL context sharing

    for ( unsigned int i = 0; i < numOfPlatforms; i++ )
    {
        for ( unsigned int j = 0; j < numOfDevices[ i ]; j++ )
        {
            properties[ 1 ] = reinterpret_cast< cl_context_properties >( platforms[ i ] );

            perContextInfo.m_clViewInfo.m_context = clCreateContext( properties, 1, &devices[ i ][ j ], 0, 0, &clError );

            if ( clError == CL_SUCCESS )
            {
                perContextInfo.m_clViewInfo.m_device = devices[ i ][ j ];

                i = numOfPlatforms;

                break;
            }
        }
    }

    // create CL context without GL context sharing if context creation failed

    if ( clError != CL_SUCCESS )
    {
        perContextInfo.m_contextSharing = false;

        properties[ 2 ] = 0;

        for ( unsigned int i = 0; i < numOfPlatforms; i++ )
        {
            for ( unsigned int j = 0; j < numOfDevices[ i ]; j++ )
            {
                properties[ 1 ] = reinterpret_cast< cl_context_properties >( platforms[ i ] );

                perContextInfo.m_clViewInfo.m_context = clCreateContext( properties, 1, &devices[ i ][ j ], 0, 0, &clError );

                if ( clError == CL_SUCCESS )
                {
                    perContextInfo.m_clViewInfo.m_device = devices[ i ][ j ];

                    i = numOfPlatforms;

                    break;
                }
            }
        }
    }

    for ( unsigned int i = 0; i < numOfPlatforms; i++ )
    {
        delete[] devices[ i ];
    }

    delete[] devices;
    delete[] platforms;
    delete[] numOfDevices;

    if ( clError != CL_SUCCESS )
    {
        osg::notify(osg::FATAL) << "Could not create an OpenCL context: " << getCLError( clError ) << std::endl;

        return false;
    }

    // create CL command queue

    perContextInfo.m_clViewInfo.m_commQueue = clCreateCommandQueue
    (
        perContextInfo.m_clViewInfo.m_context,
        perContextInfo.m_clViewInfo.m_device,
        0, &clError
    );

    if ( clError != CL_SUCCESS )
    {
        osg::notify( osg::FATAL ) << "Could not create an OpenCL command-queue: " << getCLError( clError ) << std::endl;

        clReleaseContext( perContextInfo.m_clViewInfo.m_context );

        return false;
    }

    return true;
}

/*-------------------------------------------------------------------------------------------------------------------*/

bool WCLRenderNode::initBuffers( PerContextInformation& perContextInfo, osg::State& state ) const
{
    // release existing buffers

    if ( perContextInfo.m_buffersInitialized )
    {
        clReleaseMemObject( perContextInfo.m_clViewInfo.m_colorBuffer );
        clReleaseMemObject( perContextInfo.m_clViewInfo.m_depthBuffer );
    }

    // resize color and depth texture

    state.applyTextureAttribute( 0, m_colorBuffer );

    glTexImage2D
    (
        GL_TEXTURE_2D, 0, GL_RGBA32F,
        perContextInfo.m_clViewInfo.m_width, perContextInfo.m_clViewInfo.m_height,
        0, GL_RGBA, GL_FLOAT, 0
    );

    state.applyTextureAttribute( 1, m_depthBuffer );

    glTexImage2D
    (
        GL_TEXTURE_2D, 0, GL_R32F,
        perContextInfo.m_clViewInfo.m_width, perContextInfo.m_clViewInfo.m_height,
        0, GL_RED, GL_FLOAT, 0
    );

    // create colorBuffer and depthBuffer

    cl_int clError;

    if ( perContextInfo.m_contextSharing )
    {
        unsigned int contextID = state.getContextID();

        perContextInfo.m_clViewInfo.m_colorBuffer = clCreateFromGLTexture2D
        (
            perContextInfo.m_clViewInfo.m_context,
            CL_MEM_READ_WRITE, GL_TEXTURE_2D, 0,
            m_colorBuffer->getTextureObject( contextID )->_id, &clError
        );

        if ( clError != CL_SUCCESS )
        {
            osg::notify( osg::FATAL ) << "Could not create the color buffer: " << getCLError( clError ) << std::endl;

            return false;
        }

        perContextInfo.m_clViewInfo.m_depthBuffer = clCreateFromGLTexture2D
        (
            perContextInfo.m_clViewInfo.m_context,
            CL_MEM_READ_WRITE, GL_TEXTURE_2D, 0,
            m_depthBuffer->getTextureObject(contextID)->_id, &clError
        );

        if ( clError != CL_SUCCESS )
        {
            osg::notify( osg::FATAL ) << "Could not create the depth buffer: " << getCLError( clError ) << std::endl;

            clReleaseMemObject( perContextInfo.m_clViewInfo.m_colorBuffer );

            return false;
        }
    }
    else
    {
	    cl_image_format format;

	    format.image_channel_order = CL_RGBA;
	    format.image_channel_data_type = CL_FLOAT;

	    perContextInfo.m_clViewInfo.m_colorBuffer = clCreateImage2D
	    (
            perContextInfo.m_clViewInfo.m_context,
            CL_MEM_READ_WRITE, &format,
            perContextInfo.m_clViewInfo.m_width, perContextInfo.m_clViewInfo.m_height,
            0, 0, &clError
	    );

	    if ( clError != CL_SUCCESS )
	    {
            osg::notify( osg::FATAL ) << "Could not create the color buffer: " << getCLError( clError ) << std::endl;

            return false;
	    }

	    format.image_channel_order = CL_R;

	    perContextInfo.m_clViewInfo.m_depthBuffer = clCreateImage2D
	    (
            perContextInfo.m_clViewInfo.m_context,
            CL_MEM_READ_WRITE, &format,
            perContextInfo.m_clViewInfo.m_width, perContextInfo.m_clViewInfo.m_height,
            0, 0, &clError
	    );

	    if ( clError != CL_SUCCESS )
	    {
            osg::notify( osg::FATAL ) << "Could not create the depth buffer: " << getCLError( clError ) << std::endl;

            clReleaseMemObject( perContextInfo.m_clViewInfo.m_colorBuffer );

            return false;
	    }
    }

    // set buffer kernel arguments

    setBuffers( perContextInfo.m_clViewInfo,perContextInfo.m_clProgramDataSet );

    return true;
}

/*-------------------------------------------------------------------------------------------------------------------*/
