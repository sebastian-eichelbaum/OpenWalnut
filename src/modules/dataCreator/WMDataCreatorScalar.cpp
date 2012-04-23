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
#include <string>

#include "core/common/WAssert.h"
#include "core/common/WProgress.h"
#include "core/common/WStrategyHelper.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/kernel/WKernel.h"

#include "WDataCreatorSphere.h"
#include "WDataCreatorRandom.h"

#include "WMDataCreatorScalar.xpm"
#include "WMDataCreatorScalar.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMDataCreatorScalar )

WMDataCreatorScalar::WMDataCreatorScalar():
    WModule(),
    m_strategy( "Dataset Creators", "Select one of the dataset creators and configure it to your needs.", NULL,
                "Creator", "A list of all known creators." )
{
    // add some strategies here
    m_strategy.addStrategy( WDataCreatorSphere::SPtr( new WDataCreatorSphere() ) );
    m_strategy.addStrategy( WDataCreatorRandom::SPtr( new WDataCreatorRandom() ) );
}

WMDataCreatorScalar::~WMDataCreatorScalar()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMDataCreatorScalar::factory() const
{
    return boost::shared_ptr< WModule >( new WMDataCreatorScalar() );
}

const char** WMDataCreatorScalar::getXPMIcon() const
{
    return datacreator_xpm;
}

const std::string WMDataCreatorScalar::getName() const
{
    return "Scalar Data Creator";
}

const std::string WMDataCreatorScalar::getDescription() const
{
    return "Allows the user to create scalar data sets on a regular grid by providing a bunch of data creation schemes.";
}

void WMDataCreatorScalar::connectors()
{
    // initialize connectors
    m_output = WModuleOutputData< WDataSetScalar >::createAndAdd( shared_from_this(), "out", "The data that has been created" );

    // call WModule's initialization
    WModule::connectors();
}

void WMDataCreatorScalar::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // how much voxels?
    m_nbVoxelsX = m_properties->addProperty( "Voxels X", "The number of voxels in X direction.", 128, m_propCondition );
    m_nbVoxelsX->setMin( 2 );
    m_nbVoxelsX->setMax( 4096 );
    m_nbVoxelsY = m_properties->addProperty( "Voxels Y", "The number of voxels in Y direction.", 128, m_propCondition );
    m_nbVoxelsY->setMin( 2 );
    m_nbVoxelsY->setMax( 4096 );
    m_nbVoxelsZ = m_properties->addProperty( "Voxels Z", "The number of voxels in Z direction.", 128, m_propCondition );
    m_nbVoxelsZ->setMin( 2 );
    m_nbVoxelsZ->setMax( 4096 );

    // grid transform information
    m_origin = m_properties->addProperty( "Origin", "Coordinate of the origin (voxel 0,0,0).", WPosition( 0.0, 0.0, 0.0 ), m_propCondition );
    m_size = m_properties->addProperty( "Size", "The size of the dataset along the X,Y, and Z axis in the OpenWalnut coordinate system.",
                                        WPosition( 128.0, 128.0, 128.0 ), m_propCondition );

    // now, setup the strategy helper.
    m_properties->addProperty( m_strategy.getProperties() );

    // call WModule's initialization
    WModule::properties();
}

void WMDataCreatorScalar::moduleMain()
{
    // let the main loop awake if the data changes or the properties changed.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );
    // we need to wake up if some strategy prop changed
    m_moduleState.add( m_strategy.getProperties()->getChildUpdateCondition() );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        // Now, the moduleState variable comes into play. The module can wait for the condition, which gets fired whenever the input receives data
        // or an property changes. The main loop now waits until something happens.
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }

        debugLog() << "Re-creating dataset";

        // create a new WGridRegular3D
        WGridTransformOrtho transform( m_size->get( true ).x() / static_cast< double >( m_nbVoxelsX->get( true ) - 1 ), // NOLINT
                                       //  - its not std::transform
                                       m_size->get( true ).y() / static_cast< double >( m_nbVoxelsY->get( true ) - 1 ),
                                       m_size->get( true ).z() / static_cast< double >( m_nbVoxelsZ->get( true ) - 1 )
        );
        // apply transform to new origin
        transform.translate( m_origin->get( true ) );
        WGridRegular3D::SPtr grid( new WGridRegular3D( m_nbVoxelsX->get(), m_nbVoxelsY->get(), m_nbVoxelsZ->get(), transform ) );

        // get the current strategy
        WValueSetBase::SPtr valueSet = m_strategy()->operator()( grid );

        debugLog() << "Created dataset with " << grid->size() << " voxels.";

        // build dataset
        WDataSetScalar::SPtr ds( new WDataSetScalar( valueSet, grid ) );

        // done. update output
        m_output->updateData( ds );
    }
}

