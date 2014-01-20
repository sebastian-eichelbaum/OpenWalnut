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

#include <vector>
#include <string>

#include <osg/Depth>
#include <osg/Drawable>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Hint>
#include <osg/LineWidth>
#include <osg/Point>
#include <osg/PointSprite>
#include <osgDB/Export>
#include <osgDB/Registry>
#include <osgDB/WriteFile>

#include "core/common/WPropertyHelper.h"
#include "core/common/WPropertyObserver.h"
#include "core/dataHandler/WDataHandler.h"
#include "core/dataHandler/WDataSetFiberClustering.h"
#include "core/dataHandler/WDataSetFibers.h"
#include "core/graphicsEngine/WGEColormapping.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/callbacks/WGEFunctorCallback.h"
#include "core/graphicsEngine/callbacks/WGENodeMaskCallback.h"
#include "core/graphicsEngine/postprocessing/WGEPostprocessingNode.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/shaders/WGEShaderDefineOptions.h"
#include "core/graphicsEngine/shaders/WGEShaderPropertyDefineOptions.h"
#include "core/kernel/WKernel.h"

// Compatibility between OSG 3.2 and earlier versions
#include "core/graphicsEngine/WOSG.h"

#include "WMFiberDisplay.h"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMFiberDisplay )

WMFiberDisplay::WMFiberDisplay():
    WModule()
{
}

WMFiberDisplay::~WMFiberDisplay()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMFiberDisplay::factory() const
{
    return boost::shared_ptr< WModule >( new WMFiberDisplay() );
}

const std::string WMFiberDisplay::getName() const
{
    return "Fiber Display";
}

const std::string WMFiberDisplay::getDescription() const
{
    return "Display fibers. This module allows filtering by ROIs and provides full fletched graphical output.";
}

void WMFiberDisplay::connectors()
{
    m_fiberInput = WModuleInputData < WDataSetFibers >::createAndAdd( shared_from_this(), "fibers", "The fiber dataset to color" );
    m_fiberClusteringInput = WModuleInputData < WDataSetFiberClustering >::createAndAdd( shared_from_this(), "fiberClustering",
        "Optional input to filter the fibers using a clustering.");

    WModule::connectors();
}

void WMFiberDisplay::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_roiFiltering = m_properties->addProperty( "ROI Filtering", "When active, you can use the ROI mechanism to filter fibers.", true );
    m_roiFilterColors = m_properties->addProperty( "ROI Coloring", "When active, you will see the coloring specified by the ROI branches.", true );

    m_coloringGroup = m_properties->addPropertyGroup( "Coloring", "Options for defining the coloring of the lines." );
    m_illuminationEnable = m_coloringGroup->addProperty( "Illumination", "Enable line illumination.", true );
    m_plainColorMode = m_coloringGroup->addProperty( "Use plain color",
                            "When activated, the specified color is used for the lines instead of the dataset colors.", false, m_propCondition );
    m_plainColor = m_coloringGroup->addProperty( "Color", "Choose how to color the lines.", defaultColor::WHITE, m_propCondition );

    m_colormapEnabled = m_coloringGroup->addProperty( "Enable colormapping", "Check this to enable colormapping. On large data, this can cause "
                                                                              "massive FPS drop.", false );
    m_colormapRatio = m_coloringGroup->addProperty( "Colormap Ratio", "Defines the ratio between colormap and line color.", 0.0 );
    m_colormapRatio->setMin( 0.0 );
    m_colormapRatio->setMax( 1.0 );

    m_clipPlaneGroup = m_properties->addPropertyGroup( "Clipping",  "Clip the fiber data basing on an arbitrary plane." );
    m_clipPlaneEnabled = m_clipPlaneGroup->addProperty( "Enabled", "If set, clipping of fibers is done using an arbitrary plane and plane distance.",
                                                        false );
    m_clipPlaneShowPlane = m_clipPlaneGroup->addProperty( "Show Clip Plane", "If set, the clipping plane will be shown.", false );
    m_clipPlanePoint = m_clipPlaneGroup->addProperty( "Plane point", "An point on the plane.", WPosition( 0.0, 0.0, 0.0 ) );
    m_clipPlaneVector = m_clipPlaneGroup->addProperty( "Plane normal", "The normal of the plane.", WPosition( 1.0, 0.0, 0.0 ) );
    m_clipPlaneDistance= m_clipPlaneGroup->addProperty( "Clip distance", "The distance from the plane where fibers get clipped.",  10.0 );
    //m_clipPlaneDistance->removeConstraint( m_clipPlaneDistance->getMax() ); // there simply is no max.
    m_clipPlaneDistance->setMin( 0.0 );
    m_clipPlaneDistance->setMax( 1000.0 );

    m_lineGroup = m_properties->addPropertyGroup( "Line Rendering", "Line rendering specific options." );
    m_lineWidth = m_lineGroup->addProperty( "Width", "The line width.", 1.0 );
    m_lineWidth->setMin( 1.0 );
    m_lineWidth->setMax( 10.0 );
    m_lineSmooth = m_lineGroup->addProperty( "Anti-Alias", "Anti-aliased line rendering. This can be slow!", false );

    m_tubeGroup = m_properties->addPropertyGroup( "Tube Rendering", "Tube rendering specific options." );
    m_tubeEnable = m_tubeGroup->addProperty( "Enable Tubes", "If set, fake-tube rendering is used.", false, m_propCondition  );
    m_tubeEndCapsEnable = m_tubeGroup->addProperty( "Use end caps", "If set, fake-tube ends are rendered using another quad simulation a proper"
                                                                    " ending.", true );
    m_tubeRibbon = m_tubeGroup->addProperty( "Ribbon mode", "If set, the tubes look like flat ribbons.", false );
    m_tubeZoomable = m_tubeGroup->addProperty( "Zoomable", "If set, fake-tube get thicker when zoomed in. If not set, they always keep the same "
                                                            "size in screen-space. This emulates standard OpenGL lines.", true );
    m_tubeSize = m_tubeGroup->addProperty( "Width", "The size of the tubes.", 2.0 );
    m_tubeSize->setMin( 0.10 );
    m_tubeSize->setMax( 25.0 );

    // call WModule's initialization
    WModule::properties();
}

/**
 * Enables everything which is needed for proper transparency.
 *
 * \param state the state of the node
 */
void enableTransparency( osg::StateSet* state )
{
    // NOTE: this does not en/disable blending. This is always on.
    state->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

    // Enable depth test so that an opaque polygon will occlude a transparent one behind it.
    state->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

    // Conversely, disable writing to depth buffer so that a transparent polygon will allow polygons behind it to shine through.
    // OSG renders transparent polygons after opaque ones.
    osg::Depth* depth = new osg::Depth;
    depth->setWriteMask( false );
    state->setAttributeAndModes( depth, osg::StateAttribute::ON );

    // disable light for this geode as lines can't be lit properly
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
}

/**
 * Disables everything which is needed for proper transparency.
 *
 * \param state the state of the node
 */
void disableTransparency( osg::StateSet* state )
{
    // TODO(ebaum): this transparency is problematic. Depth write is on because we need the depth buffer for post-processing
    // Enable depth test so that an opaque polygon will occlude a transparent one behind it.
    state->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

    // disable light for this geode as lines can't be lit properly
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
}

void WMFiberDisplay::moduleMain()
{
    m_shader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMFiberDisplay", m_localPath ) );

    // this shader also includes a geometry shader, so set some needed options
    m_endCapShader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMFiberDisplay-EndCap", m_localPath ) );
    m_endCapShader->setParameter( GL_GEOMETRY_VERTICES_OUT_EXT, 4 );
    m_endCapShader->setParameter( GL_GEOMETRY_INPUT_TYPE_EXT, GL_POINTS );
    m_endCapShader->setParameter( GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP );

    // initialize illumination shader
    WGEShaderPreprocessor::SPtr defineTmp = WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_illuminationEnable, "ILLUMINATION_DISABLED", "ILLUMINATION_ENABLED" ) );
    m_shader->addPreprocessor( defineTmp );
    m_endCapShader->addPreprocessor( defineTmp );

    // initialize clipping shader
    osg::ref_ptr< WGEPropertyUniform< WPropPosition > > clipPlanePointUniform    = new WGEPropertyUniform< WPropPosition >( "u_planePoint",
                                                                                                                            m_clipPlanePoint );
    osg::ref_ptr< WGEPropertyUniform< WPropPosition > > clipPlaneVectorUniform   = new WGEPropertyUniform< WPropPosition >( "u_planeVector",
                                                                                                                            m_clipPlaneVector );
    osg::ref_ptr< WGEPropertyUniform< WPropDouble > >   clipPlaneDistanceUniform = new WGEPropertyUniform< WPropDouble >( "u_distance",
                                                                                                                           m_clipPlaneDistance );
    defineTmp = WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_clipPlaneEnabled, "CLIPPLANE_DISABLED", "CLIPPLANE_ENABLED" ) );
    m_shader->addPreprocessor( defineTmp );
    m_endCapShader->addPreprocessor( defineTmp );

    // init tube shader
    defineTmp = WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_tubeEnable, "TUBE_DISABLED", "TUBE_ENABLED" ) );
    m_shader->addPreprocessor( defineTmp );
    m_endCapShader->addPreprocessor( defineTmp );

    defineTmp = WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_tubeZoomable, "ZOOMABLE_DISABLED", "ZOOMABLE_ENABLED" ) );
    m_shader->addPreprocessor( defineTmp );
    m_endCapShader->addPreprocessor( defineTmp );

    defineTmp = WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_tubeRibbon, "RIBBON_DISABLED", "RIBBON_ENABLED" ) );
    m_shader->addPreprocessor( defineTmp );
    m_endCapShader->addPreprocessor( defineTmp );

    defineTmp = WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_tubeEndCapsEnable, "ENDCAPS_DISABLED", "ENDCAPS_ENABLED" ) );
    m_endCapShader->addPreprocessor( defineTmp );

    WGEShaderDefineOptions::SPtr clusterFilterOption = WGEShaderDefineOptions::SPtr(
        new WGEShaderDefineOptions( "CLUSTER_FILTER_ENABLED" )
    );
    clusterFilterOption->deactivateAllOptions();
    m_shader->addPreprocessor( clusterFilterOption );
    m_endCapShader->addPreprocessor( clusterFilterOption );

    osg::ref_ptr< WGEPropertyUniform< WPropDouble > > tubeSizeUniform = new WGEPropertyUniform< WPropDouble >( "u_tubeSize", m_tubeSize );
    osg::ref_ptr< WGEPropertyUniform< WPropDouble > > colormapRationUniform =
        new WGEPropertyUniform< WPropDouble >( "u_colormapRatio", m_colormapRatio );
    m_shader->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_colormapEnabled, "COLORMAPPING_DISABLED", "COLORMAPPING_ENABLED" ) )
    );

    // ROI Filter support:
    defineTmp = WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_roiFiltering, "BITFIELD_DISABLED", "BITFIELD_ENABLED" ) );
    m_shader->addPreprocessor( defineTmp );
    m_endCapShader->addPreprocessor( defineTmp );

    // ROI Filter color support
    defineTmp = WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_roiFilterColors, "SECONDARY_COLORING_DISABLED", "SECONDARY_COLORING_ENABLED" ) );
    m_shader->addPreprocessor( defineTmp );
    m_endCapShader->addPreprocessor( defineTmp );

    m_shader->addBindAttribLocation( "a_bitfield", 6 );
    m_endCapShader->addBindAttribLocation( "a_bitfield", 6 );

    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );
    m_moduleState.add( m_fiberClusteringInput->getDataChangedCondition() );
    // Remember the condition provided to some properties in properties()? The condition can now be used with this condition set.
    m_moduleState.add( m_propCondition );

    // create the post-processing node which actually does the nice stuff to the rendered image
    osg::ref_ptr< WGEPostprocessingNode > postNode = new WGEPostprocessingNode(
        WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCamera()
    );
    postNode->addUpdateCallback( new WGENodeMaskCallback( m_active ) ); // disable the postNode with m_active
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( postNode );
    // provide the properties of the post-processor to the user
    m_properties->addProperty( postNode->getProperties() );

    // do not forget to add the uniforms
    osg::StateSet* rootState = postNode->getOrCreateStateSet();
    rootState->addUniform( clipPlanePointUniform );
    rootState->addUniform( clipPlaneVectorUniform );
    rootState->addUniform( clipPlaneDistanceUniform );
    rootState->addUniform( tubeSizeUniform );
    rootState->addUniform( colormapRationUniform );

    // we need another uniform for turning on/off the ROI - coloring when no fibers are filtered out (no branches/no ROIs). In these cases, the
    // m_roiFilterColors might still be true, but we need to turn off coloring though.
    // NOTE: 1.0 means the ROI colors are used.
    m_roiFilterColorsOverride = new osg::Uniform( "u_roiFilterColorOverride", 0.0f );
    rootState->addUniform( m_roiFilterColorsOverride );
    ready();

    // needed to observe the properties of the input connector data
    boost::shared_ptr< WPropertyObserver > propObserver = WPropertyObserver::create();
    m_moduleState.add( propObserver );

    m_plane = createClipPlane();

    // main loop
    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish?
        if( m_shutdownFlag() )
        {
            break;
        }

        //////////////////////////////////////////////////////////////////////////////////////////
        // Get and check data
        //////////////////////////////////////////////////////////////////////////////////////////

        // changed? And set new current data pointers
        bool fibersUpdated = m_fiberInput->updated();
        bool clusteringUpdated = m_fiberClusteringInput->updated();

        // To query whether an input was updated, simply ask the input:
        boost::shared_ptr< WDataSetFibers > fibers = m_fiberInput->getData();
        boost::shared_ptr< WDataSetFiberClustering > fiberClustering = m_fiberClusteringInput->getData();

        bool dataValid = ( fibers );
        bool dataPropertiesUpdated = propObserver->updated();
        bool propertiesUpdated = m_tubeEnable->changed() || m_plainColorMode->changed() || m_plainColor->changed();

        // reset graphics if noting is on the input
        if( !dataValid )
        {
            debugLog() << "Resetting.";
            // remove geode if no valid data is available
            postNode->clear();

            // remove the fib's properties from my props
            m_coloringGroup->removeProperty( m_fibProps );
            m_fibProps.reset();
        }

        // something happened we are interested in?
        if( !( dataValid && ( propertiesUpdated || dataPropertiesUpdated || fibersUpdated || clusteringUpdated ) ) )
        {
            debugLog() << "Nothing to do.";
            continue;
        }

        if( clusteringUpdated && ( fiberClustering != m_fiberClustering ) )
        {
            debugLog() << "Clustering updated.";
            m_fiberClustering = fiberClustering;
            // also inform the shader to use cluster filter stuff
            if( m_fiberClustering )
            {
                clusterFilterOption->activateOption( 0 );
            }
            else
            {
                clusterFilterOption->deactivateOption( 0 );
            }
            m_fiberClusteringUpdate = true;
        }

        if( fibersUpdated || dataPropertiesUpdated || propertiesUpdated )
        {
            debugLog() << "Fibers updated.";
            m_fibers = fibers;

            // update the prop observer if new data is available
            m_coloringGroup->removeProperty( m_fibProps );
            m_fibProps = fibers->getProperties();
            propObserver->observe( m_fibProps );
            propObserver->handled();
            // also add the fib props to own props. This allows the user to modify the fib props directly
            m_coloringGroup->addProperty( m_fibProps );

            //////////////////////////////////////////////////////////////////////////////////////////
            // Create new rendering
            //////////////////////////////////////////////////////////////////////////////////////////

            // add geode to module node
            postNode->clear();

            // get a new fiber selector
            m_fiberSelector = boost::shared_ptr<WFiberSelector>( new WFiberSelector( fibers ) );

            // register dirty notifier
            m_roiUpdateConnection.disconnect();
            m_roiUpdateConnection = m_fiberSelector->getDirtyCondition()->subscribeSignal(
                boost::bind( &WMFiberDisplay::roiUpdate, this )
            );

            // we force the module to check if we need to enable normal colormapping even if ROI coloring is set
            roiUpdate();

            // create the fiber geode
            osg::ref_ptr< osg::Geode > geode = new osg::Geode();
            osg::ref_ptr< osg::Geode > endCapGeode = new osg::Geode();

            // this avoids that the pick handler tries to pick in millions if lines and quads
            geode->setName( "_Line Geode" );
            endCapGeode->setName( "_Tube Cap Geode" );
            geode->setNodeMask( 0x0000000F );
            endCapGeode->setNodeMask( 0x0000000F );

            createFiberGeode( fibers, geode, endCapGeode );

            // Apply the shader. This is for clipping.
            m_shader->apply( geode );
            m_endCapShader->apply( endCapGeode );
            // apply colormapping
            WGEColormapping::apply( geode, m_shader );
            WGEColormapping::apply( endCapGeode, m_endCapShader );

            // for line smoothing and width features
            geode->getOrCreateStateSet()->setUpdateCallback( new WGEFunctorCallback< osg::StateSet >(
                boost::bind( &WMFiberDisplay::lineGeodeStateCallback, this, _1 ) )
            );

            // Add geometry
            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_plane );

            m_fiberClusteringUpdate = true;
            postNode->insert( geode, m_shader );
            postNode->insert( endCapGeode, m_endCapShader );
        }
    }

    // At this point, the container managing this module signalled to shutdown. The main loop has ended and you should clean up. Always remove
    // allocated memory and remove all OSG nodes.
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( postNode );
}

void WMFiberDisplay::clipPlaneCallback( osg::Node* node ) const
{
    // NOTE: this callback is only executed if the plane is enabled since the NodeMaskCallback ensures proper activation of the node
    osg::MatrixTransform* transform = static_cast< osg::MatrixTransform* >( node );

    WPosition v = m_clipPlaneVector->get();
    WPosition p = m_clipPlanePoint->get();

    // the point p can be interpreted as translation:
    osg::Matrix translation = osg::Matrix::translate( p.as< osg::Vec3d >() );

    // the geometry that was specified has the normal ( 1.0, 0.0, 0.0 ). So it is possible to interpret any other normal as a rotation
    osg::Matrix rotation = osg::Matrix::rotate( osg::Vec3d( 1.0, 0.0, 0.0 ), v );

    transform->setMatrix( rotation * translation );
}

osg::ref_ptr< osg::Node > WMFiberDisplay::createClipPlane() const
{
    // add the clipping plane
    osg::ref_ptr< osg::Geode > planeGeode = new osg::Geode();
    osg::ref_ptr< osg::MatrixTransform > planeTransform = new osg::MatrixTransform();
    osg::ref_ptr< osg::Vec3Array > planeVertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec4Array > planeColor = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    osg::ref_ptr< wosg::Geometry > planeGeometry = osg::ref_ptr< wosg::Geometry >( new wosg::Geometry );

    // the plane vertices
    planeColor->push_back( osg::Vec4( 1.0, 0.0, 0.0, 0.125 ) );
    planeVertices->push_back( osg::Vec3( 0.0, -100.0, -100.0 ) );
    planeVertices->push_back( osg::Vec3( 0.0, -100.0,  100.0 ) );
    planeVertices->push_back( osg::Vec3( 0.0,  100.0,  100.0 ) );
    planeVertices->push_back( osg::Vec3( 0.0,  100.0, -100.0 ) );

    // build geometry
    planeGeometry->setVertexArray( planeVertices );
    planeGeometry->setColorArray( planeColor );
    planeGeometry->setColorBinding( wosg::Geometry::BIND_OVERALL );
    planeGeometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, 4 ) );
    planeGeode->addDrawable( planeGeometry );

    enableTransparency( planeGeode->getOrCreateStateSet() );
    planeGeode->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );

    // add a callback for showing and hiding the plane
    planeTransform->addUpdateCallback( new WGENodeMaskCallback( m_clipPlaneShowPlane ) );
    // add a callback which actually moves, scales and rotates the plane according to the plane parameter
    planeTransform->addUpdateCallback( new WGEFunctorCallback< osg::Node >(
        boost::bind( &WMFiberDisplay::clipPlaneCallback, this, _1 ) )
    );

    // add the geode to the root and provide an callback
    planeTransform->addChild( planeGeode );

    return planeTransform;
}

void WMFiberDisplay::createFiberGeode( boost::shared_ptr< WDataSetFibers > fibers, osg::ref_ptr< osg::Geode > fibGeode,
                                                                                         osg::ref_ptr< osg::Geode > endCapGeode )
{
    // geode and geometry
    osg::StateSet* state = fibGeode->getOrCreateStateSet();
    osg::StateSet* endState = endCapGeode->getOrCreateStateSet();

    // disable light for this geode as lines can't be lit properly
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

    // create everything needed for the line_strip drawable
    osg::ref_ptr< osg::Vec3Array > vertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec4Array > colors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    osg::ref_ptr< osg::Vec3Array > tangents = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    osg::ref_ptr< osg::FloatArray > texcoords = osg::ref_ptr< osg::FloatArray >( new osg::FloatArray );
    osg::ref_ptr< wosg::Geometry > geometry = osg::ref_ptr< wosg::Geometry >( new wosg::Geometry );

    // new attribute array
    m_bitfieldAttribs = new osg::FloatArray( m_fibers->getLineStartIndexes()->size() );
    m_secondaryColor = new osg::Vec3Array( m_fibers->getLineStartIndexes()->size() );

    // this is needed for the end- sprites
    osg::ref_ptr< osg::Vec3Array > endVertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec4Array > endColors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    osg::ref_ptr< osg::Vec3Array > endTangents = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    osg::ref_ptr< wosg::Geometry > endGeometry = osg::ref_ptr< wosg::Geometry >( new wosg::Geometry );

    // this is needed for the end- sprites
    // NOTE: we handle start caps and end caps separately here since the vertex attributes are per line so we need to have one cap per line.
    osg::ref_ptr< osg::Vec3Array > startVertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec4Array > startColors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    osg::ref_ptr< osg::Vec3Array > startTangents = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    osg::ref_ptr< wosg::Geometry > startGeometry = osg::ref_ptr< wosg::Geometry >( new wosg::Geometry );


    // needed arrays for iterating the fibers
    WDataSetFibers::IndexArray  fibStart = fibers->getLineStartIndexes();
    WDataSetFibers::LengthArray fibLen   = fibers->getLineLengths();
    WDataSetFibers::VertexArray fibVerts = fibers->getVertices();
    WDataSetFibers::TangentArray fibTangents = fibers->getTangents();

    // get current color scheme - the mode is important as it defines the number of floats in the color array per vertex.
    WDataSetFibers::ColorScheme::ColorMode fibColorMode = fibers->getColorScheme()->getMode();
    debugLog() << "Color mode is " << fibColorMode << ".";
    WDataSetFibers::ColorArray  fibColors = fibers->getColorScheme()->getColor();
    bool usePlainColor = m_plainColorMode->get( true );
    WColor plainColor = m_plainColor->get( true );

    // enable blending, select transparent bin
    // Either we use the fiber colors or the plain color
    if( ( !usePlainColor && ( fibColorMode == WDataSetFibers::ColorScheme::RGBA ) ) ||
        ( usePlainColor && ( plainColor.a() != 1.0  ) ) )
    {
        enableTransparency( state );
        enableTransparency( endState );
    }
    else
    {
        disableTransparency( state );
        disableTransparency( endState );
    }
    // blending is always needed for the filter attributes
    state->setMode( GL_BLEND, osg::StateAttribute::ON );
    // NOTE: this must be turned on in any case as it is used to discard some caps
    endState->setMode( GL_BLEND, osg::StateAttribute::ON );

    // progress indication
    boost::shared_ptr< WProgress > progress1( new WProgress( "Adding fibers to geode", fibStart->size() ) );
    m_progress->addSubProgress( progress1 );

    // for each fiber:
    debugLog() << "Iterating over " << fibStart->size() << " fibers.";
    debugLog() << "Number of vertices: " << fibVerts->size();
    size_t currentStart = 0;
    bool tubeMode = m_tubeEnable->get( true );
    for( size_t fidx = 0; fidx < fibStart->size() ; ++fidx )
    {
        ++*progress1;

        // the start vertex index
        size_t sidx = fibStart->at( fidx ) * 3;
        size_t csidx = fibStart->at( fidx ) * fibColorMode;

        // the length of the fiber
        size_t len = fibLen->at( fidx );

        // also initialize the ROI filter bitfield
        ( *m_bitfieldAttribs )[ fidx ] = m_fiberSelector->getBitfield()->at( fidx );
        // NOTE: secondary color arrays only support RGB colors
        WColor c = m_fiberSelector->getFiberColor( fidx );
        ( *m_secondaryColor )[ fidx ] = osg::Vec3( c.r(), c.g(), c.b() );

        // a line needs 2 verts at least
        if( len < 2 )
        {
            continue;
        }

        // provide tangents and vertices for the end-caps
        if( tubeMode )
        {
            // NOTE: we could also use the tangents stored in the tangents array but we cannot ensure they are oriented always outwards.
            // grab first and second vertex.
            osg::Vec3 firstVert = osg::Vec3( fibVerts->at( ( 3 * 0 ) + sidx ),
                                             fibVerts->at( ( 3 * 0 ) + sidx + 1 ),
                                             fibVerts->at( ( 3 * 0 ) + sidx + 2 ) );
            osg::Vec3 secondVert = osg::Vec3( fibVerts->at( ( 3 * 1 ) + sidx ),
                                              fibVerts->at( ( 3 * 1 ) + sidx + 1 ),
                                              fibVerts->at( ( 3 * 1 ) + sidx + 2 ) );
            osg::Vec3 lastVert = osg::Vec3( fibVerts->at( ( 3 * ( len - 1 ) ) + sidx ),
                                            fibVerts->at( ( 3 * ( len - 1 ) ) + sidx + 1 ),
                                            fibVerts->at( ( 3 * ( len - 1 ) ) + sidx + 2 ) );
            osg::Vec3 secondLastVert = osg::Vec3( fibVerts->at( ( 3 * ( len - 2 ) ) + sidx ),
                                                  fibVerts->at( ( 3 * ( len - 2 ) ) + sidx + 1 ),
                                                  fibVerts->at( ( 3 * ( len - 2 ) ) + sidx + 2 ) );
            osg::Vec3 startNormal = firstVert - secondVert;
            osg::Vec3 endNormal = lastVert - secondLastVert;
            startTangents->push_back( startNormal );
            startVertices->push_back( firstVert );
            endTangents->push_back( endNormal );
            endVertices->push_back( lastVert );
        }

        // walk along the fiber
        for( size_t k = 0; k < len; ++k )
        {
            osg::Vec3 vert = osg::Vec3( fibVerts->at( ( 3 * k ) + sidx ),
                                        fibVerts->at( ( 3 * k ) + sidx + 1 ),
                                        fibVerts->at( ( 3 * k ) + sidx + 2 ) );

            osg::Vec3 tangent = osg::Vec3( fibTangents->at( ( 3 * k ) + sidx ),
                                           fibTangents->at( ( 3 * k ) + sidx + 1 ),
                                           fibTangents->at( ( 3 * k ) + sidx + 2 ) );
            tangent.normalize();

            osg::Vec4 color = plainColor;
            if( !usePlainColor )
            {
                color = osg::Vec4( fibColors->at( ( fibColorMode * k ) + csidx + ( 0 % fibColorMode ) ),
                                   fibColors->at( ( fibColorMode * k ) + csidx + ( 1 % fibColorMode ) ),
                                   fibColors->at( ( fibColorMode * k ) + csidx + ( 2 % fibColorMode ) ),
                                     ( fibColorMode == WDataSetFibers::ColorScheme::RGBA ) ?
                                       fibColors->at( ( fibColorMode * k ) + csidx + ( 3 % fibColorMode ) ) : 1.0 );
            }

            vertices->push_back( vert );
            colors->push_back( color );
            tangents->push_back( tangent );

            if( tubeMode )
            {
                // if in tube-mode, some final sprites are needed to provide some kind of ending for the tube
                if( k == 0 )
                {
                    startColors->push_back( color );
                }
                if( k == len - 1 )
                {
                    endColors->push_back( color );
                }

                vertices->push_back( vert );
                colors->push_back( color );
                tangents->push_back( tangent );

                // tex coords are only needed for fake-tubes
                // NOTE: another possibility to transport the information of top and bottom vertex is to use the sign of tangent.x for example.
                // This saves some mem.
                texcoords->push_back( 1.0 );
                texcoords->push_back( -1.0 );
            }
        }

        // add the above line-strip
        if( m_tubeEnable->get() )
        {
            geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUAD_STRIP, 2 * currentStart, 2 * len ) );
            }
        else
        {
            geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, currentStart, len ) );
        }

        currentStart += len;
    }

    // combine these arrays to the geometry
    geometry->setVertexArray( vertices );
    geometry->setColorArray( colors );
    geometry->setColorBinding( wosg::Geometry::BIND_PER_VERTEX );
    geometry->setNormalArray( tangents );
    geometry->setNormalBinding( wosg::Geometry::BIND_PER_VERTEX );
    if( tubeMode )    // tex coords are only needed for fake-tubes
    {
        geometry->setTexCoordArray( 0, texcoords );

        // also create the end-sprite geometry
        endGeometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::POINTS, 0, endVertices->size() ) );
        endGeometry->setVertexArray( endVertices );
        endGeometry->setColorArray( endColors );
        endGeometry->setColorBinding( wosg::Geometry::BIND_PER_VERTEX );
        endGeometry->setNormalArray( endTangents );
        endGeometry->setNormalBinding( wosg::Geometry::BIND_PER_VERTEX );

        startGeometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::POINTS, 0, startVertices->size() ) );
        startGeometry->setVertexArray( startVertices );
        startGeometry->setColorArray( startColors );
        startGeometry->setColorBinding( wosg::Geometry::BIND_PER_VERTEX );
        startGeometry->setNormalArray( startTangents );
        startGeometry->setNormalBinding( wosg::Geometry::BIND_PER_VERTEX );

        endCapGeode->addDrawable( startGeometry );
        endCapGeode->addDrawable( endGeometry );

        endState->setAttribute( new osg::Point( 1.0f ), osg::StateAttribute::ON );
    }
    // enable VBO
    geometry->setUseDisplayList( false );
    geometry->setUseVertexBufferObjects( true );
    startGeometry->setUseDisplayList( false );
    startGeometry->setUseVertexBufferObjects( true );
    endGeometry->setUseDisplayList( false );
    endGeometry->setUseVertexBufferObjects( true );

    // bind the attribute
    geometry->setVertexAttribArray( 6, m_bitfieldAttribs );
    geometry->setSecondaryColorArray( m_secondaryColor );
    // the attributes are define per line strip, thus we bind the array accordingly
    geometry->setVertexAttribBinding( 6, wosg::Geometry::BIND_PER_PRIMITIVE_SET );
    geometry->setSecondaryColorBinding( wosg::Geometry::BIND_PER_PRIMITIVE_SET );


    if( tubeMode )
    {
        // we have one vertex per line, so bind the attribute array per vertex
        startGeometry->setVertexAttribArray( 6, m_bitfieldAttribs );
        startGeometry->setVertexAttribBinding( 6, wosg::Geometry::BIND_PER_VERTEX );
        endGeometry->setVertexAttribArray( 6, m_bitfieldAttribs );
        endGeometry->setVertexAttribBinding( 6, wosg::Geometry::BIND_PER_VERTEX );

        startGeometry->setSecondaryColorArray( m_secondaryColor );
        startGeometry->setSecondaryColorBinding( wosg::Geometry::BIND_PER_PRIMITIVE_SET );
        endGeometry->setSecondaryColorArray( m_secondaryColor );
        endGeometry->setSecondaryColorBinding( wosg::Geometry::BIND_PER_PRIMITIVE_SET );
    }

    // add an update callback which later handles several things like the filter attribute array
    geometry->setUpdateCallback( new WGEFunctorCallback< osg::Drawable >( boost::bind( &WMFiberDisplay::geometryUpdate, this, _1 ) ) );

    // set drawable
    fibGeode->addDrawable( geometry );

    debugLog() << "Iterating over all fibers: done!";
    progress1->finish();
}

void WMFiberDisplay::geometryUpdate( osg::Drawable* geometry )
{
    if( m_fiberSelectorChanged )
    {
        bool overrideROIFiltering = m_fiberSelector->isNothingFiltered();
        m_roiFilterColorsOverride->set( overrideROIFiltering ? 1.0f : 0.0f );

        m_fiberSelectorChanged = false;
        // now initialize attribute array
        for( size_t fidx = 0; fidx < m_fibers->getLineStartIndexes()->size() ; ++fidx )
        {
            ( *m_bitfieldAttribs )[ fidx ] = overrideROIFiltering | m_fiberSelector->getBitfield()->at( fidx );
            WColor c = m_fiberSelector->getFiberColor( fidx );
            ( *m_secondaryColor )[ fidx ] = osg::Vec3( c.r(), c.g(), c.b() );
        }
        m_bitfieldAttribs->dirty();
        m_secondaryColor->dirty();
    }

    if( m_fiberClusteringUpdate && m_fiberClustering )
    {
        m_fiberClusteringUpdate = false;
        size_t maxFibIdx = m_fibers->getLineStartIndexes()->size() - 1;
        osg::ref_ptr< osg::Vec3Array > attribs = new osg::Vec3Array( maxFibIdx + 1 );
        // now initialize attribute array
        for( size_t fidx = 0; fidx < m_fibers->getLineStartIndexes()->size() ; ++fidx )
        {
            ( *attribs)[ fidx ] = osg::Vec3( 0.0, 0.0, 0.0 );
        }

        // go through each of the clusters
        for( WDataSetFiberClustering::ClusterMap::const_iterator iter = m_fiberClustering->begin(); iter != m_fiberClustering->end(); ++iter )
        {
            // for each of the fiber IDs:
            const WFiberCluster::IndexList& ids = ( *iter ).second->getIndices();
            for( WFiberCluster::IndexList::const_iterator fibIter = ids.begin(); fibIter != ids.end(); ++fibIter )
            {
                // be nice here. If the clustering contains some invalid IDs, ignore it.
                if( *fibIter > maxFibIdx )
                {
                    continue;
                }
                // set the color
                ( *attribs)[ *fibIter ] = osg::Vec3(
                        ( *iter ).second->getColor().r(),
                        ( *iter ).second->getColor().g(),
                        ( *iter ).second->getColor().b()
                );
            }
        }
        static_cast< wosg::Geometry* >( geometry )->setSecondaryColorArray( attribs );
    }
}

void WMFiberDisplay::lineGeodeStateCallback( osg::StateSet* state )
{
    if( m_lineWidth->changed() )
    {
        state->setAttributeAndModes( new osg::LineWidth( m_lineWidth->get( true ) ), osg::StateAttribute::ON );
    }

    if( m_lineSmooth->changed( true ) )
    {
        // Line smoothing. Will be very slow!
        osg::StateAttribute::GLModeValue onoff = m_lineSmooth->get() ? osg::StateAttribute::ON : osg::StateAttribute::OFF;
        state->setAttributeAndModes( new osg::Hint( GL_LINE_SMOOTH_HINT, GL_NICEST ), onoff );
        state->setMode( GL_LINE_SMOOTH, onoff );

        // always keep blending enabled
        state->setMode( GL_BLEND, osg::StateAttribute::ON );
    }
}

void WMFiberDisplay::roiUpdate()
{
    m_fiberSelectorChanged = true;
}
