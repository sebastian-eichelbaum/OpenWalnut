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

#include <vector>

#include <core/common/WAssert.h>

#include "WTuringPatternCreator.h"
#include "WDataCreatorTuring.h"

WDataCreatorTuring::WDataCreatorTuring()
    : WObjectNDIP< WDataSetSingleCreatorInterface >( "Turing", "Creates a volume with evenly distributed dots." )
{
    m_spotSize = m_properties->addProperty( "Spot size", "", 0.65 );
    m_spotSize->setMax( 1.0 );
    m_spotSize->setMin( 0.0 );

    m_spotIrregularity = m_properties->addProperty( "Spot irregularity", "", 0.0 );
    m_spotIrregularity->setMax( 1.0 );
    m_spotIrregularity->setMin( 0.0 );

    m_iterations = m_properties->addProperty( "Turing integration steps", "", 100 );
    m_iterations->setMin( 0 );
    m_iterations->setMax( 1000000000 );
}

WDataCreatorTuring::~WDataCreatorTuring()
{
}

WValueSetBase::SPtr WDataCreatorTuring::operator()( WProgress::SPtr progress, WGridRegular3D::ConstSPtr grid, unsigned char order,
                                                    unsigned char dimension, dataType type )
{
    WAssert( order == 0 && dimension == 1 && type == W_DT_FLOAT, "The data creator only supports floating point scalar data." );

    WTuringPatternCreator creator( progress );
    creator.setSpotSize( m_spotSize->get() );
    creator.setSpotIrregularity( m_spotIrregularity->get() );
    creator.setNumIterations( m_iterations->get() );

    boost::shared_ptr< std::vector< float > > data = creator.create( grid->getNbCoordsX(), grid->getNbCoordsY(), grid->getNbCoordsZ() );

    return WValueSet< float >::SPtr( new WValueSet< float >( 0, 1, data ) );
}

