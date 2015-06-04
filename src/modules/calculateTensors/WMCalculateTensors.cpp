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

#include <string>
#include <vector>

#include "core/common/math/WSymmetricSphericalHarmonic.h"
#include "core/common/math/WMatrix.h"
#include "core/common/math/linearAlgebra/WVectorFixed.h"
#include "core/common/WLimits.h"
#include "core/kernel/WKernel.h"

#include "WMCalculateTensors.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMCalculateTensors )

WMCalculateTensors::WMCalculateTensors():
    WModule(),
    m_SHToTensorMat( 1, 1 )
{
}

WMCalculateTensors::~WMCalculateTensors()
{
}

boost::shared_ptr< WModule > WMCalculateTensors::factory() const
{
    return boost::shared_ptr< WModule >( new WMCalculateTensors() );
}

const char** WMCalculateTensors::getXPMIcon() const
{
    return NULL;
}
const std::string WMCalculateTensors::getName() const
{
    return "Calculate Tensors";
}

const std::string WMCalculateTensors::getDescription() const
{
    return "Calculates diffusion tensors from an input sh dataset.";
}

void WMCalculateTensors::connectors()
{
    m_input = boost::shared_ptr< WModuleInputData< WDataSetSphericalHarmonics > >(
                            new WModuleInputData< WDataSetSphericalHarmonics >( shared_from_this(),
                                "shInput", "A spherical harmonics dataset." )
            );

    m_output = boost::shared_ptr< WModuleOutputData< WDataSetDTI > >( new WModuleOutputData< WDataSetDTI >( shared_from_this(),
                "dtiOutput", "The diffusion tensor image." )
            );

    addConnector( m_input );
    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}

void WMCalculateTensors::properties()
{
    m_exceptionCondition = boost::shared_ptr< WCondition >( new WCondition() );

    WModule::properties();
}

void WMCalculateTensors::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_exceptionCondition );

    // calc sh->tensor conversion matrix
    std::vector< WUnitSphereCoordinates< double > > orientations;
    orientations.push_back( WUnitSphereCoordinates< double >( normalize( WVector3d( 1.0, 0.0, 0.0 ) ) ) );
    orientations.push_back( WUnitSphereCoordinates< double >( normalize( WVector3d( 0.6, -0.1, 0.2 ) ) ) );
    orientations.push_back( WUnitSphereCoordinates< double >( normalize( WVector3d( 1.0, 1.0, 1.0 ) ) ) );
    orientations.push_back( WUnitSphereCoordinates< double >( normalize( WVector3d( -0.1, -0.3, 0.5 ) ) ) );
    orientations.push_back( WUnitSphereCoordinates< double >( normalize( WVector3d( -0.56347, 0.374, -0.676676 ) ) ) );
    orientations.push_back( WUnitSphereCoordinates< double >( normalize( WVector3d( 0.0, 4.0, 1.0 ) ) ) );

    m_SHToTensorMat = WSymmetricSphericalHarmonic< double >::calcSHToTensorSymMatrix( 2, orientations );

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
            resetTensorPool();
            if( m_tensorPool )
            {
                m_tensorPool->run();
            }
            debugLog() << "Running computation.";
        }
        else if( m_tensorPool && ( m_tensorPool->status() == W_THREADS_FINISHED || m_tensorPool->status() == W_THREADS_ABORTED ) )
        {
            debugLog() << "Computation finished.";
            m_currentProgress->finish();
            boost::shared_ptr< WDataSetSingle > temp = m_tensorFunc->getResult();
            m_result = boost::shared_ptr< WDataSetDTI >( new WDataSetDTI( temp->getValueSet(), temp->getGrid() ) );
            m_tensorPool = boost::shared_ptr< TensorPoolType >();
            m_tensorFunc = boost::shared_ptr< TensorFuncType >();

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
    if( m_tensorPool )
    {
        if( m_tensorPool->status() == W_THREADS_RUNNING || m_tensorPool->status() == W_THREADS_STOP_REQUESTED )
        {
            m_tensorPool->stop();
            m_tensorPool->wait();
        }
    }
}

void WMCalculateTensors::resetTensorPool()
{
    if( m_tensorPool )
    {
        WThreadedFunctionStatus s = m_tensorPool->status();
        if( s != W_THREADS_FINISHED && s != W_THREADS_ABORTED )
        {
            m_tensorPool->stop();
            m_tensorPool->wait();
            s = m_tensorPool->status();
            WAssert( s == W_THREADS_FINISHED || s == W_THREADS_ABORTED, "" );
        }
        m_moduleState.remove( m_tensorPool->getThreadsDoneCondition() );
    }
    // the threadpool should have finished computing by now

    if( m_dataSet->getSphericalHarmonicAt( 0 ).getOrder() == 2 )
    {
        boost::shared_ptr< WGridRegular3D > g = boost::dynamic_pointer_cast< WGridRegular3D >( m_dataSet->getGrid() );
        WAssert( g, "" );
        resetProgress( g->getNbCoordsX() * g->getNbCoordsY() * g->getNbCoordsZ() );

        // create a new one
        m_tensorFunc = boost::shared_ptr< TensorFuncType >( new TensorFuncType( m_dataSet, boost::bind( &This::perVoxelTensorFunc, this, _1 ) ) );
        m_tensorPool = boost::shared_ptr< TensorPoolType >( new TensorPoolType( 0, m_tensorFunc ) );
        m_tensorPool->subscribeExceptionSignal( boost::bind( &This::handleException, this, _1 ) );
        m_moduleState.add( m_tensorPool->getThreadsDoneCondition() );
    }
    else
    {
        warnLog() << "SH dataset has order > 2";
    }
}

void WMCalculateTensors::handleException( WException const& e )
{
    m_lastException = boost::shared_ptr< WException >( new WException( e ) );
    m_exceptionCondition->notify();
}

void WMCalculateTensors::resetProgress( std::size_t todo )
{
    if( m_currentProgress )
    {
        m_currentProgress->finish();
    }
    m_currentProgress = boost::shared_ptr< WProgress >( new WProgress( "calculate tensors", todo ) );
    m_progress->addSubProgress( m_currentProgress );
}

boost::array< double, 6 > WMCalculateTensors::perVoxelTensorFunc( WValueSet< double >::SubArray const& s )
{
    ++*m_currentProgress;
    boost::array< double, 6 > a;
    WValue<double> v( 6 );

    // calculation
    for( std::size_t k = 0; k < 6; ++k )
    {
        v[ k ] = s[ k ];
    }

    v = m_SHToTensorMat * v;

    for( std::size_t k = 0; k < 6; ++k )
    {
        a[ k ] = v[ k ];
    }

    return a;
}
