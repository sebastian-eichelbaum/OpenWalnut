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

#include "../../kernel/WKernel.h"
#include "../../common/WPropertyHelper.h"
#include "../../common/WPropertyObserver.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WDataTexture3D.h"

#include "WMFiberDisplaySimple.h"
#include "WMFiberDisplaySimple.xpm"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMFiberDisplaySimple )

WMFiberDisplaySimple::WMFiberDisplaySimple():
    WModule()
{
}

WMFiberDisplaySimple::~WMFiberDisplaySimple()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMFiberDisplaySimple::factory() const
{
    return boost::shared_ptr< WModule >( new WMFiberDisplaySimple() );
}

const char** WMFiberDisplaySimple::getXPMIcon() const
{
    return WMFiberDisplaySimple_xpm;
}

const std::string WMFiberDisplaySimple::getName() const
{
    return "Fiber Display Simple";
}

const std::string WMFiberDisplaySimple::getDescription() const
{
    return "Display fibers. This module, unlike the full-fletched Fiber Display, can't handle ROIs. It simply draws fibers.";
}

void WMFiberDisplaySimple::connectors()
{
    // The input fiber dataset
    m_fiberInput = boost::shared_ptr< WModuleInputData < WDataSetFibers > >(
        new WModuleInputData< WDataSetFibers >( shared_from_this(), "fibers", "The fiber dataset to color" )
    );

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_fiberInput );

    // call WModule's initialization
    WModule::connectors();
}

void WMFiberDisplaySimple::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // call WModule's initialization
    WModule::properties();
}

void WMFiberDisplaySimple::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );
    // Remember the condition provided to some properties in properties()? The condition can now be used with this condition set.
    m_moduleState.add( m_propCondition );

    ready();

    // this node keeps the geode
    m_rootNode = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_active ) );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );

    // needed to observe the properties of the input connector data
    boost::shared_ptr< WPropertyObserver > propObserver = WPropertyObserver::create();
    m_moduleState.add( propObserver );

    // main loop
    while ( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish?
        if ( m_shutdownFlag() )
        {
            break;
        }

        // To query whether an input was updated, simply ask the input:
        bool dataUpdated = m_fiberInput->updated();
        boost::shared_ptr< WDataSetFibers > fibers = m_fiberInput->getData();
        bool dataValid = ( fibers );
        bool dataPropertiesUpdated = propObserver->updated();
        if ( !( dataValid && ( dataPropertiesUpdated || dataUpdated ) ) )
        {
            continue;
        }

        // update the prop observer if new data is available
        propObserver->observe( fibers->getProperties() );
        propObserver->handled();

        //////////////////////////////////////////////////////////////////////////////////////////
        // Create new fiber geode
        //////////////////////////////////////////////////////////////////////////////////////////

        // geode and geometry
        osg::ref_ptr< osg::Geode > geode = new osg::Geode();
        osg::ref_ptr< osg::Vec3Array > vertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
        osg::ref_ptr< osg::Vec4Array > colors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
        osg::ref_ptr< osg::Geometry > geometry = osg::ref_ptr< osg::Geometry >( new osg::Geometry );

        // needed arrays for iterating the fibers
        WDataSetFibers::IndexArray  fibStart = fibers->getLineStartIndexes();
        WDataSetFibers::LengthArray fibLen   = fibers->getLineLengths();
        WDataSetFibers::VertexArray fibVerts = fibers->getVertices();
        // get current color scheme - the mode is important as it defines the number of floats in the color array per vertex.
        WDataSetFibers::ColorScheme::ColorMode fibColorMode = fibers->getColorScheme()->getMode();
        WDataSetFibers::ColorArray  fibColors = fibers->getColorScheme()->getColor();

        // progress indication
        boost::shared_ptr< WProgress > progress1 = boost::shared_ptr< WProgress >( new WProgress( "Adding fibers to geode", fibStart->size() ) );
        m_progress->addSubProgress( progress1 );

        // for each fiber:
        debugLog() << "Iterating over all fibers.";
        size_t currentStart = 0;
        for( size_t fidx = 0; fidx < fibStart->size() ; ++fidx )
        {
            ++*progress1;

            // the start vertex index
            size_t sidx = fibStart->at( fidx ) * 3;
            size_t csidx = fibStart->at( fidx ) * fibColorMode;

            // the length of the fiber
            size_t len = fibLen->at( fidx );

            // walk along the fiber
            for ( size_t k = 0; k < len; ++k )
            {
                vertices->push_back( osg::Vec3( fibVerts->at( ( 3 * k ) + sidx ),
                                                fibVerts->at( ( 3 * k ) + sidx + 1 ),
                                                fibVerts->at( ( 3 * k ) + sidx + 2 ) ) );

                // for correctly indexing the color array, the offset depends on the color mode.
                colors->push_back( osg::Vec4( fibColors->at( ( fibColorMode * k ) + csidx + ( 0 % fibColorMode ) ),
                                              fibColors->at( ( fibColorMode * k ) + csidx + ( 1 % fibColorMode ) ),
                                              fibColors->at( ( fibColorMode * k ) + csidx + ( 2 % fibColorMode ) ),
                                              fibColors->at( ( fibColorMode * k ) + csidx + ( 3 % fibColorMode ) ) ) );
            }

            // add the above line-strip
            geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, currentStart, len ) );
            currentStart += len;
        }

        geometry->setVertexArray( vertices );
        geometry->setColorArray( colors );
        geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

        geode->addDrawable( geometry );

        osg::StateSet* state = geode->getOrCreateStateSet();

        // enable blending, select transparent bin if RGBA mode is used
        if ( fibColorMode == WDataSetFibers::ColorScheme::RGBA )
        {
            state->setMode( GL_BLEND, osg::StateAttribute::ON );
            state->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

            // Enable depth test so that an opaque polygon will occlude a transparent one behind it.
            state->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

            // Conversely, disable writing to depth buffer so that a transparent polygon will allow polygons behind it to shine through.
            // OSG renders transparent polygons after opaque ones.
            osg::Depth* depth = new osg::Depth;
            depth->setWriteMask( false );
            state->setAttributeAndModes( depth, osg::StateAttribute::ON );
        }
        else
        {
            state->setMode( GL_BLEND, osg::StateAttribute::OFF );
        }

        // disable light for this geode as lines can't be lit properly
        state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

        // add geode to module node
        m_rootNode->clear();
        m_rootNode->insert( geode );

        debugLog() << "Iterating over all fibers: done!";
        progress1->finish();
    }

    // At this point, the container managing this module signalled to shutdown. The main loop has ended and you should clean up. Always remove
    // allocated memory and remove all OSG nodes.
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

