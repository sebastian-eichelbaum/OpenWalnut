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
#include <iostream>
#include <set>
#include <string>
#include <typeinfo>
#include <vector>

#include "WModule.h"
#include "WModuleCombiner.h"
#include "../common/WLogger.h"
#include "../modules/applyMask/WMApplyMask.h"
#include "../modules/arbitraryRois/WMArbitraryRois.h"
#include "../modules/boundingBox/WMBoundingBox.h"
#include "../modules/clusterParamDisplay/WMClusterParamDisplay.h"
#include "../modules/clusterSlicer/WMClusterSlicer.h"
#include "../modules/coordinateSystem/WMCoordinateSystem.h"
#include "../modules/dataTypeConversion/WMDataTypeConversion.h"
#include "../modules/deterministicFTMori/WMDeterministicFTMori.h"
#include "../modules/isosurfaceRaytracer/WMIsosurfaceRaytracer.h"
#include "../modules/distanceMap/WMDistanceMap.h"
#include "../modules/distanceMap/WMDistanceMapIsosurface.h"
#include "../modules/eegView/WMEEGView.h"
#include "../modules/detTractClustering/WMDetTractClustering.h"
#include "../modules/detTractCulling/WMDetTractCulling.h"
#include "../modules/fiberDisplay/WMFiberDisplay.h"
#include "../modules/fiberSelection/WMFiberSelection.h"
#include "../modules/fiberTransform/WMFiberTransform.h"
#include "../modules/gaussFiltering/WMGaussFiltering.h"
#include "../modules/imageExtractor/WMImageExtractor.h"
#include "../modules/hud/WMHud.h"
#include "../modules/lic/WMLIC.h"
#include "../modules/marchingCubes/WMMarchingCubes.h"
#include "../modules/meshReader/WMMeshReader.h"
#include "../modules/navSlices/WMNavSlices.h"
#include "../modules/probTractDisplay/WMProbTractDisplay.h"
#include "../modules/scalarSegmentation/WMScalarSegmentation.h"
#include "../modules/superquadricGlyphs/WMSuperquadricGlyphs.h"
#include "../modules/template/WMTemplate.h"
#include "../modules/triangleMeshRenderer/WMTriangleMeshRenderer.h"
#include "../modules/vectorPlot/WMVectorPlot.h"
#include "../modules/voxelizer/WMVoxelizer.h"
#include "../modules/writeNIfTI/WMWriteNIfTI.h"
#include "../modules/writeTracts/WMWriteTracts.h"
#include "../modules/splineSurface/WMSplineSurface.h"
#include "../modules/atlasSurfaces/WMAtlasSurfaces.h"
#include "combiner/WApplyCombiner.h"
#include "exceptions/WPrototypeNotUnique.h"
#include "exceptions/WPrototypeUnknown.h"
#include "WModuleFactory.h"

// factory instance as singleton
boost::shared_ptr< WModuleFactory > WModuleFactory::m_instance = boost::shared_ptr< WModuleFactory >();

WModuleFactory::WModuleFactory():
    m_prototypes()
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
    PrototypeSharedContainerType::WriteTicket m_prototypeAccess = m_prototypes.getWriteTicket();

    // currently the prototypes are added by hand. This will be done automatically later.
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMApplyMask() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMArbitraryRois() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMBoundingBox() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMClusterParamDisplay() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMClusterSlicer() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMCoordinateSystem() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMData() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMDataTypeConversion() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMDeterministicFTMori() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMIsosurfaceRaytracer() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMDistanceMap() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMDistanceMapIsosurface() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMEEGView() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMDetTractClustering() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMDetTractCulling() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMFiberDisplay() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMFiberSelection() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMFiberTransform() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMGaussFiltering() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMHud() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMImageExtractor() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMLIC() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMMarchingCubes() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMMeshReader() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMNavSlices() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMProbTractDisplay() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMScalarSegmentation() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMSuperquadricGlyphs() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMTemplate() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMTriangleMeshRenderer() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMVectorPlot() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMVoxelizer() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMWriteNIfTI() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMWriteTracts() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMSplineSurface() ) );
    m_prototypeAccess->get().insert( boost::shared_ptr< WModule >( new WMAtlasSurfaces() ) );

    // unlock as read lock is sufficient for the further steps
    m_prototypeAccess.reset();

    // for this a read lock is sufficient, gets unlocked if it looses scope
    PrototypeSharedContainerType::ReadTicket l = m_prototypes.getReadTicket();

    // initialize every module in the set
    std::set< std::string > names;  // helper to find duplicates
    for( PrototypeContainerConstIteratorType listIter = l->get().begin(); listIter != l->get().end();
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
}

bool WModuleFactory::isPrototype( boost::shared_ptr< WModule > module )
{
    // for this a read lock is sufficient, gets unlocked if it looses scope
    PrototypeSharedContainerType::ReadTicket l = getModuleFactory()->m_prototypes.getReadTicket();
    return getModuleFactory()->checkPrototype( module, l );
}

bool WModuleFactory::checkPrototype( boost::shared_ptr< WModule > module, PrototypeSharedContainerType::ReadTicket ticket )
{
    return ( ticket->get().count( module ) != 0 );
}

boost::shared_ptr< WModule > WModuleFactory::create( boost::shared_ptr< WModule > prototype )
{
    wlog::debug( "ModuleFactory" ) << "Creating new instance of prototype \"" << prototype->getName() << "\".";

    // for this a read lock is sufficient, gets unlocked if it looses scope
    PrototypeSharedContainerType::ReadTicket l = m_prototypes.getReadTicket();

    // ensure this one is a prototype and nothing else
    if ( !checkPrototype( prototype, l ) )
    {
        throw WPrototypeUnknown( "Could not clone module \"" + prototype->getName() + "\" since it is no prototype." );
    }

    // explicitly unlock
    l.reset();

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
    // for this a read lock is sufficient, gets unlocked if it looses scope
    PrototypeSharedContainerType::ReadTicket l = m_prototypes.getReadTicket();

    // find first and only prototype (ensured during load())
    boost::shared_ptr< WModule > ret = boost::shared_ptr< WModule >();
    for( std::set< boost::shared_ptr< WModule > >::const_iterator listIter = l->get().begin(); listIter != l->get().end();
            ++listIter )
    {
        if ( ( *listIter )->getName() == name )
        {
            ret = ( *listIter );
            break;
        }
    }

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

WModuleFactory::PrototypeSharedContainerType::ReadTicket WModuleFactory::getPrototypes() const
{
    return m_prototypes.getReadTicket();
}

WCombinerTypes::WCompatiblesList WModuleFactory::getCompatiblePrototypes( boost::shared_ptr< WModule > module )
{
    WCombinerTypes::WCompatiblesList compatibles;

    // for this a read lock is sufficient, gets unlocked if it looses scope
    PrototypeSharedContainerType::ReadTicket l = m_prototypes.getReadTicket();

    // First, add all modules with no input connector.
    for( PrototypeContainerConstIteratorType listIter = l->get().begin(); listIter != l->get().end();
            ++listIter )
    {
        // get connectors of this prototype
        WModule::InputConnectorList pcons = ( *listIter )->getInputConnectors();
        if(  pcons.size() == 0  )
        {
            // the modules which match every time need their own groups
            WCombinerTypes::WOneToOneCombiners lComp;

            // NOTE: it is OK here to use the variable module even if it is NULL as the combiner in this case only adds the specified module
            lComp.push_back( boost::shared_ptr< WApplyCombiner >( new WApplyCombiner( module, "", *listIter, "" ) ) );

            // add this list
            compatibles.push_back( WCombinerTypes::WCompatiblesGroup( ( *listIter ), lComp ) );
        }
    }

    // if NULL was specified, only return all modules without any inputs
    if ( !module )
    {
        return compatibles;
    }

    // go through every prototype
    for( PrototypeContainerConstIteratorType listIter = l->get().begin(); listIter != l->get().end();
            ++listIter )
    {
        WCombinerTypes::WOneToOneCombiners lComp = WApplyCombiner::createCombinerList< WApplyCombiner >( module, ( *listIter ) );

        // add the group
        if ( lComp.size() != 0 )
        {
            compatibles.push_back( WCombinerTypes::WCompatiblesGroup( ( *listIter ), lComp ) );
        }
    }

    // unlock. No locking needed for further steps.
    l.reset();

    // sort the compatibles
    std::sort( compatibles.begin(), compatibles.end(), WCombinerTypes::compatiblesSort );

    return compatibles;
}
