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
#include <vector>

#include "core/dataHandler/WDataSetScalar.h"
#include "core/graphicsEngine/WGERequirement.h"
#include "core/kernel/WKernel.h"
#include "core/ui/WUIRequirement.h"

#include "WMPickingDVREvaluation.h"

//Module Defines
#define WMPICKINGDVR_MAX_INTENS        "Maximum intensity"
#define WMPICKINGDVR_FIRST_HIT         "First hit        "
#define WMPICKINGDVR_THRESHOLD         "Opacity threshold        "
#define WMPICKINGDVR_MOST_CONTRIBUTING "Most contributing"
#define WMPICKINGDVR_HIGHEST_OPACITY   "Highest opacity  "
#define WMPICKINGDVR_WYSIWYP           "WYSIWYP          "

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMPickingDVREvaluation )

WMPickingDVREvaluation::WMPickingDVREvaluation():
    WModule()
{
    m_bbox = WBoundingBox();
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
    m_viewDirection =  m_properties->addProperty( "Viewing direction",
                                                  "Viewing and thus projection direction for DVR. "
                                                  "If [0,0,0], a multi-directional sampling will be performed.",
                                                  WVector3d( 0.0, 0.0, -1.0 ),
                                                  m_propCondition );
    m_sampleSteps = m_properties->addProperty( "Samples - steps",
                      "Number of samples. Choose this appropriately for the settings used for the DVR itself.",
                       256,
                       m_propCondition );
    m_samplesEval = m_properties->addProperty( "Samples - evaluation",
                      "Number of samples. For evaluating Delta_vi.",
                       101,
                       m_propCondition );

    {
        m_pickingCriteriaList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
        m_pickingCriteriaList->addItem( WMPICKINGDVR_FIRST_HIT, WMPICKINGDVR_FIRST_HIT );
        //m_pickingCriteriaList->addItem( WMPICKINGDVR_THRESHOLD, WMPICKINGDVR_THRESHOLD );
        m_pickingCriteriaList->addItem( WMPICKINGDVR_MOST_CONTRIBUTING, WMPICKINGDVR_MOST_CONTRIBUTING );
        m_pickingCriteriaList->addItem( WMPICKINGDVR_HIGHEST_OPACITY, WMPICKINGDVR_HIGHEST_OPACITY );
        //m_pickingCriteriaList->addItem( WMPICKINGDVR_WYSIWYP, WMPICKINGDVR_WYSIWYP );
        m_pickingCriteriaList->addItem( WMPICKINGDVR_MAX_INTENS, WMPICKINGDVR_MAX_INTENS );

        m_pickingCriteriaCur = m_properties->addProperty( "Picking method",
                                                          "Select a picking method",
                                                          m_pickingCriteriaList->getSelectorFirst(),
                                                          m_propCondition );

        WPropertyHelper::PC_SELECTONLYONE::addTo( m_pickingCriteriaCur );
        WPropertyHelper::PC_NOTEMPTY::addTo( m_pickingCriteriaCur );
    }

    {
        m_impFuncList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
        m_impFuncList->addItem( "Uniform (=1)" );
        m_impFuncList->addItem( "Opacity ([0,1])" );
        m_impFuncList->addItem( "Intensity" );
        m_importanceFunctionCur = m_properties->addProperty( "Importance function",
                                                             "Importance function.",
                                                             m_impFuncList->getSelectorFirst(),
                                                             m_propCondition );

        WPropertyHelper::PC_SELECTONLYONE::addTo( m_importanceFunctionCur );
        WPropertyHelper::PC_NOTEMPTY::addTo( m_importanceFunctionCur );
    }

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

WPosition WMPickingDVREvaluation::intersectBoundingBoxWithRay( const WBoundingBox& bbox, const WPosition& origin, const WVector3d& dir )
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
        // debugLog() << "HIT minX: " << hit << "   " << intersectionPoint;
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
        // debugLog() << "HIT minY: " << hit << "   " << intersectionPoint;
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
        // debugLog() << "HIT minZ: " << hit << "   " << intersectionPoint;
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
        // debugLog() << "HIT minX: " << hit << "   " << intersectionPoint;
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
        // debugLog() << "HIT minY: " << hit << "   " << intersectionPoint;
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
        // debugLog() << "HIT minZ: " << hit << "   " << intersectionPoint;
        return result = intersectionPoint;
    }

    return result;
}

double sampleTFOpacity( boost::shared_ptr< WDataSetSingle > transferFunctionData,
                 boost::shared_ptr< WDataSetScalar > scalarData,
                 double value )
{
    //Get Transferfunction Values
    boost::shared_ptr< WValueSetBase > transferFunctionValues = transferFunctionData->getValueSet();

    double max  = scalarData->getMax();
    double min  = scalarData->getMin();

    //Classification Variables
    double nominator = value - min;
    double denominator = max - min;

    if( denominator == 0.0 )
    {
        denominator = 0.0001;
    }

    //Classification: Convert Scalar to Color
    double scalarPercentage = nominator / denominator;
    int  iColorIdx   = scalarPercentage * transferFunctionValues->size();

    const double normalizationFactor = 255.0; // Unsigned char
    return transferFunctionData->getSingleRawValue( iColorIdx * 4 + 3 ) / normalizationFactor;
}


double  WMPickingDVREvaluation::importance( WPosition pos )
{
    bool interpolationSuccess = false;

    double value  = m_scalarDataSet->interpolate( pos, &interpolationSuccess );
    assert( interpolationSuccess && "Should not fail. Please file a bug report if it does." );

    if( m_importanceFunctionCur->get( true ).getItemIndexOfSelected( 0 ) == 0 )
    {
        return 1.0;
    }
    else if( m_importanceFunctionCur->get( true ).getItemIndexOfSelected( 0 ) == 1 )
    {
        double opacity = sampleTFOpacity( m_transferFunctionData, m_scalarDataSet, value );
        return opacity;
    }
    else if( m_importanceFunctionCur->get( true ).getItemIndexOfSelected( 0 ) == 2 )
    {
        return value;
    }
    else
    {
        assert( false && "Should not fail. Internal module error or bug if it does." );
        return 0;
    }
}

WPosition WMPickingDVREvaluation::interactionMapping( const WPosition& startPos, const WVector3d& viewDir )
{
    WPosition endPos = intersectBoundingBoxWithRay( m_bbox, startPos, viewDir );

    //Get Picking Mode
    WItemSelector selector  = m_pickingCriteriaCur->get( true );
    std::string  pickModeName = selector.at( 0 )->getName();

    double maxValue = -wlimits::MAX_DOUBLE;

    WPosition resultPos;
    double accAlpha  = 0.0;
    double oldOpacity = 0.0;

    WVector3d rayStep = ( 1.0 / m_sampleSteps->get( true ) ) * ( endPos - startPos );

    for( int sampleId = 0; sampleId < m_sampleSteps->get( true ); ++sampleId )
    {
        // * 0.9999 to get samples inside grid
        WPosition samplePos = startPos + sampleId * rayStep *.999999;

        bool interpolationSuccess = false;
        double scalar  = m_scalarDataSet->interpolate( samplePos, &interpolationSuccess );
        assert( interpolationSuccess && "Should not fail. Please file a bug report if it does." );

        if( pickModeName == WMPICKINGDVR_MAX_INTENS )
        {
            if( scalar > maxValue )
            {
                maxValue = scalar;
                resultPos = samplePos;
            }
        }
        else if( pickModeName == WMPICKINGDVR_FIRST_HIT )
        {
            double opacity = sampleTFOpacity( m_transferFunctionData, m_scalarDataSet, scalar );
            if( opacity > 0.0 )
            {
                resultPos = samplePos;
                break;
            }
        }
        else if( pickModeName == WMPICKINGDVR_HIGHEST_OPACITY )
        {
            double opacity = sampleTFOpacity( m_transferFunctionData, m_scalarDataSet, scalar );
            if( opacity > maxValue )
            {
                maxValue = opacity;
                resultPos = samplePos;
            }
        }
        else if( pickModeName == WMPICKINGDVR_MOST_CONTRIBUTING )
        {
            double opacity = sampleTFOpacity( m_transferFunctionData, m_scalarDataSet, scalar );
            accAlpha  = opacity + ( accAlpha - opacity * accAlpha );
            double contribution = accAlpha - oldOpacity;
            if( contribution > maxValue )
            {
                maxValue = contribution;
                resultPos = samplePos;
            }
            oldOpacity = accAlpha;
        }
        else
        {
            assert( false && "This should not happen. Please file a bug report if it does." );
        }
    }

    return resultPos;
}

WPosition WMPickingDVREvaluation::visualizationMapping( const WPosition& pos, const WVector3d& viewDir )
{
    // -1 because we want to project towards the viewer.
    // * 0.999999 to get samples inside grid also for border vertices
    return intersectBoundingBoxWithRay( m_bbox, pos, -1 * viewDir ) * 0.999999;
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
        debugLog() << "Processing ...";

        WDataSet::SPtr dataSet = m_scalarData->getData();

        WDataSetSingle::SPtr dsSingle = boost::dynamic_pointer_cast< WDataSetSingle >( dataSet );
        if( !dsSingle )
        {
            errorLog() << "[Invalid data set]";
            continue;
        }

        //Get scalar field
        m_scalarDataSet = m_scalarData->getData();
        if( !m_scalarDataSet )
        {
            errorLog() << "[Invalid scalar field]";
            continue;
        }

        //Get transfer function data
        m_transferFunctionData = m_transferFunction->getData();
        if( !m_transferFunctionData )
        {
            errorLog() << "[Invalid transfer function data]";
            continue;
        }

        // Is this a data set with a regular grid?
        WGridRegular3D::SPtr regGrid;
        regGrid = boost::dynamic_pointer_cast< WGridRegular3D >( dsSingle->getGrid() );
        if( !regGrid )
        {
            errorLog() << "[Invalid data set]";
            continue;
        }

        m_bbox = regGrid->getBoundingBox();

        double deltaVI = 0;

        std::vector< WVector3d > viewingDirections;
        if( m_viewDirection->get( true ) == WVector3d() )
        {
            debugLog() << "Found [0,0,0] viewing direction -> performing multi-directional sampling.";
            for( int i_ID = -1; i_ID <= 1; ++i_ID )
            {
                for( int j_ID = -1; j_ID <= 1; ++j_ID )
                {
                    for( int k_ID = -1; k_ID <= 1; ++k_ID )
                    {
                        const WVector3d dir( i_ID, j_ID, k_ID );
                        if( dir != WVector3d() )
                        {
                            viewingDirections.push_back( normalize( dir ) );
                        }
                    }
                }
            }
        }
        else
        {
            viewingDirections.push_back( m_viewDirection->get( true ) );
        }

        boost::shared_ptr< WProgress > progress( new WProgress( "Sampling",  m_samplesEval->get( true ) * viewingDirections.size() ) );
        m_progress->addSubProgress( progress );


        for( size_t viewingDirId = 0; viewingDirId < viewingDirections.size(); ++viewingDirId )
        {
            debugLog() << "Sampling in direction " << viewingDirections[viewingDirId];

            for( int sampleId = 0; sampleId < m_samplesEval->get( true ); ++sampleId )
            {
                assert( regGrid->getOrigin() == WPosition( 0.0, 0.0, 0.0 )
                        && "0.999999 in the following works only if origin is at zero." );

                size_t posId = sampleId * ( regGrid->size() / m_samplesEval->get( true ) );

                // * 0.9999 to get samples inside grid also for border vertices
                WPosition samplePos = regGrid->getPosition( posId ) * 0.999999;
                //debugLog() << "SamplePos: " << samplePos;

                WVector3d vd = viewingDirections[viewingDirId];
                double distance =  length( samplePos - interactionMapping( visualizationMapping( samplePos, vd ), vd ) );
                deltaVI += importance( samplePos ) * distance;
                //debugLog() << "Distance: " << distance;
                ++*progress;
            }
        }

        // Normalization
        deltaVI /= m_samplesEval->get( true ) * viewingDirections.size();

        {
            WItemSelector selector  = m_pickingCriteriaCur->get( true );
            std::string  pickModeName = selector.at( 0 )->getName();
            infoLog() << pickModeName << " deltaVI = " << deltaVI;
        }

        progress->finish();
    }
}
