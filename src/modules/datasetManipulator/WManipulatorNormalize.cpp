//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2017 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
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

#include "WManipulatorNormalize.h"
#include "core/dataHandler/WDataSetSingle.h"

WManipulatorNormalize::WManipulatorNormalize( boost::shared_ptr< WDataSet >* dataSet )
    : WObjectNDIP< WManipulatorInterface >( "Normalize", "Scales the dataset to form a unit cube." ),
    m_dataSet( dataSet )
{
    m_normalize = m_properties->addProperty( "Scale", "Scale of the data.", WPosition( 1.0, 1.0, 1.0 ) );
}

WManipulatorNormalize::~WManipulatorNormalize()
{
}

WMatrixFixed< double, 4, 4 > WManipulatorNormalize::getTransformationMatrix() const
{
    WMatrixFixed< double, 4, 4 > m = WMatrixFixed< double, 4, 4 >::identity();

    WDataSetSingle::SPtr dsSingle = boost::dynamic_pointer_cast< WDataSetSingle >( *m_dataSet );
    if( !dsSingle )
    {
        return m;
    }

    WBoundingBox bbox;

    // Is this a data set with a regular grid?
    WGridRegular3D::SPtr regGrid;
    regGrid = boost::dynamic_pointer_cast< WGridRegular3D >( dsSingle->getGrid() );
    if( !regGrid )
    {
        return m;
    }
    else
    {
        bbox = regGrid->getBoundingBox();
    }

    for( size_t id = 0; id < 3; ++id )
    {
        double distance = bbox.getMax()[id] - bbox.getMin()[id];
        if( distance > 0.0 )
        {
            m( id, id ) = 1.0 / ( distance );
        }
    }
    m_normalize->set( WPosition( m( 0, 0 ), m( 1, 1 ), m( 2, 2 ) ) );

    return m;
}

bool WManipulatorNormalize::transformationChanged() const
{
    return m_normalize->changed();
}

void WManipulatorNormalize::reset()
{
    m_normalize->set( WPosition( 1.0, 1.0, 1.0 ) );
}
