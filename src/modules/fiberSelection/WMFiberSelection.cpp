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

#include <osg/ShapeDrawable>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Material>
#include <osg/StateAttribute>

#include "../../kernel/WKernel.h"
#include "../../common/WColor.h"

#include "WMFiberSelection.h"
#include "fiberSelection.xpm"

WMFiberSelection::WMFiberSelection():
    WModule()
{
}

WMFiberSelection::~WMFiberSelection()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMFiberSelection::factory() const
{
    return boost::shared_ptr< WModule >( new WMFiberSelection() );
}

const char** WMFiberSelection::getXPMIcon() const
{
    return fiberSelection_xpm;
}

const std::string WMFiberSelection::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Fiber Selection";
}

const std::string WMFiberSelection::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    return "Select fibers in a given dataset using two additional datasets as volume of interest.";
}

void WMFiberSelection::connectors()
{
    // The input fiber dataset
    m_fiberInput = boost::shared_ptr< WModuleInputData < WDataSetFibers > >(
        new WModuleInputData< WDataSetFibers >( shared_from_this(), "fibers", "The fiber dataset to select fibers from" )
    );

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_fiberInput );

    // The first VOI dataset
    m_voi1Input = boost::shared_ptr< WModuleInputData < WDataSetSingle > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(), "VOI1", "The first volume of interest." )
    );

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_voi1Input );

    // The second VOI dataset
    m_voi2Input = boost::shared_ptr< WModuleInputData < WDataSetSingle > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(), "VOI2", "The second volume of interest." )
    );

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_voi2Input );

    // the selected fibers go to this output
    m_output = boost::shared_ptr< WModuleOutputData < WDataSetFibers > >(
        new WModuleOutputData< WDataSetFibers >( shared_from_this(),
                                                               "out", "The fiber dataset only containing fibers which got through both volume of"
                                                               "interest." )
    );

    // As above: make it known.
    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}

void WMFiberSelection::properties()
{
    // used to notify about changed properties
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
}

void WMFiberSelection::moduleMain()
{
    //
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );
    m_moduleState.add( m_voi1Input->getDataChangedCondition() );
    m_moduleState.add( m_voi2Input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    // Signal ready state. Now your module can be connected by the container, which owns the module.
    ready();

    // Now wait for data
    while ( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        // woke up since the module is requested to finish
        if ( m_shutdownFlag() )
        {
            break;
        }

        boost::shared_ptr< WDataSetFibers > newFibers = m_fiberInput->getData();
        boost::shared_ptr< WDataSetSingle > newVoi1 = m_voi1Input->getData();
        boost::shared_ptr< WDataSetSingle > newVoi2 = m_voi2Input->getData();

        bool dataChanged = ( m_fibers != newFibers ) || ( m_voi1 != newVoi1 ) ||( m_voi2 != newVoi2 );
        bool dataValid =   ( newFibers && newVoi1 && newVoi2 );

        if ( dataChanged && dataValid )
        {
            debugLog() << "Data received. Recalculating.";

            // replace old data
            m_fibers = newFibers;
            m_voi1 = newVoi1;
            m_voi2 = newVoi2;

            // now the fibers get iterated. While doing this, the fibers get checked whether they belong to the VOI1 and VOI2 or not.
            // a bitlist is used to actually store this information
            std::vector< bool > bitlist( m_fibers->size(), false );

            // get the fiber definitions
            boost::shared_ptr< std::vector< size_t > > fibStart = m_fibers->getLineStartIndexes();
            boost::shared_ptr< std::vector< size_t > > fibLen   = m_fibers->getLineLengths();
            boost::shared_ptr< std::vector< float > >  fibVerts = m_fibers->getVertices();

            // TODO(ebaum): currently, both grids need to be the same
            // the grid of voi1 and voi2 is needed here
            boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_voi1->getGrid() );

            // progress indication
            boost::shared_ptr< WProgress > progress1 = boost::shared_ptr< WProgress >( new WProgress( "Checking fibers against ",
                                                                                                      fibStart->size() ) );
            m_progress->addSubProgress( progress1 );

            // for each fiber:
            for( size_t fidx = 0; fidx < fibStart->size() ; ++fidx )
            {
                ++*progress1;

                // the start vertex index
                size_t sidx = fibStart->at( fidx ) * 3;

                // the length of the fiber
                size_t len = fibLen->at( fidx ) * 3;

                // iterate along the fiber
                for ( size_t k = 0; k < len; ++k )
                {
                    float x = fibVerts->at( ( 3 * k ) + sidx );
                    float y = fibVerts->at( ( 3 * k ) + sidx + 1 );
                    float z = fibVerts->at( ( 3 * k ) + sidx + 2 );

                    // get the voxel id
                    size_t voxel = grid->getVoxelNum( wmath::WPosition( x, y, z ) );

                    // get the value
                    double value = m_voi1->getValueAt( voxel );
                }
            }

            progress1->finish();
        }
    }

}

void WMFiberSelection::activate()
{
    // handle activation

    // Always call WModule's activate!
    WModule::activate();
}

