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
#include <vector>

#include <osg/Geode>
#include <osg/Geometry>

#include "WFiberCulling.h"
#include "../../math/WFiber.h"
#include "../../common/WLogger.h"
#include "../../common/WColor.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WSubject.h"
#include "../../dataHandler/WDataSetFibers.h"
#include "../../dataHandler/WLoaderManager.h"
#include "../../kernel/WKernel.h"
#include "../../utils/WColorUtils.h"

WFiberCulling::WFiberCulling()
    : WModule()
{
}

WFiberCulling::~WFiberCulling()
{
}

const std::string WFiberCulling::getName() const
{
    return std::string( "FiberCulling" );
}

const std::string WFiberCulling::getDescription() const
{
    return std::string( "Removes or culls out fibers from a WDataSetFibers" );
}

void WFiberCulling::threadMain()
{
    boost::shared_ptr< WDataHandler > dataHandler;
    // TODO(wiebel): fix this hack when possible by using an input connector.
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

//    osg::ref_ptr< osg::Group > group = osg::ref_ptr< osg::Group >( new osg::Group );
//
//    for( size_t i = 0; i < fibers.size(); ++i )
//    {
//        group->addChild( genFiberGeode( fibers[i] ).get() );
//    }
//    group->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
//
//    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->addChild( group.get() );
//

    cullOutFibers( fiberDS );

    // Since the modules run in a separate thread: such loops are possible
    while ( !m_FinishRequested )
    {
        // do fancy stuff
        sleep( 1 );
    }
}

void WFiberCulling::cullOutFibers( boost::shared_ptr< WDataSetFibers > fibers )
{
    size_t numFibers = fibers->size();
    std::cout << "Recoginzed " << numFibers << " fibers" << std::endl;

    fibers->sortDescLength();  // sort the fiber on their length (biggest first)
    std::cout << "Sorted fibers done." << std::endl;

    const double proximity_t_Square = m_proximity_t * m_proximity_t;
    std::vector< bool > unusedFibers( numFibers, false );
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
            double dst = q.dSt( r, proximity_t_Square );

            if( dst < m_dSt_culling_t )  // cullout small fibs nearby long fibs
            {
                if( q.size() < r.size() ) {
                    unusedFibers[i] = true;
                    break;
                }
                else {
                    unusedFibers[j] = true;
                }
            }
        }
    }

//    if( m_saveCulledCurves ) {
//        saveFib( m_savePath, fibers, unusedFibers );
//        std::cout << "Saving culled fibers to " << m_savePath << " done." << std::endl;
//    }

    // create new DataSet where unused fibers are not contained
    // fibers->erase( unusedFibers );
    // std::cout << "Erasing done." << std::endl;
    // std::cout << "Culled out " << numFibers - fibers->size() << " fibers" << std::endl;
    // std::cout << "There are " << fibers->size() << " fibers left." << std::endl;
}
