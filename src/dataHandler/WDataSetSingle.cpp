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

#include "WDataTexture3D.h"
#include "WValueSet.h"
#include "WGrid.h"
#include "../common/WPrototyped.h"

#include "WDataSetSingle.h"

// prototype instance as singleton
boost::shared_ptr< WPrototyped > WDataSetSingle::m_prototype = boost::shared_ptr< WPrototyped >();

WDataSetSingle::WDataSetSingle( boost::shared_ptr< WValueSetBase > newValueSet,
                                boost::shared_ptr< WGrid > newGrid )
    : WDataSet()
{
    assert( newValueSet );
    assert( newGrid );
    assert( newValueSet->size() == newGrid->size() );

    m_valueSet = newValueSet;
    m_grid = newGrid;
    m_texture3D = boost::shared_ptr< WDataTexture3D >( new WDataTexture3D( m_valueSet, m_grid ) );
}

WDataSetSingle::WDataSetSingle()
    : WDataSet(),
    m_valueSet(),
    m_grid(),
    m_texture3D()
{
    // default constructor used by the prototype mechanism
}

WDataSetSingle::~WDataSetSingle()
{
}

boost::shared_ptr< WValueSetBase > WDataSetSingle::getValueSet() const
{
    return m_valueSet;
}

boost::shared_ptr< WGrid > WDataSetSingle::getGrid() const
{
    return m_grid;
}

bool WDataSetSingle::isTexture() const
{
    // TODO(all): this is not sophisticated. This should depend on type of data (vectors? scalars? tensors?)
    return true;
}

boost::shared_ptr< WDataTexture3D > WDataSetSingle::getTexture()
{
    return m_texture3D;
}

const std::string WDataSetSingle::getName() const
{
    return "WDataSetSingle";
}

const std::string WDataSetSingle::getDescription() const
{
    // TODO(all): write this
    return "Please write me.";
}

boost::shared_ptr< WPrototyped > WDataSetSingle::getPrototype()
{
    if ( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSetSingle() );
    }

    return m_prototype;
}

double WDataSetSingle::getValueAt( size_t id )
{
    switch( getValueSet()->getDataType() )
    {
        case W_DT_UNSIGNED_CHAR:
        {
            return static_cast< double >( boost::shared_dynamic_cast< WValueSet< unsigned char > >( getValueSet() )->getScalar( id ) );
        }
        case W_DT_INT16:
        {
            return static_cast< double >( boost::shared_dynamic_cast< WValueSet< int16_t > >( getValueSet() )->getScalar( id ) );
        }
        case W_DT_SIGNED_INT:
        {
            return static_cast< double >( boost::shared_dynamic_cast< WValueSet< int32_t > >( getValueSet() )->getScalar( id ) );
        }
        case W_DT_FLOAT:
        {
            return static_cast< double >( boost::shared_dynamic_cast< WValueSet< float > >( getValueSet() )->getScalar( id ) );
        }
        case W_DT_DOUBLE:
        {
            return static_cast< double >( boost::shared_dynamic_cast< WValueSet< double > >( getValueSet() )->getScalar( id ) );
        }
        default:
            assert( false && "Unknow data type in dataset." );
    }

    return 0.0;
}

double WDataSetSingle::getValueAt( int x, int y, int z )
{
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_grid );
    size_t id = x + y * grid->getNbCoordsX() + z * grid->getNbCoordsX() * grid->getNbCoordsY();

    return getValueAt( id );
}

