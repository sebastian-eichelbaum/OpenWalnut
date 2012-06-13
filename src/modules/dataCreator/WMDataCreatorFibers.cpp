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

#include <algorithm>
#include <string>

#include "core/common/WAssert.h"
#include "core/common/WProgress.h"
#include "core/common/WStrategyHelper.h"
#include "core/dataHandler/WDataSetFibers.h"
#include "core/kernel/WKernel.h"

#include "WDataCreatorFiberSpiral.h"

#include "WMDataCreatorFibers.xpm"
#include "WMDataCreatorFibers.h"

WMDataCreatorFibers::WMDataCreatorFibers():
    WModule(),
    m_strategy( "Dataset Creators", "Select one of the dataset creators and configure it to your needs.", NULL,
                "Creator", "A list of all known creators." )
{
    // add some strategies here
    m_strategy.addStrategy( WDataCreatorFiberSpiral::SPtr( new WDataCreatorFiberSpiral() ) );
}

WMDataCreatorFibers::~WMDataCreatorFibers()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMDataCreatorFibers::factory() const
{
    return boost::shared_ptr< WModule >( new WMDataCreatorFibers() );
}

const char** WMDataCreatorFibers::getXPMIcon() const
{
    return datacreator_xpm;
}

const std::string WMDataCreatorFibers::getName() const
{
    return "Fiber Data Creator";
}

const std::string WMDataCreatorFibers::getDescription() const
{
    return "Allows the user to create fiber data sets providing a bunch of data creation schemes.";
}

void WMDataCreatorFibers::connectors()
{
    // initialize connectors
    m_output = WModuleOutputData< WDataSetFibers >::createAndAdd( shared_from_this(), "out", "The data that has been created" );

    // call WModule's initialization
    WModule::connectors();
}

void WMDataCreatorFibers::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // grid transform information
    m_origin = m_properties->addProperty( "Origin", "Coordinate of the origin (voxel 0,0,0).", WPosition( 0.0, 0.0, 0.0 ), m_propCondition );
    m_size = m_properties->addProperty( "Size", "The size of the dataset along the X,Y, and Z axis in the OpenWalnut coordinate system.",
                                        WPosition( 128.0, 128.0, 128.0 ), m_propCondition );

    // how much fibs and verts?
    m_numFibers = m_properties->addProperty( "Num Fibers", "The number of fibers to create.", 500, m_propCondition );
    m_numFibers->setMin( 1 );
    m_numFibers->setMax( 10000 );
    m_numVertsPerFiber = m_properties->addProperty( "Num Vertices", "Vertices per fiber.", 1000, m_propCondition );
    m_numVertsPerFiber->setMin( 1 );
    m_numVertsPerFiber->setMax( 10000 );

    m_fibColor = m_properties->addProperty( "Color", "Color for the fibers.", defaultColor::WHITE, m_propCondition );

    // now, setup the strategy helper.
    m_properties->addProperty( m_strategy.getProperties() );

    // call WModule's initialization
    WModule::properties();
}

void WMDataCreatorFibers::moduleMain()
{
    // let the main loop awake if the data changes or the properties changed.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );
    // we need to wake up if some strategy prop changed
    m_moduleState.add( m_strategy.getProperties()->getUpdateCondition() );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        debugLog() << "Creating dataset";

        WProgress::SPtr progress( new WProgress( "Creating Dataset", m_numFibers->get() ) );
        m_progress->addSubProgress( progress );

        // build structures for keeping the data
        size_t numFibers = m_numFibers->get();
        size_t numVertsPerFiber = m_numVertsPerFiber->get();
        size_t numVerts = numVertsPerFiber * numFibers;

        WDataSetFibers::VertexArray vertices = WDataSetFibers::VertexArray( new WDataSetFibers::VertexArray::element_type() );
        WDataSetFibers::IndexArray fibIdx  = WDataSetFibers::IndexArray( new WDataSetFibers::IndexArray::element_type() );
        WDataSetFibers::LengthArray lengths = WDataSetFibers::LengthArray( new WDataSetFibers::LengthArray::element_type() );
        WDataSetFibers::IndexArray fibIdxVertexMap = WDataSetFibers::IndexArray( new WDataSetFibers::IndexArray::element_type() );
        WDataSetFibers::ColorArray colors = WDataSetFibers::ColorArray( new WDataSetFibers::ColorArray::element_type() );
        vertices->reserve( numVerts * 3 );
        fibIdx->reserve( numFibers );
        lengths->reserve( numFibers );
        fibIdxVertexMap->reserve( numVerts );
        colors->reserve( numVerts * 3 );

        // get the current strategy
        m_strategy()->operator()( progress, m_fibColor->get(),
                                            numFibers,
                                            numVertsPerFiber,
                                            m_origin->get(),
                                            m_size->get(),
                                            vertices, fibIdx, lengths, fibIdxVertexMap, colors );

        // build the dataset
        WDataSetFibers::SPtr ds = WDataSetFibers::SPtr( new WDataSetFibers( vertices, fibIdx, lengths, fibIdxVertexMap ) );
        ds->addColorScheme( colors, "Fiber Creator", "The color from Fiber Creator." );

        // done. Notify user.
        debugLog() << "Created dataset with " << m_numFibers->get() << " fibers.";
        progress->finish();
        m_progress->removeSubProgress( progress );

        // done. update output
        m_output->updateData( ds );

        // Now, the moduleState variable comes into play. The module can wait for the condition, which gets fired whenever the input receives data
        // or an property changes. The main loop now waits until something happens.
        debugLog() << "Waiting ...";
        m_moduleState.wait();
    }
}

