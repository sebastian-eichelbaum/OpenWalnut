//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2014 OpenWalnut Community
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

#include <algorithm>
#include <string>
#include <vector>

#include <osg/Geometry>
#include <osg/ShapeDrawable>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Material>
#include <osg/StateAttribute>

#include "core/dataHandler/WDataSetScalar.h"
#include "core/graphicsEngine/callbacks/WGELinearTranslationCallback.h"
#include "core/graphicsEngine/shaders/WGEPropertyUniform.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModuleInputData.h"

#include "core/graphicsEngine/WGEUtils.h"
#include "core/graphicsEngine/WGEColormapping.h"

#include "WMProjectionsAsContext.xpm"
#include "WMProjectionsAsContext.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMProjectionsAsContext )

WMProjectionsAsContext::WMProjectionsAsContext():
WModule(),
    m_propCondition( new WCondition() )
{
}

WMProjectionsAsContext::~WMProjectionsAsContext()
{
}

boost::shared_ptr< WModule > WMProjectionsAsContext::factory() const
{
    return boost::shared_ptr< WModule >( new WMProjectionsAsContext() );
}

const char** WMProjectionsAsContext::getXPMIcon() const
{
    return WMProjectionsAsContext_xpm;
}
const std::string WMProjectionsAsContext::getName() const
{
    return "Projections As Context";
}

const std::string WMProjectionsAsContext::getDescription() const
{
    return "This module draws projections by using different algorithms on a bounding box.";
}

void WMProjectionsAsContext::connectors()
{
    m_scalarIC = WModuleInputData< WDataSetScalar >::createAndAdd( shared_from_this(), "scalarData", "Scalar Data." );

    m_transferFunction = WModuleInputData< WDataSetSingle >::createAndAdd( shared_from_this(), "transfer function", "1D Transfer Function." );

    WModule::connectors();
}

void WMProjectionsAsContext::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_possibleSelectionsUsingTypes = WItemSelection::SPtr( new WItemSelection() );
    m_possibleSelectionsUsingTypes->addItem(
        MyItemType::create(
            NONE,
            "None",
            "Do not show this plane."
            )
        );
    m_possibleSelectionsUsingTypes->addItem(
        MyItemType::create(
            MIP,
            "MIP Algorithm",
            "Show this plane using the MIP Algorithm."
            )
        );
    m_possibleSelectionsUsingTypes->addItem(
        MyItemType::create(
            COMPOSITING_F2B,
            "Compositing (Front-to-Back) Algorithm",
            "Show this plane using the Compositing Algorithm (F2B)."
            )
        );
    m_possibleSelectionsUsingTypes->addItem(
        MyItemType::create(
            COMPOSITING_B2F,
            "Compositing (Back-to-Front) Algorithm",
            "Show this plane using the Compositing Algorithm (B2F)."
            )
        );

    // add items to dropdown menu
    m_singleSelectionAxialTop = m_properties->addProperty( "Axial Top",
                                                           "Choose action for this Plane.",
                                                           m_possibleSelectionsUsingTypes->getSelector( 1 ),
                                                           m_propCondition );
    m_singleSelectionAxialBottom = m_properties->addProperty( "Axial Bottom",
                                                              "Choose action for this Plane.",
                                                              m_possibleSelectionsUsingTypes->getSelector( 0 ),
                                                              m_propCondition );
    m_singleSelectionCoronalTop = m_properties->addProperty( "Coronal Top",
                                                             "Choose action for this Plane.",
                                                             m_possibleSelectionsUsingTypes->getSelector( 1 ),
                                                             m_propCondition );
    m_singleSelectionCoronalBottom = m_properties->addProperty( "Coronal Bottom",
                                                                "Choose action for this Plane.",
                                                                m_possibleSelectionsUsingTypes->getSelector( 0 ),
                                                                m_propCondition );
    m_singleSelectionSagittalTop = m_properties->addProperty( "Sagittal Top",
                                                              "Choose action for this Plane.",
                                                              m_possibleSelectionsUsingTypes->getSelector( 1 ),
                                                              m_propCondition );
    m_singleSelectionSagittalBottom = m_properties->addProperty( "Sagittal Bottom",
                                                                 "Choose action for this Plane.",
                                                                 m_possibleSelectionsUsingTypes->getSelector( 0 ),
                                                                 m_propCondition );

    // specify single selection
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_singleSelectionAxialTop );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_singleSelectionAxialTop );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_singleSelectionAxialBottom );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_singleSelectionAxialBottom );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_singleSelectionCoronalTop );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_singleSelectionCoronalTop );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_singleSelectionCoronalBottom );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_singleSelectionCoronalBottom );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_singleSelectionSagittalTop );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_singleSelectionSagittalTop );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_singleSelectionSagittalBottom );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_singleSelectionSagittalBottom );

    m_earlyRayTerminationAlpha = m_properties->addProperty(
        "Early Ray Termination",
        "Used by Compositing Front-to-Back.",
        1.0,
        m_propCondition );
    m_earlyRayTerminationAlpha->setMin( 0.0 );
    m_earlyRayTerminationAlpha->setMax( 1.0 );

    WModule::properties();
}

void WMProjectionsAsContext::requirements()
{
}

void WMProjectionsAsContext::moduleMain()
{
    infoLog() << "Thrsholding example main routine started";

    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_scalarIC->getDataChangedCondition() );
    m_moduleState.add( m_transferFunction->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    // Information output
    m_maxCoordsPosition = m_infoProperties->addProperty( "Max Coords", "", WPosition( 0, 0, 0 ) );
    m_normalOut = m_infoProperties->addProperty( "Normal", "", WPosition( 0, 0, 0 ) );
    m_planeTypes[0] = m_infoProperties->addProperty( "Type Axial Top", "", 0 );
    m_planeTypes[1] = m_infoProperties->addProperty( "Type Axial Bottom", "", 0 );
    m_planeTypes[2] = m_infoProperties->addProperty( "Type Coronal Top", "", 0 );
    m_planeTypes[3] = m_infoProperties->addProperty( "Type Coronal Bottom", "", 0 );
    m_planeTypes[4] = m_infoProperties->addProperty( "Type Sagittal Top", "", 0 );
    m_planeTypes[5] = m_infoProperties->addProperty( "Type Sagittal Bottom", "", 0 );

    ready();

    // graphics setup
    m_rootNode = osg::ref_ptr< WGEManagedGroupNode >( new WGEManagedGroupNode( m_active ) );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );

    m_geode = new osg::Geode();

    // create shader
    m_shader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMProjectionsAsContext", m_localPath ) );

    m_geode->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    m_geode->getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

    // Apply colormapping
    WGEColormapping::apply( m_geode, m_shader );

    // add geode to rootnode so that the graphics will be used/drawn in OpenWalnut
    m_rootNode->insert( m_geode );

    // initialize a "list" to save the plane selection
    std::vector<osg::ref_ptr<osg::Geometry> > planeList;
    for( int i = 0; i  < 6; i++ )
        planeList.push_back( NULL );

    // for compositing
    osg::ref_ptr< osg::Image > tfImage = new osg::Image();
    osg::ref_ptr< osg::Texture1D > tfTexture = new osg::Texture1D();
    tfTexture->setDataVariance( osg::Object::DYNAMIC );
    tfTexture->setImage( tfImage );

    // bind this texture so that we can use it in our shader
    wge::bindTexture( m_geode, tfTexture, 3, "u_transferFunction" );

    // main loop
    while( !m_shutdownFlag() )
    {
        infoLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish?
        if( m_shutdownFlag() )
        {
            break;
        }

        // save data behind connectors since it might change during processing
        scalarData = m_scalarIC->getData();

        if( !scalarData )
        {
            continue;
        }

        // extract size of grid - needed for step size in shader (sampling rate)
        WDataSetSingle::SPtr dsSingle = boost::dynamic_pointer_cast< WDataSetSingle >( scalarData );
        WGridRegular3D::SPtr regGrid = boost::dynamic_pointer_cast< WGridRegular3D >( dsSingle->getGrid() );
        int X, Y, Z;
        X = regGrid->getNbCoordsX();
        Y = regGrid->getNbCoordsY();
        Z = regGrid->getNbCoordsZ();

        m_maxCoordsPosition->set( WPosition( X, Y, Z ) );

        // save selection of each dropdown box
        size_t selection[6];
        selection[0] = m_singleSelectionAxialTop->get().at( 0 )->getAs<MyItemType>()->getValue();
        selection[1] = m_singleSelectionAxialBottom->get().at( 0 )->getAs<MyItemType>()->getValue();
        selection[2] = m_singleSelectionCoronalTop->get().at( 0 )->getAs<MyItemType>()->getValue();
        selection[3] = m_singleSelectionCoronalBottom->get().at( 0 )->getAs<MyItemType>()->getValue();
        selection[4] = m_singleSelectionSagittalTop->get().at( 0 )->getAs<MyItemType>()->getValue();
        selection[5] = m_singleSelectionSagittalBottom->get().at( 0 )->getAs<MyItemType>()->getValue();

        for( int i = 0; i < 6; i++ )
            m_planeTypes[i]->set( selection[i] );

        // bind some uniforms with properties
        m_geode->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropPosition >( "maxCoords", m_maxCoordsPosition ) );
        m_geode->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropInt >( "axialTop", m_planeTypes[0] ) );
        m_geode->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropInt >( "axialBottom", m_planeTypes[1] ) );
        m_geode->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropInt >( "coronalTop", m_planeTypes[2] ) );
        m_geode->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropInt >( "coronalBottom", m_planeTypes[3] ) );
        m_geode->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropInt >( "sagittalTop", m_planeTypes[4] ) );
        m_geode->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropInt >( "sagittalBottom", m_planeTypes[5] ) );

        m_geode->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "earlyRayTerminationAlpha", m_earlyRayTerminationAlpha ) );

        // remove all selected planes
        for( int i = 0; i < 6; i++ )
            m_geode->removeDrawable( planeList[i] );

        osg::ref_ptr<osg::Geometry>  planeGeometry = NULL;
        for( int i = 0; i < 6; i++ )
        {
            planeList[i] = NULL;
            if( selection[i] != 0 )
            {
                planeList[i] = osg::ref_ptr<osg::Geometry>( new osg::Geometry() );
                planeGeometry = planeList[i];
                osg::Vec3Array* planeVertices = new osg::Vec3Array;
                osg::Vec3Array* planeNormales = new osg::Vec3Array;
                WPosition normal( 0, 0, 1 );
                switch( static_cast<int>( floor( i/2.0f ) ) )
                {
                    case 0:
                    {
                        WPosition tempV0( 0-10, ( i%2 == 0 )?0-10:Y+10, 0-10 );
                        WPosition tempV1( X+10, ( i%2 == 0 )?0-10:Y+10, 0-10 );
                        WPosition tempV2( X+10, ( i%2 == 0 )?0-10:Y+10, Z+10 );
                        WPosition tempV3( 0-10, ( i%2 == 0 )?0-10:Y+10, Z+10 );

                        planeVertices->push_back( tempV0 );
                        planeVertices->push_back( tempV1 );
                        planeVertices->push_back( tempV2 );
                        planeVertices->push_back( tempV3 );

                        normal[0] = normal[2] = 0;
                        normal[1] = 1;

                        if( i%2 != 0 )
                            normal[1] = -1;
                    }
                    break; // Axial
                    case 1:
                    {
                        WPosition tempV0( 0-10, 0-10, ( i%2 == 0 )?0-10:Z+10 );
                        WPosition tempV1( X+10, 0-10, ( i%2 == 0 )?0-10:Z+10 );
                        WPosition tempV2( X+10, Y+10, ( i%2 == 0 )?0-10:Z+10 );
                        WPosition tempV3( 0-10, Y+10, ( i%2 == 0 )?0-10:Z+10 );
                        planeVertices->push_back( tempV0 );
                        planeVertices->push_back( tempV1 );
                        planeVertices->push_back( tempV2 );
                        planeVertices->push_back( tempV3 );

                        normal[0] = normal[1] = 0;
                        normal[2] = 1;

                        if( i%2 != 0 )
                            normal[2] = -1;
                    }
                    break; // Coronal
                    case 2:
                    {
                        WPosition tempV0( ( i%2 == 0 )?0-10:X+10, 0-10, 0-10 );
                        WPosition tempV1( ( i%2 == 0 )?0-10:X+10, 0-10, Z+10 );
                        WPosition tempV2( ( i%2 == 0 )?0-10:X+10, Y+10, Z+10 );
                        WPosition tempV3( ( i%2 == 0 )?0-10:X+10, Y+10, 0-10 );
                        planeVertices->push_back( tempV0 );
                        planeVertices->push_back( tempV1 );
                        planeVertices->push_back( tempV2 );
                        planeVertices->push_back( tempV3 );

                        normal[1] = normal[2] = 0;
                        normal[0] = 1;

                        if( i%2 != 0 )
                            normal[0] = -1;
                    }
                    break; // Sagittal
                    default:
                        break; // error occured
                }
                // add 4 normals to this plane. One for each vertex because of BIND_PER_VERTEX
                planeNormales->push_back( normal );
                planeNormales->push_back( normal );
                planeNormales->push_back( normal );
                planeNormales->push_back( normal );
                planeGeometry->setVertexArray( planeVertices );
                planeGeometry->setNormalArray( planeNormales );
                planeGeometry->setNormalBinding( ( osg::Geometry::AttributeBinding )4 );

                // add plane to geode
                osg::DrawElementsUInt* quad = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
                quad->push_back( 3 );
                quad->push_back( 2 );
                quad->push_back( 1 );
                quad->push_back( 0 );
                planeGeometry->addPrimitiveSet( quad );

                m_geode->addDrawable( planeGeometry );
            }
        }

        if( m_transferFunction->updated() )
        {
            wge::bindTexture( m_geode, tfTexture, 3, "u_transferFunction" );
            boost::shared_ptr< WDataSetSingle > dataSet = m_transferFunction->getData();
            if( !dataSet )
            {
                debugLog() << "no data set?";
            }
            else
            {
                WAssert( dataSet, "data set" );
                boost::shared_ptr< WValueSetBase > valueSet = dataSet->getValueSet();
                WAssert( valueSet, "value set" );
                boost::shared_ptr< WValueSet< unsigned char > > cvalueSet(
                    boost::dynamic_pointer_cast<WValueSet< unsigned char> >( valueSet )
                    );
                if( !cvalueSet )
                {
                    debugLog() << "invalid type";
                }
                else
                {
                    size_t tfsize = cvalueSet->rawSize();
                    // create image and copy the TF
                    tfImage->allocateImage( tfsize/4, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE );
                    tfImage->setInternalTextureFormat( GL_RGBA );
                    unsigned char* data = reinterpret_cast< unsigned char* >( tfImage->data() );
                    std::copy( cvalueSet->rawData(), &cvalueSet->rawData()[ tfsize ], data );

                    // force OpenGl to use the new texture
                    tfTexture->dirtyTextureObject();
                }
            }
        }
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}
