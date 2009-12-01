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
#include "../../dataHandler/io/WWriterVTK.h"
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

    cullOutFibers( fiberDS );

    std::cout << "done." << std::endl;

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
    std::cout << "Recoginzed " << numFibers << " fibers" << std::endl;

    fibers->sortDescLength();  // sort the fiber on their length (biggest first)
    std::cout << "Sorted fibers done." << std::endl;
    m_proximity_t = 1.0;
    std::cout << "Proximity threshold: " << m_proximity_t << std::endl;
    m_dSt_culling_t = 6.5;
    std::cout << "Culling threshold: " << m_dSt_culling_t << std::endl;
    std::vector< bool > unusedFibers( numFibers, false );
    WDSTMetric dSt( m_proximity_t * m_proximity_t );
    WStatusReport st( numFibers );
    for( size_t i = 0; i < numFibers; ++i )  // loop over all streamlines
    {
        if( unusedFibers[i] )
        {
            continue;
        }
        const wmath::WFiber& q = (*fibers)[ i ];
        for( size_t j = i + 1;  j < numFibers; ++j )
        {
            if( unusedFibers[j] )
            {
                continue;
            }
            const wmath::WFiber& r = (*fibers)[j];
            double dst = dSt.dist( q, r );
            if( dst < m_dSt_culling_t )  // cullout small fibs nearby long fibs
            {
                if( q.size() < r.size() )
                {
                    unusedFibers[i] = true;
                    break;
                }
                else
                {
                    unusedFibers[j] = true;
                }
            }
        }
        std::cout << "\r" << std::fixed << std::setprecision( 2 );
        std::cout << ( ++st ).progress() << " " << st.stringBar() << std::flush;
    }
    std::cout << std::endl;

    fibers->erase( unusedFibers );
    std::cout << "Erasing done." << std::endl;
    std::cout << "Culled out " << numFibers - fibers->size() << " fibers" << std::endl;
    std::cout << "There are " << fibers->size() << " fibers left." << std::endl;
    if( true )
    {
        WWriterVTK w( "/tmp/pansen", true );
        w.writeFibs( fibers );
//        std::cout << "Saved fibers left from culling to " <<  << " done." << std::endl;
    }
}
