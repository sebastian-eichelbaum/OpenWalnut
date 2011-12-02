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

#include "core/dataHandler/WDataTexture3D.h"
#include "core/graphicsEngine/WGEColormapping.h"
#include "core/graphicsEngine/callbacks/WGENodeMaskCallback.h"
#include "core/graphicsEngine/callbacks/WGEFunctorCallback.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/widgets/labeling/WGELabel.h"
#include "core/kernel/WKernel.h"

#include "WMTransferFunctionColorBar.xpm"
#include "WMTransferFunctionColorBar.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMTransferFunctionColorBar )

WMTransferFunctionColorBar::WMTransferFunctionColorBar() :
    WModule()
{
    // initialize
}

WMTransferFunctionColorBar::~WMTransferFunctionColorBar()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMTransferFunctionColorBar::factory() const
{
    return boost::shared_ptr< WModule >( new WMTransferFunctionColorBar() );
}

const char** WMTransferFunctionColorBar::getXPMIcon() const
{
    return WMTransferFunctionColorBar_xpm;
}

const std::string WMTransferFunctionColorBar::getName() const
{
    return "Transfer Function Color Bar";
}

const std::string WMTransferFunctionColorBar::getDescription() const
{
    return "Can use the input as a texture that can be mapped to the navslices and so on.";
}

void WMTransferFunctionColorBar::connectors()
{
    m_input = WModuleInputData< WDataSetSingle >::createAndAdd( shared_from_this(), "transfer function", "Input to apply as texture." );

    // call WModules initialization
    WModule::connectors();
}

void WMTransferFunctionColorBar::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // m_replace = m_properties->addProperty( "Keep position",
    //                                        "If true, new texture on the input connector get placed in the list where the old one was.", true );

    WPropGroup colorBarGroup = m_properties->addPropertyGroup( "Colorbar", "The colorbar with several properties." );
    // m_showColorbar = colorBarGroup->addProperty( "Show Colorbar", "If true, a colorbar is shown for the current colormap.", false );
    m_colorBarBorder = colorBarGroup->addProperty( "Show Border", "If true, a thin white border is shown around the colorbar.", true );
    m_colorBarName = colorBarGroup->addProperty( "Show Name", "If true, a shortened version of the data name is shown.", true );
    m_colorBarLabels = colorBarGroup->addProperty( "Colorbar Labels", "This defines the number of labels.", 10 );
    m_colorBarLabels->setMin( 0 );
    m_colorBarLabels->setMax( 55 );

    WModule::properties();
}

namespace
{
    /**
     * Formats a given string to have the specified maximum length.
     *
     * \param str the string
     * \param maxLen max length
     *
     * \return formatted string
     */
    std::string format( std::string str, size_t maxLen = 45 )
    {
        // string will be at least 9 chars long: because of " [...] " + first and last char.
        WAssert( maxLen >= 9, "MaxLen has to be 9 or more." );
        std::ostringstream ss;

        // cut away some stuff
        if( str.length() > maxLen )
        {
            size_t keep = maxLen - 3;   // how much chars to keep?
            size_t keepFront = keep / 2;
            size_t keepEnd = keep - keepFront;

            ss << str.substr( 0, keepFront ) << " [...] " << str.substr( str.length() - keepEnd, keepEnd );
        }
        else
        {
            ss << str;
        }
        return ss.str();
    }
}

/**
 * Formats a number properly to be usable as label
 *
 * \tparam T the type of value.
 * \param number the number to format
 *
 * \return the formated number
 */
template< typename T >
std::string format( T number )
{
    std::ostringstream ss;
    ss.precision( 3 );
    ss << number;
    return ss.str();
}

void WMTransferFunctionColorBar::moduleMain()
{
    // let the main loop awake if the data changes or the properties changed.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    osg::ref_ptr< WGEShader > colormapShader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMTransferFunctionColorBar", m_localPath ) );

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
            if( dataSet/* && dataSet->isTexture()*/ )
            {
                // create camera oriented 2d projection
                m_barProjection = new osg::Projection();
                // m_barProjection->addUpdateCallback( new WGENodeMaskCallback( m_showColorbar ) );
                m_barProjection->addUpdateCallback( new WGENodeMaskCallback( m_active ) );
                m_barProjection->setMatrix( osg::Matrix::ortho2D( 0, 1.0, 0, 1.0 ) );
                m_barProjection->getOrCreateStateSet()->setRenderBinDetails( 15, "RenderBin" );
                m_barProjection->getOrCreateStateSet()->setDataVariance( osg::Object::DYNAMIC );
                m_barProjection->getOrCreateStateSet()->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
                m_barProjection->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

                float borderWidth = 0.001;

                // create a colorbar geode
                m_colorBar = wge::genFinitePlane( osg::Vec3( 0.025, 0.1, 0.0 ), osg::Vec3( 0.025, 0.0, 0.0 ), osg::Vec3( 0.0, 0.8, 0.0 ) );
                osg::ref_ptr< osg::Geode > colorBarBorder = wge::genFinitePlane( osg::Vec3( 0.025 - borderWidth, 0.1 - borderWidth, -0.1 ),
                                                                                 osg::Vec3( 0.025 + 2.0 * borderWidth, 0.0, -0.1 ),
                                                                                 osg::Vec3( 0.0, 0.8 + 2.0 * borderWidth, -0.1 ) );
                // m_colorBar->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropSelection >( "u_colormap",
                //                                                dataSet->getTexture()->colormap() ) );
                colormapShader->apply( m_colorBar );

                // create the texture for color lookup
                // boost::shared_ptr< WDataSetSingle > dataSet = m_transferFunction->getData();
                if ( !dataSet )
                {
                    debugLog() << "no data set?";
                }
                else
                {
                    WAssert( dataSet, "data set" );
                    boost::shared_ptr< WValueSetBase > valueSet = dataSet->getValueSet();
                    WAssert( valueSet, "value set" );
                    boost::shared_ptr< WValueSet< unsigned char > > cvalueSet( boost::shared_dynamic_cast<WValueSet< unsigned char> >( valueSet ) );
                    if ( !cvalueSet )
                    {
                        debugLog() << "invalid type";
                    }
                    else
                    {
                        size_t tfsize =  cvalueSet->rawSize();
                        const unsigned char* orig =  cvalueSet->rawData();
                        unsigned char* data =  new unsigned char[  tfsize ];
                        std::copy(  orig, &orig[  tfsize ], data );
                        osg::ref_ptr <  osg::Image >  tfImg(  new osg::Image() );
                        tfImg->setImage(  tfsize/4, 1, 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE,
                                data, osg::Image::USE_NEW_DELETE ); // FIXME: check allocation mode
                        // tfTexEnableDefine->setActive( false );
                        osg::ref_ptr< osg::Texture1D > tfTexture = new osg::Texture1D();
                        tfTexture->setImage( tfImg );
                        wge::bindTexture( m_colorBar, tfTexture, 0, "u_transferFunction" );
                        // tfTexEnableDefine->setActive( true );
                    }
                }

                // add the name label
                osg::ref_ptr< WGELabel > nameLabel = new WGELabel();
                nameLabel->setPosition( osg::Vec3( 0.015, 0.9, 0.0 ) );
                // nameLabel->setText( format( dataSet->getTexture()->name()->get() ) );
                nameLabel->setText( format( "TransferFunction" ) );
                nameLabel->setCharacterSize( 0.015 );
                nameLabel->setLayout( osgText::TextBase::VERTICAL );
                nameLabel->setAlignment( osgText::Text::BASE_LINE );
                // nameLabel->setUpdateCallback( new WGEFunctorCallback< osg::Drawable >(
                //     boost::bind( &WMTransferFunctionColorBar::updateColorbarName, this, _1 ) )
                // );

                // the bar and the labels need to be added in an identity modelview matrix node
                osg::ref_ptr< osg::MatrixTransform > matrix = new osg::MatrixTransform();
                matrix->setMatrix( osg::Matrix::identity() );
                matrix->setReferenceFrame( osg::Transform::ABSOLUTE_RF );

                // this geode contains the labels
                osg::ref_ptr< osg::Geode > labels = new osg::Geode();
                labels->addDrawable( nameLabel );
                m_scaleLabels = new osg::Geode();
                m_scaleLabels->addUpdateCallback( new WGEFunctorCallback< osg::Node >(
                    boost::bind( &WMTransferFunctionColorBar::updateColorbarScale, this, _1 )
                ) );

                // set some callbacks
                colorBarBorder->addUpdateCallback( new WGENodeMaskCallback( m_colorBarBorder ) );
                labels->addUpdateCallback( new WGENodeMaskCallback( m_colorBarName ) );

                // build pipeline
                matrix->addChild( colorBarBorder );
                matrix->addChild( m_colorBar );
                matrix->addChild( m_scaleLabels );
                matrix->addChild( labels );
                m_barProjection->addChild( matrix );

                m_valueMin = 0.0; // dataSet->getTexture()->minimum()->get();
                m_valueScale = 1.0; // dataSet->getTexture()->scale()->get();

                // add
                WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_barProjection );
            }
        }
    }

    // remove colorbar and its labels
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_barProjection );
}

void WMTransferFunctionColorBar::activate()
{
    // deactivate the output if wanted
    // if( m_lastDataSet )
    // {
    //    m_lastDataSet->getTexture()->active()->set( m_active->get( true ) );
    // }

    // Always call WModule's activate!
    WModule::activate();
}

// void WMTransferFunctionColorBar::updateColorbarName( osg::Drawable* label )
// {
    // if( m_lastDataSet )
    // {
    //     dynamic_cast< WGELabel* >( label )->setText( format( m_lastDataSet->getTexture()->name()->get() ) );
    // }
// }

void WMTransferFunctionColorBar::updateColorbarScale( osg::Node* scaleLabels )
{
    if( m_colorBarLabels->changed( true ) )
    {
        const double labelXPos = 0.060;
        osg::Geode* g = scaleLabels->asGeode();
        g->removeDrawables( 0, g->getNumDrawables() );

        size_t num = m_colorBarLabels->get( true );
        double coordStep = 0.8 / static_cast< double >( num - 1 );
        double valueStep = m_valueScale / static_cast< double >( num - 1 );

        // less than 2 labels is useless
        if( num < 2 )
        {
            return;
        }

        osg::Vec3Array* lineVerts = new osg::Vec3Array();

        // create enough labels.
        for( size_t i = 0; i < num; ++i )
        {
            double value = m_valueMin + ( valueStep * i );

            // create the label
            osg::ref_ptr< WGELabel > label = new WGELabel();
            label->setPosition( osg::Vec3( labelXPos, 0.1 + i * coordStep, 0.0 ) );  // bar goes from 0.1 to 0.9 in our coordinate system
            label->setText( format( value ) );
            label->setCharacterSize( 0.015 );
            label->setAlignment( osgText::Text::LEFT_CENTER );

            g->addDrawable( label );

            // create the line next to the label
            lineVerts->push_back( osg::Vec3( labelXPos - 0.010, 0.1 + i * coordStep, 0.0 ) );
            lineVerts->push_back( osg::Vec3( labelXPos - 0.005, 0.1 + i * coordStep, 0.0 ) );
        }

        // create the line drawable
        osg::Geometry* lines = new osg::Geometry();
        lines->setVertexArray( lineVerts );
        osg::Vec3Array* color = new osg::Vec3Array();
        color->push_back( osg::Vec3( 1.0, 1.0, 1.0 ) );
        lines->setColorArray( color );
        lines->setColorBinding( osg::Geometry::BIND_OVERALL );
        lines->addPrimitiveSet( new osg::DrawArrays( GL_LINES, 0, lineVerts->size() ) );
        g->addDrawable( lines );
    }
}

