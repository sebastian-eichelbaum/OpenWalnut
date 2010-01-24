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

#include <boost/filesystem.hpp>

#include "../../common/WColor.h"
#include "../../common/WLogger.h"
#include "../../common/WProgress.h"
#include "../../dataHandler/WDataSetFibers.h"
#include "../../dataHandler/WSubject.h"
#include "../../dataHandler/io/WWriterFiberVTK.h"
#include "../../kernel/WKernel.h"
#include "../../math/WFiber.h"
#include "../../math/fiberSimilarity/WDSTMetric.h"
#include "WMFiberCulling.h"

WMFiberCulling::WMFiberCulling()
    : WModule(),
      m_proximity_t( 1.0 ),
      m_dSt_culling_t( 6.5 ),
      m_saveCulledCurves( false )
{
}

WMFiberCulling::~WMFiberCulling()
{
}

boost::shared_ptr< WModule > WMFiberCulling::factory() const
{
    return boost::shared_ptr< WModule >( new WMFiberCulling() );
}

void WMFiberCulling::moduleMain()
{
    // additional fire-condition: "data changed" flag
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );

    ready();

    while ( !m_shutdownFlag() ) // loop until the module container requests the module to quit
    {
        m_dataset = m_fiberInput->getData();
        if ( !m_dataset.get() ) // ok, the output has not yet sent data
        {
            m_moduleState.wait();
            continue;
        }

        m_moduleState.wait(); // waits for firing of m_moduleState ( dataChanged, shutdown, etc. )
    }
}

void WMFiberCulling::update()
{
    infoLog() << "Proximity threshold: " << m_proximity_t;
    infoLog() << "Culling threshold: " << m_dSt_culling_t;

    cullOutFibers();

    infoLog() << "Clulling done.";
}

void WMFiberCulling::connectors()
{
    using boost::shared_ptr;
    typedef WModuleInputData< WDataSetFibers > FiberInputData;  // just an alias

    m_fiberInput = shared_ptr< FiberInputData >( new FiberInputData( shared_from_this(), "fiberInput", "A loaded fiber dataset." ) );

    addConnector( m_fiberInput );
    WModule::connectors();  // call WModules initialization
}

void WMFiberCulling::properties()
{
    m_properties->addDouble( "min distance threshold",
                             m_dSt_culling_t,
                             false,
                             "Minimum distance of two \"different\" fibers."
                           )->connect( boost::bind( &WMFiberCulling::slotPropertyChanged, this, _1 ) );
    m_properties->addDouble( "proximity threshold",
                             m_proximity_t,
                             false,
                             "defines the minimum distance between two fibers which should be considered in distance measure."
                           )->connect( boost::bind( &WMFiberCulling::slotPropertyChanged, this, _1 ) );
    m_properties->addBool( "GO",
                           false,
                           false,
                           "initiate run"
                         )->connect( boost::bind( &WMFiberCulling::slotPropertyChanged, this, _1 ) );
}

void WMFiberCulling::slotPropertyChanged( std::string propertyName )
{
    std::cout << "prop: " << propertyName << " has changed" << std::endl;
    if( propertyName == "GO" )
    {
        update();
    }
    else if( propertyName == "min distance threshold" )
    {
        m_dSt_culling_t = m_properties->getValue< double >( propertyName );
    }
    else if( propertyName == "proximity threshold" )
    {
        m_proximity_t = m_properties->getValue< double >( propertyName );
    }
    else
    {
        // instead of WLogger we must use std::cerr since WLogger needs to much time!
        std::cerr << propertyName << std::endl;
        assert( 0 && "This property name is not supported by this function yet." );
    }
}

void WMFiberCulling::cullOutFibers()
{
    size_t numFibers = m_dataset->size();
    infoLog() << "Recoginzed " << numFibers << " fibers";

    m_dataset->sortDescLength();  // biggest first, this is for speed up
    infoLog() << "Sorted fibers done.";

    std::vector< bool > unusedFibers( numFibers, false );

    WDSTMetric dSt( m_proximity_t * m_proximity_t );

    boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Fiber culling", numFibers ) );
    m_progress->addSubProgress( progress );

    for( size_t q = 0; q < numFibers; ++q )  // loop over all streamlines
    {
        if( unusedFibers[q] )
        {
            ++*progress;
            continue;
        }
        for( size_t r = q + 1;  r < numFibers; ++r )
        {
            if( unusedFibers[r] )
            {
                continue;
            }
            double dst = dSt.dist( (*m_dataset)[q], (*m_dataset)[r] );
            if( dst < m_dSt_culling_t )  // cullout small fibs nearby long fibs
            {
                if( (*m_dataset)[q].size() < (*m_dataset)[r].size() )
                {
                    unusedFibers[q] = true;
                    break;
                }
                else
                {
                    unusedFibers[r] = true;
                }
            }
        }
        ++*progress;
    }

    m_dataset->erase( unusedFibers );
    infoLog() << "Erasing done.";
    infoLog() << "Culled out " << numFibers - m_dataset->size() << " fibers";
    infoLog() << "There are " << m_dataset->size() << " fibers left.";

    WWriterFiberVTK w( saveFileName(), true );
    w.writeFibs( m_dataset );
}

std::string WMFiberCulling::saveFileName() const
{
    std::stringstream newExtension;
    newExtension << std::fixed << std::setprecision( 2 );
    newExtension << ".pt-" << m_proximity_t << ".dst-" << m_dSt_culling_t << ".fib";
    boost::filesystem::path fibFileName( m_dataset->getFileName() );
    return fibFileName.replace_extension( newExtension.str() ).string();
}
