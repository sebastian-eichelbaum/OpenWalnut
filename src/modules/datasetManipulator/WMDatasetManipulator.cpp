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
#include <utility>

#include "core/kernel/WKernel.h"

#include "core/dataHandler/WGridRegular3D.h"
#include "core/dataHandler/WGridTransformOrtho.h"

#include "core/dataHandler/WDataSetSingle.h"
#include "core/dataHandler/WDataSetFibers.h"
#include "core/dataHandler/WDataSetPoints.h"
#include "core/graphicsEngine/WTriangleMesh.h"

#include "WManipulatorTranslation.h"
#include "WManipulatorRotation.h"
#include "WManipulatorScaling.h"

#include "WMDatasetManipulator.xpm"
#include "WMDatasetManipulator.h"

W_LOADABLE_MODULE( WMDatasetManipulator )

WMDatasetManipulator::WMDatasetManipulator():
    WModule(),
    m_strategy( "Manipulators", "Select one of the manipulators.", NULL, "Manipulator", "A list of all available manipulators" )
{
    m_strategy.addStrategy( boost::shared_ptr< WObjectNDIP< WManipulatorInterface > >( new WManipulatorTranslation() ) );
    m_strategy.addStrategy( boost::shared_ptr< WObjectNDIP< WManipulatorInterface > >( new WManipulatorRotation() ) );
    m_strategy.addStrategy( boost::shared_ptr< WObjectNDIP< WManipulatorInterface > >( new WManipulatorScaling() ) );
}

WMDatasetManipulator::~WMDatasetManipulator()
{
}

boost::shared_ptr< WModule > WMDatasetManipulator::factory() const
{
    return boost::shared_ptr< WModule >( new WMDatasetManipulator() );
}

char const** WMDatasetManipulator::getXPMIcon() const
{
    return WMDatasetManipulator_xpm;
}

std::string const WMDatasetManipulator::getName() const
{
    return "Dataset Manipulator";
}

std::string const WMDatasetManipulator::getDescription() const
{
    return "This module allows manipulation of the dataset properties via manipulators in the 3D scene.";
}

void WMDatasetManipulator::connectors()
{
    m_input = boost::shared_ptr< WModuleInputData< WDataSet > >(
        new WModuleInputData< WDataSet >( shared_from_this(), "in", "The input dataset." ) );
    addConnector( m_input );

    m_output = boost::shared_ptr< WModuleOutputData < WDataSet  > >(
        new WModuleOutputData< WDataSet >( shared_from_this(), "out", "The transformed dataset." ) );
    addConnector( m_output );

    WModule::connectors();
}

void WMDatasetManipulator::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition );

    m_resetTrigger = m_properties->addProperty( "Reset", "Reset transformation.", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );
    m_applyTrigger = m_properties->addProperty( "Apply", "Apply transformation.", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );

    m_properties->addProperty( m_strategy.getProperties() );

    WModule::properties();
}

void WMDatasetManipulator::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_strategy.getProperties()->getUpdateCondition() );

    ready();

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        if( m_input->getData() && m_data != m_input->getData() )
        {
            m_data = m_input->getData();
            initMatrix();
            m_output->updateData( m_data );
        }

        if( m_data && m_strategy()->transformationChanged() )
        {
            WMatrixFixed< double, 4, 4 > mat = m_strategy()->getTransformationMatrix();

            boost::shared_ptr< WDataSet > data = transformData( mat * m_currentMat );

            m_output->updateData( data );
        }

        if( m_resetTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            m_resetTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );

            if( m_data )
            {
                initMatrix();
                m_output->updateData( m_data );
            }
        }

        if( m_applyTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            m_applyTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );

            if( m_data )
            {
                WMatrixFixed< double, 4, 4 > mat = m_strategy()->getTransformationMatrix();
                m_currentMat = mat * m_currentMat;
                boost::shared_ptr< WDataSet > data = transformData( m_currentMat );

                m_output->updateData( data );
                m_strategy()->reset();
            }
        }
    }
}

boost::shared_ptr< WDataSet > WMDatasetManipulator::transformData( WMatrixFixed< double, 4, 4 > const& mat )
{
    boost::shared_ptr< WDataSetSingle > asSingle = boost::dynamic_pointer_cast< WDataSetSingle >( m_data );
    if( asSingle )
    {
        boost::shared_ptr< WGridRegular3D > grid = boost::dynamic_pointer_cast< WGridRegular3D >( asSingle->getGrid() );
        if( !grid )
        {
            return boost::shared_ptr< WDataSet >();
        }

        WMatrixFixed< double, 4, 4 > m = grid->getTransformationMatrix();
        m *= mat;

        WMatrix< double > z( m );
        WGridTransformOrtho tr( z );
        boost::shared_ptr< WGrid > newGrid( new WGridRegular3D( grid->getNbCoordsX(), grid->getNbCoordsY(), grid->getNbCoordsZ(), tr ) );

        return asSingle->clone( newGrid );
    }

    boost::shared_ptr< WDataSetFibers > asFibers = boost::dynamic_pointer_cast< WDataSetFibers >( m_data );
    if( asFibers )
    {
        debugLog() << "Got fibers!";

        boost::shared_ptr< std::vector< float > > vertices = asFibers->getVertices();
        boost::shared_ptr< std::vector< float > > newVertices( new std::vector< float >( vertices->size() ) );

        for( std::size_t k = 0; k < vertices->size() / 3; ++k )
        {
            WVector4d v( ( *vertices )[ 3 * k + 0 ], ( *vertices )[ 3 * k + 1 ], ( *vertices )[ 3 * k + 2 ], 1.0 );
            WVector4d w = mat * v;
            ( *newVertices )[ 3 * k + 0 ] = w[ 0 ] / w[ 3 ];
            ( *newVertices )[ 3 * k + 1 ] = w[ 1 ] / w[ 3 ];
            ( *newVertices )[ 3 * k + 2 ] = w[ 2 ] / w[ 3 ];
        }

        boost::shared_ptr< WDataSetFibers > data( new WDataSetFibers( newVertices, asFibers->getLineStartIndexes(),
                                                    asFibers->getLineLengths(), asFibers->getVerticesReverse() ) );
        return data;
    }

    boost::shared_ptr< WDataSetPoints > asPoints = boost::dynamic_pointer_cast< WDataSetPoints >( m_data );
    if( asPoints )
    {
        boost::shared_ptr< std::vector< float > > p( new std::vector< float >( asPoints->getVertices()->size() ) );

        for( std::size_t k = 0; k < asPoints->getVertices()->size(); k += 3 )
        {
            WVector4d vec( asPoints->getVertices()->operator[]( k + 0 ),
                           asPoints->getVertices()->operator[]( k + 1 ),
                           asPoints->getVertices()->operator[]( k + 2 ),
                           1.0 );

            vec = mat * vec;
            p->operator[] ( k + 0 ) = vec[ 0 ];
            p->operator[] ( k + 1 ) = vec[ 1 ];
            p->operator[] ( k + 2 ) = vec[ 2 ];
        }

        return boost::shared_ptr< WDataSetPoints >( new WDataSetPoints( p, asPoints->getColors() ) );
    }

    return boost::shared_ptr< WDataSet >();
}

void WMDatasetManipulator::initMatrix()
{
    boost::shared_ptr< WDataSetSingle > asSingle = boost::dynamic_pointer_cast< WDataSetSingle >( m_data );
    if( asSingle )
    {
        boost::shared_ptr< WGridRegular3D > grid = boost::dynamic_pointer_cast< WGridRegular3D >( asSingle->getGrid() );
        if( !grid )
        {
            m_currentMat = WMatrixFixed< double, 4, 4 >::identity();
        }

        m_currentMat = grid->getTransformationMatrix();
    }
    else if( boost::dynamic_pointer_cast< WDataSetFibers >( m_data ) )
    {
        m_currentMat = WMatrixFixed< double, 4, 4 >::identity();
    }
    else if( boost::dynamic_pointer_cast< WDataSetPoints >( m_data ) )
    {
        m_currentMat = WMatrixFixed< double, 4, 4 >::identity();
    }
}
