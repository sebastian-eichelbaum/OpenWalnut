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

#include <string>

#include <boost/lexical_cast.hpp>

#include <osg/Projection>
#include <osg/Geode>

#include "../../dataHandler/WDataTexture3D_2.h"
#include "../../graphicsEngine/WGEColormapping.h"
#include "../../graphicsEngine/callbacks/WGENodeMaskCallback.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../graphicsEngine/shaders/WGEShader.h"
#include "../../graphicsEngine/widgets/labeling/WGELabel.h"
#include "../../kernel/WKernel.h"

#include "WMColormapper.xpm"
#include "WMColormapper.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMColormapper )

WMColormapper::WMColormapper() :
    WModule()
{
    // initialize
}

WMColormapper::~WMColormapper()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMColormapper::factory() const
{
    return boost::shared_ptr< WModule >( new WMColormapper() );
}

const char** WMColormapper::getXPMIcon() const
{
    return WMColormapper_xpm;
}

const std::string WMColormapper::getName() const
{
    return "Colormapper";
}

const std::string WMColormapper::getDescription() const
{
    return "Can use the input as a texture that can be mapped to the navslices and so on.";
}

void WMColormapper::connectors()
{
    m_input = WModuleInputData< WDataSetSingle >::createAndAdd( shared_from_this(), "input", "Input to apply as texture." );

    // call WModules initialization
    WModule::connectors();
}

void WMColormapper::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_replace = m_properties->addProperty( "Keep position",
                                           "If true, new texture on the input connector get placed in the list where the old one was.", true );

    m_showColorbar = m_properties->addProperty( "Show Colorbar", "If true, a colorbar is shown for the current colormap.", true );

    WModule::properties();
}

void WMColormapper::moduleMain()
{
    // let the main loop awake if the data changes or the properties changed.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    osg::ref_ptr< WGEShader > colormapShader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMColormapper", m_localPath ) );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        // Now, the moduleState variable comes into play. The module can wait for the condition, which gets fired whenever the input receives data
        // or an property changes. The main loop now waits until something happens.
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }

        // has the data changed?
        if( m_input->handledUpdate() )
        {
            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_barProjection );
            colormapShader->deactivate( m_colorBar );

            boost::shared_ptr< WDataSetSingle > dataSet = m_input->getData();

            // add a colorbar
            if ( dataSet && dataSet->isTexture() )
            {
                // TODO(ebaum): this is not the best possible solution. Actually, its a hack.
                //              A nice solution would be some more abstract "widget" system

                // create camera oriented 2d projection
                m_barProjection = new osg::Projection();
                m_barProjection->addUpdateCallback( new WGENodeMaskCallback( m_showColorbar ) );
                m_barProjection->setMatrix( osg::Matrix::ortho2D( 0, 1.0, 0, 1.0 ) );
                m_barProjection->getOrCreateStateSet()->setRenderBinDetails( 11, "RenderBin" );
                m_barProjection->getOrCreateStateSet()->setDataVariance( osg::Object::DYNAMIC );
                m_barProjection->getOrCreateStateSet()->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
                m_barProjection->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

                float borderWidth = 0.001;

                // create a colorbar geode
                m_colorBar = wge::genFinitePlane( osg::Vec3( 0.025, 0.1, 0.0 ), osg::Vec3( 0.025, 0.0, 0.0 ), osg::Vec3( 0.0, 0.8, 0.0 ) );
                osg::ref_ptr< osg::Geode > colorBarBorder = wge::genFinitePlane( osg::Vec3( 0.025 - borderWidth, 0.1 - borderWidth, 0.0 ),
                                                                                 osg::Vec3( 0.025 + 2.0 * borderWidth, 0.0, 0.0 ),
                                                                                 osg::Vec3( 0.0, 0.8 + 2.0 * borderWidth, 0.0 ) );
                m_colorBar->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropSelection >( "u_colormap",
                                                               dataSet->getTexture2()->colormap() ) );
                colormapShader->apply( m_colorBar );

                // add the label scale
                osg::ref_ptr< WGELabel > topLabel = new WGELabel();
                topLabel->setCharacterSize( 2 );
                topLabel->setPosition( osg::Vec3( 0.055, 0.9, 0.0 ) );
                topLabel->setText(
                    boost::lexical_cast< std::string >(
                        dataSet->getTexture2()->minimum()->get() + dataSet->getTexture2()->scale()->get()
                    )
                );
                topLabel->setCharacterSize( 0.02 );
                topLabel->setAlignment( osgText::Text::LEFT_TOP );
                osg::ref_ptr< WGELabel > bottomLabel = new WGELabel();
                bottomLabel->setPosition( osg::Vec3( 0.055, 0.1, 0.0 ) );
                bottomLabel->setText( boost::lexical_cast< std::string >( dataSet->getTexture2()->minimum()->get() ) );
                bottomLabel->setCharacterSize( 0.02 );

                // the bar and the labels need to be added in an identity modelview matrix node
                osg::ref_ptr< osg::MatrixTransform > matrix = new osg::MatrixTransform();
                matrix->setMatrix( osg::Matrix::identity() );
                matrix->setReferenceFrame( osg::Transform::ABSOLUTE_RF );

                // this geode contains the labels
                osg::ref_ptr< osg::Geode > labels = new osg::Geode();
                labels->addDrawable( topLabel );
                labels->addDrawable( bottomLabel );

                // build pipeline
                matrix->addChild( colorBarBorder );
                matrix->addChild( m_colorBar );
                matrix->addChild( labels );
                m_barProjection->addChild( matrix );

                // add
                WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_barProjection );
            }

            // replace texture instead of removing it?
            if ( dataSet && dataSet->isTexture() && m_lastDataSet && m_replace->get( true ) )
            {
                debugLog() << "Replacing texture \"" << m_lastDataSet->getTexture2()->name()->get() << "\" with \"" <<
                                                        dataSet->getTexture2()->name()->get() << "\".";
                m_properties->removeProperty( m_lastDataSet->getTexture2()->getProperties() );
                m_infoProperties->removeProperty( m_lastDataSet->getTexture2()->getInformationProperties() );
                m_properties->addProperty( dataSet->getTexture2()->getProperties() );
                m_infoProperties->addProperty( dataSet->getTexture2()->getInformationProperties() );
                WGEColormapping::replaceTexture( m_lastDataSet->getTexture2(), dataSet->getTexture2() );
                m_lastDataSet = dataSet;
            }
            else
            {
                // de-register last input
                if ( m_lastDataSet )
                {
                    debugLog() << "Removing previous texture \"" << m_lastDataSet->getTexture2()->name()->get() << "\".";
                    m_properties->removeProperty( m_lastDataSet->getTexture2()->getProperties() );
                    m_infoProperties->removeProperty( m_lastDataSet->getTexture2()->getInformationProperties() );
                    WGEColormapping::deregisterTexture( m_lastDataSet->getTexture2() );
                    m_lastDataSet.reset();
                }

                // register only valid data
                if( dataSet && dataSet->isTexture() )
                {
                    m_lastDataSet = dataSet;

                    // register new
                    debugLog() << "Registering new texture \"" << m_lastDataSet->getTexture2()->name()->get() << "\".";
                    m_properties->addProperty( m_lastDataSet->getTexture2()->getProperties() );
                    m_infoProperties->addProperty( m_lastDataSet->getTexture2()->getInformationProperties() );
                    WGEColormapping::registerTexture( m_lastDataSet->getTexture2() );
                }
            }
        }
    }

    // remove if module is removed
    if ( m_lastDataSet )
    {
        debugLog() << "Removing previous texture \"" << m_lastDataSet->getTexture2()->name()->get() << "\".";
        WGEColormapping::deregisterTexture( m_lastDataSet->getTexture2() );
        // NOTE: the props get removed automatically
    }

    // remove colorbar and its labels
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_barProjection );
}

void WMColormapper::activate()
{
    // deactivate the output if wanted
    if ( m_lastDataSet )
    {
        m_lastDataSet->getTexture2()->active()->set( m_active->get( true ) );
    }

    // Always call WModule's activate!
    WModule::activate();
}

