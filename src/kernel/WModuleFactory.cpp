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

#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <string>
#include <typeinfo>

#include "../common/WLogger.h"
#include "../modules/applyMask/WMApplyMask.h"
#include "../modules/arbitraryRois/WMArbitraryRois.h"
#include "../modules/boundingBox/WMBoundingBox.h"
#include "../modules/clusterParamDisplay/WMClusterParamDisplay.h"
#include "../modules/clusterSlicer/WMClusterSlicer.h"
#include "../modules/connectomeView/WMConnectomeView.h"
#include "../modules/coordinateSystem/WMCoordinateSystem.h"
#include "../modules/dataTypeConversion/WMDataTypeConversion.h"
#include "../modules/deterministicFTMori/WMDeterministicFTMori.h"
#include "../modules/directVolumeRendering/WMDirectVolumeRendering.h"
#include "../modules/distanceMap/WMDistanceMap.h"
#include "../modules/distanceMap/WMDistanceMapIsosurface.h"
#include "../modules/eegView/WMEEGView.h"
#include "../modules/fiberClustering/WMFiberClustering.h"
#include "../modules/fiberCulling/WMFiberCulling.h"
#include "../modules/fiberDisplay/WMFiberDisplay.h"
#include "../modules/fiberSelection/WMFiberSelection.h"
#include "../modules/gaussFiltering/WMGaussFiltering.h"
#include "../modules/geometryGlyphs/WMGeometryGlyphs.h"
#include "../modules/hud/WMHud.h"
#include "../modules/lic/WMLIC.h"
#include "../modules/marchingCubes/WMMarchingCubes.h"
#include "../modules/meshReader/WMMeshReader.h"
#include "../modules/navSlices/WMNavSlices.h"
#include "../modules/surfaceParticles/WMSurfaceParticles.h"
#include "../modules/template/WMTemplate.h"
#include "../modules/triangleMeshRenderer/WMTriangleMeshRenderer.h"
#include "../modules/vectorPlot/WMVectorPlot.h"
#include "../modules/voxelizer/WMVoxelizer.h"
#include "../modules/writeNIfTI/WMWriteNIfTI.h"
#include "combiner/WApplyPrototypeCombiner.h"
#include "exceptions/WPrototypeNotUnique.h"
#include "exceptions/WPrototypeUnknown.h"
#include "WModuleFactory.h"

// factory instance as singleton
boost::shared_ptr< WModuleFactory > WModuleFactory::m_instance = boost::shared_ptr< WModuleFactory >();

WModuleFactory::WModuleFactory()
{
    // initialize members
}

WModuleFactory::~WModuleFactory()
{
    // cleanup
}

void WModuleFactory::load()
{
    // load modules
    WLogger::getLogger()->addLogMessage( "Loading Modules", "ModuleFactory", LL_INFO );

    // operation must be exclusive
    boost::unique_lock< boost::shared_mutex > lock = boost::unique_lock< boost::shared_mutex >( m_prototypesLock );

    // currently the prototypes are added by hand. This will be done automatically later.
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMTemplate() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMBoundingBox() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMData() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMNavSlices() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMDeterministicFTMori() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMFiberDisplay() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMFiberCulling() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMFiberClustering() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMCoordinateSystem() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMMarchingCubes() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMDistanceMapIsosurface() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMDistanceMap() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMApplyMask() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMGaussFiltering() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMHud() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMEEGView() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMVoxelizer() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMTriangleMeshRenderer() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMDirectVolumeRendering() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMWriteNIfTI() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMDataTypeConversion() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMConnectomeView() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMClusterParamDisplay() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMFiberSelection() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMSurfaceParticles() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMClusterSlicer() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMVectorPlot() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMGeometryGlyphs() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMArbitraryRois() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMMeshReader() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMLIC() ) );

    lock.unlock();

    // for this a read lock is sufficient
    boost::shared_lock< boost::shared_mutex > slock = boost::shared_lock< boost::shared_mutex >( m_prototypesLock );

    // initialize every module in the set
    std::set< std::string > names;  // helper to find duplicates
    for( std::set< boost::shared_ptr< WModule > >::iterator listIter = m_prototypes.begin(); listIter != m_prototypes.end();
            ++listIter )
    {
        WLogger::getLogger()->addLogMessage( "Loading module: \"" + ( *listIter )->getName() + "\"", "ModuleFactory", LL_INFO );

        // that should not happen. Names should not occur multiple times since they are unique
        if ( names.count( ( *listIter )->getName() ) )
        {
            throw WPrototypeNotUnique( "Module \"" + ( *listIter )->getName() + "\" is not unique. Modules have to have a unique name." );
        }
        names.insert( ( *listIter )->getName() );

        initializeModule( ( *listIter ) );
    }

    slock.unlock();
}

boost::shared_ptr< WModule > WModuleFactory::create( boost::shared_ptr< WModule > prototype )
{
    wlog::debug( "ModuleFactory" ) << "Creating new instance of prototype \"" << prototype->getName() << "\".";

    // for this a read lock is sufficient
    boost::shared_lock< boost::shared_mutex > slock = boost::shared_lock< boost::shared_mutex >( m_prototypesLock );

    // ensure this one is a prototype and nothing else
    if ( m_prototypes.count( prototype ) == 0 )
    {
        throw WPrototypeUnknown( "Could not clone module \"" + prototype->getName() + "\" since it is no prototype." );
    }

    slock.unlock();

    // call prototypes factory function
    boost::shared_ptr< WModule > clone = boost::shared_ptr< WModule >( prototype->factory() );
    initializeModule( clone );

    return clone;
}

void WModuleFactory::initializeModule( boost::shared_ptr< WModule > module )
{
    module->initialize();
}

boost::shared_ptr< WModuleFactory > WModuleFactory::getModuleFactory()
{
    if ( !m_instance )
    {
        m_instance = boost::shared_ptr< WModuleFactory >( new WModuleFactory() );
    }

    return m_instance;
}


const boost::shared_ptr< WModule > WModuleFactory::isPrototypeAvailable( std::string name )
{
    // for this a read lock is sufficient
    boost::shared_lock< boost::shared_mutex > slock = boost::shared_lock< boost::shared_mutex >( m_prototypesLock );

    // find first and only prototype (ensured during load())
    boost::shared_ptr< WModule > ret = boost::shared_ptr< WModule >();
    for( std::set< boost::shared_ptr< WModule > >::iterator listIter = m_prototypes.begin(); listIter != m_prototypes.end();
            ++listIter )
    {
        if ( ( *listIter )->getName() == name )
        {
            ret = ( *listIter );
            break;
        }
    }

    slock.unlock();

    return ret;
}

const boost::shared_ptr< WModule > WModuleFactory::getPrototypeByName( std::string name )
{
    boost::shared_ptr< WModule > ret = isPrototypeAvailable( name );

    // if not found -> throw
    if ( ret == boost::shared_ptr< WModule >() )
    {
        throw WPrototypeUnknown( "Could not find prototype \"" + name + "\"." );
    }

    return ret;
}

const boost::shared_ptr< WModule > WModuleFactory::getPrototypeByInstance( boost::shared_ptr< WModule > instance )
{
    return getPrototypeByName( instance->getName() );
}

/**
 * Sorting function for sorting the compatibles list. It uses the alphabetical order of the names.
 *
 * \param lhs the first combiner
 * \param rhs the second combiner
 *
 * \return true if lhs < rhs
 */
bool compatiblesSort( boost::shared_ptr< WApplyPrototypeCombiner >  lhs, boost::shared_ptr< WApplyPrototypeCombiner > rhs )
{
    return ( lhs->getTargetPrototype()->getName() < rhs->getTargetPrototype()->getName() );
}

std::vector< boost::shared_ptr< WApplyPrototypeCombiner > > WModuleFactory::getCompatiblePrototypes( boost::shared_ptr< WModule > module )
{
    std::vector< boost::shared_ptr < WApplyPrototypeCombiner > > compatibles;

    // for this a read lock is sufficient
    boost::shared_lock< boost::shared_mutex > slock = boost::shared_lock< boost::shared_mutex >( m_prototypesLock );

    // get offered outputs
    std::set<boost::shared_ptr<WModuleOutputConnector> > cons = module->getOutputConnectors();

    // First, add all modules with no input connector.
    for( std::set< boost::shared_ptr< WModule > >::iterator listIter = m_prototypes.begin(); listIter != m_prototypes.end(); ++listIter )
    {
        // get connectors of this prototype
        std::set<boost::shared_ptr<WModuleInputConnector> > pcons = ( *listIter )->getInputConnectors();
        if(  pcons.size() == 0  )
        {
            compatibles.push_back( boost::shared_ptr< WApplyPrototypeCombiner >( new WApplyPrototypeCombiner( module, "", *listIter, "" ) ) );
        }
    }

    // return early if we have no output connector, because the modules with no input connector
    // are already added at this point.
    if( cons.size() == 0 )
    {
        return compatibles;
    }

    if ( cons.size() > 1 )
    {
        wlog::warn( "ModuleFactory" ) << "Can not find compatibles for " << module->getName() <<  " module (more than 1 output connector). Using "
                                      << ( *cons.begin() )->getCanonicalName()
                                      << " for compatibility check.";
    }

    // go through every prototype
    for( std::set< boost::shared_ptr< WModule > >::iterator listIter = m_prototypes.begin(); listIter != m_prototypes.end(); ++listIter )
    {
        // get connectors of this prototype
        std::set<boost::shared_ptr<WModuleInputConnector> > pcons = ( *listIter )->getInputConnectors();

        // ensure we have 1 connector
        if( pcons.size() == 0 )
        {
            continue;
        }
        if ( pcons.size() > 1 )
        {
            wlog::warn( "ModuleFactory" ) << "Can not find compatibles for " << ( *listIter )->getName()
                                          << " module (more than 1 input connector). Using "
                                          << ( *pcons.begin() )->getCanonicalName() << " for compatibility check.";
        }

        // check whether the outputs are compatible with the inputs of the prototypes
        if( ( *cons.begin() )->connectable( *pcons.begin() )  &&  ( *pcons.begin() )->connectable( *cons.begin() ) )
        {
            // it is compatible -> add to list
            compatibles.push_back( boost::shared_ptr< WApplyPrototypeCombiner >(
                new WApplyPrototypeCombiner( module, ( *cons.begin() )->getName(), *listIter, ( *pcons.begin() )->getName() ) )
            );
        }
    }

    slock.unlock();

    // sort the compatibles
    std::sort( compatibles.begin(), compatibles.end(), compatiblesSort );

    return compatibles;
}

