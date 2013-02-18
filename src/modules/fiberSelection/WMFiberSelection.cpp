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
#include <vector>
#include <string>

#include <boost/tuple/tuple.hpp>

#include <osg/ShapeDrawable>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Material>
#include <osg/StateAttribute>

#include "core/kernel/WKernel.h"
#include "core/common/WColor.h"

#include "WMFiberSelection.h"
#include "WMFiberSelection.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMFiberSelection )

WMFiberSelection::WMFiberSelection():
    WModule()
{
}

WMFiberSelection::~WMFiberSelection()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMFiberSelection::factory() const
{
    return boost::shared_ptr< WModule >( new WMFiberSelection() );
}

const char** WMFiberSelection::getXPMIcon() const
{
    return fiberSelection_xpm;
}

const std::string WMFiberSelection::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Fiber Selection";
}

const std::string WMFiberSelection::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    return "Select fibers in a given dataset using two additional datasets as volume of interest. The purpose of this module is to allow the user"
        "to select a bunch of fibers which go through two regions defined by two scalar datasets. A fiber goes through both volumes of interest"
        "if and only if at least one of the vertices of a fiber is inside a voxel of the scalar datasets with a value greater than a specified"
        "threshold. Currently, this only works good for large regions and/or densely sampled fibers.";
}

void WMFiberSelection::connectors()
{
    // The input fiber dataset
    m_fiberInput = boost::shared_ptr< WModuleInputData < WDataSetFibers > >(
        new WModuleInputData< WDataSetFibers >( shared_from_this(), "fibers", "The fiber dataset to select fibers from" )
    );

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_fiberInput );

    // The first VOI dataset
    m_voi1Input = boost::shared_ptr< WModuleInputData < WDataSetSingle > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(), "VOI1", "The first volume of interest." )
    );

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_voi1Input );

    // The second VOI dataset
    m_voi2Input = boost::shared_ptr< WModuleInputData < WDataSetSingle > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(), "VOI2", "The second volume of interest." )
    );

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_voi2Input );

    // the selected fibers go to this output
    m_fiberOutput = boost::shared_ptr< WModuleOutputData < WDataSetFibers > >(
        new WModuleOutputData< WDataSetFibers >( shared_from_this(),
                                        "out", "The fiber dataset only containing fibers which got through both volume of interest." )
    );

    // As above: make it known.
    addConnector( m_fiberOutput );

    // the selected fibers (as clusters) go to this output
    m_clusterOutput = boost::shared_ptr< WModuleOutputData < WFiberCluster > >(
        new WModuleOutputData< WFiberCluster >( shared_from_this(),
                                        "cluster", "The cluster dataset only containing fibers which got through both volume of interest." )
    );

    // As above: make it known.
    addConnector( m_clusterOutput );

    // call WModules initialization
    WModule::connectors();
}

void WMFiberSelection::properties()
{
    // used to notify about changed properties
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // the threshold for testing whether a fiber vertex is inside a volume of interest.
    m_voi1Threshold = m_properties->addProperty( "VOI1 threshold", "The threshold uses for determining whether a fiber is inside the first VOI"
                                                                    "dataset or not.", 5.0, m_propCondition );
    m_voi2Threshold = m_properties->addProperty( "VOI2 threshold", "The threshold uses for determining whether a fiber is inside the second VOI"
                                                                    "dataset or not.", 5.0, m_propCondition );
    m_cutFibers     = m_properties->addProperty( "Cut fibers",     "Cut the fibers after they gone through both VOI.", true, m_propCondition );

    m_preferShortestPath = m_properties->addProperty( "Prefer shortest path", "Determines whether the fibers should be cut on the entry and "
                            "exit of a VOI. This should prevent the fibers from going deep into the VOI's.", false, m_propCondition );

    WModule::properties();
}

/**
 * Namespace for the FibTrace structure.
 */
namespace
{
    /**
     * This structure stores some tracing information along the fiber. It is defined in this file scope as local types can't be used as template
     * parameters. (but it is needed in a list)
     */
    typedef struct
    {
        size_t idx;         //!< current index
        bool which;         //!< true if isInside is true for VOI1, for VOI2 it is false
        // size_t lastIdx;     //!< the last index , idx-1 <- not needed
        bool isInside;      //!< true if idx is inside a VOI
        bool wasInside;     //!< true if idx-1 was inside a VOI
    }
    FibTrace;
}

void WMFiberSelection::moduleMain()
{
    //
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );
    m_moduleState.add( m_voi1Input->getDataChangedCondition() );
    m_moduleState.add( m_voi2Input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    // Signal ready state. Now your module can be connected by the container, which owns the module.
    ready();

    // Now wait for data
    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }

        boost::shared_ptr< WDataSetFibers > newFibers = m_fiberInput->getData();
        boost::shared_ptr< WDataSetSingle > newVoi1 = m_voi1Input->getData();
        boost::shared_ptr< WDataSetSingle > newVoi2 = m_voi2Input->getData();

        bool dataChanged = ( m_fibers != newFibers ) || ( m_voi1 != newVoi1 ) ||( m_voi2 != newVoi2 );
        bool dataValid =   ( newFibers && newVoi1 && newVoi2 );
        bool propChanged = ( m_cutFibers->changed() || m_voi1Threshold->changed() || m_voi2Threshold->changed() || m_preferShortestPath->changed() );

        // cleanup if no valid data is available
        if( !dataValid )
        {
            debugLog() << "Resetting output.";

            // remove my refs to the data
            m_fibers.reset();
            m_voi1.reset();
            m_voi2.reset();

            // reset outputs too
            m_fiberOutput->reset();
            m_clusterOutput->reset();
        }

        if( ( propChanged || dataChanged ) && dataValid )
        {
            debugLog() << "Data received. Recalculating.";

            // replace old data
            m_fibers = newFibers;
            m_voi1 = newVoi1;
            m_voi2 = newVoi2;

            // get the needed properties
            double voi1Threshold = m_voi1Threshold->get( true );
            double voi2Threshold = m_voi2Threshold->get( true );
            bool   cutFibers     = m_cutFibers->get( true );
            bool   preferShortestPath = m_preferShortestPath->get( true );

            // get the fiber definitions
            boost::shared_ptr< std::vector< size_t > > fibStart = m_fibers->getLineStartIndexes();
            boost::shared_ptr< std::vector< size_t > > fibLen   = m_fibers->getLineLengths();
            boost::shared_ptr< std::vector< float > >  fibVerts = m_fibers->getVertices();

            // currently, both grids need to be the same
            // the grid of voi1 and voi2 is needed here
            boost::shared_ptr< WGridRegular3D > grid1 = boost::dynamic_pointer_cast< WGridRegular3D >( m_voi1->getGrid() );
            boost::shared_ptr< WGridRegular3D > grid2 = boost::dynamic_pointer_cast< WGridRegular3D >( m_voi2->getGrid() );

            // the list of fibers
            std::vector< boost::tuple< size_t, size_t, size_t > > matches;  // a match contains the fiber ID, the start vertex ID and the stop ID

            // progress indication
            boost::shared_ptr< WProgress > progress1 = boost::shared_ptr< WProgress >( new WProgress( "Checking fibers against ",
                                                                                                      fibStart->size() ) );
            m_progress->addSubProgress( progress1 );

            // there are several scenarios possible, how the VOIs can be. They can intersect each other, one being inside the other or they might
            // be spatial distinct. To handle all those scenarios, the fiber segments get interpreted as some kind of ray and a list of all hit
            // points with one of the VOIs is stored in a list and can be handled afterwards in several ways.

            // for each fiber:
            debugLog() << "Iterating over all fibers.";
            for( size_t fidx = 0; fidx < fibStart->size() ; ++fidx )
            {
                ++*progress1;

                // the start vertex index
                size_t sidx = fibStart->at( fidx ) * 3;

                // the length of the fiber
                size_t len = fibLen->at( fidx );

                // trace information for both VOI
                FibTrace current1 = { 0, true, false, false };  // NOLINT
                FibTrace current2 = { 0, false, false, false }; // NOLINT
                std::vector< FibTrace > hits1;
                std::vector< FibTrace > hits2;

                // walk along the fiber
                for( size_t k = 0; k < len; ++k )
                {
                    float x = fibVerts->at( ( 3 * k ) + sidx );
                    float y = fibVerts->at( ( 3 * k ) + sidx + 1 );
                    float z = fibVerts->at( ( 3 * k ) + sidx + 2 );

                    // get the voxel id
                    int voxel1 = grid1->getVoxelNum( WPosition( x, y, z ) );
                    int voxel2 = grid2->getVoxelNum( WPosition( x, y, z ) );
                    if( ( voxel1 < 0 ) || ( voxel2 < 0 ) )
                    {
                        warnLog() << "Fiber vertex (" << x << "," << y << "," << z << ") not in VOI1 or VOI2 grid. Ignoring vertex.";
                        continue;
                    }

                    // get the value
                    double value1 = m_voi1->getValueAt( voxel1 );
                    double value2 = m_voi2->getValueAt( voxel2 );

                    // update trace structs for both VOI
                    current1.idx = k;
                    current1.wasInside = current1.isInside;
                    current1.isInside = ( value1 >= voi1Threshold );
                    current2.idx = k;
                    current2.wasInside = current2.isInside;
                    current2.isInside = ( value2 >= voi2Threshold );

                    // VOI1 hit?
                    if( current1.wasInside ^ current1.isInside )
                    {
                        hits1.push_back( current1 );
                    }
                    // VOI2 Hit?
                    if( current2.wasInside ^ current2.isInside )
                    {
                        hits2.push_back( current2 );
                    }
                }

                // Now, a list of all intersections of the current fiber "fidx" with the VOI's is available. There are three possibilities. One
                // VOI is inside the other, the VOI's intersect each other or the VOI's are spatially distinct.

                // If one VOI completely inside another, ignore the fiber since it does not really make sense. So we only need to handle the
                // other two cases

                if( !preferShortestPath )
                {
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    // Strategy 1: Always use the longest path
                    //  - this is a good strategy for both remaining cases
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                    // always use the longest fiber path
                    if( ( hits1.size() >= 1 ) && ( hits2.size() >= 1 ) )
                    {
                        debugLog() << "Fiber " << fidx << " inside VOI1 and VOI2.";
                        matches.push_back( boost::make_tuple( fidx, std::min( hits1[ 0 ].idx,                hits2[ 0 ].idx ),
                                                                    std::max( hits1[ hits1.size() - 1 ].idx, hits2[ hits2.size() - 1 ].idx ) ) );
                    }
                }
                else
                {
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    // Strategy 2: Find the shortest path between the exit-hit of VOI1 and entry-hit of VOI2
                    //  - this should create fibers if very similar length
                    //  - might be problematic with overlapping VOI's
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                    // always use the longest fiber path
                    if( ( hits1.size() >= 1 ) && ( hits2.size() >= 1 ) )
                    {
                        debugLog() << "Fiber " << fidx << " inside VOI1 and VOI2.";
                        matches.push_back( boost::make_tuple( fidx, std::min( hits1[ 0 ].idx,                hits2[ hits2.size() - 1 ].idx ),
                                                                    std::max( hits1[ hits1.size() - 1 ].idx, hits2[ 0 ].idx ) ) );
                    }
                }
            }
            debugLog() << "Iterating over all fibers: done!";
            progress1->finish();

            // give some feedback
            infoLog() << "Found " << matches.size() << " fibers going through VOI1 and VOI2.";

            // combine it to a new fiber dataset
            // the required arrays:
            boost::shared_ptr< std::vector< float > >  newFibVerts = boost::shared_ptr< std::vector< float > >( new std::vector< float >() );
            boost::shared_ptr< std::vector< size_t > > newFibStart = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t >() );
            boost::shared_ptr< std::vector< size_t > > newFibLen = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t >() );
            boost::shared_ptr< std::vector< size_t > > newFibVertsRev = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t >() );

            progress1 = boost::shared_ptr< WProgress >( new WProgress( "Creating Output Dataset.", matches.size() ) );
            m_progress->addSubProgress( progress1 );
            debugLog() << "Creating new Fiber Dataset and Cluster.";

            // the cluster which gets iteratively build
            boost::shared_ptr< WFiberCluster > newFiberCluster = boost::shared_ptr< WFiberCluster >( new WFiberCluster() );

            // add each match to the above arrays
            size_t curVertIdx = 0;  // the current index in the vertex array
            size_t curRealFibIdx = 0; // since some fibers get discarded, the for loop counter "i" is not the correct fiber index
            for( size_t i = 0; i < matches.size(); ++i )
            {
                ++*progress1;

                // start index and length of original fiber
                size_t sidx = fibStart->at( matches[ i ].get< 0 >() ) * 3;
                size_t len = fibLen->at( matches[ i ].get< 0 >() );

                if( cutFibers )
                {
                    // if the fibers should be cut: add an offset to the start index and recalculate the length
                    sidx = sidx + ( matches[ i ].get< 1 >() * 3 );
                    len =  matches[ i ].get< 2 >() - matches[ i ].get< 1 >();
                }

                // discard fibers with less than one line segment
                if( len < 2 )
                {
                    continue;
                }

                // set the new length
                newFibLen->push_back( len );

                // set new start index
                newFibStart->push_back( curVertIdx );

                // copy the index to the cluster
                WFiberCluster a( curRealFibIdx );
                newFiberCluster->merge( a );

                // copy the fiber vertices
                for( size_t vi = 0; vi < len; ++vi )
                {
                    curVertIdx++;

                    newFibVerts->push_back( fibVerts->at( sidx + ( 3 * vi ) ) );
                    newFibVerts->push_back( fibVerts->at( sidx + ( 3 * vi ) + 1 ) );
                    newFibVerts->push_back( fibVerts->at( sidx + ( 3 * vi ) + 2 ) );
                    newFibVertsRev->push_back( curRealFibIdx );
                    newFibVertsRev->push_back( curRealFibIdx );
                    newFibVertsRev->push_back( curRealFibIdx );
                }

                curRealFibIdx++;
            }
            progress1->finish();

            // create the new dataset
            boost::shared_ptr< WDataSetFibers > newFibers = boost::shared_ptr< WDataSetFibers >(
                    new WDataSetFibers( newFibVerts, newFibStart, newFibLen, newFibVertsRev )
            );

            // create a WDataSetFiberVector which is needed for the WFiberCluster
            progress1 = boost::shared_ptr< WProgress >( new WProgress( "Creating Output Vector Dataset." ) );
            m_progress->addSubProgress( progress1 );
            debugLog() << "Creating Fiber Vector Dataset.";

            boost::shared_ptr< WDataSetFiberVector > newFiberVector = boost::shared_ptr< WDataSetFiberVector >(
                    new WDataSetFiberVector( newFibers )
            );
            newFiberCluster->setDataSetReference( newFiberVector );
            newFiberCluster->generateCenterLine();
            progress1->finish();

            // finally -> update the output
            m_fiberOutput->updateData( newFibers );
            m_clusterOutput->updateData( newFiberCluster );
        }
    }
}

void WMFiberSelection::activate()
{
    // handle activation

    // Always call WModule's activate!
    WModule::activate();
}

