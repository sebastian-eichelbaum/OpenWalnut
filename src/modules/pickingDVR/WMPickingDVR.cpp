//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2015-2017 A. Betz, D. Gerlicher, OpenWalnut Community
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
#include <cmath>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include <osg/CullFace>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/ShapeDrawable>
#include <osgViewer/View>

#include "core/dataHandler/WDataSetScalar.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/WGERequirement.h"
#include "core/graphicsEngine/WGraphicsEngine.h"
#include "core/graphicsEngine/WPickHandler.h"
#include "core/graphicsEngine/WPickInfo.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModuleInputData.h"
#include "core/ui/WUIRequirement.h"

#include "WMPickingDVR.h"
#include "WMPickingDVRHelper.h"
#include "WVisiTrace.h"

// Module Defines
#define WMPICKINGDVR_MAX_INT   "Picking - Maximum Intensity"
#define WMPICKINGDVR_FIRST_HIT   "Picking - First Hit"
#define WMPICKINGDVR_THRESHOLD   "Picking - Threshold"
#define WMPICKINGDVR_MOST_CONTRIBUTING "Picking - Most Contributing"
#define WMPICKINGDVR_HIGHEST_OPACITY   "Picking - Highest Opacity"
#define WMPICKINGDVR_WYSIWYP   "Picking - WYSIWYP"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMPickingDVR )

WMPickingDVR::WMPickingDVR():
WModule(),
    m_propCondition( new WCondition() ),
    m_curve3D( 0 ),
    m_oldRayStart( 0.0, 0.0, 0.0 )
{
    m_intersected = false;
    m_posStart = osg::Vec3f( 0.0, 0.0, 0.0 );
    m_posEnd = osg::Vec3f( 0.0, 0.0, 0.0 );
}

WMPickingDVR::~WMPickingDVR()
{
}

boost::shared_ptr< WModule > WMPickingDVR::factory() const
{
    return boost::shared_ptr< WModule >( new WMPickingDVR() );
}

const std::string WMPickingDVR::getName() const
{
    return "Picking in DVR";
}

const std::string WMPickingDVR::getDescription() const
{
    return "Picks a 3D Position from the DVR rendered dataset. Designed to work with orthographic projection.";
}

void WMPickingDVR::connectors()
{
    m_transferFunction = WModuleInputData< WDataSetSingle >::createAndAdd( shared_from_this(), "transfer function", "The 1D transfer function." );
    m_scalarIC = WModuleInputData< WDataSetScalar >::createAndAdd( shared_from_this(), "scalar data", "Scalar data." );

    WModule::connectors();
}

void WMPickingDVR::properties()
{
    m_selectionTypesList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_selectionTypesList->addItem( "Position (Picking)" );
    m_selectionTypesList->addItem( "Line (First Hit)" );
    m_selectionTypesList->addItem( "Line (VisiTrace) [NOT YET CORRECTLY IMPLEMENTED]" );
    m_selectionType = m_properties->addProperty( "Selection type",
                                                 "What type of structure is to be selected in the DVR?",
                                                 m_selectionTypesList->getSelectorFirst(),
                                                 m_propCondition );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_selectionType );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_selectionType );

    m_sampleSteps = m_properties->addProperty( "Samples - steps",
                      "Number of samples. Choose this appropriately for the settings used for the DVR itself.",
                       256,
                       m_propCondition );
    m_sampleSteps->setMin( 0 );
    m_sampleSteps->setMax( 10000 );

    m_opacityCorrectionEnabled = m_properties->addProperty( "Opacity correction",
            "If enabled, opacities are assumed to be relative to the "
            "sample count. Choose this appropriately for the settings used for the DVR itself.",
            true,
            m_propCondition );

    m_continuousDrawing = m_properties->addProperty( "Continuous drawing",
            "Should line be shown during drawing action?",
            true,
            m_propCondition );

    m_lineColor = m_properties->addProperty( "Line color",
                                             "Color of line/s indicating selected position/s.",
                                             WColor( 0.5f, 0.5f, 0.5f, 1.0f ),
                                             m_propCondition );

    m_lineThickness = m_properties->addProperty( "Line thickness",
                                                 "Thickness of line/s indicating selected position/s.",
                                                 0.5,
                                                 m_propCondition );
    m_lineThickness->setMin( 0.001 );
    m_lineThickness->setMax( 5.0 );

    m_crossSize = m_properties->addProperty( "Crosshair size",
                                             "Length of crosshair lines.",
                                             100.0,
                                             m_propCondition );
    m_crossSize->setMin( 0.001 );
    m_crossSize->setMax( 1000.0 );

    m_pickingCriteriaList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_pickingCriteriaList->addItem( WMPICKINGDVR_FIRST_HIT, WMPICKINGDVR_FIRST_HIT );
    m_pickingCriteriaList->addItem( WMPICKINGDVR_THRESHOLD, WMPICKINGDVR_THRESHOLD );
    m_pickingCriteriaList->addItem( WMPICKINGDVR_MOST_CONTRIBUTING, WMPICKINGDVR_MOST_CONTRIBUTING );
    m_pickingCriteriaList->addItem( WMPICKINGDVR_HIGHEST_OPACITY, WMPICKINGDVR_HIGHEST_OPACITY );
    m_pickingCriteriaList->addItem( WMPICKINGDVR_WYSIWYP, WMPICKINGDVR_WYSIWYP );
    m_pickingCriteriaList->addItem( WMPICKINGDVR_MAX_INT, WMPICKINGDVR_MAX_INT );

    m_pickingCriteriaCur = m_properties->addProperty( "Picking method",
                                                      "Select a picking method",
                                                      m_pickingCriteriaList->getSelectorFirst(),
                                                      m_propCondition );

    WPropertyHelper::PC_SELECTONLYONE::addTo( m_pickingCriteriaCur );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_pickingCriteriaCur );

    m_alphaThreshold = m_properties->addProperty( "Alpha threshold %",
                                                  "Alpha value threshold for threshold picking mode.",
                                                  0.5,
                                                  m_propCondition );
    m_alphaThreshold->setMin( 0.0 );
    m_alphaThreshold->setMax( 1.0 );


    m_wysiwypPositionTypesList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_wysiwypPositionTypesList->addItem( "Front" );
    m_wysiwypPositionTypesList->addItem( "Center" );

    m_wysiwypPositionType = m_properties->addProperty( "Pick position type",
                                                       "Which position of the picked object should be returned?",
                                                       m_wysiwypPositionTypesList->getSelectorFirst(),
                                                       m_propCondition );

    WPropertyHelper::PC_SELECTONLYONE::addTo( m_wysiwypPositionType );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_wysiwypPositionType );


    WModule::properties();
}

void WMPickingDVR::requirements()
{
    m_requirements.push_back( new WGERequirement() );
    m_requirements.push_back( new WUIRequirement() );
}

void WMPickingDVR::moduleMain()
{
    // Get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_scalarIC->getDataChangedCondition() );
    m_moduleState.add( m_transferFunction->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    ready();

    // Graphics setup
    m_rootNode = osg::ref_ptr< WGEManagedGroupNode >( new WGEManagedGroupNode( m_active ) );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );

    //Get Camera and Register the callback
    boost::shared_ptr< WGraphicsEngine > graphicsEngine = WGraphicsEngine::getGraphicsEngine();
    boost::shared_ptr< WGEViewer > mainView = graphicsEngine->getViewerByName( "Main View" );

    // Register PickHandler
    mainView->getPickHandler()->getPickSignal()->connect( boost::bind( &WMPickingDVR::pickHandler, this, _1 ) );

    // Main loop
    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish?
        if( m_shutdownFlag() )
        {
            break;
        }

        std::string pickingMode;

        const int selectionType =  m_selectionType->get( true ).getItemIndexOfSelected( 0 );
        if( selectionType == 0 ) // Pick
        {
            pickingMode = m_pickingCriteriaCur->get( true ).at( 0 )->getName();
            debugLog() << pickingMode;
        }
        else if( selectionType == 1 ) // Line (First Hit)
        {
            pickingMode = WMPICKINGDVR_FIRST_HIT;
            debugLog() << "Line (First Hit)";
        }
        else if( selectionType == 2 ) // VisiTrace
        {
            pickingMode = WMPICKINGDVR_WYSIWYP;
            debugLog() << "VisiTrace";
        }

        updateModuleGUI( pickingMode );

        if( selectionType == 0 )
        {
            // Valid position picked on proxy cube
            if( m_intersected )
            {
                bool pickingSuccessful = false;
                const WPosition posPicking = getPickedDVRPosition( pickingMode, &pickingSuccessful );

                if( pickingSuccessful )
                {
                    updatePositionIndicator( posPicking );
                }
            }
        }
        else if( selectionType == 1)
        {
            bool pickingSuccessful = false;
            const WPosition posPicking = getPickedDVRPosition( pickingMode, &pickingSuccessful );
            if( pickingSuccessful )
            {
                m_curve3D.push_back( posPicking );
            }

            if( m_continuousDrawing->get()
                || !m_pickInProgress )
            {
                updateCurveRendering();
            }

            if( m_pickInProgress )
            {
                m_moduleState.notify(); // Make sure that main loop is excuted until picking stopped
            }
            else
            {
                m_curve3D.clear(); // Start a new line for the next line selection
            }
        }
        else if( selectionType == 2 )
        {
            if( m_oldRayStart != WPosition( m_posStart ) )
            {
                m_oldRayStart = m_posStart;

                std::vector< std::pair< double, WPosition > > candidates = computeVisiTraceCandidates();

                std::cout << std::endl << "...##... ";
                for( auto rayPos : candidates )
                {
                    std::cout << rayPos.second << " == ";
                }
                std::cout << std::endl;

                if( candidates.size() )
                {
                    m_visiTrace.addCandidatesForRay( candidates );
                }
            }

            if( m_continuousDrawing->get()
                || !m_pickInProgress )
            {
                m_curve3D = m_visiTrace.getLine();
                debugLog() << "Obtained line with " <<  m_curve3D.size() << " elements.";
                updateCurveRendering();
            }

            if( m_pickInProgress )
            {
                m_moduleState.notify(); // Make sure that main loop is excuted until picking stopped
            }
            else
            {
                m_visiTrace.reset();
                m_curve3D.clear(); // Start a new line for the next line selection
            }
        }
        else
        {
            WAssert( false, "This should not happen. Internal module error." );
        }
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

void WMPickingDVR::pickHandler( WPickInfo pickInfo )
{
    if( pickInfo.getMouseButton() != 2 ) // Not right mouse button
    {
        if( pickInfo.getName() == WPickHandler::unpickString )
        {
            m_pickInProgress = false;
        }
        return;
    }

    m_pickInProgress = true;

    boost::shared_ptr< WGraphicsEngine > graphicsEngine = WGraphicsEngine::getGraphicsEngine();
    boost::shared_ptr< WGEViewer > mainView = graphicsEngine->getViewerByName( "Main View" );

    osg::ref_ptr<osgViewer::Viewer> view = mainView->getView();
    osgUtil::LineSegmentIntersector::Intersections intersections;

    float fPosX = pickInfo.getPickPixel().x();
    float fPosY = pickInfo.getPickPixel().y();

    bool intersected = view->computeIntersections( fPosX, fPosY, intersections, 0xFFFFFFFF );
    if( intersected )
    {
        osgUtil::LineSegmentIntersector::Intersection start= *intersections.begin();
        osgUtil::LineSegmentIntersector::Intersection end = *intersections.rbegin();

        m_posStart = start.getWorldIntersectPoint();
        m_posEnd  = end.getWorldIntersectPoint();

        m_intersected = true;

        // Notify moduleMain()
        m_propCondition->notify();
    }
}

void WMPickingDVR::updateModuleGUI( std::string pickingMode )
{
    const bool picking = ( m_selectionType->get( true ).getItemIndexOfSelected( 0 ) == 0 );

    if( pickingMode == WMPICKINGDVR_THRESHOLD )
    {
        m_alphaThreshold->setHidden( false );
    }
    else
    {
        m_alphaThreshold->setHidden( true );
    }

    if( pickingMode == WMPICKINGDVR_WYSIWYP )
    {
        m_wysiwypPositionType->setHidden( false );
    }
    else
    {
        m_wysiwypPositionType->setHidden( true );
    }

    if( picking )
    {
        m_pickingCriteriaCur->setHidden( false );
        m_crossSize->setHidden( false );
        m_continuousDrawing->setHidden( true );
    }
    else
    {
        m_pickingCriteriaCur->setHidden( true );
        m_crossSize->setHidden( true );
        m_continuousDrawing->setHidden( false );
    }
}


std::pair<int, int> WMPickingDVR::getWYSIWYPBounds( const std::vector<double>& vecAlphaAcc )
{
    std::vector<int> vecIndicesLowerBounds;
    std::vector<int> vecIndicesUpperBounds;
    calculateIntervalsWYSIWYP( vecAlphaAcc, vecIndicesLowerBounds, vecIndicesUpperBounds );

    // Calculate max difference
    double diff = 0.0;
    double maxDiff = 0.0;
    int sampleLo = -1;
    int sampleUp = -1;

    for( unsigned int j = 0; j < std::min( vecIndicesLowerBounds.size(), vecIndicesUpperBounds.size() ); j++ )
    {
        // Calculate opacity jump of interval
        diff = vecAlphaAcc[vecIndicesUpperBounds[j]] - vecAlphaAcc[vecIndicesLowerBounds[j]];
        //debugLog() << "Interval [" <<  vecIndicesLowerBounds[j] << "," << vecIndicesUpperBounds[j] << "] = " << diff;

        // Is max diff
        if( diff > maxDiff )
        {
            maxDiff = diff;
            sampleLo = vecIndicesLowerBounds[j];
            sampleUp = vecIndicesUpperBounds[j];
        }
    }
    //debugLog() << "Start of largest interval " << sampleLo;

    return std::make_pair( sampleLo, sampleUp );
}


void WMPickingDVR::calculateIntervalsWYSIWYP( const std::vector<double>& vecAlphaAcc,
                                              std::vector<int>& vecIndicesLowerBounds,
                                              std::vector<int>& vecIndicesUpperBounds )
{
    std::vector<double> vecFirstDerivative;
    std::vector<double> vecSecondDerivative;

    PickingDVRHelper::calculateDerivativesWYSIWYP( vecAlphaAcc, vecFirstDerivative, vecSecondDerivative );

    // Calculate interval boundaries
    double oldDerivative;
    if( vecSecondDerivative.size() > 0 )
    {
        oldDerivative = vecSecondDerivative[0];
    }

    for( unsigned int j = 1; j < vecSecondDerivative.size(); j++ )
    {
        if( oldDerivative < 0.0 && vecSecondDerivative[j] >= 0.0
            && ( vecIndicesLowerBounds.size() > 0 ) ) // need to have a lower bound already
        {
            vecIndicesUpperBounds.push_back( j );
        }

        if( oldDerivative <= 0.0 && vecSecondDerivative[j] > 0.0 )
        {
            vecIndicesLowerBounds.push_back( j );
        }

        oldDerivative = vecSecondDerivative[j];
    }
}

void WMPickingDVR::updatePositionIndicator( osg::Vec3f position )
{
    osg::ref_ptr< osg::Geometry > geometry;
    osg::ref_ptr< osg::Geode > geode( new osg::Geode );

    const double size  = m_crossSize->get();
    const double thickness = m_lineThickness->get();

    osg::ShapeDrawable* pBoxX = new osg::ShapeDrawable( new osg::Box( position, size, thickness, thickness ) );
    osg::ShapeDrawable* pBoxY = new osg::ShapeDrawable( new osg::Box( position, thickness, size, thickness ) );
    osg::ShapeDrawable* pBoxZ = new osg::ShapeDrawable( new osg::Box( position, thickness, thickness, size ) );

    pBoxX->setColor( m_lineColor->get() );
    pBoxY->setColor( m_lineColor->get() );
    pBoxZ->setColor( m_lineColor->get() );

    geode->addDrawable( pBoxX );
    geode->addDrawable( pBoxY );
    geode->addDrawable( pBoxZ );

    m_rootNode->remove( m_geode );
    m_geode = geode;

    m_rootNode->clear();
    m_rootNode->insert( geode );
}

std::vector< std::pair< double, WPosition > > WMPickingDVR::sampleIntensityAlongRay()
{
    osg::Vec3f posStart = m_posStart;
    osg::Vec3f posEnd = m_posEnd;
    osg::Vec3f posSample = m_posStart;
    osg::Vec3f vecDir = posEnd - posStart;

    // Calculate step
    if( m_sampleSteps->get() > 0 )
    {
        vecDir = vecDir / m_sampleSteps->get();
    }

    // Get scalar field
    boost::shared_ptr< WDataSetScalar > scalarData = m_scalarIC->getData();
    if(!scalarData)
    {
        errorLog()<< "[Invalid scalar field]";
        return std::vector< std::pair< double, WPosition > >();
    }

    std::vector< std::pair< double, WPosition > > result;
    // Sampling loop
    for( int i = 0; i < m_sampleSteps->get(); i++ )
    {
        //Scalarfield values
        bool success = false;
        double value = scalarData->interpolate( posSample, &success );

        //Add Step
        posSample = posSample + vecDir;

        if( success )
        {
            result.push_back( std::make_pair( value, posSample ) );
        }
    }
    return result;
}

double WMPickingDVR::getTFAlpha( const double scalar ) const
{
    // Get transferfunction data
    boost::shared_ptr< WDataSetSingle > transferFunctionData = m_transferFunction->getData();
    if( !transferFunctionData )
    {
        WAssert( false, "Invalid transferfunction data" );
    }

    // Get transferfunction values
    boost::shared_ptr< WValueSetBase > transferFunctionValues = transferFunctionData->getValueSet();


    // Get scalar field
    boost::shared_ptr< WDataSetScalar > scalarData = m_scalarIC->getData();
    if(!scalarData)
    {
        WAssert( false, "Invalid scalar field" );
    }

    const double max = scalarData->getMax();
    const double min = scalarData->getMin();

    // Classification variables
    const double nominator = scalar - min;
    double denominator = max - min;

    if( denominator == 0.0 )
    {
        denominator = 0.0001;
    }

    // Classification: Convert scalar to color
    double scalarPercentage = nominator / denominator;
    int colorIdx = scalarPercentage * transferFunctionValues->size();

    WMPickingColor<double> color;

    // Get color from transferfunction
    color.setRed( transferFunctionData->getSingleRawValue( colorIdx * 4 + 0 ) );
    color.setGreen( transferFunctionData->getSingleRawValue( colorIdx * 4 + 1 ) );
    color.setBlue( transferFunctionData->getSingleRawValue( colorIdx * 4 + 2 ) );
    color.setAlpha( transferFunctionData->getSingleRawValue( colorIdx * 4 + 3 ) );
    color.normalize();

    return color.getAlpha();
}

double WMPickingDVR::compositingStep( double accAlpha, const double currentAlpha )
{
    double currentAlphaCorrected;

    if( m_opacityCorrectionEnabled->get( true ) )
    {
        const double defaultNumberOfSteps = 128.0;
        float relativeSampleDistance = defaultNumberOfSteps / m_sampleSteps->get();
        currentAlphaCorrected =  1.0 - pow( 1.0 - currentAlpha, relativeSampleDistance );
    }
    else
    {
        currentAlphaCorrected = currentAlpha;
    }

    accAlpha  = currentAlphaCorrected + ( accAlpha - currentAlphaCorrected * accAlpha );
    return accAlpha;
}


std::vector< std::pair< double, WPosition > > WMPickingDVR::computeVisiTraceCandidates()
{
    std::vector< std::pair< double, WPosition > > samples( 0 );
    samples = sampleIntensityAlongRay();

    if( samples.size() == 0 )
    {
        return std::vector< std::pair< double, WPosition > >();
    }

    double accAlpha = 0.0;

    std::vector<double> vecAlphaAcc;

    for( unsigned int i = 0; i < samples.size(); i++ )
    {
        const double currentAlpha = getTFAlpha( samples[i].first );
        accAlpha = compositingStep( accAlpha, currentAlpha );
        vecAlphaAcc.push_back( accAlpha );
    }

    std::vector<int> vecIndicesLowerBounds;
    std::vector<int> vecIndicesUpperBounds;
    calculateIntervalsWYSIWYP( vecAlphaAcc, vecIndicesLowerBounds, vecIndicesUpperBounds );

    std::vector<int> opacityJumps( 0 );
    for( unsigned int j = 0; j < std::min( vecIndicesLowerBounds.size(), vecIndicesUpperBounds.size() ); j++ )
    {
        opacityJumps.push_back( vecAlphaAcc[vecIndicesUpperBounds[j]] - vecAlphaAcc[vecIndicesLowerBounds[j]] );
    }

    std::vector< std::pair< double, WPosition > > candidates( 0 );
    for( size_t id = 0; id < opacityJumps.size(); ++id )
    {
        candidates.push_back( std::make_pair( opacityJumps[id], samples[vecIndicesLowerBounds[id]].second ) );
    }
    return candidates;
}

WPosition WMPickingDVR::getPickedDVRPosition(  std::string pickingMode, bool* pickingSuccess )
{
    std::vector< std::pair< double, WPosition > > samples( 0 );
    samples = sampleIntensityAlongRay();
    if( samples.size() == 0 )
    {
        *pickingSuccess = false;
        return WPosition();
    }

    WPosition posPicking = m_posStart;

    double accAlpha = 0.0;
    double accAlphaOld = 0.0;
    double pickedAlpha = 0.0;
    double maxValue =  -1 * std::numeric_limits< double >::max();

    std::vector<double> vecAlphaAcc;

    for( unsigned int i = 0; i < samples.size(); i++ )
    {
        const double currentAlpha = getTFAlpha( samples[i].first );
        accAlpha = compositingStep( accAlpha, currentAlpha );

        if( pickingMode == WMPICKINGDVR_MAX_INT )
        {
            // Maximum Intensity: maximal scalar value
            if( samples[i].first > maxValue )
            {
                maxValue = samples[i].first;
                posPicking = samples[i].second;
                *pickingSuccess = true;
            }
        }
        else if( pickingMode == WMPICKINGDVR_FIRST_HIT )
        {
            // First Hit: first alpha value > 0.0
            if( currentAlpha > 0.0 )
            {
                pickedAlpha = currentAlpha;
                posPicking  =  samples[i].second;
                *pickingSuccess  = true;
                break;
            }
        }
        else if( pickingMode == WMPICKINGDVR_THRESHOLD )
        {
            // Threshold: accumulated alpha value > threshold
            if( accAlpha > m_alphaThreshold->get() )
            {
                pickedAlpha = currentAlpha;
                posPicking  =  samples[i].second;
                *pickingSuccess  = true;
                break;
            }
        }
        else if( pickingMode == WMPICKINGDVR_HIGHEST_OPACITY )
        {
            // Highest opacity: maximal alpha value
            if( currentAlpha > pickedAlpha )
            {
                pickedAlpha = currentAlpha;
                posPicking  =  samples[i].second;
                *pickingSuccess  = true;
            }
        }
        else if( pickingMode == WMPICKINGDVR_MOST_CONTRIBUTING )
        {
            double currenAlphaIncrease = accAlpha - accAlphaOld;
            // Most Contributing: maximal alpha value increase
            if( currenAlphaIncrease > pickedAlpha )
            {
                pickedAlpha = currenAlphaIncrease;
                posPicking = samples[i].second;
                *pickingSuccess = true;
            }
            accAlphaOld = accAlpha;
        }
        else if( pickingMode == WMPICKINGDVR_WYSIWYP )
        {
            //WYSIWYP: Save all the accumulated alpha values
            vecAlphaAcc.push_back( accAlpha );
        }
    } // End of sampling loop

    // WYSIWYP: Calculate the largest interval
    if( pickingMode == WMPICKINGDVR_WYSIWYP )
    {
        std::pair<int, int> bounds = getWYSIWYPBounds( vecAlphaAcc );

        if( bounds.first >= 0 )
        {
            osg::Vec3f vecDir = m_posEnd - m_posStart;

            // Calculate step
            if( m_sampleSteps->get() > 0 )
            {
                vecDir = vecDir / m_sampleSteps->get();
            }

            // Calculate pick position
            if( m_wysiwypPositionType->get( true ).getItemIndexOfSelected( 0 ) == 0 )
            {
                posPicking = m_posStart + vecDir * bounds.first;
            }
            else
            {
                int centerSample = ( bounds.first + bounds.second ) / 2;
                posPicking = m_posStart + vecDir * centerSample;
            }

            *pickingSuccess = true;
        }
    }

    if( *pickingSuccess )
    {
        // debugLog() << "[pickedAlpha = " << pickedAlpha << "]"
        //            << "[posPicking][X = " << posPicking.x() << " ]"
        //            << "[Y = " << posPicking.y() << " ][Z = " << posPicking.z() << "]";
    }

    return posPicking;
}

namespace
{
    osg::ref_ptr< osg::Geode > generateLineStripGeode( const WLine& line, const float thickness, const WColor& color )
    {
        using osg::ref_ptr;
        ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
        ref_ptr< osg::Vec4Array > colors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
        ref_ptr< osg::Geometry > geometry = ref_ptr< osg::Geometry >( new osg::Geometry );

        for( size_t i = 1; i < line.size(); ++i )
        {
            vertices->push_back( osg::Vec3( line[i-1][0], line[i-1][1], line[i-1][2] ) );
            colors->push_back( wge::getRGBAColorFromDirection( line[i-1], line[i] ) );
        }
        vertices->push_back( osg::Vec3( line.back()[0], line.back()[1], line.back()[2] ) );
        colors->push_back( colors->back() );

        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, 0, line.size() ) );
        geometry->setVertexArray( vertices );

        if( color != WColor( 0, 0, 0, 0 ) )
        {
            colors->clear();
            colors->push_back( color );
            geometry->setColorArray( colors );
            geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
        }
        else
        {
            geometry->setColorArray( colors );
            geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX ); // This will not work on OSG 3.2 you should compute the color per vertex
        }

        // line width
        osg::StateSet* stateset = geometry->getOrCreateStateSet();
        stateset->setAttributeAndModes( new osg::LineWidth( thickness ), osg::StateAttribute::ON );
        stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

        osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
        geode->addDrawable( geometry );
        return geode;
    }
}

void WMPickingDVR::updateCurveRendering()
{
    WLine line( m_curve3D );
    if( line.size() > 1 )
    {
        m_rootNode->remove( m_geode );
        m_geode = generateLineStripGeode( line, m_lineThickness->get(), m_lineColor->get() );

        m_rootNode->clear();
        m_rootNode->insert( m_geode );
    }
}
