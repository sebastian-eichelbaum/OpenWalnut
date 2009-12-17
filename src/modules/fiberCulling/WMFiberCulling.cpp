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

#include <osg/Geode>
#include <osg/Geometry>

#include "WMFiberCulling.h"
#include "../../math/WFiber.h"
#include "../../math/fiberSimilarity/WDSTMetric.h"
#include "../../common/WColor.h"
#include "../../common/WLogger.h"
#include "../../common/WStatusReport.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WSubject.h"
#include "../../dataHandler/WDataSetFibers.h"
#include "../../dataHandler/io/WWriterFiberVTK.h"
#include "../../kernel/WKernel.h"
#include "../../utils/WColorUtils.h"

WMFiberCulling::WMFiberCulling()
    : WModule(),
      m_proximity_t( 0.0 ),
      m_dSt_culling_t( 0.0 ),
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
    // signal ready state
    ready();

    boost::shared_ptr< WDataHandler > dataHandler;
    // TODO(math): fix this hack when possible by using an input connector.
    while( !WKernel::getRunningKernel() )
    {
        sleep( 1 );
    }
    while( !( dataHandler = WKernel::getRunningKernel()->getDataHandler() ) )
    {
        sleep( 1 );
    }
    while( !dataHandler->getNumberOfSubjects() )
    {
        sleep( 1 );
    }

    boost::shared_ptr< WDataSetFibers > fiberDS;
    assert( fiberDS = boost::shared_dynamic_cast< WDataSetFibers >( dataHandler->getSubject( 0 )->getDataSet( 0 ) ) );

    // TODO(math): default parameters via property object
    m_proximity_t = 1.0;
    infoLog() << "Proximity threshold: " << m_proximity_t;
    m_dSt_culling_t = 6.5;
    infoLog() << "Culling threshold: " << m_dSt_culling_t;

    cullOutFibers( fiberDS );

    infoLog() << "done.";

    // Since the modules run in a separate thread: such loops are possible
    while ( !m_FinishRequested )
    {
        // do fancy stuff
        sleep( 1 );
    }
}

void WMFiberCulling::cullOutFibers( boost::shared_ptr< WDataSetFibers > fibers )
{
    size_t numFibers = fibers->size();
    infoLog() << "Recoginzed " << numFibers << " fibers";

    fibers->sortDescLength();  // biggest first, this is for speed up
    infoLog() << "Sorted fibers done.";

    std::vector< bool > unusedFibers( numFibers, false );

    WDSTMetric dSt( m_proximity_t * m_proximity_t );
    WStatusReport st( numFibers );

    for( size_t q = 0; q < numFibers; ++q )  // loop over all streamlines
    {
        if( unusedFibers[q] )
        {
            continue;
        }
        for( size_t r = q + 1;  r < numFibers; ++r )
        {
            if( unusedFibers[r] )
            {
                continue;
            }
            double dst = dSt.dist( (*fibers)[q], (*fibers)[r] );
            if( dst < m_dSt_culling_t )  // cullout small fibs nearby long fibs
            {
                if( (*fibers)[q].size() < (*fibers)[r].size() )
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
        std::stringstream ss;
        ss << "\r" << std::fixed << std::setprecision( 2 ) << ( ++st ).progress() << " " << st.stringBar();
        std::cout << ss.str() << std::flush;
    }
    std::cout << std::endl;

    fibers->erase( unusedFibers );
    infoLog() << "Erasing done.";
    infoLog() << "Culled out " << numFibers - fibers->size() << " fibers";
    infoLog() << "There are " << fibers->size() << " fibers left.";

    // TODO(math): make saving parameter dependent, and apply the desired path for saving
    WWriterFiberVTK w( "/tmp/pansen.fib", true );
    w.writeFibs( fibers );
}
