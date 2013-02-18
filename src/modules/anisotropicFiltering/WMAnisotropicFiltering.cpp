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
#include <string>
#include <sstream>
#include <vector>

#include "core/kernel/WKernel.h"
#include "core/dataHandler/WDataHandler.h"
#include "core/common/WPropertyHelper.h"

#include "WMAnisotropicFiltering.xpm"
#include "WMAnisotropicFiltering.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMAnisotropicFiltering )

WMAnisotropicFiltering::WMAnisotropicFiltering():
    WModule()
{
    m_k = 1.0;
}

WMAnisotropicFiltering::~WMAnisotropicFiltering()
{
}

boost::shared_ptr< WModule > WMAnisotropicFiltering::factory() const
{
    return boost::shared_ptr< WModule >( new WMAnisotropicFiltering() );
}

const std::string WMAnisotropicFiltering::getName() const
{
    return "Anisotropic Filter";
}

const std::string WMAnisotropicFiltering::getDescription() const
{
    return "Filters MRI data, preserving edges.";
}

const char** WMAnisotropicFiltering::getXPMIcon() const
{
    return WMAnisotropicFiltering_xpm;
}

void WMAnisotropicFiltering::connectors()
{
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(), "in", "The input dataset." ) );
    addConnector( m_input );

    m_output = boost::shared_ptr< WModuleOutputData < WDataSetSingle  > >(
        new WModuleOutputData< WDataSetSingle >( shared_from_this(), "out", "The extracted image." ) );
    addConnector( m_output );

    WModule::connectors();
}

void WMAnisotropicFiltering::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_iterations = m_properties->addProperty( "#Iterations", "Number of iterations.", 10, m_propCondition );
    m_iterations->setMax( 1000 );

    m_Kcoeff = m_properties->addProperty( "K", "The diffusion weighting coefficient. Increase this to better preserve edges.", 9.0, m_propCondition );
    m_Kcoeff->setMin( 0.1 );
    m_Kcoeff->setMax( 1000.0 );

    m_delta = m_properties->addProperty( "delta", "The time step for integration.", 0.1, m_propCondition );
    m_delta->setMax( 10.0 );

    WModule::properties();
}

void WMAnisotropicFiltering::activate()
{
    WModule::activate();
}

void WMAnisotropicFiltering::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    ready();

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        boost::shared_ptr< WDataSetSingle > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        bool dataValid   = ( newDataSet );

        if( dataValid )
        {
            if( dataChanged || m_iterations->changed() || m_Kcoeff->changed() )
            {
                m_dataSet = newDataSet;
                WAssert( m_dataSet, "" );
                WAssert( m_dataSet->getValueSet(), "" );

                infoLog() << "Calculating.";

                m_k = m_Kcoeff->get( true );
                m_d = m_delta->get( true );

                calcSmoothedImages( m_iterations->get( true ) );

                infoLog() << "Calculation complete.";
            }
        }
    }

    debugLog() << "Shutting down...";
    debugLog() << "Finished! Good Bye!";
}

void WMAnisotropicFiltering::calcSmoothedImages( int iterations )
{
    if( iterations < 1 )
        return;

    std::size_t numImages = m_dataSet->getValueSet()->rawSize() / m_dataSet->getGrid()->size();
    infoLog() << "Images: " << numImages;

    boost::shared_ptr< std::vector< double > > smoothed( new std::vector< double >( m_dataSet->getValueSet()->rawSize() ) );
    boost::shared_ptr< WGridRegular3D > grid = boost::dynamic_pointer_cast< WGridRegular3D >( m_dataSet->getGrid() );

    // fill in data from dataset
    copyData( smoothed, grid );

    // the 3 derivatives ( actually this is the gradient )
    std::vector< double > deriv( 3 * m_dataSet->getGrid()->size() );

    // the diffusion coeff
    std::vector< double > coeff( m_dataSet->getGrid()->size() );

    boost::shared_ptr< WProgress > prog( new WProgress( "Smoothing images", numImages ) );
    m_progress->addSubProgress( prog );

    for( std::size_t k = 0; k < numImages; ++k )
    {
        for( int i = 0; i < iterations; ++i )
        {
            calcDeriv( deriv, smoothed, grid, k, numImages );
            if( m_iterations->changed() || m_Kcoeff->changed() || m_delta->changed() || m_dataSet != m_input->getData() )
            {
                prog->finish();
                return;
            }

            calcCoeff( coeff, deriv, grid );
            if( m_iterations->changed() || m_Kcoeff->changed() || m_delta->changed() || m_dataSet != m_input->getData() )
            {
                prog->finish();
                return;
            }

            diffusion( deriv, coeff, smoothed, grid, k, numImages );
            if( m_iterations->changed() || m_Kcoeff->changed() || m_delta->changed() || m_dataSet != m_input->getData() )
            {
                prog->finish();
                return;
            }
        }
        ++*prog;
    }

    prog->finish();

    // create dataset and update connector
    boost::shared_ptr< WValueSet< double > > vs( new WValueSet< double >(
                                                    m_dataSet->getValueSet()->order(),
                                                    m_dataSet->getValueSet()->dimension(),
                                                    smoothed,
                                                    W_DT_DOUBLE ) );
    boost::shared_ptr< WDataSetSingle > ds = m_dataSet->clone( vs );

    m_output->updateData( ds );
}

std::size_t WMAnisotropicFiltering::coordsToIndex( boost::shared_ptr< WGridRegular3D > const& grid,
                                                   std::size_t x, std::size_t y, std::size_t z )
{
    return x + y * grid->getNbCoordsX() + z * grid->getNbCoordsX() * grid->getNbCoordsY();
}

void WMAnisotropicFiltering::copyData( boost::shared_ptr< std::vector< double > >& smoothed,  // NOLINT non-const ref
                                       boost::shared_ptr< WGridRegular3D > const& /* grid */ )
{
    for( std::size_t k = 0; k < m_dataSet->getValueSet()->rawSize(); ++k )
    {
        ( *smoothed )[ k ] = m_dataSet->getValueSet()->getScalarDouble( k );
    }
}

void WMAnisotropicFiltering::calcDeriv( std::vector< double >& deriv,  // NOLINT non-const ref
                                        boost::shared_ptr< std::vector< double > > const& smoothed,
                                        boost::shared_ptr< WGridRegular3D > const& grid, std::size_t image, std::size_t numImages )
{
    std::size_t s[] = { grid->getNbCoordsX(), grid->getNbCoordsY(), grid->getNbCoordsZ() };
    double d[] = { fabs( grid->getOffsetX() ), fabs( grid->getOffsetY() ), fabs( grid->getOffsetZ() ) };

    for( std::size_t x = 0; x < grid->getNbCoordsX(); ++x )
    {
        for( std::size_t y = 0; y < grid->getNbCoordsY(); ++y )
        {
            for( std::size_t z = 0; z < grid->getNbCoordsZ(); ++z )
            {
                double const& x1 = ( *smoothed )[ numImages * coordsToIndex( grid, ( x + 1 ) % s[ 0 ], y, z ) + image ];
                double const& x2 = ( *smoothed )[ numImages * coordsToIndex( grid, ( x + s[ 0 ] - 1 ) % s[ 0 ], y, z ) + image ];
                deriv[ 3 * coordsToIndex( grid, x, y, z ) + 0 ] = ( x1 - x2 ) / ( 2.0 * d[ 0 ] );

                double const& y1 = ( *smoothed )[ numImages * coordsToIndex( grid, x, ( y + 1 ) % s[ 1 ], z ) + image ];
                double const& y2 = ( *smoothed )[ numImages * coordsToIndex( grid, x, ( y + s[ 1 ] - 1 ) % s[ 1 ], z ) + image ];
                deriv[ 3 * coordsToIndex( grid, x, y, z ) + 1 ] = ( y1 - y2 ) / ( 2.0 * d[ 1 ] );

                double const& z1 = ( *smoothed )[ numImages * coordsToIndex( grid, x, y, ( z + 1 ) % s[ 2 ] ) + image ];
                double const& z2 = ( *smoothed )[ numImages * coordsToIndex( grid, x, y, ( z + s[ 2 ] - 1 ) % s[ 2 ] ) + image ];
                deriv[ 3 * coordsToIndex( grid, x, y, z ) + 2 ] = ( z1 - z2 ) / ( 2.0 * d[ 2 ] );
            }
        }
    }
}

void WMAnisotropicFiltering::calcCoeff( std::vector< double >& coeff,  // NOLINT non-const ref
                                        std::vector< double > const& deriv,
                                        boost::shared_ptr< WGridRegular3D > const& grid )
{
    for( std::size_t x = 0; x < grid->getNbCoordsX(); ++x )
    {
        for( std::size_t y = 0; y < grid->getNbCoordsY(); ++y )
        {
            for( std::size_t z = 0; z < grid->getNbCoordsZ(); ++z )
            {
                // coeff = exp( -sqr( |I|/K ) )
                double gradIAbsSquared = deriv[ 3 * coordsToIndex( grid, x, y, z ) + 0 ]
                                       * deriv[ 3 * coordsToIndex( grid, x, y, z ) + 0 ]
                                       + deriv[ 3 * coordsToIndex( grid, x, y, z ) + 1 ]
                                       * deriv[ 3 * coordsToIndex( grid, x, y, z ) + 1 ]
                                       + deriv[ 3 * coordsToIndex( grid, x, y, z ) + 2 ]
                                       * deriv[ 3 * coordsToIndex( grid, x, y, z ) + 2 ];
                coeff[ coordsToIndex( grid, x, y, z ) ] = 1.0 / exp( gradIAbsSquared / ( m_k * m_k ) );
            }
        }
    }
}

void WMAnisotropicFiltering::diffusion( std::vector< double > const& deriv, std::vector< double > const& coeff,
                                        boost::shared_ptr< std::vector< double > >& smoothed,  // NOLINT non-const ref
                                        boost::shared_ptr< WGridRegular3D > const& grid, std::size_t image, std::size_t numImages )
{
    std::size_t s[] = { grid->getNbCoordsX(), grid->getNbCoordsY(), grid->getNbCoordsZ() };
    double d[] = { fabs( grid->getOffsetX() ), fabs( grid->getOffsetY() ), fabs( grid->getOffsetZ() ) };

    for( std::size_t x = 0; x < grid->getNbCoordsX(); ++x )
    {
        for( std::size_t y = 0; y < grid->getNbCoordsY(); ++y )
        {
            for( std::size_t z = 0; z < grid->getNbCoordsZ(); ++z )
            {
                // first deriv of the image intensity
                double const& dIx = deriv[ 3 * coordsToIndex( grid, x, y, z ) + 0 ];
                double const& dIy = deriv[ 3 * coordsToIndex( grid, x, y, z ) + 1 ];
                double const& dIz = deriv[ 3 * coordsToIndex( grid, x, y, z ) + 2 ];

                // first deriv of the diffusion coeff
                double dcx = ( coeff[ coordsToIndex( grid, ( x + 1 ) % s[ 0 ], y, z ) ]
                             - coeff[ coordsToIndex( grid, ( x + s[ 0 ] - 1 ) % s[ 0 ], y, z ) ] )
                              / ( 2.0 * d[ 0 ] );
                double dcy = ( coeff[ coordsToIndex( grid, x, ( y + 1 ) % s[ 1 ], z ) ]
                             - coeff[ coordsToIndex( grid, x, ( y + s[ 1 ] - 1 ) % s[ 1 ], z ) ] )
                              / ( 2.0 * d[ 1 ] );
                double dcz = ( coeff[ coordsToIndex( grid, x, y, ( z + 1 ) % s[ 2 ] ) ]
                             - coeff[ coordsToIndex( grid, x, y, ( z + s[ 2 ] - 1 ) % s[ 2 ] ) ] )
                              / ( 2.0 * d[ 2 ] );

                // diffusion coeff
                double const& c = coeff[ coordsToIndex( grid, x, y, z ) ];

                // 2nd derivatives in x, y, and z of the image intensity
                double ddIxx = ( deriv[ 3 * coordsToIndex( grid, ( x + 1 ) % s[ 0 ], y, z ) + 0 ]
                               - deriv[ 3 * coordsToIndex( grid, ( x + s[ 0 ] - 1 ) % s[ 0 ], y, z ) + 0 ] )
                                / ( 2 * d[ 0 ] );
                double ddIyy = ( deriv[ 3 * coordsToIndex( grid, x, ( y + 1 ) % s[ 1 ], z ) + 1 ]
                               - deriv[ 3 * coordsToIndex( grid, x, ( y + s[ 1 ] - 1 ) % s[ 1 ], z ) + 1 ] )
                                / ( 2 * d[ 1 ] );
                double ddIzz = ( deriv[ 3 * coordsToIndex( grid, x, y, ( z + 1 ) % s[ 2 ] ) + 2 ]
                               - deriv[ 3 * coordsToIndex( grid, x, y, ( z + s[ 2 ] - 1 ) % s[ 2 ] ) + 2 ] )
                                / ( 2 * d[ 2 ] );

                // d * ( grad I * grad c + c * grad grad I )
                ( *smoothed )[ numImages * coordsToIndex( grid, x, y, z ) + image ] += m_d * ( dIx * dcx + dIy * dcy + dIz * dcz
                                                                                             + c * ( ddIxx + ddIyy + ddIzz ) );
            }
        }
    }
}
