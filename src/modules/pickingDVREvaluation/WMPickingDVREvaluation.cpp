//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2017 OpenWalnut Community
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

#include "core/dataHandler/WDataSetScalar.h"
#include "core/graphicsEngine/WGERequirement.h"
#include "core/kernel/WKernel.h"
#include "core/ui/WUIRequirement.h"

#include "WMPickingDVREvaluation.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMPickingDVREvaluation )

WMPickingDVREvaluation::WMPickingDVREvaluation():
    WModule()
{
}

WMPickingDVREvaluation::~WMPickingDVREvaluation()
{
}

boost::shared_ptr< WModule > WMPickingDVREvaluation::factory() const
{
    return boost::shared_ptr< WModule >( new WMPickingDVREvaluation() );
}

const std::string WMPickingDVREvaluation::getName() const
{
    return "Picking in DVR Evaluation";
}

const std::string WMPickingDVREvaluation::getDescription() const
{
    return "Evaluate different picking techniques with regard to certain metrics.";
}

void WMPickingDVREvaluation::connectors()
{
    // The transfer function for our DVR
    m_transferFunction = WModuleInputData< WDataSetSingle >::createAndAdd( shared_from_this(), "transfer function", "The 1D transfer function." );

    // Scalar field
    m_scalarData = WModuleInputData< WDataSetScalar >::createAndAdd( shared_from_this(), "scalar data", "Scalar data." );

    WModule::connectors();
}

void WMPickingDVREvaluation::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    m_viewDirection =  m_properties->addProperty( "Viewing Direction",
                                                  "Viewing and thus projection direction for DVR.",
                                                  WVector3d( 0.0, 0.0, -1.0 ),
                                                  m_propCondition );
    m_sampleSteps = m_properties->addProperty( "Samples - steps",
                      "Number of samples. Choose this appropriately for the settings used for the DVR itself.",
                       256,
                       m_propCondition );

    WModule::properties();
}

void WMPickingDVREvaluation::requirements()
{
    m_requirements.push_back( new WGERequirement() );
    m_requirements.push_back( new WUIRequirement() );
}


/** \brief Intersects a ray with a plane and calculates the intersection point
 * \param planePoint - point on plane
 * \param v1 - first  vector of plane
 * \param v2 - second vector of plane
 * \param rayOrigin - point on line
 * \param rayDir  - vector on line (i.e. slope)
 **/
inline bool intersectPlaneWithRay( WPosition* cutpoint,
                                   WPosition const& planePoint,
                                   WVector3d const& v1,
                                   WVector3d const& v2,
                                   WVector3d const& rayDir,
                                   WPosition const& rayOrigin )
{
    WVector3d n = cross( v1, v2 );
    n = normalize( n );              // normal vector of the plane

    double const d = dot( n, planePoint );

    static const double MY_EPSILON = 1e-9;
    if( std::abs( dot( n, rayDir ) ) < MY_EPSILON)         // plane and line are parallel
    {
        *cutpoint = planePoint;   // otherwise it would be undefined
        return false;
    }

    double const t = ( d - dot( n, rayOrigin ) ) / ( dot( n, rayDir ) );

    *cutpoint = rayOrigin + t * rayDir;

    return true;
}

WPosition intersectBoundingBoxWithRay( const WBoundingBox& bbox, const WPosition& origin, const WVector3d& dir )
{
    WPosition result;
    // FIXME: check order of plane vectors for outward normals

    WPosition intersectionPoint;
    bool hit;

    // min X plane
    hit = intersectPlaneWithRay( &intersectionPoint,
                                 bbox.getMin(),
                                 WVector3d( 0.0, 1.0, 0.0 ),
                                 WVector3d( 0.0, 0.0, 1.0 ),
                                 dir,
                                 origin );
    if( hit
        && bbox.contains( intersectionPoint )
        && dot( dir, intersectionPoint - origin ) > 0.0 )
    {
        std::cout << "HIT minX: " << hit << "   " << intersectionPoint << std::endl;
        return result = intersectionPoint;
    }

    // min Y plane
    hit = intersectPlaneWithRay( &intersectionPoint,
                                 bbox.getMin(),
                                 WVector3d( 1.0, 0.0, 0.0 ),
                                 WVector3d( 0.0, 0.0, 1.0 ),
                                 dir,
                                 origin );
    if( hit
        && bbox.contains( intersectionPoint )
        && dot( dir, intersectionPoint - origin ) > 0.0 )
    {
        std::cout << "HIT minY: " << hit << "   " << intersectionPoint << std::endl;
        return result = intersectionPoint;
    }

    // min Z plane
    hit = intersectPlaneWithRay( &intersectionPoint,
                                 bbox.getMin(),
                                 WVector3d( 0.0, 1.0, 0.0 ),
                                 WVector3d( 1.0, 0.0, 0.0 ),
                                 dir,
                                 origin );
    if( hit
        && bbox.contains( intersectionPoint )
        && dot( dir, intersectionPoint - origin ) > 0.0 )
    {
        std::cout << "HIT minZ: " << hit << "   " << intersectionPoint << std::endl;
        return result = intersectionPoint;
    }

    // max X plane
    hit = intersectPlaneWithRay( &intersectionPoint,
                                 bbox.getMax(),
                                 WVector3d( 0.0, 1.0, 0.0 ),
                                 WVector3d( 0.0, 0.0, 1.0 ),
                                 dir,
                                 origin );
    if( hit
        && bbox.contains( intersectionPoint )
        && dot( dir, intersectionPoint - origin ) > 0.0 )
    {
        std::cout << "HIT minX: " << hit << "   " << intersectionPoint << std::endl;
        return result = intersectionPoint;
    }

    // max Y plane
    hit = intersectPlaneWithRay( &intersectionPoint,
                                 bbox.getMax(),
                                 WVector3d( 1.0, 0.0, 0.0 ),
                                 WVector3d( 0.0, 0.0, 1.0 ),
                                 dir,
                                 origin );
    if( hit
        && bbox.contains( intersectionPoint )
        && dot( dir, intersectionPoint - origin ) > 0.0 )
    {
        std::cout << "HIT minY: " << hit << "   " << intersectionPoint << std::endl;
        return result = intersectionPoint;
    }

    // max Z plane
    hit = intersectPlaneWithRay( &intersectionPoint,
                                 bbox.getMax(),
                                 WVector3d( 0.0, 1.0, 0.0 ),
                                 WVector3d( 1.0, 0.0, 0.0 ),
                                 dir,
                                 origin );
    if( hit
        && bbox.contains( intersectionPoint )
        && dot( dir, intersectionPoint - origin ) > 0.0 )
    {
        std::cout << "HIT minZ: " << hit << "   " << intersectionPoint << std::endl;
        return result = intersectionPoint;
    }

    assert( 0 && "This should not happen. Tell \"wiebel\" if it does." );
    return result;
}

void WMPickingDVREvaluation::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_scalarData->getDataChangedCondition() );
    m_moduleState.add( m_transferFunction->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    ready();

    // main loop
    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        WDataSet::SPtr dataSet = m_scalarData->getData();
        // is this a DS with a regular grid?
        WDataSetSingle::SPtr dsSingle = boost::dynamic_pointer_cast< WDataSetSingle >( dataSet );
        if( dsSingle )
        {
            WGridRegular3D::SPtr regGrid;
            regGrid = boost::dynamic_pointer_cast< WGridRegular3D >( dsSingle->getGrid() );
            WBoundingBox bbox = regGrid->getBoundingBox();
            debugLog() << "BBox min " << bbox.getMin()[0] << " " << bbox.getMin()[1] << " " << bbox.getMin()[2];
            debugLog() << "BBox max " << bbox.getMax()[0] << " " << bbox.getMax()[1] << " " << bbox.getMax()[2];

            WPosition origin( 0.2, 0.3, 0.4 );
            intersectBoundingBoxWithRay( bbox, origin, m_viewDirection->get( true ) );
        }
    }
}
