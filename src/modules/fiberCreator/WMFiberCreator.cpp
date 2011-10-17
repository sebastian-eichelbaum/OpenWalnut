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

#include <cmath>
#include <algorithm>
#include <vector>
#include <string>

#include "../../kernel/WKernel.h"
#include "../../common/WPropertyHelper.h"
#include "../../common/math/WMath.h"
#include "../../dataHandler/WDataHandler.h"

#include "WMFiberCreator.h"
#include "WMFiberCreator.xpm"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMFiberCreator )

WMFiberCreator::WMFiberCreator():
    WModule()
{
}

WMFiberCreator::~WMFiberCreator()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMFiberCreator::factory() const
{
    return boost::shared_ptr< WModule >( new WMFiberCreator() );
}

const char** WMFiberCreator::getXPMIcon() const
{
    return WMFiberCreator_xpm;
}

const std::string WMFiberCreator::getName() const
{
    return "Fiber Creator";
}

const std::string WMFiberCreator::getDescription() const
{
    return "This module can create fiber datasets using certain schemes. This can be useful to test your algorithms against special artificial "
           "data that shows certain strengths or weaknesses of your module.";
}

void WMFiberCreator::connectors()
{
    // the selected fibers go to this output
    m_fiberOutput = WModuleOutputData< WDataSetFibers >::createAndAdd( shared_from_this(), "out", "The created fiber data." );

    // call WModule's initialization
    WModule::connectors();
}

void WMFiberCreator::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_numFibers = m_properties->addProperty( "Num Fibers", "The number of fibers to create.", 500, m_propCondition );
    m_numFibers->setMin( 1 );
    m_numFibers->setMax( 10000 );
    m_numVertsPerFiber = m_properties->addProperty( "Num Vertices", "Vertices per fiber.", 1000, m_propCondition );
    m_numVertsPerFiber->setMin( 1 );
    m_numVertsPerFiber->setMax( 10000 );

    m_fibColor = m_properties->addProperty( "Color", "Color for the fibers.", defaultColor::WHITE, m_propCondition );

    // call WModule's initialization
    WModule::properties();
}

void WMFiberCreator::spiral( size_t numFibers, size_t numVertsPerFiber,
            WDataSetFibers::VertexArray vertices,
            WDataSetFibers::IndexArray fibIdx,
            WDataSetFibers::LengthArray lengths,
            WDataSetFibers::IndexArray fibIdxVertexMap,
            WDataSetFibers::ColorArray colors
        )
{
    WPosition m_centerPoint = WPosition( 0.0, 0.0, 0.0 );
    double m_spiralRadius = 10.0;
    double m_tubeRadius = 1.0;
    double m_height = 25.0;
    double m_rotations = 10.0;
    WColor fibColor = m_fibColor->get();

    // create each
    for( size_t fidx = 0; fidx < numFibers; ++fidx )
    {
        size_t vertOffset = fidx * numVertsPerFiber;
        fibIdx->push_back( vertOffset );
        lengths->push_back( numVertsPerFiber );

        double a1 = static_cast< double >( std::rand() % 255 ) / 255.0;
        double a2 = static_cast< double >( std::rand() % 255 ) / 255.0;

        double seedX = cos( 2.0 * piDouble * a1 ) * m_tubeRadius;
        double seedY = sin( 2.0 * piDouble * a2 ) * m_tubeRadius;
        double seedZ = sqrt( m_tubeRadius - ( seedX * seedX ) - ( seedY * seedY ) );
        WPosition seed = m_centerPoint + WPosition( seedX, seedY, seedZ );

        // create the vertices
        for( size_t vidx = 0; vidx < numVertsPerFiber; ++vidx )
        {
            double v = static_cast< double >( vidx ) / static_cast< double >( numVertsPerFiber - 1 );
            double degree = v * 2.0 * piDouble * m_rotations;

            double X = seed.x() + cos( degree ) * v * m_spiralRadius;
            double Y = seed.y() + sin( degree ) * v * m_spiralRadius;
            double Z = seed.z() + ( v * m_height );

            vertices->push_back( X );
            vertices->push_back( Y );
            vertices->push_back( Z );

            colors->push_back( fibColor.x() );
            colors->push_back( fibColor.y() );
            colors->push_back( fibColor.z() );
            fibIdxVertexMap->push_back( fidx );
        }
    }
}

void WMFiberCreator::crossing( size_t numFibers, size_t numVertsPerFiber,
            WDataSetFibers::VertexArray vertices,
            WDataSetFibers::IndexArray fibIdx,
            WDataSetFibers::LengthArray lengths,
            WDataSetFibers::IndexArray fibIdxVertexMap,
            WDataSetFibers::ColorArray colors
        )
{
    WPosition starts[2];
    WPosition ends[2];
    starts[0] = WPosition( -10.0, 0.0, -10.0 );
    starts[1] = WPosition( -10.0, 5.0,  10.0 );
    ends[0] = WPosition( 10.0, 0.0,  10.0 );
    ends[1] = WPosition( 10.0, 5.0, -10.0 );
    WColor fibColor = m_fibColor->get();

    // create each
    unsigned char pidx = 0;
    for( size_t fidx = 0; fidx < numFibers; ++fidx )
    {
        size_t vertOffset = fidx * numVertsPerFiber;
        fibIdx->push_back( vertOffset );
        lengths->push_back( numVertsPerFiber );

        // current bundle
        pidx = ( fidx >= ( numFibers / 2 ) );

        // offset from start for this fib
        WPosition offset( 0.0, 0.0, static_cast< double >( fidx ) / static_cast< double >( numFibers - 1 ) );

        // create the vertices
        for( size_t vidx = 0; vidx < numVertsPerFiber; ++vidx )
        {
            double v = static_cast< double >( vidx ) / static_cast< double >( numVertsPerFiber - 1 );
            WPosition vert = v * ( offset + starts[pidx] ) + ( 1.0 - v ) * ( offset + ends[pidx] );

            vertices->push_back( vert.x() );
            vertices->push_back( vert.y() );
            vertices->push_back( vert.z() );

            colors->push_back( fibColor.x() );
            colors->push_back( fibColor.y() );
            colors->push_back( fibColor.z() );
            fibIdxVertexMap->push_back( fidx );
        }
    }
}

void WMFiberCreator::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    // Remember the condition provided to some properties in properties()? The condition can now be used with this condition set.
    m_moduleState.add( m_propCondition );

    ready();

    // main loop
    while ( !m_shutdownFlag() )
    {
        // woke up since the module is requested to finish?
        if ( m_shutdownFlag() )
        {
            break;
        }

        // To query whether an input was updated, simply ask the input:
        bool propUpdated = true;
        if ( !propUpdated )
        {
            continue;
        }

        // for each fiber:
        size_t numFibers = m_numFibers->get();
        size_t numVertsPerFiber = m_numVertsPerFiber->get();
        size_t numVerts = numVertsPerFiber * numFibers;
        debugLog() << "Creating " << numFibers << " fibers.";
        boost::shared_ptr< WProgress > progress1 = boost::shared_ptr< WProgress >( new WProgress( "Creating fibers.",
                                                                                                  numFibers ) );

        // Create needed arrays:
        WDataSetFibers::VertexArray vertices = WDataSetFibers::VertexArray( new WDataSetFibers::VertexArray::element_type() );
        WDataSetFibers::IndexArray fibIdx  = WDataSetFibers::IndexArray( new WDataSetFibers::IndexArray::element_type() );
        WDataSetFibers::LengthArray lengths = WDataSetFibers::LengthArray( new WDataSetFibers::LengthArray::element_type() );
        WDataSetFibers::IndexArray fibIdxVertexMap = WDataSetFibers::IndexArray( new WDataSetFibers::IndexArray::element_type() );
        WDataSetFibers::ColorArray colors = WDataSetFibers::ColorArray( new WDataSetFibers::ColorArray::element_type() );
        vertices->reserve( numVerts * 3 );
        fibIdx->reserve( numFibers );
        lengths->reserve( numFibers );
        fibIdxVertexMap->reserve( numVerts );
        colors->reserve( numVerts * 3 );

        // the bounding box. We calc it during creation to save some time later during WDataSetFibers creation.
        WBoundingBox bbox;

        std::srand( time( 0 ) );
        spiral( numFibers, numVertsPerFiber, vertices, fibIdx, lengths, fibIdxVertexMap, colors );
        //crossing( numFibers, numVertsPerFiber, vertices, fibIdx, lengths, fibIdxVertexMap, colors );

        // update output:
        debugLog() << "Done. Updating output.";
        WDataSetFibers::SPtr d = WDataSetFibers::SPtr( new WDataSetFibers( vertices, fibIdx, lengths, fibIdxVertexMap ) );
        d->addColorScheme( colors, "Creator", "The color from Fiber Creator." );
        m_fiberOutput->updateData( d );

        debugLog() << "Waiting ...";
        m_moduleState.wait();
    }
}

