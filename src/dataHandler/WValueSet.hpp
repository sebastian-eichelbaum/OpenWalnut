//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#ifndef WVALUESET_H
#define WVALUESET_H

#include <cassert>
#include <cstddef>
#include <vector>

#include "WValueSetBase.h"

/**
 * Base Class for all value set types.
 * \ingroup dataHandler
 */
template< typename T > class WValueSet : public WValueSetBase
{
/**
 * Only UnitTests are allowed to be friends
 */
friend class WValueSetTest;

public:
    /**
     * Constructs a value set with values of type T. Sets order and dimension
     * to allow to interprete the values as tensors of a certain order and dimension.
     */
    WValueSet( char order, char dimension, const std::vector< T > data )
        : WValueSetBase( order, dimension ),
          m_data( data )
    {
    }

    /**
     * \return The number of tensors stored in this set.
     */
    virtual size_t size() const
    {
        switch( m_order )
        {
            case 0  : // scalar
                      assert( m_dimension == 1 && "but m_order was 0" );
                      return rawSize();
            case 1  : // vector
                      assert( rawSize() % m_dimension == 0 );
                      return rawSize() / m_dimension;
            case 2  : // matrix
                      assert( rawSize() % ( m_dimension * m_dimension ) == 0 );
                      return rawSize() / ( m_dimension * m_dimension );
            default : // other
                      assert( 1 == 0 && "Unsupported tensor order" );
                      return 0;
        }
    }

    /**
     * \return The number of integrals stored in this set.
     */
    virtual size_t rawSize() const
    {
        return m_data.size();
    }

    /**
     * Sometimes we need raw access to the data array, for e.g. OpenGL.
     */
    const T * rawData() const
    {
        return &m_data[0];
    }

protected:

private:
    /**
     * Stores the values of type T as simple array which never should be modified.
     */
    const std::vector< T > m_data;  // WARNING: don't remove constness since &m_data[0] won't work anymore!
};

#endif  // WVALUESET_H
