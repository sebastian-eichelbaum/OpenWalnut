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

#include "../../common/datastructures/WFiber.h"
#include "../../common/WColor.h"
#include "../../common/WLogger.h"
#include "../../common/WProgress.h"
#include "../../common/WPropertyHelper.h"
#include "../../dataHandler/io/WWriterFiberVTK.h"
#include "../../dataHandler/WDataSetFiberVector.h"
#include "../../dataHandler/WSubject.h"
#include "../../kernel/WKernel.h"
#include "fiberCulling.xpm"
#include "WMFiberCulling.h"

WMFiberCulling::WMFiberCulling()
    : WModule(),
      m_recompute( new WCondition() )
{
}

WMFiberCulling::~WMFiberCulling()
{
}

boost::shared_ptr< WModule > WMFiberCulling::factory() const
{
    return boost::shared_ptr< WModule >( new WMFiberCulling() );
}

const char** WMFiberCulling::getXPMIcon() const
{
    return fiberCulling_xpm;
}

void WMFiberCulling::moduleMain()
{
    // when conditions are fireing while wait() is not reached: wait terminates
    // and behaves as if the appropriate conditions have had fired. But it is
    // not detectable how many times a condition has fired.
    m_moduleState.setResetable();
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );
    m_moduleState.add( m_recompute );

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
            assert( m_rawDataset );
            infoLog() << "Start: WDataSetFibers => WDataSetFiberVector";
            m_dataset = boost::shared_ptr< WDataSetFiberVector >( new WDataSetFiberVector( m_rawDataset ) );
            m_numTracts->set( static_cast< int32_t >( m_dataset->size() ) );
            infoLog() << "Stop:  WDataSetFibers => WDataSetFiberVector";
        }

        assert( m_savePath );
        if( m_savePath->get().string() == "/no/such/file" )
        {
            m_savePath->set( saveFileName( m_dataset->getFileName() ) );
        }

        if( m_run->changed() )
        {
            m_run->get( true ); // eat change
            cullOutFibers();
        }

        m_moduleState.wait();
    }
}

void WMFiberCulling::connectors()
{
    typedef WModuleInputData< WDataSetFibers > FiberInputData;  // just an alias
    m_fiberInput = boost::shared_ptr< FiberInputData >( new FiberInputData( shared_from_this(), "fiberInput", "A loaded fiber dataset." ) );

    typedef WModuleOutputData< WDataSetFibers > FiberOutputData;  // just an alias
    m_output = boost::shared_ptr< FiberOutputData >( new FiberOutputData( shared_from_this(), "fiberOutput", "The fibers that survied culling." ) );

    addConnector( m_fiberInput );
    addConnector( m_output );
    WModule::connectors();  // call WModules initialization
}

void WMFiberCulling::properties()
{
    m_dSt_culling_t    = m_properties->addProperty( "Min fiber distance", "If below, the shorter fiber is culled out", 6.5 );
    m_proximity_t      = m_properties->addProperty( "Min point distance", "Min distance of points of two fibers which should be considered", 1.0 );
    m_saveCulledCurves = m_properties->addProperty( "Save result", "If true the remaining fibers are save to a file", false );
    m_savePath         = m_properties->addProperty( "Save path", "Where to save the result", boost::filesystem::path( "/no/such/file" ) );
    m_run              = m_properties->addProperty( "Run", "Go go go with those parameters", false, m_recompute );
    m_run->get( true ); // reset so no initial run occurs
    WPropertyHelper::PC_PATHEXISTS::addTo( m_savePath );
    m_numTracts = m_infoProperties->addProperty( "#Tracts", "Number of tracts beeing processed", 0 );
    m_numTracts->setMin( 0 );
    m_numTracts->setMax( wlimits::MAX_INT32_T );
}

void WMFiberCulling::cullOutFibers()
{
    // get parameter from GUI (properties)
    double proximity_t      = m_proximity_t->get();
    double dSt_culling_t    = m_dSt_culling_t->get();
    bool   saveCulledCurves = m_saveCulledCurves->get();
    boost::filesystem::path savePath = m_savePath->get();

    size_t numTracts = m_dataset->size();

    boost::shared_ptr< WProgress > sortProcess( new WProgress( "Sorting tracts", numTracts ) );
    // TODO(math): implement sort progress in WDataSetFiberVector
    m_dataset->sortDescLength();  // biggest first, this is for speed up
    sortProcess->finish();

    std::vector< bool > unusedFibers( numTracts, false );

    boost::function< double ( const wmath::WFiber& q, const wmath::WFiber& r ) > dSt; // NOLINT
    const double proxSquare = proximity_t * proximity_t;
    dSt = boost::bind( wmath::WFiber::distDST, proxSquare, _1, _2 );

    boost::shared_ptr< WProgress > progress( new WProgress( "Fiber culling", numTracts ) );
    m_progress->addSubProgress( progress );

    for( size_t q = 0; q < numTracts && !m_shutdownFlag(); ++q )  // loop over all tracts
    {
        if( unusedFibers[q] )
        {
            ++*progress;
            continue;
        }
        for( size_t r = q + 1; r < numTracts && !m_shutdownFlag(); ++r )
        {
            if( unusedFibers[r] )
            {
                continue;
            }
            double dst = dSt( (*m_dataset)[q], (*m_dataset)[r] );
            if( dst < dSt_culling_t )  // cullout small fibs nearby long fibs
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
    if( m_shutdownFlag() )
    {
        return; // abort requested
    }
    progress->finish();

    boost::shared_ptr< const WDataSetFiberVector > result =  m_dataset->generateDataSetOutOfUsedFibers( unusedFibers );
    infoLog() << "Start: WDataSetFibers <= WDataSetFiberVector";
    m_output->updateData( result->toWDataSetFibers() );
    infoLog() << "Stop:  WDataSetFibers <= WDataSetFiberVector";

    infoLog() << "Erasing done.";
    infoLog() << "Culled out " << numTracts - m_output->getData()->size() << " fibers";
    infoLog() << "There are " << m_output->getData()->size() << " fibers left.";

    if( saveCulledCurves )
    {
        WWriterFiberVTK w( savePath, true );
        w.writeFibs( result );
    }
    infoLog() << "Clulling done.";
}

boost::filesystem::path WMFiberCulling::saveFileName( std::string dataFileName ) const
{
    std::stringstream newExtension;
    newExtension << std::fixed << std::setprecision( 2 );
    newExtension << ".pt-" << m_proximity_t->get() << ".dst-" << m_dSt_culling_t->get() << ".fib";
    boost::filesystem::path fibFileName( dataFileName );
    return fibFileName.replace_extension( newExtension.str() );
}
