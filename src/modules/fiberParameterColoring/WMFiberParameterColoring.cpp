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

#include "core/kernel/WKernel.h"
#include "core/common/WPropertyHelper.h"
#include "core/common/math/WMath.h"
#include "core/dataHandler/WDataHandler.h"

#include "WMFiberParameterColoring.h"
#include "WMFiberParameterColoring.xpm"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMFiberParameterColoring )

WMFiberParameterColoring::WMFiberParameterColoring():
    WModule()
{
}

WMFiberParameterColoring::~WMFiberParameterColoring()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMFiberParameterColoring::factory() const
{
    return boost::shared_ptr< WModule >( new WMFiberParameterColoring() );
}

const char** WMFiberParameterColoring::getXPMIcon() const
{
    return WMFiberParameterColoring_xpm;
}

const std::string WMFiberParameterColoring::getName() const
{
    return "Fiber Parameter Coloring";
}

const std::string WMFiberParameterColoring::getDescription() const
{
    return "This module can color fibers according to different parameters and schemes.";
}

void WMFiberParameterColoring::connectors()
{
    // The input fiber dataset
    m_fiberInput = boost::shared_ptr< WModuleInputData < WDataSetFibers > >(
        new WModuleInputData< WDataSetFibers >( shared_from_this(), "fibers", "The fiber dataset to color" )
    );

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_fiberInput );

    // the selected fibers go to this output
    m_fiberOutput = boost::shared_ptr< WModuleOutputData < WDataSetFibers > >(
        new WModuleOutputData< WDataSetFibers >( shared_from_this(),
                                        "out", "The fiber dataset colored by this module." )
    );

    // As above: make it known.
    addConnector( m_fiberOutput );

    // call WModule's initialization
    WModule::connectors();
}

void WMFiberParameterColoring::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_baseColor  = m_properties->addProperty( "Base Color", "The base color. Some kind of color offset.", WColor( 1.0, 1.0, 1.0, 0.0 ),
                                              m_propCondition );
    m_scaleColor = m_properties->addProperty( "Scale Color", "The color which gets scaled with the calculated value and added to the base color.",
                                              WColor( 0.0, 0.0, 0.0, 1.0 ), m_propCondition );

    // call WModule's initialization
    WModule::properties();
}

inline double getSegmentVector( size_t segment, size_t offset, boost::shared_ptr< std::vector< float > > verts, double* vec )
{
    // get segment coordinates
    double x = verts->at( ( 3 * segment ) + offset + 0 ) - verts->at( ( 3 * ( segment + 1 ) ) + offset + 0 );
    double y = verts->at( ( 3 * segment ) + offset + 1 ) - verts->at( ( 3 * ( segment + 1 ) ) + offset + 1 );
    double z = verts->at( ( 3 * segment ) + offset + 2 ) - verts->at( ( 3 * ( segment + 1 ) ) + offset + 2 );

    // get length
    double len = std::sqrt( x * x + y * y + z * z );

    // create vector from this and the previous point
    vec[0] = x / len;
    vec[1] = y / len;
    vec[2] = z / len;

    return len;
}

void WMFiberParameterColoring::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );
    // Remember the condition provided to some properties in properties()? The condition can now be used with this condition set.
    m_moduleState.add( m_propCondition );

    ready();

    m_fibCurvatureColors = WDataSetFibers::ColorArray();
    m_fibLengthColors = WDataSetFibers::ColorArray();

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
        bool dataUpdated = m_fiberInput->handledUpdate();
        boost::shared_ptr< WDataSetFibers > dataSet = m_fiberInput->getData();
        bool dataValid = ( dataSet != NULL );
        bool propUpdated = m_baseColor->changed() || m_scaleColor->changed();

        // reset everything if input was disconnected/invalid
        if( !dataValid )
        {
            debugLog() << "Resetting output.";
            m_fiberOutput->reset();
            continue;
        }

        if( dataValid && !( dataUpdated || propUpdated ) )
        {
            continue;
        }

        // remove old colorings
        dataSet->removeColorScheme( m_fibCurvatureColors );     // this can be safely used with NULL pointers
        dataSet->removeColorScheme( m_fibLengthColors );

        // get the fiber definitions
        boost::shared_ptr< std::vector< size_t > > fibStart = dataSet->getLineStartIndexes();
        boost::shared_ptr< std::vector< size_t > > fibLen   = dataSet->getLineLengths();
        boost::shared_ptr< std::vector< float > >  fibVerts = dataSet->getVertices();
        m_fibCurvatureColors = WDataSetFibers::ColorArray( new WDataSetFibers::ColorArray::element_type() );
        m_fibLengthColors = WDataSetFibers::ColorArray( new WDataSetFibers::ColorArray::element_type() );
        WDataSetFibers::ColorScheme::ColorMode colorMode = WDataSetFibers::ColorScheme::RGBA;
        m_fibCurvatureColors->resize( colorMode * ( fibVerts->size() / 3  ), 0.0 ); // create an RGBA coloring
        m_fibLengthColors->resize( colorMode * ( fibVerts->size() / 3  ), 0.0 ); // create an RGBA coloring

        // progress indication
        boost::shared_ptr< WProgress > progress1( new WProgress( "Coloring fibers.", fibStart->size() ) );
        boost::shared_ptr< WProgress > progress2( new WProgress( "Scaling Colors.", fibStart->size() ) );
        m_progress->addSubProgress( progress1 );
        m_progress->addSubProgress( progress2 );

        // for fastness:
        WColor baseColor = m_baseColor->get( true );
        double baseColorR = baseColor[0];
        double baseColorG = baseColor[1];
        double baseColorB = baseColor[2];
        double baseColorA = baseColor[3];
        WColor scaleColor = m_scaleColor->get( true );
        double scaleColorR = scaleColor[0];
        double scaleColorG = scaleColor[1];
        double scaleColorB = scaleColor[2];
        double scaleColorA = scaleColor[3];

        // for each fiber:
        debugLog() << "Iterating over all fibers.";
        std::vector< double > maxSegLengths;
        maxSegLengths.resize( fibStart->size(), 0.0 );
        for( size_t fidx = 0; fidx < fibStart->size() ; ++fidx )
        {
            ++*progress1;

            // the start vertex index
            size_t sidx = fibStart->at( fidx ) * 3;
            size_t cidx = fibStart->at( fidx ) * colorMode;

            // the length of the fiber, if a fiber is smaller than two segments, skip it ( it already is colored white by default )
            size_t len = fibLen->at( fidx );
            if( len < 3 )
            {
                continue;
            }

            // get the first vector and second vertex
            double prev[3];
            // we do not need zero length segments
            if( getSegmentVector( 0, sidx, fibVerts, &prev[0] ) == 0.0 )
            {
                continue;
            }

            // walk along the fiber
            double lenLast = 0.0;
            double lenMax = 0.0;
            for( size_t k = 1; k < len - 1; ++k )  // len -1 because we interpret it as segments
            {
                // get the vector of this segment
                double current[3];
                // we do not need zero length segments
                double segLen = getSegmentVector( k, sidx, fibVerts, &current[0] );
                if( segLen == 0.0 )
                {
                    continue;
                }

                // how to calculate the curvature?
                // -------------------------------
                // Variant 1:

                // calculate angle between both segments
                // double dot = ( current[0] * prev[0] ) + ( current[1] * prev[1] ) + ( current[2] * prev[2] );
                // dot = std::max( -1.0, std::min( dot, 1.0 ) ); // dot must not be larger than 1. Unfortunately it might get
                // larger in the 10^-10th.

                // get angle and curvature
                // double angleRad = std::acos( dot );
                // double curvature2 = 2.0 * angleRad / ( lenLast + segLen );

                // Variant 2:

                double x = ( 2.0 / ( lenLast + segLen ) ) * ( current[0] - prev[0] );
                double y = ( 2.0 / ( lenLast + segLen ) ) * ( current[1] - prev[1] );
                double z = ( 2.0 / ( lenLast + segLen ) ) * ( current[2] - prev[2] );
                double curvature = std::sqrt( x*x + y*y + z*z );

                ( *m_fibCurvatureColors )[ ( colorMode * k ) + cidx + 0 ] = baseColorR + ( 1.5 * scaleColorR * curvature );
                ( *m_fibCurvatureColors )[ ( colorMode * k ) + cidx + 1 ] = baseColorG + ( 1.5 * scaleColorG * curvature );
                ( *m_fibCurvatureColors )[ ( colorMode * k ) + cidx + 2 ] = baseColorB + ( 1.5 * scaleColorB * curvature );
                ( *m_fibCurvatureColors )[ ( colorMode * k ) + cidx + 3 ] = baseColorA + ( 1.5 * scaleColorA * curvature );

                ( *m_fibLengthColors )[ ( colorMode * k ) + cidx + 0 ] = segLen;
                ( *m_fibLengthColors )[ ( colorMode * k ) + cidx + 1 ] = 0.0;
                ( *m_fibLengthColors )[ ( colorMode * k ) + cidx + 2 ] = 0.0;
                ( *m_fibLengthColors )[ ( colorMode * k ) + cidx + 3 ] = 1.0;

                prev[0] = current[0];
                prev[1] = current[1];
                prev[2] = current[2];
                lenLast = segLen;
                lenMax = std::max( segLen, lenMax );
            }
            maxSegLengths[ fidx ] = lenMax;
        }
        progress1->finish();

        // finally, apply the global scalings needed
        debugLog() << "Iterating over all fibers for scaling per fiber.";
        for( size_t fidx = 0; fidx < fibStart->size() ; ++fidx )
        {
            ++*progress2;

            // the start vertex index
            size_t cidx = fibStart->at( fidx ) * colorMode;
            size_t len = fibLen->at( fidx );
            if( len < 3 )
            {
                continue;
            }

            // walk along the fiber
            for( size_t k = 1; k < len - 1; ++k )  // len -1 because we interpret it as segments
            {
                double relSegLen = ( *m_fibLengthColors )[ ( colorMode * k ) + cidx + 0 ] / maxSegLengths[ fidx ];
                ( *m_fibLengthColors )[ ( colorMode * k ) + cidx + 0 ] = baseColorR + ( scaleColorR * relSegLen );
                ( *m_fibLengthColors )[ ( colorMode * k ) + cidx + 1 ] = baseColorG + ( scaleColorG * relSegLen );
                ( *m_fibLengthColors )[ ( colorMode * k ) + cidx + 2 ] = baseColorB + ( scaleColorB * relSegLen );
                ( *m_fibLengthColors )[ ( colorMode * k ) + cidx + 3 ] = baseColorA + ( scaleColorA * relSegLen );
            }
        }
        progress2->finish();

        // add the new scheme
        dataSet->addColorScheme( m_fibCurvatureColors, "Curvature Coloring",
                                 "The speed of changing angles between consecutive segments represents the color." );
        dataSet->addColorScheme( m_fibLengthColors, "Segment Length Coloring",
                                 "The length of a segment in relation to the longest segment in the fiber." );

        // forward the data
        m_fiberOutput->updateData( dataSet );

        // remove the progress indicators
        m_progress->removeSubProgress( progress1 );
        m_progress->removeSubProgress( progress2 );

        debugLog() << "Done";
    }
}

