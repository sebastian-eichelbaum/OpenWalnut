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
#include "../../dataHandler/WDataSetFiberVector.h"
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
      m_saveCulledCurves( false ),
      m_run( new WCondition(), false )
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
    // when conditions are fireing while wait() is not reached: wait terminates
    // and behaves as if the appropriate conditions have had fired. But it is
    // not detectable how many times a condition has fired.
    m_moduleState.setResetable();
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );
    m_moduleState.add( m_run.getCondition() );

    ready();

    while ( !m_shutdownFlag() ) // loop until the module container requests the module to quit
    {
        if ( !m_fiberInput->getData().get() ) // ok, the output has not yet sent data
        {
            m_moduleState.wait();
            continue;
        }

        if( m_rawDataset != m_fiberInput->getData() ) // in case data has changed
        {
            m_rawDataset = m_fiberInput->getData();
            assert( m_rawDataset.get() );
            infoLog() << "Start: WDataSetFibers => WDataSetFiberVector";
            m_dataset = boost::shared_ptr< WDataSetFiberVector >( new WDataSetFiberVector( m_rawDataset ) );
            infoLog() << "Stop:  WDataSetFibers => WDataSetFiberVector";
        }

        if( m_savePath.empty() )
        {
            if( m_properties->findProp( "save path" ) )
            {
                m_properties->findProp( "save path" )->setValue( saveFileName() );
            }
            else
            {
                m_savePath = saveFileName();
            }
        }

        if( m_run.get() )
        {
            update();
            m_run.set( false );
            m_properties->reemitChangedValueSignals();
        }

        m_moduleState.wait();
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

    typedef WModuleOutputData< WDataSetFibers > FiberOutputData;  // just an alias
    m_output = shared_ptr< FiberOutputData >( new FiberOutputData( shared_from_this(), "fiberOutput", "The fibers that survied culling." ) );

    addConnector( m_fiberInput );
    addConnector( m_output );
    WModule::connectors();  // call WModules initialization
}

void WMFiberCulling::properties()
{
    m_properties->addDouble( "min distance threshold", m_dSt_culling_t, false, "Min distance of two \"different\" fibers"
                           )->connect( boost::bind( &WMFiberCulling::slotPropertyChanged, this, _1 ) );
    m_properties->addDouble( "proximity threshold", m_proximity_t, false, "Min distance of points of two fibers which should be considered"
                           )->connect( boost::bind( &WMFiberCulling::slotPropertyChanged, this, _1 ) );
    m_properties->addBool( "save culled curves", m_saveCulledCurves, false
                         )->connect( boost::bind( &WMFiberCulling::slotPropertyChanged, this, _1 ) );
    m_properties->addString( "save path", m_savePath, false, "path where to save culled curves"
                           )->connect( boost::bind( &WMFiberCulling::slotPropertyChanged, this, _1 ) );
    m_properties->addBool( "GO", false, false, "initiate run"
                         )->connect( boost::bind( &WMFiberCulling::slotPropertyChanged, this, _1 ) );
}

void WMFiberCulling::slotPropertyChanged( std::string propertyName )
{
    if( m_run.get() )
    {
        assert( m_properties->findProp( propertyName ) );
        m_properties->findProp( propertyName )->dirty( true );
        debugLog() << "Property: " << propertyName << " marked as dirty";
    }
    else
    {
        debugLog() << "Property: " << propertyName << " has changed";
        if( propertyName == "GO" )
        {
            m_run.set( true );
        }
        else if( propertyName == "min distance threshold" )
        {
            m_dSt_culling_t = m_properties->getValue< double >( propertyName );
        }
        else if( propertyName == "proximity threshold" )
        {
            m_proximity_t = m_properties->getValue< double >( propertyName );
        }
        else if( propertyName == "save culled curves" )
        {
            m_saveCulledCurves = m_properties->getValue< bool >( propertyName );
        }
        else if( propertyName == "save path" )
        {
            m_savePath = m_properties->getValue< std::string >( propertyName );
        }
        else
        {
            std::cerr << propertyName << std::endl; // we must use std::cerr since WLogger needs to much time!
            assert( 0 && "This property name is not supported by this function yet." );
        }
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
    progress->finish();

    boost::shared_ptr< const WDataSetFiberVector > result =  m_dataset->generateDataSetOutOfUsedFibers( unusedFibers );
    infoLog() << "Start: WDataSetFibers <= WDataSetFiberVector";
    m_output->updateData( result->toWDataSetFibers() );
    infoLog() << "Stop:  WDataSetFibers <= WDataSetFiberVector";

    infoLog() << "Erasing done.";
    infoLog() << "Culled out " << numFibers - m_output->getData()->size() << " fibers";
    infoLog() << "There are " << m_output->getData()->size() << " fibers left.";

    if( m_saveCulledCurves )
    {
        WWriterFiberVTK w( m_savePath, true );
        w.writeFibs( result );
    }
}

std::string WMFiberCulling::saveFileName() const
{
    std::stringstream newExtension;
    newExtension << std::fixed << std::setprecision( 2 );
    newExtension << ".pt-" << m_proximity_t << ".dst-" << m_dSt_culling_t << ".fib";
    boost::filesystem::path fibFileName( m_dataset->getFileName() );
    return fibFileName.replace_extension( newExtension.str() ).string();
}
