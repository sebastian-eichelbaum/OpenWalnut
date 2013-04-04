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

#include <string>
#include <vector>

#include "core/common/math/WSymmetricSphericalHarmonic.h"
#include "core/common/WLimits.h"
#include "core/common/math/linearAlgebra/WVectorFixed.h"
#include "core/common/math/WGeometryFunctions.h"
#include "core/kernel/WKernel.h"
#include "WMCalculateGFA.xpm"

#include "WMCalculateGFA.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMCalculateGFA )

WMCalculateGFA::WMCalculateGFA():
    WModule(),
    m_BMat( 1, 1 )
{
}

WMCalculateGFA::~WMCalculateGFA()
{
}

boost::shared_ptr< WModule > WMCalculateGFA::factory() const
{
    return boost::shared_ptr< WModule >( new WMCalculateGFA() );
}

const char** WMCalculateGFA::getXPMIcon() const
{
    return WMCalculateGFA_xpm;
}
const std::string WMCalculateGFA::getName() const
{
    return "Calculate GFA";
}

const std::string WMCalculateGFA::getDescription() const
{
    return "Calculates the generalized fractional anisotropy as defined in \"Q-Ball Imaging\" by S.Tuch in 2004";
}

void WMCalculateGFA::connectors()
{
    m_input = boost::shared_ptr< WModuleInputData< WDataSetSphericalHarmonics > >(
                            new WModuleInputData< WDataSetSphericalHarmonics >( shared_from_this(),
                                "inSH", "A spherical harmonics dataset." )
            );

    m_output = boost::shared_ptr< WModuleOutputData< WDataSetScalar > >( new WModuleOutputData< WDataSetScalar >( shared_from_this(),
                "outGFA", "The generalized fractional anisotropy map." )
            );

    addConnector( m_input );
    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}

void WMCalculateGFA::properties()
{
    m_exceptionCondition = boost::shared_ptr< WCondition >( new WCondition() );

    WModule::properties();
}

void WMCalculateGFA::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_exceptionCondition );

    std::vector< unsigned int > temp;
    std::vector< WVector3d > grad;

    tesselateIcosahedron( &grad, &temp, 2 );

    temp.clear();

    std::vector< WUnitSphereCoordinates< double > > ori;
    for( std::size_t k = 0; k < grad.size(); ++k )
    {
        if( grad[ k ][ 0 ] >= 0.0 )
        {
            ori.push_back( WUnitSphereCoordinates< double >( grad[ k ] ) );
        }
    }
    grad.clear();

    m_BMat = WSymmetricSphericalHarmonic< double >::calcBaseMatrix( ori, 4 );
    ori.clear();

    ready();

    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting.";
        m_moduleState.wait();

        boost::shared_ptr< WDataSetSphericalHarmonics > inData = m_input->getData();
        bool dataChanged = ( m_dataSet != inData );

        if( dataChanged && inData )
        {
            m_dataSet = inData;

            // start computation
            resetGFAPool();
            m_gfaPool->run();
            debugLog() << "Running computation.";
        }
        else if( m_gfaPool && ( m_gfaPool->status() == W_THREADS_FINISHED || m_gfaPool->status() == W_THREADS_ABORTED ) )
        {
            debugLog() << "Computation finished.";
            m_currentProgress->finish();
            m_result = boost::dynamic_pointer_cast< WDataSetScalar >( m_gfaFunc->getResult() );
            m_gfaPool = boost::shared_ptr< GFAPoolType >();
            m_gfaFunc = boost::shared_ptr< GFAFuncType >();

            // forward result
            m_output->updateData( m_result );
        }
        else if( m_lastException )
        {
            throw WException( *m_lastException );
        }
    }

    // module shutdown
    debugLog() << "Shutting down module.";
    if( m_gfaPool )
    {
        if( m_gfaPool->status() == W_THREADS_RUNNING || m_gfaPool->status() == W_THREADS_STOP_REQUESTED )
        {
            m_gfaPool->stop();
            m_gfaPool->wait();
        }
    }
}

void WMCalculateGFA::resetGFAPool()
{
    if( m_gfaPool )
    {
        WThreadedFunctionStatus s = m_gfaPool->status();
        if( s != W_THREADS_FINISHED && s != W_THREADS_ABORTED )
        {
            m_gfaPool->stop();
            m_gfaPool->wait();
            s = m_gfaPool->status();
            WAssert( s == W_THREADS_FINISHED || s == W_THREADS_ABORTED, "" );
        }
        m_moduleState.remove( m_gfaPool->getThreadsDoneCondition() );
    }
    // the threadpool should have finished computing by now

    boost::shared_ptr< WGridRegular3D > g = boost::dynamic_pointer_cast< WGridRegular3D >( m_dataSet->getGrid() );
    WAssert( g, "" );
    resetProgress( g->getNbCoordsX() * g->getNbCoordsY() * g->getNbCoordsZ() );

    // create a new one
    m_gfaFunc = boost::shared_ptr< GFAFuncType >( new GFAFuncType( m_dataSet, boost::bind( &This::perVoxelGFAFunc, this, _1 ) ) );
    m_gfaPool = boost::shared_ptr< GFAPoolType >( new GFAPoolType( 0, m_gfaFunc ) );
    m_gfaPool->subscribeExceptionSignal( boost::bind( &This::handleException, this, _1 ) );
    m_moduleState.add( m_gfaPool->getThreadsDoneCondition() );
}

void WMCalculateGFA::handleException( WException const& e )
{
    m_lastException = boost::shared_ptr< WException >( new WException( e ) );
    m_exceptionCondition->notify();
}

void WMCalculateGFA::resetProgress( std::size_t todo )
{
    if( m_currentProgress )
    {
        m_currentProgress->finish();
    }
    m_currentProgress = boost::shared_ptr< WProgress >( new WProgress( "calculate gfa", todo ) );
    m_progress->addSubProgress( m_currentProgress );
}

boost::array< double, 1 > WMCalculateGFA::perVoxelGFAFunc( WValueSet< double >::SubArray const& s )
{
    ++*m_currentProgress;
    boost::array< double, 1 > a;
    WValue<double> w( 15 );
    for( int i = 0; i < 15; ++i )
    {
        w[ i ] = s[ i ];
    }
    WSymmetricSphericalHarmonic< double > h( w );
    a[ 0 ] = h.calcGFA( m_BMat );

    return a;
}
