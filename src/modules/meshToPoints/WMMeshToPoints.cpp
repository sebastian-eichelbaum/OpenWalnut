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

#include <algorithm>
#include <string>

#include "core/kernel/WKernel.h"
#include "core/common/WPropertyHelper.h"
#include "core/common/math/WMath.h"
#include "core/dataHandler/WDataHandler.h"

#include "WMMeshToPoints.h"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMMeshToPoints )

WMMeshToPoints::WMMeshToPoints():
    WModule()
{
}

WMMeshToPoints::~WMMeshToPoints()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMMeshToPoints::factory() const
{
    return boost::shared_ptr< WModule >( new WMMeshToPoints() );
}

const char** WMMeshToPoints::getXPMIcon() const
{
    return NULL;
}

const std::string WMMeshToPoints::getName() const
{
    return "Mesh To Points";
}

const std::string WMMeshToPoints::getDescription() const
{
    return "This module converts mesh data to points by using the fiber vertex points and colors..";
}

void WMMeshToPoints::connectors()
{
    // The input fiber dataset
    m_meshInput = boost::shared_ptr< WModuleInputData < WTriangleMesh > >(
        new WModuleInputData< WTriangleMesh >( shared_from_this(), "mesh", "The mesh dataset" )
    );

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_meshInput );

    // the points
    m_pointsOutput = boost::shared_ptr< WModuleOutputData < WDataSetPoints > >(
        new WModuleOutputData< WDataSetPoints >( shared_from_this(), "out", "The point data." )
    );

    // As above: make it known.
    addConnector( m_pointsOutput );

    // call WModule's initialization
    WModule::connectors();
}

void WMMeshToPoints::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_doRefinement = m_properties->addProperty( "Resample", "If enabled, the mesh is sampled more dense.", false, m_propCondition );
    m_minDistance = m_properties->addProperty( "Min Distance",
                                               "Minimal distance between two samples. If reached, iteration will stop.", 1.0,
                                               m_propCondition );
    m_maxIterations = m_properties->addProperty( "Max Iterations", "Maximum number of iterations.", 2, m_propCondition );

    // call WModule's initialization
    WModule::properties();
}

void refine( const osg::Vec3& v1, const osg::Vec3& v2, const osg::Vec3& v3, WDataSetPoints::VertexArray points,
             size_t c, const size_t& maxCount, const double& minDistance )
{
    // add first verticies
    if( c == 0 )
    {
        points->push_back( v1.x() );
        points->push_back( v1.y() );
        points->push_back( v1.z() );
        points->push_back( v2.x() );
        points->push_back( v2.y() );
        points->push_back( v2.z() );
        points->push_back( v3.x() );
        points->push_back( v3.y() );
        points->push_back( v3.z() );
    }

    // abort
    if( c == maxCount )
    {
        return;
    }

    // get vertices in the middle of each edge
    osg::Vec3 v12 = v1 + ( ( v2 - v1 ) * 0.5 );
    osg::Vec3 v23 = v2 + ( ( v3 - v2 ) * 0.5 );
    osg::Vec3 v31 = v3 + ( ( v1 - v3 ) * 0.5 );

    bool l12 = ( ( v2 - v1 ).length() < minDistance );
    bool l23 = ( ( v3 - v2 ).length() < minDistance );
    bool l31 = ( ( v1 - v3 ).length() < minDistance );
    if( l12 + l23 + l31 >= 2 )
    {
        return;
    }

    points->push_back( v12.x() );
    points->push_back( v12.y() );
    points->push_back( v12.z() );
    points->push_back( v23.x() );
    points->push_back( v23.y() );
    points->push_back( v23.z() );
    points->push_back( v31.x() );
    points->push_back( v31.y() );
    points->push_back( v31.z() );

    // build new triangle and refine this
    refine( v1, v12, v31, points, c + 1, maxCount, minDistance );
    refine( v2, v23, v12, points, c + 1, maxCount, minDistance );
    refine( v3, v31, v23, points, c + 1, maxCount, minDistance );
    refine( v12, v23, v31, points, c + 1, maxCount, minDistance );
}

void WMMeshToPoints::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_meshInput->getDataChangedCondition() );
    // Remember the condition provided to some properties in properties()? The condition can now be used with this condition set.
    m_moduleState.add( m_propCondition );

    ready();

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
        bool dataUpdated = m_meshInput->handledUpdate();
        boost::shared_ptr< WTriangleMesh > dataSet = m_meshInput->getData();
        bool dataValid = ( dataSet );
        bool propsChanged = m_doRefinement->changed() || m_maxIterations->changed() || m_minDistance->changed();

        // reset everything if input was disconnected/invalid
        if( !dataValid )
        {
            debugLog() << "Resetting output.";
            m_pointsOutput->reset();
            continue;
        }

        if( dataValid && !dataUpdated && !propsChanged )
        {
            continue;
        }

        // progress indication
        WProgress::SPtr progress = WProgress::SPtr( new WProgress( "Creating Points from mesh." ) );
        m_progress->addSubProgress( progress );

        osg::ref_ptr< osg::Vec3Array >  verts = dataSet->getVertexArray();
        osg::ref_ptr< osg::Vec4Array >  colors = dataSet->getVertexColorArray();
        std::vector< size_t > faces = dataSet->getTriangles();

        debugLog() << "Creating point data. Num Points = " << verts->size() / 3 << ".";

        // convert to std array
        WDataSetPoints::VertexArray vecConv( new WDataSetPoints::VertexArray::element_type() );
        WDataSetPoints::ColorArray colConv( new WDataSetPoints::ColorArray::element_type() );

        WAssert( verts->size() == colors->size(), "Not enough colors for all vertices." );
        for( size_t i = 0; i < faces.size(); i += 3 )
        {
            osg::Vec3 v1 = verts->operator[]( faces[ i + 0 ] );
            osg::Vec3 v2 = verts->operator[]( faces[ i + 1 ] );
            osg::Vec3 v3 = verts->operator[]( faces[ i + 2 ] );

            refine( v1, v2, v3, vecConv, 0, m_doRefinement->get() ? m_maxIterations->get() : 0, m_minDistance->get() );
        }

        // TriangleMesh is implemented in a very ugly way, causing the color array to be (0,0,0,0) for each color -> invisible points.
        WDataSetPoints::SPtr result( new WDataSetPoints( vecConv, WDataSetPoints::ColorArray() ) );
        m_pointsOutput->updateData( result );

        progress->finish();
        m_progress->removeSubProgress( progress );
    }
}

