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

#include <sstream>
#include <string>
#include <vector>

#include "../../common/WAssert.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../dataHandler/io/WWriterFiberVTK.h"
#include "../../dataHandler/WValueSet.h"
#include "../../common/math/WLinearAlgebraFunctions.h"

#include "WEigenThread.h"
#include "WMoriThread.h"

#include "WMDeterministicFTMori.h"
#include "moriTracking.xpm"

WMDeterministicFTMori::WMDeterministicFTMori()
    : WModule(),
      m_dataSet(),
      m_fiberSet()
{
}

WMDeterministicFTMori::~WMDeterministicFTMori()
{
}

boost::shared_ptr< WModule > WMDeterministicFTMori::factory() const
{
    return boost::shared_ptr< WModule >( new WMDeterministicFTMori() );
}

const char** WMDeterministicFTMori::getXPMIcon() const
{
    return moriTracking_xpm;
}

const std::string WMDeterministicFTMori::getName() const
{
    return "Mori Det. Tracking";
}

const std::string WMDeterministicFTMori::getDescription() const
{
    return "Implements the simple deterministic fibertracking algorithm by Mori et al.";
}

void WMDeterministicFTMori::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    ready();

    while ( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        boost::shared_ptr< WDataSetSingle > inData = m_input->getData();
        bool dataChanged = ( m_dataSet != inData );
        if( dataChanged || !m_dataSet )
        {
            m_dataSet = inData;
            if( !m_dataSet )
            {
                continue;
            }
        }

        if( dataChanged || m_minFA->changed() || m_minPoints->changed() || m_minCos->changed() || m_run->changed() )
        {
            bool run = m_run->get( true );

            double minFA = m_minFA->get( true );
            unsigned int minPoints = static_cast< unsigned int >( m_minPoints->get( true ) );
            double minCos = m_minCos->get( true );

            if( dataChanged )
            {
                doEigen();
            }

            if( run )
            {
                doMori( minFA, minPoints, minCos );
            }

            m_output->updateData( m_fiberSet );
        }
    }
}

void WMDeterministicFTMori::connectors()
{
    m_output = boost::shared_ptr< WModuleOutputData< WDataSetFibers > >( new WModuleOutputData< WDataSetFibers >( shared_from_this(),
                "det FT Mori output fibers", "A set of fibers extracted from the input set." )
            );

    m_input = boost::shared_ptr< WModuleInputData< WDataSetSingle > >( new WModuleInputData< WDataSetSingle >( shared_from_this(),
                "det FT Mori input tensor field", "An input set of 2nd-order tensors on a regular 3d-grid." )
            );

    addConnector( m_input );
    addConnector( m_output );

    WModule::connectors();
}

void WMDeterministicFTMori::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_minFA = m_properties->addProperty( "Min. FA", "The fractional anisotropy threshold value needed by "
                                                    "Mori's fiber tracking algorithm.", 0.2, m_propCondition );
    m_minFA->setMax( 1.0 );
    m_minFA->setMin( 0.0 );

    m_minPoints = m_properties->addProperty( "Min. Points", "The minimum number of points per fiber.", 30, m_propCondition );
    m_minPoints->setMax( 100 );
    m_minPoints->setMin( 1 );

    m_minCos = m_properties->addProperty( "Min. Cosine", "Minimum cosine of the angle between two"
                                           " adjacent fiber segments.", 0.80, m_propCondition );
    m_minCos->setMax( 1.0 );
    m_minCos->setMin( 0.0 );

    m_run = m_properties->addProperty( "Start the Algorithm", "Starts the algorithm using the given paratemers.", false, m_propCondition );
}

void WMDeterministicFTMori::activate()
{
    WModule::activate();
}

void WMDeterministicFTMori::doEigen()
{
    WAssert( m_dataSet, "" );
    WAssert( m_dataSet->getValueSet(), "" );
    WAssert( m_dataSet->getGrid(), "" );

    size_t num_positions = m_dataSet->getGrid()->size();

    m_eigenVectors = boost::shared_ptr< std::vector< wmath::WVector3D > >( new std::vector< wmath::WVector3D >() );
    m_FA = boost::shared_ptr< std::vector< double > >( new std::vector< double >() );

    if( m_dataSet->getValueSet()->getDataType() != W_DT_FLOAT )
    {
        warnLog() << "This module needs float type tensors. Input ignored.";
        return;
    }

    boost::shared_ptr< WValueSet< float > > values = boost::shared_dynamic_cast< WValueSet< float > >( m_dataSet->getValueSet() );
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );

    if( grid->size() == 0 )
    {
        warnLog() << "No data positions! Input ignored.";
        return;
    }
    else
    {
        debugLog() << "Grid size: " << grid->getNbCoordsX() << "x" << grid->getNbCoordsY() << "x"
                   << grid->getNbCoordsZ() << " (" << grid->size() << ").";
    }

    m_eigenVectors->resize( num_positions );
    m_FA->resize( num_positions );

    boost::shared_ptr< WProgress > pro( new WProgress( "det Mori FT - eigendecomposition", num_positions ) );
    m_progress->addSubProgress( pro );

    // TODO( reichenbach ): the number of threads should depend on the number of processors available
    size_t numThreads = 8;

    std::vector< boost::shared_ptr< WEigenThread > > threads( numThreads );

    for( size_t k = 0; k < numThreads; ++k )
    {
        threads[ k ] = boost::shared_ptr< WEigenThread >( new WEigenThread( grid, values, pro, k, m_eigenVectors, m_FA ) );
        threads[ k ]->run();
    }

    for( size_t k = 0; k < numThreads; ++k )
    {
        threads[ k ]->wait();
    }

    pro->finish();
}

void WMDeterministicFTMori::doMori( double const minFA, unsigned int const minPoints, double minCos )
{
    WAssert( m_dataSet->getGrid(), "" );
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );

    size_t jobSize = ( grid->getNbCoordsX() - 2 ) * ( grid->getNbCoordsY() - 2 ) * ( grid->getNbCoordsZ() - 2 );

    boost::shared_ptr< WProgress > pro( new WProgress( "det Mori FT - tracking", jobSize ) );
    m_progress->addSubProgress( pro );

    boost::shared_ptr< WMoriThread::WFiberAccumulator > accu = boost::shared_ptr< WMoriThread::WFiberAccumulator >(
                                                                                            new WMoriThread::WFiberAccumulator() );

    size_t numThreads = 8;

    std::vector< boost::shared_ptr< WMoriThread > > threads( numThreads );

    for( size_t k = 0; k < numThreads; ++k )
    {
        threads[ k ] = boost::shared_ptr< WMoriThread >( new WMoriThread( grid, m_eigenVectors, m_FA, minFA, minPoints,
                                                                          minCos, pro, k, accu ) );
        threads[ k ]->run();
    }

    for( size_t k = 0; k < numThreads; ++k )
    {
        threads[ k ]->wait();
    }

    pro->finish();

    m_fiberSet = accu->buildDataSet();

    debugLog() << "Done!";
}

