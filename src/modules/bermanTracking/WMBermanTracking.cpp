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

#include <algorithm>
#include <ctime>
#include <string>
#include <vector>

#include <teem/elf.h> // NOLINT: the stylechecker interprets this as c-header which is not true!

#include "core/common/math/WSymmetricSphericalHarmonic.h"
#include "core/common/WLimits.h"
#include "core/common/exceptions/WPreconditionNotMet.h"
#include "core/graphicsEngine/WROIBox.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WROIManager.h"
#include "WMBermanTracking.xpm"
#include "WMBermanTracking.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMBermanTracking )

WMBermanTracking::WMBermanTracking():
    WModule(),
    m_frtMat( 1, 1 ),
    m_SHFittingMat( 1, 1 ),
    m_HMat( 1, 1 ),
    m_BMat( 1, 1 )
{
}

WMBermanTracking::~WMBermanTracking()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMBermanTracking::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMBermanTracking() );
}

const char** WMBermanTracking::getXPMIcon() const
{
    // a real icon
    return bermanTracking_xpm;
}
const std::string WMBermanTracking::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Berman Probabilistic Tracking";
}

const std::string WMBermanTracking::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return "Implements the probabilistic tracking algorithm with residual bootstrapping by Berman et al.";
}

void WMBermanTracking::connectors()
{
    m_input = boost::shared_ptr< WModuleInputData< WDataSetSphericalHarmonics > >(
                            new WModuleInputData< WDataSetSphericalHarmonics >( shared_from_this(),
                                "inSH", "A spherical harmonics dataset." )
            );

    m_inputGFA = boost::shared_ptr< WModuleInputData< WDataSetScalar > >( new WModuleInputData< WDataSetScalar >( shared_from_this(),
                "inGFA", "GFA." )
            );

    m_inputResidual = boost::shared_ptr< WModuleInputData< WDataSetRawHARDI > >( new WModuleInputData< WDataSetRawHARDI >( shared_from_this(),
                "inResiduals", "The residual HARDI data." )
            );

    m_output = boost::shared_ptr< WModuleOutputData< WDataSetScalar > >( new WModuleOutputData< WDataSetScalar >( shared_from_this(),
                "outProb", "The probabilistic tracking result." )
            );

    // temporary
    m_outputFibers = boost::shared_ptr< WModuleOutputData< WDataSetFibers > >( new WModuleOutputData< WDataSetFibers >( shared_from_this(),
                "outFibers", "The probabilistic fibers." )
            );

    addConnector( m_input );
    addConnector( m_inputResidual );

    addConnector( m_inputGFA );

    addConnector( m_output );

    // temporary
    addConnector( m_outputFibers );

    // call WModules initialization
    WModule::connectors();
}

void WMBermanTracking::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    m_exceptionCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_minFA = m_properties->addProperty( "Min. FA", "The fractional anisotropy threshold value needed by "
                                                    "Berman's fiber tracking algorithm.", 0.02, m_propCondition );
    m_minFA->setMax( 1.0 );
    m_minFA->setMin( 0.0 );

    m_minPoints = m_properties->addProperty( "Min. points", "The minimum number of points per fiber.", 30, m_propCondition );
    m_minPoints->setMax( 100 );
    m_minPoints->setMin( 1 );

    m_minCos = m_properties->addProperty( "Min. cosine", "Minimum cosine of the angle between two"
                                           " adjacent fiber segments.", 0.30, m_propCondition );
    m_minCos->setMax( 1.0 );
    m_minCos->setMin( 0.0 );

    m_probabilistic = m_properties->addProperty( "Probabilistic tracking?", "Use bootstrapping for SH generation per voxel.", true, m_propCondition );

    m_seedsPerDirection = m_properties->addProperty( "Seeds per dir", "The number of seeds in a voxel per direction.", 1, m_propCondition );
    m_seedsPerDirection->setMin( 1 );
    m_seedsPerDirection->setMax( 16 );

    m_seedsPerPosition = m_properties->addProperty( "Seeds per pos", "The number of seeds in a voxel per position.", 1, m_propCondition );
    m_seedsPerPosition->setMax( 1000 );
    m_seedsPerPosition->setMin( 1 );

    m_startTrigger = m_properties->addProperty( "Start", "Start the tracking.", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );

    m_ratio = m_properties->addProperty( "Ratio?", "Temporary", 3.0, m_propCondition );
    m_epsImpr = m_properties->addProperty( "EpsImpr?", "Temporary", 1.0, m_propCondition );

    WModule::properties();
}

void WMBermanTracking::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_inputResidual->getDataChangedCondition() );
    m_moduleState.add( m_inputGFA->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );
    m_moduleState.add( m_exceptionCondition );

    m_random = boost::shared_ptr< Random >( new Random() );
    m_random->seed( std::time( 0 ) );

    m_frtMat = WSymmetricSphericalHarmonic::calcFRTMatrix( 4 );
    WAssert( m_frtMat.cols() == m_frtMat.rows(), "" );

    m_seedROI = boost::shared_ptr< WROIBox >( new WROIBox( WVector3d( 0.0, 0.0, 0.0 ), WVector3d( 200.0, 200.0, 200.0 ) ) );

    ready();

    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting.";
        m_moduleState.wait();

        boost::shared_ptr< WDataSetSphericalHarmonics > inData = m_input->getData();
        boost::shared_ptr< WDataSetRawHARDI > inData2 = m_inputResidual->getData();
        boost::shared_ptr< WDataSetScalar > inData3 = m_inputGFA->getData();
        bool dataChanged = ( m_dataSet != inData ) || ( m_dataSetResidual != inData2 ) || ( m_gfa != inData3 );

        if( dataChanged )
        {
            if( m_trackingPool )
            {
                m_trackingPool->stop();
                m_trackingPool->wait();
                if( m_currentProgress )
                {
                    m_currentProgress->finish();
                }
            }

            m_dataSet = inData;
            m_dataSetResidual = inData2;
            m_gfa = inData3;
        }

        if( m_startTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            m_startTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );

            if( m_dataSet && m_dataSetResidual && m_gfa )
            {
                // calculate some matrices needed for the residuals
                std::vector< WUnitSphereCoordinates > c;
                for( std::size_t i = 0; i < m_dataSetResidual->getOrientations().size(); ++i )
                {
                    if( length( m_dataSetResidual->getOrientations().at( i ) ) != 0.0 )
                    {
                        c.push_back( WUnitSphereCoordinates( m_dataSetResidual->getOrientations().at( i ) ) );
                    }
                }
                m_SHFittingMat = WSymmetricSphericalHarmonic::getSHFittingMatrix( c, 4, 0.0, true );
                m_BMat = WSymmetricSphericalHarmonic::calcBaseMatrix( c, 4 );
                m_HMat = m_BMat * WSymmetricSphericalHarmonic::getSHFittingMatrix( c, 4, 0.0, false );
                WAssert( m_HMat.cols() == m_HMat.rows(), "" );

                // get current properties
                m_currentMinFA = m_minFA->get( true );
                m_currentMinPoints = static_cast< std::size_t >( m_minPoints->get( true ) );
                m_currentMinCos = m_minCos->get( true );
                m_currentProbabilistic = m_probabilistic->get( true );
                m_currentRatio = m_ratio->get( true );
                m_currentEpsImpr = m_epsImpr->get( true );

                // start tracking
                resetTracking();
                m_hits = boost::shared_ptr< std::vector< float > >( new std::vector< float >( m_dataSet->getGrid()->size(), 0.0 ) );
                m_trackingPool->run();
                debugLog() << "Running tracking function.";
            }
        }
        else if( m_trackingPool && m_trackingPool->status() == W_THREADS_FINISHED )
        {
            // get result
            // fiber accumulation is a test, will be removed later
            m_fiberSet = m_fiberAccu.buildDataSet();
            m_fiberAccu.clear();
            m_currentProgress->finish();
            debugLog() << "Tracking done.";
            // forward result
            m_outputFibers->updateData( m_fiberSet );
            m_trackingPool = boost::shared_ptr< TrackingFuncType >();

            boost::shared_ptr< WValueSet< float > > vs( new WValueSet< float >( 0, 1, m_hits, DataType< float >::type ) );

            m_result = boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, m_dataSet->getGrid() ) );
            m_result->setFileName( "Berman_prob_tracking_result" );

            m_result->getTexture()->threshold()->set( 0.05f );
            m_result->getTexture()->colormap()->set( m_result->getTexture()->colormap()->get().newSelector( WItemSelector::IndexList( 1, 2 ) ) );
            m_result->getTexture()->interpolation()->set( false );
            m_output->updateData( m_result );

            m_hits = boost::shared_ptr< std::vector< float > >();
        }
        else if( m_lastException )
        {
            throw WException( *m_lastException );
        }
    }
    // module shutdown
    debugLog() << "Shutting down module.";
    if( m_trackingPool )
    {
        if( m_trackingPool->status() == W_THREADS_RUNNING || m_trackingPool->status() == W_THREADS_STOP_REQUESTED )
        {
            m_trackingPool->stop();
            m_trackingPool->wait();
            debugLog() << "Aborting fiber tracking because the module was ordered to shut down.";
        }
    }
}

void WMBermanTracking::resetTracking()
{
    // check if we need to stop the currently running tracking
    if( m_trackingPool )
    {
        debugLog() << "Stopping tracking.";
        WThreadedFunctionStatus s = m_trackingPool->status();
        if( s != W_THREADS_FINISHED && s != W_THREADS_ABORTED )
        {
            m_trackingPool->stop();
            m_trackingPool->wait();
            s = m_trackingPool->status();
            WAssert( s == W_THREADS_FINISHED || s == W_THREADS_ABORTED, "" );
        }
        m_moduleState.remove( m_trackingPool->getThreadsDoneCondition() );
    }
    // the threadpool should have finished computing by now

    m_fiberAccu.clear();

    boost::shared_ptr< WGridRegular3D > g = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
    WAssert( g, "" );

    std::vector< int > v0( 3 );
    v0[ 0 ] = std::max( 1, static_cast< int >( m_seedROI->getMinPos()[ 0 ] ) );
    v0[ 1 ] = std::max( 1, static_cast< int >( m_seedROI->getMinPos()[ 1 ] ) );
    v0[ 2 ] = std::max( 1, static_cast< int >( m_seedROI->getMinPos()[ 2 ] ) );

    std::vector< int > v1( 3 );
    v1[ 0 ] = std::min( static_cast< int >( m_seedROI->getMaxPos()[ 0 ] + 1 ), static_cast< int >( g->getNbCoordsX() - 1 ) );
    v1[ 1 ] = std::min( static_cast< int >( m_seedROI->getMaxPos()[ 1 ] + 1 ), static_cast< int >( g->getNbCoordsY() - 1 ) );
    v1[ 2 ] = std::min( static_cast< int >( m_seedROI->getMaxPos()[ 2 ] + 1 ), static_cast< int >( g->getNbCoordsZ() - 1 ) );

    if( v0[ 0 ] > v1[ 0 ] )
    {
        std::swap( v0[ 0 ], v1[ 0 ] );
    }
    if( v0[ 1 ] > v1[ 1 ] )
    {
        std::swap( v0[ 1 ], v1[ 1 ] );
    }
    if( v0[ 2 ] > v1[ 2 ] )
    {
        std::swap( v0[ 2 ], v1[ 2 ] );
    }

    debugLog() << v0[ 0 ] << " " << v1[ 0 ] << std::endl;
    debugLog() << v0[ 1 ] << " " << v1[ 1 ] << std::endl;
    debugLog() << v0[ 2 ] << " " << v1[ 2 ] << std::endl;

    resetProgress( ( v1[ 0 ] - v0[ 0 ] ) * ( v1[ 1 ] - v0[ 1 ] ) * ( v1[ 2 ] - v0[ 2 ] )
                   * m_seedsPerDirection->get( false ) * m_seedsPerDirection->get( false ) * m_seedsPerDirection->get( false )
                   * m_seedsPerPosition->get( false ) );

    // create a new one
    boost::shared_ptr< Tracking > t( new Tracking( m_dataSet,
                                                   boost::bind( &This::getDirFunc, this, _1, _2 ),
                                                   boost::bind( &wtracking::WTrackingUtility::followToNextVoxel, _1, _2, _3 ),
                                                   boost::bind( &This::fiberVis, this, _1 ),
                                                   boost::bind( &This::pointVis, this, _1 ),
                                                   m_seedsPerDirection->get( true ), m_seedsPerPosition->get( true ), v0, v1 ) );
    m_trackingPool = boost::shared_ptr< TrackingFuncType >( new TrackingFuncType( 0, t ) );
    m_trackingPool->subscribeExceptionSignal( boost::bind( &This::handleException, this, _1 ) );
    m_moduleState.add( m_trackingPool->getThreadsDoneCondition() );
}

void WMBermanTracking::handleException( WException const& e )
{
    m_lastException = boost::shared_ptr< WException >( new WException( e ) );
    m_exceptionCondition->notify();
}

void WMBermanTracking::resetProgress( std::size_t todo )
{
    if( m_currentProgress )
    {
        m_currentProgress->finish();
    }
    m_currentProgress = boost::shared_ptr< WProgress >( new WProgress( "Berman tracking", todo ) );
    m_progress->addSubProgress( m_currentProgress );
}

WVector3d WMBermanTracking::getDirFunc( boost::shared_ptr< WDataSetSingle const >, wtracking::WTrackingUtility::JobType const& j )
{
    boost::shared_ptr< WGridRegular3D > g( boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() ) );
    // extract fiber directions from odf
    int v = g->getVoxelNum( j.first );

    WPrecondDiffers( v, -1 );
    WPrecond( m_gfa, "" );

    if( boost::shared_dynamic_cast< WDataSetSingle >( m_gfa )->getValueAt( v ) < m_currentMinFA )
    {
        return WVector3d( 0.0, 0.0, 0.0 );
    }

    WSymmetricSphericalHarmonic h;
    if( m_currentProbabilistic )
    {
        // create a random odf from the data and residuals
        h = createRandomODF( v );
    }
    else
    {
        // test
        h = m_dataSet->getSphericalHarmonicAt( v );
    }

    return getBestDirectionFromSH( h, j );
}

WSymmetricSphericalHarmonic WMBermanTracking::createRandomODF( std::size_t i )
{
    WSymmetricSphericalHarmonic h = m_dataSet->getSphericalHarmonicAt( i );
    WAssert( h.getOrder() >= 4, "" );
    WAssert( m_HMat.rows() == static_cast< int >( m_dataSetResidual->getNumberOfMeasurements() ), "" );
    WAssert( m_HMat.cols() == static_cast< int >( m_dataSetResidual->getNumberOfMeasurements() ), "" );
    WAssert( m_frtMat.rows() == 15, "" );
    WAssert( m_frtMat.cols() == 15, "" );

    WMatrix_2 m( 15, 1 );
    for( int k = 0; k < 15; ++k )
    {
        m( k, 0 ) = h.getCoefficients()[ k ] / m_frtMat( k, k );
    }

    WSymmetricSphericalHarmonic g( m );
    WAssert( g.getOrder() == 4, "" );

    // calc hardi data from sh + residual
    WMatrix_2 v = m_BMat * m;

    WAssert( v.rows() == static_cast< int >( m_dataSetResidual->getNumberOfMeasurements() ), "" );

    WMatrix_2 q( v.rows(), 1 );
    q.setZero();

    for( int k = 0; k < v.rows(); ++k )
    {
        int z = static_cast< int >( static_cast< double >( ( *m_random )() ) / ( 1.0 + static_cast< double >( 0u - 1u ) ) * ( v.rows() ) );
        WAssert( z >= 0 && z < static_cast< int >( v.rows() ), "" );

        // "-", because the residuals in the input dataset have differing sign
        q( k, 0 ) = v( k, 0 ) - ( m_dataSetResidual->getValueAt( i * v.rows() + z ) / ( sqrt( 1.0 - m_HMat( k, k ) ) ) );

        WAssert( !wlimits::isnan( q( k, 0 ) ), "" );
    }

    // now calc new sh coeffs from the resampled hardi data
    return WSymmetricSphericalHarmonic( m_SHFittingMat * q );
}

WVector3d WMBermanTracking::getBestDirectionFromSH( WSymmetricSphericalHarmonic const& h,
                                                    wtracking::WTrackingUtility::JobType const& j )
{
    WVector_2 w = h.getCoefficients();
    double gfa = h.calcGFA( m_BMat );
    const tijk_type *type = tijk_4o3d_sym;
    double sh[ 15 ];
    WAssert( type->num == 15, "" );

    for( unsigned int i = 0; i < type->num; ++i )
    {
        sh[ i ] = w[ i ];
    }

    // adjust the coefficients to teem's sh basefunctions
    int u = 0;
    for( int l = 0; l <= 4; l += 2 )
    {
        for( int m = -l; m <= l; ++m )
        {
            if( m < 0 && m % 2 == 1 )
            {
                sh[ u ] = -sh[ u ];
            }
            else if( m > 0 && m % 2 == 0 )
            {
                sh[ u ] = -sh[ u ];
            }
            ++u;
        }
    }
    WAssert( u == 15, "" );

    double ten[ 15 ];
    double res[ 15 ];
    double s[ 6 ];
    double f[ 18 ];


    // TEST
    //boost::shared_ptr< WGridRegular3D > g = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
    //int k = 1;
    //double gfa = m_gfa->getValueAt( g->getVoxelNum( j.first ) );

    int k = 1;
    if( gfa < 0.1 )
    {
        ++k;
    }
    if( gfa < 0.05 )
    {
        ++k;
    }

    double rat[ 4 ] = { m_currentRatio, m_currentRatio };
    tijk_esh_to_3d_sym_d( ten, sh, 4 );
    tijk_refine_rankk_parm *parm = tijk_refine_rankk_parm_new();
    parm->pos = 1;
    tijk_approx_heur_parm* p = tijk_approx_heur_parm_new();
    p->ratios = rat;
    //p->eps_impr = m_currentEpsImpr;
    p->refine_parm = parm;
    k = tijk_approx_heur_3d_d( s, f, res, ten, type, k, p );
//    int k = tijk_approx_heur_3d_d( s, f, res, ten, type, 3, p );
    p = tijk_approx_heur_parm_nix( p );

    //std::vector< WVector3d > fb;
    //for( int i = 0; i < k; ++i )
    //{
        //WVector3d v( g->getXVoxelCoord( j.first ), g->getYVoxelCoord( j.first ), g->getZVoxelCoord( j.first ) );
        //if( v[ 1 ] != 110 )
            //break;
        //fb.push_back( v );
        //v += WVector3d( f[ 3 * i + 0 ] * 0.6, f[ 3 * i + 1 ] * 0.6, f[ 3 * i + 2 ] * 0.6 );
        //fb.push_back( v );
    //}
    //m_fiberAccu.add( fb );
    //return WVector3d();

    WVector3d r;

    if( k == 0 )
    {
        r = WVector3d( 0.0, 0.0, 0.0 );
    }
    else if( length( j.second ) == 0.0 )
    {
        int i = static_cast< int >( static_cast< double >( ( *m_random )() ) / ( 1.0 + static_cast< double >( 0u - 1u ) ) * ( 2.0 * k ) );
        WAssert( i >= 0 && i <= 2 * k, "" );

        r = WVector3d( f[ 3 * ( i % k ) ], f[ 3 * ( i % k ) + 1 ], f[ 3 * ( i % k ) + 2 ] );
        if( i >= k )
        {
            r *= -1.0;
        }
    }
    else
    {
        int z = -1;
        double m = 1.0;
        double a = m_currentMinCos;

        for( int i = 0; i < k; ++i )
        {
            r = WVector3d( f[ 3 * i ], f[ 3 * i + 1 ], f[ 3 * i + 2 ] );
            if( dot( r, j.second ) > a )
            {
                z = i;
                a = dot( r, j.second );
                m = 1.0;
            }
            else if( dot( j.second, r * -1.0 ) > a )
            {
                z = i;
                a = dot( j.second, r * -1.0 );
                m = -1.0;
            }
        }

        if( z == -1 )
        {
            r = WVector3d( 0.0, 0.0, 0.0 );
        }
        else
        {
            r = WVector3d( f[ 3 * z ], f[ 3 * z + 1 ], f[ 3 * z + 2 ] );
            r *= m;
        }
    }
    return r;
}

void WMBermanTracking::fiberVis( std::vector< WVector3d > const& fiber )
{
    // test!
    if( fiber.size() >= m_currentMinPoints )
    {
        m_fiberAccu.add( fiber );

        // test
        boost::shared_ptr< WGridRegular3D > g( boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() ) );
        for( std::size_t k = 0; k < fiber.size(); ++k )
        {
            int v = g->getVoxelNum( fiber[ k ] );
            WAssert( v != -1, "" );
            m_hits->operator[] ( v ) += 1.0;
        }
    }
    ++*m_currentProgress;
}

void WMBermanTracking::pointVis( WVector3d const& )
{
}
