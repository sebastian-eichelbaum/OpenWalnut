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

#include <string>
#include <vector>

#include "core/dataHandler/WDataSetVector.h"
#include "core/kernel/WKernel.h"
#include "modules/emptyIcon.xpm" // Please put a real icon here.
#include "WMVectorAlign.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMVectorAlign )

WMVectorAlign::WMVectorAlign():
    WModule()
{
}

WMVectorAlign::~WMVectorAlign()
{
}

boost::shared_ptr< WModule > WMVectorAlign::factory() const
{
    return boost::shared_ptr< WModule >( new WMVectorAlign() );
}

const char** WMVectorAlign::getXPMIcon() const
{
    return emptyIcon_xpm; // Please put a real icon here.
}

const std::string WMVectorAlign::getName() const
{
    return "Vector Align";
}

const std::string WMVectorAlign::getDescription() const
{
    return "Aligns vectors of each grid cell, so they are aligned in same halfspace.";
}

void WMVectorAlign::connectors()
{
    m_vectorIC = WModuleInputData< WDataSetVector >::createAndAdd( shared_from_this(), "vectors", "Some vector dataset." );
    m_vectorOC = WModuleOutputData< WDataSetVector >::createAndAdd( shared_from_this(), "alignedVectors",
            "Dataset where vectors are aligned inside the halfspace" );

    WModule::connectors();
}

void WMVectorAlign::properties()
{
    WModule::properties();
}

void WMVectorAlign::requirements()
{
}

namespace
{
    boost::shared_ptr< WDataSetVector > alignVectors( boost::shared_ptr< WDataSetVector > vectors )
    {
        size_t numVecs = vectors->getValueSet()->size();
        std::vector< WVector3d > newData;
        newData.reserve( numVecs );
        for( size_t i = 0; i < numVecs; ++i  )
        {
            newData.push_back( vectors->getVectorAt( i ) );
        }

        boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( vectors->getGrid() );
        if( !grid )
        {
            return vectors;
        }

        boost::shared_ptr< std::vector< double > > data( new std::vector< double > );

        for( size_t i = 0; i < numVecs; ++i  )
        {
            std::vector< size_t > neighbours = grid->getNeighbours( i );
            for( size_t j = 0; j < neighbours.size(); ++j )
            {
                double sign = 1.0;
                if( neighbours[j] > i ) // just to speed up and ensure that once vectors written to data will never be changed afterwards
                {
                    if( dot( newData[i], newData[ neighbours[j] ] ) < 0.0 )
                    {
                        sign = -1.0;
                    }
                    newData[ neighbours[ j ] ] *= sign;
                }
            }
            // newData[i] will never change from now on anymore
            data->push_back( newData[i][0] );
            data->push_back( newData[i][1] );
            data->push_back( newData[i][2] );
        }
        boost::shared_ptr< WValueSet< double > > values( new WValueSet< double >( 1, 3, data, W_DT_DOUBLE ) );
        return boost::shared_ptr< WDataSetVector >( new WDataSetVector( values, grid ) );
    }
}

void WMVectorAlign::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_vectorIC->getDataChangedCondition() );

    ready();

    // main loop
    while( !m_shutdownFlag() )
    {
        infoLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish?
        if( m_shutdownFlag() )
        {
            break;
        }

        // save data behind connectors since it might change during processing
        boost::shared_ptr< WDataSetVector > vectors = m_vectorIC->getData();

        if( !vectors ) // if data valid
        {
            continue;
        }

        m_vectorOC->updateData( alignVectors( vectors ) );
    }
}
