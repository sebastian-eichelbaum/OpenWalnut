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

#include <algorithm>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>

#include <osg/Drawable>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osgText/Text>

#include "core/common/WStringUtils.h"
#include "core/common/WPathHelper.h"
#include "core/graphicsEngine/WGERequirement.h"
#include "core/ui/WCustomWidget.h"
#include "core/ui/WCustomWidgetEventHandler.h"
#include "core/ui/WUI.h"
#include "core/kernel/WKernel.h"

// Compatibility between OSG 3.2 and earlier versions
#include "core/graphicsEngine/WOSG.h"

#include "WMHistogramView.h"
#include "WMHistogramView.xpm"

//! The number of inputs/datasets/histograms.
#define NUM_INPUTS 3

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMHistogramView )

InstanceCounter WMHistogramView::m_instanceCounter;

WMHistogramView::WMHistogramView()
    : WModule(),
      m_mousePos(),
      m_frameSize( 0.04 )
{
}

WMHistogramView::~WMHistogramView()
{
}

boost::shared_ptr< WModule > WMHistogramView::factory() const
{
    return boost::shared_ptr< WModule >( new WMHistogramView() );
}

const char** WMHistogramView::getXPMIcon() const
{
    return WMHistogramView_xpm;
}
const std::string WMHistogramView::getName() const
{
    return "HistogramView";
}

const std::string WMHistogramView::getDescription() const
{
    return "Draws histograms of one or more scalar datasets.";
}

void WMHistogramView::connectors()
{
    m_input.resize( NUM_INPUTS );

    for( std::size_t k = 0; k < m_input.size(); ++k )
    {
        m_input[ k ] = boost::shared_ptr< WModuleInputData< WDataSetScalar > >(
                                      new WModuleInputData< WDataSetScalar >(
                                            shared_from_this(),
                                            std::string( "Input dataset #" ) + string_utils::toString( k ),
                                            "A dataset to show in the histogram viewer." ) );
        addConnector( m_input[ k ] );
    }

    WModule::connectors();
}

void WMHistogramView::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_histoBins = m_properties->addProperty( "Histogram bins", "Number of bins for the histogram.", 100, m_propCondition );
    m_histoBins->setMin( 1 );
    m_histoBins->setMax( 2000 );

    boost::shared_ptr< WItemSelection > selections( new WItemSelection() );

    // add the possible histogram styles and
    // corresponding geometry generation functions
    selections->addItem( "Bars", "Draws transparent bars on top of each other." );
    m_geometryFunctions.push_back( boost::bind( &WMHistogramView::createGeometryBars, this, 1 ) );

    selections->addItem( "Parallel Bars", "Draws bars of the datasets next to each other per bin." );
    m_geometryFunctions.push_back( boost::bind( &WMHistogramView::createGeometryBars, this, 2 ) );

    selections->addItem( "Cumulative Bars", "Draws stacked bars." );
    m_geometryFunctions.push_back( boost::bind( &WMHistogramView::createGeometryBars, this, 3 ) );

    selections->addItem( "Curves", "Draws curves." );
    m_geometryFunctions.push_back( boost::bind( &WMHistogramView::createGeometryCurves, this, 1 ) );

    selections->addItem( "Cumulative Curves", "Draws stacked curves." );
    m_geometryFunctions.push_back( boost::bind( &WMHistogramView::createGeometryCurves, this, 2 ) );

    selections->addItem( "Stairs", "Draws 'stairs'." );
    m_geometryFunctions.push_back( boost::bind( &WMHistogramView::createGeometryStairs, this, 1 ) );

    selections->addItem( "Cumulative Stairs", "Draws stacked 'staris'" );
    m_geometryFunctions.push_back( boost::bind( &WMHistogramView::createGeometryStairs, this, 2 ) );

    // add the actual selection property
    m_styleSelection = m_properties->addProperty( "Histogram style", "How the histograms should be rendered",
                                                  selections->getSelectorFirst(), m_propCondition );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_styleSelection );

    // add NUM_INPUTS color selection properties
    m_colors.resize( NUM_INPUTS );

    for( std::size_t k = 0; k < m_colors.size(); ++k )
    {
        m_colors[ k ] = m_properties->addProperty( std::string( "Input " ) + string_utils::toString( k ) + " color",
                                                   std::string( "Choose a color for the histogram of input dataset " )
                                                   + string_utils::toString( k ), WColor( 1.0, 0.0, 0.0, 1.0 ), m_propCondition );
    }

    WModule::properties();
}

void WMHistogramView::requirements()
{
    // we need graphics to draw anything
    m_requirements.push_back( new WGERequirement() );
}

void WMHistogramView::handleMouseMove( WVector2f pos )
{
    if( m_infoNode )
    {
        m_mainNode->remove( m_infoNode );
    }
    if( m_markerNode )
    {
        m_mainNode->remove( m_markerNode );
    }

    if( !m_histograms.empty() ) // Bug: module will crash on mouse events when no data was connected
    {
        createInfo( pos );
    }
}

void WMHistogramView::handleResize( int /* x */, int /* y */, int width, int height )
{
    m_windowWidth = width;
    m_windowHeight = height;

    m_redrawMutex.lock();

    m_mainNode->clear();
    if( m_windowHeight != 0 && m_windowWidth != 0 && m_histograms.size() != 0 )
    {
        redraw();
    }

    m_redrawMutex.unlock();
}

void WMHistogramView::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );

    for( std::size_t k = 0; k < m_input.size(); ++k )
    {
        m_moduleState.add( m_input[ k ]->getDataChangedCondition() );
    }

    m_instanceID = ++m_instanceCounter;

    // resize data vector to the number of input connectors
    m_data.resize( m_input.size() );

    ready();

    m_widget = WKernel::getRunningKernel()->getUI()->openCustomWidget( getName() + string_utils::toString( m_instanceID ),
            WGECamera::TWO_D, m_shutdownFlag.getValueChangeCondition() );
    osg::ref_ptr< WCustomWidgetEventHandler > eh = new WCustomWidgetEventHandler( m_widget );
    eh->subscribeMove( boost::bind( &WMHistogramView::handleMouseMove, this, _1 ) );
    eh->subscribeResize( boost::bind( &WMHistogramView::handleResize, this, _1, _2, _3, _4 ) );
    m_widget->addEventHandler( eh );

    if( m_widget )
    {
        // window width and height
        m_windowWidth = m_widget->width();
        m_windowHeight = m_widget->height();

        m_mainNode = m_widget->getScene();
        if( !m_mainNode )
        {
            errorLog() << "Could not aquire scene node from widget.";
        }
    }
    else
    {
        errorLog() << "Could not create widget for the histogram.";
    }

    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";

        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        // if we do not have a main node, there is no point in doing anything
        if( m_mainNode )
        {
            m_redrawMutex.lock();

            bool dataChanged = false;
            bool hasData = false;
            for( std::size_t k = 0; k < m_data.size(); ++k )
            {
                dataChanged = dataChanged || ( m_input[ k ]->getData() && m_data[ k ] != m_input[ k ]->getData() );
                hasData = hasData || ( m_input[ k ]->getData() || m_data[ k ] );
            }

            if( !hasData )
            {
                continue;
            }

            bool colorChanged = false;
            for( std::size_t k = 0; k < m_colors.size(); ++k )
            {
                colorChanged = colorChanged | m_colors[ k ]->changed();
            }

            if( dataChanged || colorChanged || m_histoBins->changed() || m_styleSelection->changed() )
            {
                infoLog() << "Recalculating histogram.";

                // get current data
                for( std::size_t k = 0; k < m_data.size(); ++k )
                {
                    m_data[ k ] = m_input[ k ]->getData();
                }

                if( dataChanged || m_histoBins->changed() )
                {
                    calculateHistograms();
                }

                // remove all child nodes from main node
                m_mainNode->clear();
                redraw();
            }

            m_redrawMutex.unlock();
        }
    }

    debugLog() << "Shutting down...";

    // clear main node, just in case
    m_mainNode->clear();

    WKernel::getRunningKernel()->getUI()->closeCustomWidget( m_widget );

    debugLog() << "Finished. Good bye!";
}

void WMHistogramView::redraw()
{
    int sel = m_styleSelection->get( true ).getItemIndexOfSelected( 0 );
    if( sel >= static_cast< int >( m_geometryFunctions.size() ) )
    {
        errorLog() << "BUG: There is no geometry generation function for this style!";
    }
    else
    {
        // call sel'th geometry generation function
        // this adds the osg nodes that draw the histogram
        // depending on which style was selected
        m_geometryFunctions[ sel ]();

        // this creates the frame and labels
        createFrame();
    }
}

void WMHistogramView::calculateHistograms()
{
    m_histograms = std::vector< boost::shared_ptr< WHistogramBasic > >( m_data.size() );
    int histoBins = m_histoBins->get( true );

    // get the maximum and minimum of all datasets
    double min = std::numeric_limits< double >::max();
    double max = std::numeric_limits< double >::min();
    for( std::size_t k = 0; k < m_data.size(); ++k )
    {
        if( !m_data[ k ] )
        {
            continue;
        }
        if( min > m_data[ k ]->getMin() )
        {
            min = m_data[ k ]->getMin();
        }
        if( max < m_data[ k ]->getMax() )
        {
            max = m_data[ k ]->getMax();
        }
    }

    // init histograms
    for( std::size_t k = 0; k < m_data.size(); ++k )
    {
        // create new histogram
        // we do not use WDataSetScalar's getHistogram here as we want to set the min and max of the histogram ourselves
        m_histograms[ k ] = boost::shared_ptr< WHistogramBasic >( new WHistogramBasic( min, max, histoBins ) );

        if( m_data[ k ] )
        {
            // add data
            for( std::size_t j = 0; j < m_data[ k ]->getGrid()->size(); ++j )
            {
                m_histograms[ k ]->insert( m_data[ k ]->getValueSet()->getScalarDouble( j ) );
            }
        }
    }

    // these are the lower left and upper right corners of the histogram (excluding frame and labels)
    m_histogramLowerLeft[ 0 ] = min;
    m_histogramLowerLeft[ 1 ] = 0.0;

    m_histogramUpperRight[ 0 ] = max;
    // this sets m_histogramUpperRight[ 1 ]
    updateHistogramMax( false );
}

void WMHistogramView::updateHistogramMax( bool cumulative )
{
    double max = std::numeric_limits< double >::min();

    // for all bins/buckets
    for( std::size_t j = 0; j < m_histograms[ 0 ]->size(); ++j )
    {
        double val = 0.0;
        // for all histograms
        for( std::size_t k = 0; k < m_data.size(); ++k )
        {
            if( cumulative )
            {
                val += m_histograms[ k ]->at( j );
            }
            else
            {
                val = std::max( val, static_cast< double >( m_histograms[ k ]->at( j ) ) );
            }
        }
        if( val > max )
        {
            max = val;
        }
    }
    m_histogramUpperRight[ 1 ] = max;
}

void WMHistogramView::createGeometryBars( int type )
{
    // the number of valid inputs
    std::size_t numData = 0;
    for( std::size_t k = 0; k < m_data.size(); ++k )
    {
        if( m_data[ k ] )
        {
            numData++;
        }
    }

    double const onenth = 1.0 / numData; // 1 / n

    // k will be the index of the data in the m_data array
    // while h will be index into the valid pointers in m_data
    std::size_t h = 0;

    // we will accumulate histogram values in this vector
    std::vector< std::size_t > accu( m_histograms[ 0 ]->size(), 0 );

    // update the histogram size member
    updateHistogramMax( type == 3 );

    // update the frame size
    calculateFrameSize();
    calculateFramePosition();

    // create a drawable for every dataset
    for( int k = m_data.size() - 1; k >= 0; --k )
    {
        // if we do not have data, there is no point in creating geometry
        if( !m_data[ k ] )
        {
            // reset changed flag of colors
            m_colors[ k ]->get( true );
            continue;
        }

        // this is the geode for the histogram bars
        osg::ref_ptr< osg::Geode > geode = new osg::Geode();
        geode->setDataVariance( osg::Object::STATIC );

        osg::ref_ptr< osg::Vec2Array > quadVertices = new osg::Vec2Array;
        osg::ref_ptr< osg::Vec2Array > quadTexCoords = new osg::Vec2Array;
        osg::ref_ptr< osg::Vec4Array > quadColors = new osg::Vec4Array;

        osg::ref_ptr< osg::Vec2Array > lineVertices = new osg::Vec2Array;
        osg::ref_ptr< osg::Vec4Array > lineColors = new osg::Vec4Array;

        // one color per dataset
        WColor color = m_colors[ k ]->get( true );
        WColor lighterColor = WColor( color[ 0 ] * 1.1, color[ 1 ] * 1.1, color[ 2 ] * 1.1, 1.0 );
        WColor darkerColor = WColor( color[ 0 ] * 0.9, color[ 1 ] * 0.9, color[ 2 ] * 0.9, 1.0 );

        if( type == 1 && h != 0 )
        {
            color[ 3 ] = lighterColor[ 3 ] = darkerColor[ 3 ] = 0.8;
        }
        quadColors->push_back( color );

        // add a quad for every bar/bucket/bin
        for( std::size_t j = 0; j < m_histograms[ k ]->size(); ++j )
        {
            // 'histogram' coords for bar j
            std::pair< double, double > barPosHistoCoordsX = m_histograms[ k ]->getIntervalForIndex( j );
            WVector2d barLowerLeft( barPosHistoCoordsX.first, accu[ j ] );
            WVector2d barUpperRight( barPosHistoCoordsX.second, accu[ j ] + m_histograms[ k ]->at( j ) );

            // vertex position depends on whether we want to draw bars from different datasets
            // on top of each other or next to each other
            if( type == 2 )
            {
                // if next to each other, reduce width to one n'th where n is m_data.size() and then
                // move by h / n in x-direction
                double newWidth = onenth * ( barUpperRight[ 0 ] - barLowerLeft[ 0 ] );
                barLowerLeft[ 0 ] = barLowerLeft[ 0 ] + newWidth * h;
                barUpperRight[ 0 ] = barLowerLeft[ 0 ] + newWidth;
            }

            // vertices
            quadVertices->push_back( histogramSpaceToWindowSpace( barLowerLeft ) );
            quadVertices->push_back( histogramSpaceToWindowSpace( WVector2d( barUpperRight[ 0 ], barLowerLeft[ 1 ] ) ) );
            quadVertices->push_back( histogramSpaceToWindowSpace( barUpperRight ) );
            quadVertices->push_back( histogramSpaceToWindowSpace( WVector2d( barLowerLeft[ 0 ], barUpperRight[ 1 ] ) ) );

            // tex coords
            // these are not used yet,
            // but they may be used to color the bars with the dataset's colormap
            quadTexCoords->push_back( WVector2d( barPosHistoCoordsX.first, 0.0 ) );
            quadTexCoords->push_back( WVector2d( barPosHistoCoordsX.second, 0.0 ) );
            quadTexCoords->push_back( WVector2d( barPosHistoCoordsX.second, 0.0 ) );
            quadTexCoords->push_back( WVector2d( barPosHistoCoordsX.first, 0.0 ) );

            // outline vertices
            lineVertices->push_back( histogramSpaceToWindowSpace( barLowerLeft ) );
            lineVertices->push_back( histogramSpaceToWindowSpace( WVector2d( barLowerLeft[ 0 ], barUpperRight[ 1 ] ) ) );
            lineVertices->push_back( histogramSpaceToWindowSpace( WVector2d( barLowerLeft[ 0 ], barUpperRight[ 1 ] ) ) );
            lineVertices->push_back( histogramSpaceToWindowSpace( barUpperRight ) );
            lineVertices->push_back( histogramSpaceToWindowSpace( barUpperRight ) );
            lineVertices->push_back( histogramSpaceToWindowSpace( WVector2d( barUpperRight[ 0 ], barLowerLeft[ 1 ] ) ) );

            // outline colors
            lineColors->push_back( lighterColor );
            lineColors->push_back( lighterColor );
            lineColors->push_back( darkerColor );

            if( type == 3 )
            {
                accu[ j ] += m_histograms[ k ]->at( j );
            }
        }

        // create drawable for the quads
        {
            osg::ref_ptr< wosg::Geometry > geometry = new wosg::Geometry;

            geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, 4 * m_histograms[ k ]->size() ) );
            geometry->setVertexArray( quadVertices );
            geometry->setColorArray( quadColors );
            geometry->setColorBinding( wosg::Geometry::BIND_OVERALL );
            geometry->setTexCoordArray( 0, quadTexCoords );

            // enable VBO
            geometry->setUseDisplayList( false );
            geometry->setUseVertexBufferObjects( true );

            geode->addDrawable( geometry );
        }

        // create drawable for the outlines
        {
            osg::ref_ptr< wosg::Geometry > geometry = new wosg::Geometry;

            geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINES, 0, 6 * m_histograms[ k ]->size() ) );
            geometry->setVertexArray( lineVertices );
            geometry->setColorArray( lineColors );
            geometry->setColorBinding( wosg::Geometry::BIND_PER_PRIMITIVE );

            // enable VBO
            geometry->setUseDisplayList( false );
            geometry->setUseVertexBufferObjects( true );

            geode->addDrawable( geometry );
        }

        ++h;

        // we do not want any lighting
        osg::StateSet* state = geode->getOrCreateStateSet();
        state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

        // no depth test
        state->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );

        // enable blending if we draw bars on top of each other
        if( type == 1 )
        {
            state->setMode( GL_BLEND, osg::StateAttribute::ON );
            state->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
        }
        else
        {
            state->setMode( GL_BLEND, osg::StateAttribute::OFF );
        }
        state->setRenderBinDetails( 1001 + m_data.size() - k, "RenderBin" );

        m_mainNode->insert( geode );
    }
}

void WMHistogramView::createGeometryCurves( int type )
{
    // we will accumulate histogram values in this vector
    std::vector< std::size_t > accu( m_histograms[ 0 ]->size(), 0 );

    // update the histogram size member
    updateHistogramMax( type == 2 );

    // update the frame size
    calculateFrameSize();
    calculateFramePosition();

    // create a drawable for every dataset
    for( std::size_t k = 0; k < m_data.size(); ++k )
    {
        // if we do not have data, there is no point in creating geometry
        if( !m_data[ k ] )
        {
            // reset changed flag of color
            m_colors[ k ]->get( true );
            continue;
        }

        // this is the geode for the histogram curve
        osg::ref_ptr< osg::Geode > geode = new osg::Geode();
        geode->setDataVariance( osg::Object::STATIC );

        osg::ref_ptr< osg::Vec2Array > quadVertices = new osg::Vec2Array;
        osg::ref_ptr< osg::Vec2Array > quadTexCoords = new osg::Vec2Array;
        osg::ref_ptr< osg::Vec4Array > quadColors = new osg::Vec4Array;

        osg::ref_ptr< osg::Vec2Array > lineVertices = new osg::Vec2Array;
        osg::ref_ptr< osg::Vec4Array > lineColors = new osg::Vec4Array;

        // one color per dataset
        WColor color = m_colors[ k ]->get( true );
        WColor c = color;

        if( type == 1 )
        {
            c[ 3 ] = 0.2;
        }
        else
        {
            c[ 3 ] = 1.0;
            c[ 2 ] = 0.8 + c[ 2 ] * 0.2;
            c[ 1 ] = 0.8 + c[ 1 ] * 0.2;
            c[ 0 ] = 0.8 + c[ 0 ] * 0.2;
        }

        quadColors->push_back( c );
        lineColors->push_back( color );

        // add a quad for every bar/bucket/bin
        for( std::size_t j = 0; j < m_histograms[ k ]->size() - 1; ++j )
        {
            // 'histogram' coords for bar j
            double quadLeft = m_histograms[ k ]->getIntervalForIndex( j ).first + m_histograms[ k ]->getIntervalForIndex( j ).second;
            quadLeft *= 0.5;
            double quadRight = m_histograms[ k ]->getIntervalForIndex( j + 1 ).first + m_histograms[ k ]->getIntervalForIndex( j + 1 ).second;
            quadRight *= 0.5;

            WVector2d quad[ 4 ];
            quad[ 0 ] = WVector2d( quadLeft, accu[ j ] );
            quad[ 1 ] = WVector2d( quadRight, accu[ j + 1 ] );
            quad[ 2 ] = WVector2d( quadRight, accu[ j + 1 ] + m_histograms[ k ]->at( j + 1 ) );
            quad[ 3 ] = WVector2d( quadLeft, accu[ j ] + m_histograms[ k ]->at( j ) );

            // transform to window coords
            for( std::size_t i = 0; i < 4; ++i )
            {
                quad[ i ] = histogramSpaceToWindowSpace( quad[ i ] );
                quadVertices->push_back( quad[ i ] );
            }

            // tex coords
            quadTexCoords->push_back( WVector2d( quadLeft, 0.0 ) );
            quadTexCoords->push_back( WVector2d( quadRight, 0.0 ) );
            quadTexCoords->push_back( WVector2d( quadRight, 0.0 ) );
            quadTexCoords->push_back( WVector2d( quadLeft, 0.0 ) );

            // line vertices
            if( j == 0 )
            {
                lineVertices->push_back( quad[ 3 ] );
            }
            lineVertices->push_back( quad[ 2 ] );
        }

        if( type == 2 )
        {
            for( std::size_t j = 0; j < m_histograms[ k ]->size(); ++j )
            {
                accu[ j ] += m_histograms[ k ]->at( j );
            }
        }

        // create drawable for the quads
        {
            osg::ref_ptr< wosg::Geometry > geometry = new wosg::Geometry;

            geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, 4 * m_histograms[ k ]->size() - 4 ) );
            geometry->setVertexArray( quadVertices );
            geometry->setColorArray( quadColors );
            geometry->setColorBinding( wosg::Geometry::BIND_OVERALL );
            geometry->setTexCoordArray( 0, quadTexCoords );

            // enable VBO
            geometry->setUseDisplayList( false );
            geometry->setUseVertexBufferObjects( true );

            geode->addDrawable( geometry );
        }

        // create drawable for the outlines
        {
            osg::ref_ptr< wosg::Geometry > geometry = new wosg::Geometry;

            geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, 0, m_histograms[ k ]->size() ) );
            geometry->setVertexArray( lineVertices );
            geometry->setColorArray( lineColors );
            geometry->setColorBinding( wosg::Geometry::BIND_OVERALL );

            // enable VBO
            geometry->setUseDisplayList( false );
            geometry->setUseVertexBufferObjects( true );

            geode->addDrawable( geometry );
        }

        // we do not want any lighting
        osg::StateSet* state = geode->getOrCreateStateSet();
        state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

        // no depth test
        state->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );

        // enable blending if we draw stuff on top of each other
        if( type == 1 )
        {
            state->setMode( GL_BLEND, osg::StateAttribute::ON );
            state->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
        }
        else
        {
            state->setMode( GL_BLEND, osg::StateAttribute::OFF );
        }

        state->setRenderBinDetails( 1001 + m_data.size() - k, "RenderBin" );

        m_mainNode->insert( geode );
    }
}

void WMHistogramView::createGeometryStairs( int type )
{
    // we will accumulate histogram values in this vector
    std::vector< std::size_t > accu( m_histograms[ 0 ]->size(), 0 );

    updateHistogramMax( type == 2 );

    // update the frame size
    calculateFrameSize();
    calculateFramePosition();

    // create a drawable for every dataset
    for( int k = m_data.size() - 1; k >= 0; --k )
    {
        // if we do not have data, there is no point in creating geometry
        if( !m_data[ k ] )
        {
            // reset changed flag of color
            m_colors[ k ]->get( true );
            continue;
        }

        // this is the geode for the histogram bars
        osg::ref_ptr< osg::Geode > geode = new osg::Geode();
        geode->setDataVariance( osg::Object::STATIC );

        osg::ref_ptr< osg::Vec2Array > lineVertices = new osg::Vec2Array;
        osg::ref_ptr< osg::Vec4Array > lineColors = new osg::Vec4Array;

        // one color per dataset
        WColor color = m_colors[ k ]->get( true );
        color[ 3 ] = 1.0;
        lineColors->push_back( color );

        // add lines for every bar/bucket/bin
        for( std::size_t j = 0; j < m_histograms[ k ]->size(); ++j )
        {
            // 'histogram' coords for bar j
            std::pair< double, double > barPosHistoCoordsX = m_histograms[ k ]->getIntervalForIndex( j );
            WVector2d barLowerLeft( barPosHistoCoordsX.first, accu[ j ] );
            WVector2d barUpperRight( barPosHistoCoordsX.second, accu[ j ] + m_histograms[ k ]->at( j ) );

            // transform to window coords
            barLowerLeft = histogramSpaceToWindowSpace( barLowerLeft );
            barUpperRight = histogramSpaceToWindowSpace( barUpperRight );

            // line vertices
            if( j == 0 )
            {
                lineVertices->push_back( barLowerLeft );
            }
            lineVertices->push_back( WVector2d( barLowerLeft[ 0 ], barUpperRight[ 1 ] ) );
            lineVertices->push_back( barUpperRight );
            if( j == m_histograms[ k ]->size() - 1 )
            {
                lineVertices->push_back( WVector2d( barUpperRight[ 0 ], barLowerLeft[ 1 ] ) );
            }

            if( type == 2 )
            {
                accu[ j ] += m_histograms[ k ]->at( j );
            }
        }

        // create drawable for the lines
        {
            osg::ref_ptr< wosg::Geometry > geometry = new wosg::Geometry;

            geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, 0, 2 * m_histograms[ k ]->size() + 2 ) );
            geometry->setVertexArray( lineVertices );
            geometry->setColorArray( lineColors );
            geometry->setColorBinding( wosg::Geometry::BIND_OVERALL );

            // enable VBO
            geometry->setUseDisplayList( false );
            geometry->setUseVertexBufferObjects( true );

            geode->addDrawable( geometry );
        }

        // we do not want any lighting
        osg::StateSet* state = geode->getOrCreateStateSet();
        state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

        // no depth test
        state->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );

        state->setRenderBinDetails( 1001 + m_data.size() - k, "RenderBin" );

        m_mainNode->insert( geode );
    }
}

double WMHistogramView::findOptimalSpacing( double intervalLength, double availableSpace, double textSize )
{
    if( intervalLength < 0.0 )
    {
        throw WException( "Error in label spacing calculation!" );
    }

    // minimum distance of two labels in window coordinates (pixels)
    double const minDistance = 2 * textSize;

    if( availableSpace < minDistance )
    {
        return 0.0;
    }

    // fact will be the power of 10 close to the interval length
    double fact = 1.0;
    double l = intervalLength;
    while( l > 10.0 )
    {
        l *= 0.1;
        fact *= 10.0;
    }
    while( l < 1.0 )
    {
        l *= 10.0;
        fact *= 0.1;
    }

    // try different spacings until the distance between labels
    // for the chosen spacing f[ k ] * fact is larger than the minimum
    // distance
    double f[ 9 ] = { 0.1, 0.2, 0.25, 0.5, 1.0, 2.0, 2.5, 5.0, 10.0 };
    double distance;
    int k = 0;
    do
    {
        distance = f[ k ] * fact * availableSpace / intervalLength;
        ++k;
    }
    while( distance < minDistance && k < 9 );
    return f[ k ] * fact;
}

void WMHistogramView::calculateFrameSize()
{
    // find optimal label spacing for y direction
    double l = m_histogramUpperRight[ 1 ];
    double textSize = 12;
    m_frameSpacing[ 1 ] = findOptimalSpacing( l, m_windowHeight * ( 1.0 - m_frameSize ) - m_framePosition[ 1 ], textSize );

    int factorYEnd = 1;
    if( m_frameSpacing[ 1 ] != 0.0 )
    {
        // now round the maximum value up to the next multiple of spacing
        factorYEnd = static_cast< int >( m_histogramUpperRight[ 1 ] / m_frameSpacing[ 1 ] );
        if( factorYEnd * m_frameSpacing[ 1 ] < m_histogramUpperRight[ 1 ] )
        {
            factorYEnd++;
        }
        // minumum value is 0 in this case
    }
    else
    {
        m_frameSpacing[ 1 ] = m_histogramUpperRight[ 1 ];
    }

    // find optimal spacing for x direction
    l = m_histogramUpperRight[ 0 ] - m_histogramLowerLeft[ 0 ];
    textSize = 8 * static_cast< int >( std::abs( std::log10( m_frameSpacing[ 0 ] ) ) + 1 );
    m_frameSpacing[ 0 ] = findOptimalSpacing( l, m_windowWidth * ( 1.0 - m_frameSize ) - m_framePosition[ 0 ], textSize );

    int factorXStart = 0;
    int factorXEnd = 1;

    if( m_frameSpacing[ 0 ] != 0.0 )
    {
        // now round the minimum value down to the next multiple of spacing
        factorXStart = static_cast< int >( m_histogramLowerLeft[ 0 ] / m_frameSpacing[ 0 ] );
        if( m_histogramLowerLeft[ 0 ] >= 0.0 && factorXStart * m_frameSpacing[ 0 ] < m_histogramLowerLeft[ 0 ] )
        {
            factorXStart++;
        }
        if( m_histogramLowerLeft[ 0 ] < 0.0 && factorXStart * m_frameSpacing[ 0 ] > m_histogramLowerLeft[ 0 ] )
        {
            factorXStart--;
        }

        // now round the maximum value up to the next multiple of spacing
        factorXEnd = static_cast< int >( m_histogramUpperRight[ 0 ] / m_frameSpacing[ 0 ] );
        if( m_histogramUpperRight[ 0 ] >= 0.0 && factorXEnd * m_frameSpacing[ 0 ] < m_histogramUpperRight[ 0 ] )
        {
            factorXEnd++;
        }
        if( m_histogramUpperRight[ 0 ] < 0.0 && factorXEnd * m_frameSpacing[ 0 ] > m_histogramUpperRight[ 0 ] )
        {
            factorXEnd--;
        }
    }
    else
    {
        m_frameSpacing[ 0 ] = l;
    }

    m_frameLowerLeft[ 0 ] = factorXStart * m_frameSpacing[ 0 ];
    m_frameLowerLeft[ 1 ] = 0.0;
    m_frameUpperRight[ 0 ] = factorXEnd * m_frameSpacing[ 0 ];
    m_frameUpperRight[ 1 ] = factorYEnd * m_frameSpacing[ 1 ];
}

void WMHistogramView::calculateFramePosition()
{
    // the funny equation estimates the number of characters needed to write a label times
    // the estimated size of a character
    m_framePosition[ 0 ] = 1.3 * 8.0 * static_cast< int >( std::abs( log10( m_frameSpacing[ 1 ] ) ) + 1 );
    // as we write from left to right, text size is constant in y-direction
    m_framePosition[ 1 ] = 2 * 1.000000 * 12;
    // all these formulas are somewhat arbitrary
}

WVector2d WMHistogramView::histogramSpaceToWindowSpace( WVector2d const& v )
{
    WVector2d res = v;

    // move the histogram to 0,0
    res -= m_histogramLowerLeft;

    // scale by the actual size of the part of the window where we want to draw to
    // the size of that part is the window size minus some space over and to the right of the histogram
    // and some space under and left to the histogram reserved for the frame and labels
    //
    // we also divide by the frame size to fit the framed histogram region to the part of the window
    //
    // we use the frame size instead of the histogram size (that we also know) because we want our frame size to be a
    // multiple of the tick/label spacing
    // by using the frame size for scaling, we ensure the frame fits into the window (except when the window is too
    // small, but then you won't see anything anyway)
    res[ 0 ] *= ( ( 1.0 - m_frameSize ) * m_windowWidth - m_framePosition[ 0 ] ) / ( m_frameUpperRight[ 0 ] - m_frameLowerLeft[ 0 ] );
    res[ 1 ] *= ( ( 1.0 - m_frameSize ) * m_windowHeight - m_framePosition[ 1 ] ) / ( m_frameUpperRight[ 1 ] - m_frameLowerLeft[ 1 ] );

    // now translate by the frame position
    res += m_framePosition;

    // note that when changing this function, you should change windowSpaceToHistogramSpace(...) accordingly
    return res;
}

WVector2d WMHistogramView::windowSpaceToHistogramSpace( WVector2d const& v )
{
    // note that when changing this function, you should change histogramSpaceToWindowSpace(...) accordingly
    WVector2d res = v;

    res -= m_framePosition;

    res[ 0 ] /= ( ( 1.0 - m_frameSize ) * m_windowWidth - m_framePosition[ 0 ] ) / ( m_frameUpperRight[ 0 ] - m_frameLowerLeft[ 0 ] );
    res[ 1 ] /= ( ( 1.0 - m_frameSize ) * m_windowHeight - m_framePosition[ 1 ] ) / ( m_frameUpperRight[ 1 ] - m_frameLowerLeft[ 1 ] );

    res += m_histogramLowerLeft;

    return res;
}

void WMHistogramView::createFrame()
{
    m_frameNode = new osg::Geode();
    m_frameNode->setDataVariance( osg::Object::STATIC );

    osg::ref_ptr< osg::Vec2Array > lineVertices = new osg::Vec2Array;
    osg::ref_ptr< osg::Vec4Array > lineColors = new osg::Vec4Array;

    // add color
    WColor const frameColor( 0.2, 0.2, 0.2, 0.8 );
    lineColors->push_back( frameColor );

    // horizontal frame
    lineVertices->push_back( histogramSpaceToWindowSpace( m_frameLowerLeft ) );
    lineVertices->push_back( histogramSpaceToWindowSpace( WVector2d( m_frameUpperRight[ 0 ], m_frameLowerLeft[ 1 ] ) ) );

    // vertical frame
    lineVertices->push_back( histogramSpaceToWindowSpace( m_frameLowerLeft ) );
    lineVertices->push_back( histogramSpaceToWindowSpace( WVector2d( m_frameLowerLeft[ 0 ], m_frameUpperRight[ 1 ] ) ) );

    WVector2d offsetX( 6, 0.0 );
    WVector2d offsetY( 0.0, 6 );

    // label ticks
    int numLabels = 0;
    // x axis
    for( double i = m_frameLowerLeft[ 0 ]; i <= m_frameUpperRight[ 0 ]; i += m_frameSpacing[ 0 ] )
    {
        lineVertices->push_back( histogramSpaceToWindowSpace( WVector2d( i, 0.0 ) ) );
        lineVertices->push_back( histogramSpaceToWindowSpace( WVector2d( i, 0.0 ) ) - offsetY );
        ++numLabels;

        // add label text
        osgText::Text* text = new osgText::Text;
        WVector2d textPos = histogramSpaceToWindowSpace( WVector2d( i, 0.0 ) ) - offsetY * 1.2;

        text->setFont( WPathHelper::getAllFonts().Default.string() );
        text->setColor( frameColor );
        text->setCharacterSize( 12 );
        text->setAlignment( osgText::TextBase::CENTER_TOP );
        text->setPosition( WVector3d( textPos[ 0 ], textPos[ 1 ], 0.0 ) );
        text->setLayout( osgText::Text::LEFT_TO_RIGHT );
        text->setText( string_utils::toString( i ) );

        m_frameNode->addDrawable( text );
    }

    // y axis
    for( double i = m_frameLowerLeft[ 1 ]; i <= m_frameUpperRight[ 1 ]; i += m_frameSpacing[ 1 ] )
    {
        lineVertices->push_back( histogramSpaceToWindowSpace( WVector2d( m_frameLowerLeft[ 0 ], i ) ) );
        lineVertices->push_back( histogramSpaceToWindowSpace( WVector2d( m_frameLowerLeft[ 0 ], i ) ) - offsetX );
        ++numLabels;

        // add label text
        osgText::Text* text = new osgText::Text;
        WVector2d textPos = histogramSpaceToWindowSpace( WVector2d( m_frameLowerLeft[ 0 ], i ) ) - offsetX * 1.2;

        text->setFont( WPathHelper::getAllFonts().Default.string() );
        text->setColor( frameColor );
        text->setCharacterSize( 12 );
        text->setAlignment( osgText::TextBase::RIGHT_CENTER );
        text->setPosition( WVector3d( textPos[ 0 ], textPos[ 1 ], 0.0 ) );
        text->setLayout( osgText::Text::LEFT_TO_RIGHT );
        text->setText( string_utils::toString( i ) );

        m_frameNode->addDrawable( text );
    }

    // create drawable for the lines
    {
        osg::ref_ptr< wosg::Geometry > geometry = new wosg::Geometry;

        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINES, 0, 4 + 2 * numLabels ) );
        geometry->setVertexArray( lineVertices );
        geometry->setColorArray( lineColors );
        geometry->setColorBinding( wosg::Geometry::BIND_OVERALL );

        // enable VBO
        geometry->setUseDisplayList( false );
        geometry->setUseVertexBufferObjects( true );

        m_frameNode->addDrawable( geometry );
    }

    // we do not want any lighting
    osg::StateSet* state = m_frameNode->getOrCreateStateSet();
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

    // no depth test
    state->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );

    state->setRenderBinDetails( 1002 + m_data.size(), "RenderBin" );
    state->setMode( GL_BLEND, osg::StateAttribute::OFF );

    m_mainNode->insert( m_frameNode );
}

void WMHistogramView::createInfo( WVector2f mousePos )
{
    m_createInfoMutex.lock();
    // transform mouse position to histogram space
    WVector2d m = windowSpaceToHistogramSpace( WVector2d( mousePos ) );

    if( m_data.size() == 0 )
    {
        return;
    }

    if( m[ 0 ] >= m_histogramLowerLeft[ 0 ] && m[ 0 ] <= m_histogramUpperRight[ 0 ] )
    {
        m_infoNode = new osg::Geode;

        osg::StateSet* state = m_infoNode->getOrCreateStateSet();
        state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
        state->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
        state->setMode( GL_BLEND, osg::StateAttribute::OFF );
        state->setRenderBinDetails( 1002 + m_data.size(), "RenderBin" );

        // this finds the bin selected by the mouse cursor
        std::size_t bin;
        for( bin = 0; m_histograms[ 0 ]->getIntervalForIndex( bin ).second < m[ 0 ]; ++bin )
        {
        }

        // if the bin is in the histogram
        if( bin < m_histograms[ 0 ]->size() )
        {
            // add the bin value for every dataset as a text in the top right corner of the window
            int h = 1;

            for( std::size_t k = 0; k < m_data.size(); ++k )
            {
                if( !m_data[ k ] )
                {
                    continue;
                }

                WVector3d textPos( m_windowWidth - 20.0, m_windowHeight - h * 16, 0.0 );

                osgText::Text* text = new osgText::Text;

                text->setFont( WPathHelper::getAllFonts().Default.string() );
                text->setColor( m_colors[ k ]->get( false ) );
                text->setCharacterSize( 12 );
                text->setAlignment( osgText::TextBase::RIGHT_CENTER );
                text->setPosition( textPos );
                text->setLayout( osgText::Text::LEFT_TO_RIGHT );
                text->setText( string_utils::toString( m_histograms[ k ]->at( bin ) ) );

                m_infoNode->addDrawable( text );

                ++h;
            }

            // add the bin minimum and maximum
            WVector3d textPos( m_windowWidth - 20.0, m_windowHeight - h * 16, 0.0 );

            osgText::Text* text = new osgText::Text;
            std::stringstream s;
            s << "[" << m_histograms[ 0 ]->getIntervalForIndex( bin ).first
              << "," << m_histograms[ 0 ]->getIntervalForIndex( bin ).second
              << ")";

            text->setFont( WPathHelper::getAllFonts().Default.string() );
            text->setColor( WColor( 0.0, 0.0, 0.0, 1.0 ) );
            text->setCharacterSize( 12 );
            text->setAlignment( osgText::TextBase::RIGHT_CENTER );
            text->setPosition( textPos );
            text->setLayout( osgText::Text::LEFT_TO_RIGHT );
            text->setText( s.str() );

            m_infoNode->addDrawable( text );

            // mark the currently selected histogram bin by simple drawing a bar in the background
            m_markerNode = new osg::Geode;

            osg::StateSet* markerState = m_markerNode->getOrCreateStateSet();
            markerState->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
            markerState->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
            markerState->setMode( GL_BLEND, osg::StateAttribute::OFF );
            markerState->setRenderBinDetails( 1000, "RenderBin" );

            osg::ref_ptr< osg::Vec2Array > quadVertices = new osg::Vec2Array;
            osg::ref_ptr< osg::Vec4Array > quadColors = new osg::Vec4Array;

            // the color should be adapted to the window background color
            quadColors->push_back( WColor( 0.95, 0.95, 0.95, 1.0 ) );

            quadVertices->push_back( histogramSpaceToWindowSpace(
                                        WVector2d( m_histograms[ 0 ]->getIntervalForIndex( bin ).first, 0.0 ) ) );
            quadVertices->push_back( histogramSpaceToWindowSpace(
                                        WVector2d( m_histograms[ 0 ]->getIntervalForIndex( bin ).second, 0.0 ) ) );
            quadVertices->push_back( histogramSpaceToWindowSpace(
                                        WVector2d( m_histograms[ 0 ]->getIntervalForIndex( bin ).second, m_histogramUpperRight[ 1 ] ) ) );
            quadVertices->push_back( histogramSpaceToWindowSpace(
                                        WVector2d( m_histograms[ 0 ]->getIntervalForIndex( bin ).first, m_histogramUpperRight[ 1 ] ) ) );

            wosg::Geometry* geometry = new wosg::Geometry;

            geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, 4 ) );
            geometry->setVertexArray( quadVertices );
            geometry->setColorArray( quadColors );
            geometry->setColorBinding( wosg::Geometry::BIND_OVERALL );

            // enable VBO
            geometry->setUseDisplayList( false );
            geometry->setUseVertexBufferObjects( true );

            m_markerNode->addDrawable( geometry );

            // TODO(reichenbach): only insert the info node if there is enough space to do so

            m_mainNode->insert( m_infoNode );
            m_mainNode->insert( m_markerNode );
        }
    }
    m_createInfoMutex.unlock();
}

void WMHistogramView::createNothing()
{
    errorLog() << "This histogram style is not yet implemented.";
}
