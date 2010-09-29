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

#include <set>
#include <string>

#include "../../kernel/WKernel.h"

#include "../../dataHandler/datastructures/WJoinContourTree.h"
#include "WMJoinTreeTester.h"

W_LOADABLE_MODULE( WMJoinTreeTester )

WMJoinTreeTester::WMJoinTreeTester():
    WModule()
{
}

WMJoinTreeTester::~WMJoinTreeTester()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMJoinTreeTester::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMJoinTreeTester() );
}

const std::string WMJoinTreeTester::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "JoinTreeTester";
}

const std::string WMJoinTreeTester::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return "Someone should add some documentation here. "
    "Probably the best person would be the modules's creator, i.e. \"lmath\"";
}

void WMJoinTreeTester::connectors()
{
    // Put the code for your connectors here. See "src/modules/template/" for an extensively documented example.
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
            new WModuleInputData< WDataSetSingle >( shared_from_this(),
                "in", "The dataset to display" )
            );

    addConnector( m_input );


    // call WModules initialization
    WModule::connectors();
}

void WMJoinTreeTester::properties()
{
    m_dbl = m_properties->addProperty( "Iso", "", 0.0 );
    m_dbl->setMin( -1000000 );
    m_dbl->setMax( 1000000 );

    WModule::properties();
}

void WMJoinTreeTester::moduleMain()
{
    debugLog() << "Entering moduleMain()";

    debugLog() << "Doing time consuming operations";
    sleep( 5 );

    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_dbl->getCondition() );

    ready();
    debugLog() << "Module is now ready.";

    debugLog() << "Entering main loop";
    while ( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        if ( m_shutdownFlag() )
        {
            break;
        }

        boost::shared_ptr< WDataSetSingle > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        bool dataValid   = ( newDataSet );

        if ( dataChanged && dataValid )
        {
            debugLog() << "Received Data.";
            m_dataSet = newDataSet;

            debugLog() << "Building Join Tree";
            m_joinTree = boost::shared_ptr< WJoinContourTree >( new WJoinContourTree( m_dataSet ) );
            m_joinTree->buildJoinTree();
            debugLog() << "Finished building Join Tree";
        }
        if( m_dbl->changed() && dataValid )
        {
            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
            boost::shared_ptr< std::set< size_t > > isoVoxels = m_joinTree->getVolumeVoxelsEnclosedByIsoSurface( m_dbl->get() );
            boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
            assert( grid );
            boost::shared_ptr< std::set< wmath::WPosition > > points;
            points = boost::shared_ptr< std::set< wmath::WPosition > >( new std::set< wmath::WPosition > );
            for( std::set< size_t >::const_iterator cit = isoVoxels->begin(); cit != isoVoxels->end(); ++cit )
            {
                points->insert( grid->getPosition( *cit ) );
            }
            m_rootNode = wge::genPointBlobs< std::set< wmath::WPosition > >( points, grid->getOffsetX()-0.00001 );
            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );
        }
    }
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}
