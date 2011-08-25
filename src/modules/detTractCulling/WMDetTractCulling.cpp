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

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

// Use filesystem version 2 for compatibility with newer boost versions.
#ifndef BOOST_FILESYSTEM_VERSION
    #define BOOST_FILESYSTEM_VERSION 2
#endif
#include <boost/filesystem.hpp>

#include "core/common/datastructures/WFiber.h"
#include "core/common/WColor.h"
#include "core/common/WLogger.h"
#include "core/common/WProgress.h"
#include "core/common/WPropertyHelper.h"
#include "core/dataHandler/exceptions/WDHIOFailure.h"
#include "core/dataHandler/io/WWriterFiberVTK.h"
#include "core/dataHandler/WDataSetFiberVector.h"
#include "core/dataHandler/WSubject.h"
#include "core/kernel/WKernel.h"
#include "WMDetTractCulling.xpm"
#include "WMDetTractCulling.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMDetTractCulling )

WMDetTractCulling::WMDetTractCulling()
    : WModule(),
      m_recompute( new WCondition() )
{
}

WMDetTractCulling::~WMDetTractCulling()
{
}

boost::shared_ptr< WModule > WMDetTractCulling::factory() const
{
    return boost::shared_ptr< WModule >( new WMDetTractCulling() );
}

const char** WMDetTractCulling::getXPMIcon() const
{
    return detTractCulling_xpm;
}

void WMDetTractCulling::moduleMain()
{
    m_moduleState.setResetable( true, true ); // remember actions when actually not waiting for actions
    m_moduleState.add( m_tractIC->getDataChangedCondition() );
    m_moduleState.add( m_recompute );

    ready();

    while( !m_shutdownFlag() ) // loop until the module container requests the module to quit
    {
        if( !m_tractIC->getData().get() ) // ok, the output has not yet sent data
        {
            m_moduleState.wait();
            continue;
        }

        if( m_rawDataset != m_tractIC->getData() ) // in case data has changed
        {
            m_rawDataset = m_tractIC->getData();
            boost::shared_ptr< WProgress > convertProgress( new WProgress( "Converting tracts", 1 ) );
            m_progress->addSubProgress( convertProgress );
            m_dataset = boost::shared_ptr< WDataSetFiberVector >( new WDataSetFiberVector( m_rawDataset ) );
            m_numTracts->set( static_cast< int32_t >( m_dataset->size() ) );
            convertProgress->finish();
        }

        if( m_savePath->get().string() == "/no/such/file" )
        {
            m_savePath->set( saveFileName( m_dataset->getFileName() ) );
        }

        if( m_run->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            infoLog() << "Start processing tracts";
            cullOutTracts();
            infoLog() << "Processing finished";
            m_run->set( WPVBaseTypes::PV_TRIGGER_READY, false );
        }

        m_moduleState.wait();
    }
}

void WMDetTractCulling::connectors()
{
    m_tractIC = WModuleInputData< WDataSetFibers >::createAndAdd( shared_from_this(), "tractInput", "A loaded tract dataset." );
    m_tractOC = WModuleOutputData< WDataSetFibers >::createAndAdd( shared_from_this(),  "tractOutput", "The tracts that survied culling." );

    WModule::connectors();  // call WModules initialization
}

void WMDetTractCulling::properties()
{
    m_dSt_culling_t    = m_properties->addProperty( "Min tract distance", "If below, the shorter tract is culled out", 6.5 );
    m_proximity_t      = m_properties->addProperty( "Min point distance", "Min distance of points of two tracts which should be considered", 1.0 );
    m_saveCulledCurves = m_properties->addProperty( "Save result", "If true the remaining tracts are save to a file", false );
    m_savePath         = m_properties->addProperty( "Save path", "Where to save the result", boost::filesystem::path( "/no/such/file" ) );
    m_run              = m_properties->addProperty( "Start culling", "Start", WPVBaseTypes::PV_TRIGGER_READY, m_recompute );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_savePath );
    m_numRemovedTracts = m_infoProperties->addProperty( "#Tracts removed", "Number of tracts beeing culled out", 0 );
    m_numRemovedTracts->setMin( 0 );
    m_numRemovedTracts->setMax( wlimits::MAX_INT32_T );
    m_numTracts = m_infoProperties->addProperty( "#Tracts", "Number of tracts beeing processed", 0 );
    m_numTracts->setMin( 0 );
    m_numTracts->setMax( wlimits::MAX_INT32_T );

    WModule::properties();
}

void WMDetTractCulling::cullOutTracts()
{
    // store the parameters from GUI incase they may change during computation
    double proximity_t      = m_proximity_t->get();
    double dSt_culling_t    = m_dSt_culling_t->get();

    size_t numTracts = m_dataset->size();

    boost::shared_ptr< WProgress > sortProgress( new WProgress( "Sorting tracts", numTracts ) );
    m_progress->addSubProgress( sortProgress );
    m_dataset->sortDescLength();  // biggest first, this is for speed up, incrementing progress not possible (std::sort is used)
    sortProgress->finish();

    std::vector< bool > unusedTracts( numTracts, false );

    boost::function< double ( const WFiber& q, const WFiber& r ) > dSt; // NOLINT
    const double proxSquare = proximity_t * proximity_t;
    dSt = boost::bind( WFiber::distDST, proxSquare, _1, _2 );

    boost::shared_ptr< WProgress > progress( new WProgress( "Tract culling", numTracts ) );
    m_progress->addSubProgress( progress );

    for( size_t q = 0; q < numTracts && !m_shutdownFlag(); ++q )  // loop over all tracts
    {
        if( unusedTracts[q] )
        {
            ++*progress;
            continue;
        }
        for( size_t r = q + 1; r < numTracts && !m_shutdownFlag(); ++r )
        {
            if( unusedTracts[r] )
            {
                continue;
            }
            double dst = dSt( (*m_dataset)[q], (*m_dataset)[r] );
            if( dst < dSt_culling_t )  // cullout small tracts nearby long tracts
            {
                if( (*m_dataset)[q].size() < (*m_dataset)[r].size() )
                {
                    unusedTracts[q] = true;
                    break;
                }
                else
                {
                    unusedTracts[r] = true;
                }
            }
        }
        ++*progress;
    }
    if( m_shutdownFlag() )
    {
        return; // abort requested
    }
    progress->finish();
    saveGainedTracts( unusedTracts );
}

void WMDetTractCulling::saveGainedTracts( const std::vector< bool >& unusedTracts )
{
    boost::shared_ptr< WProgress > eraseProgress( new WProgress( "Erasing tracts", unusedTracts.size() ) );
    m_progress->addSubProgress( eraseProgress );
    boost::shared_ptr< const WDataSetFiberVector > result =  m_dataset->generateDataSetOutOfUsedFibers( unusedTracts );
    m_tractOC->updateData( result->toWDataSetFibers() );
    eraseProgress->finish();

    m_numRemovedTracts->set( unusedTracts.size() - m_tractOC->getData()->size() );

    boost::shared_ptr< WProgress > saveProgress( new WProgress( "Saving tracts", unusedTracts.size() ) );
    m_progress->addSubProgress( saveProgress );
    if( m_saveCulledCurves->get() )
    {
        try
        {
            WWriterFiberVTK w( m_savePath->get(), true );
            w.writeFibs( result );
        }
        catch( const WDHIOFailure& e )
        {
            errorLog() << "While writing tracts to file: " << e.what();
        }
    }
    saveProgress->finish();
}

boost::filesystem::path WMDetTractCulling::saveFileName( std::string dataFileName ) const
{
    std::stringstream newExtension;
    newExtension << std::fixed << std::setprecision( 2 );
    newExtension << ".pt-" << m_proximity_t->get() << ".dst-" << m_dSt_culling_t->get() << ".fib";
    boost::filesystem::path tractFileName( dataFileName );
    return tractFileName.replace_extension( newExtension.str() );
}
