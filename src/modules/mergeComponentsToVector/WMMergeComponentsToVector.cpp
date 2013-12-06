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

#include "core/kernel/WKernel.h"
#include "core/common/WPropertyHelper.h"
#include "core/dataHandler/WDataHandler.h"

#include "WMMergeComponentsToVector.h"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMMergeComponentsToVector )

WMMergeComponentsToVector::WMMergeComponentsToVector():
    WModule()
{
}

WMMergeComponentsToVector::~WMMergeComponentsToVector()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMMergeComponentsToVector::factory() const
{
    return boost::shared_ptr< WModule >( new WMMergeComponentsToVector() );
}

const std::string WMMergeComponentsToVector::getName() const
{
    return "Merge Components To Vector";
}

const std::string WMMergeComponentsToVector::getDescription() const
{
    return "Merge components to vector field.";
}

void WMMergeComponentsToVector::connectors()
{
    // the dataset to process. Only accept scalar data.
    m_scalarInX = WModuleInputData< WDataSetScalar >::createAndAdd( shared_from_this(), "X", "The X component field."
                                                                                             "Needs to be in the same grid as the others." );

    m_scalarInY = WModuleInputData< WDataSetScalar >::createAndAdd( shared_from_this(), "Y", "The Y component field."
                                                                                             "Needs to be in the same grid as the others." );

    m_scalarInZ = WModuleInputData< WDataSetScalar >::createAndAdd( shared_from_this(), "Z", "The Z component field."
                                                                                             "Needs to be in the same grid as the others." );

    // output vector data
    m_vectorOut = WModuleOutputData< WDataSetVector >::createAndAdd( shared_from_this(), "merged", "The vector dataset" );

    // call WModule's initialization
    WModule::connectors();
}

void WMMergeComponentsToVector::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // call WModule's initialization
    WModule::properties();
}

void WMMergeComponentsToVector::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_scalarInX->getDataChangedCondition() );
    m_moduleState.add( m_scalarInY->getDataChangedCondition() );
    m_moduleState.add( m_scalarInZ->getDataChangedCondition() );
    // Remember the condition provided to some properties in properties()? The condition can now be used with this condition set.
    m_moduleState.add( m_propCondition );

    ready();

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

        // To query whether an input was updated, simply ask the input:
        bool dataUpdated = m_scalarInX->handledUpdate() ||
                           m_scalarInY->handledUpdate() ||
                           m_scalarInZ->handledUpdate();

        boost::shared_ptr< WDataSetScalar > dataSetX = m_scalarInX->getData();
        boost::shared_ptr< WDataSetScalar > dataSetY = m_scalarInY->getData();
        boost::shared_ptr< WDataSetScalar > dataSetZ = m_scalarInZ->getData();

        bool dataValid = ( dataSetX && dataSetY && dataSetZ );

        // reset output if input was reset/disconnected
        if( !dataValid )
        {
            debugLog() << "Resetting output.";
            m_vectorOut->reset();
            continue;
        }
        if( dataValid && !dataUpdated )
        {
            continue;
        }

        // loop through each voxel
        boost::shared_ptr< WGridRegular3D > gridX = boost::dynamic_pointer_cast< WGridRegular3D >( dataSetX->getGrid() );
        boost::shared_ptr< WGridRegular3D > gridY = boost::dynamic_pointer_cast< WGridRegular3D >( dataSetY->getGrid() );
        boost::shared_ptr< WGridRegular3D > gridZ = boost::dynamic_pointer_cast< WGridRegular3D >( dataSetZ->getGrid() );
        if( !( gridX && gridY && gridZ ) )
        {
            errorLog() << "Only regular 3D grids allowed.";
            continue;
        }

        // ensure the grids are the same
        if( !( ( *gridX ) == ( *gridY ) ) || !( ( *gridX ) == ( *gridZ ) ) )
        {
            errorLog() << "Grids do not match.";
            continue;
        }

        // prepare progress indicators
        boost::shared_ptr< WProgress > progress( new WProgress( "Processing", dataSetX->getValueSet()->rawSize() ) );
        m_progress->addSubProgress( progress );

        // FIXME: this should be done using visitors, making it data type invariant. For now, using doubles all the time is enough
        if( !( ( dataSetX->getValueSet()->rawSize() == dataSetY->getValueSet()->rawSize() ) &&
               ( dataSetX->getValueSet()->rawSize() == dataSetZ->getValueSet()->rawSize() ) ) )
        {
            errorLog() << "Value set sizes do not match.";
            continue;
        }

        // create target valueset
        boost::shared_ptr< std::vector< double > > data( new std::vector< double >() );
        data->reserve( dataSetX->getValueSet()->rawSize() * 3 );

        for( size_t i = 0; i < dataSetX->getValueSet()->rawSize(); ++i )
        {
            double x = dataSetX->getValueSet()->getScalarDouble( i );
            double y = dataSetY->getValueSet()->getScalarDouble( i );
            double z = dataSetZ->getValueSet()->getScalarDouble( i );

            data->push_back( x );
            data->push_back( y );
            data->push_back( z );

            ++( *progress );
        }

        // create valueset, dataset and update
        boost::shared_ptr< WValueSetBase > vs( new WValueSet< double >( 1, 3, data, W_DT_DOUBLE ) );
        WDataSetVector::SPtr ds( new WDataSetVector( vs, gridX ) );
        m_vectorOut->updateData( ds );

        // work done
        progress->finish();
        m_progress->removeSubProgress( progress );

        debugLog() << "Done";
    }
}

