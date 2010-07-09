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

#ifndef WVALUESET_H
#define WVALUESET_H

#include <cstddef>
#include <vector>

#include "../common/math/WValue.h"
#include "../common/math/WVector3D.h"
#include "../common/WAssert.h"
#include "WDataHandlerEnums.h"
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
     * \param order tensor order of values stored in the value set
     * \param dimension tensor dimension of values stored in the value set
     * \param data the vector holding the raw data
     * \param inDataType indicator teeling us which dataType comes in
     */
    WValueSet( size_t order, size_t dimension, const std::vector< T > data, dataType inDataType )
        : WValueSetBase( order, dimension, inDataType ),
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
            case 0  :  // scalar
                WAssert( m_dimension == 1, "Although order zero, (dimension != 1) was found." );
                return rawSize();
            case 1  :  // vector
                WAssert( rawSize() % m_dimension == 0, "Raw size and dimension don't fit." );
                return rawSize() / m_dimension;
            case 2  :  // matrix
                WAssert( rawSize() % ( m_dimension * m_dimension ) == 0, "Raw size and dimension don't fit." );
                return rawSize() / ( m_dimension * m_dimension );
            default :  // other
                WAssert( false, "Unsupported tensor order." );
                return 0;
        }
    }

    /**
     * \return The number of integral types stored in this set.
     */
    virtual size_t rawSize() const
    {
        return m_data.size();
    }

    /**
     * \param i id of the scalar to retrieve
     * \return The i-th scalar stored in this value set. There are rawSize() such scalars.
     */
    virtual T getScalar( size_t i ) const
    {
        return m_data[i];
    }

    /**
     * \param i id of the scalar to retrieve
     * \return The i-th scalar stored in this value set. There are rawSize() such scalars.
     */
    virtual double getScalarDouble( size_t i ) const
    {
        return static_cast< double >( m_data[i] );
    }

    /**
     * Get the i'th vector
     *
     * \param index the index number of the vector
     *
     * \return the vector
     */
    wmath::WVector3D getVector3D( size_t index ) const;


    /**
     * Get the i'th WValue with the dimension of WValueSet
     *
     * \param index the index number of the WValue
     *
     * \return a WValue with the dimension WValueSet
     */
    wmath::WValue< T > getWValue( size_t index ) const;

    /**
     * Sometimes we need raw access to the data array, for e.g. OpenGL.
     */
    const T * rawData() const
    {
        return &m_data[0];
    }

    /**
     * Sometimes we need raw access to the data vector.
     */
    const std::vector< T >* rawDataVectorPointer() const
    {
        return &m_data;
    }


protected:

private:
    /**
     * Stores the values of type T as simple array which never should be modified.
     */
    const std::vector< T > m_data;  // WARNING: don't remove constness since &m_data[0] won't work anymore!
};

template< typename T > wmath::WVector3D WValueSet< T >::getVector3D( size_t index ) const
{
    WAssert( m_order == 1 && m_dimension == 3, "WValueSet<T>::getVector3D only implemented for order==1, dim==3 value sets" );
    WAssert( ( index + 1 ) * 3 <= m_data.size(), "index in WValueSet<T>::getVector3D too big" );
    size_t offset = index * 3;
    return wmath::WVector3D( m_data[ offset ], m_data[ offset + 1 ], m_data[ offset + 2 ] );
}

template< typename T > wmath::WValue< T > WValueSet< T >::getWValue( size_t index ) const
{
    WAssert( m_order == 1, "WValueSet<T>::getWValue only implemented for order==1 value sets" );
    WAssert( ( index + 1 ) * m_dimension <= m_data.size(), "index in WValueSet<T>::getWValue too big" );

    size_t offset = index * m_dimension;

    wmath::WValue< T > result( m_dimension );

    // copying values
    for ( std::size_t i = 0; i < m_dimension; i++ )
      result[i] = m_data[ offset+i ];

    return result;
}

#endif  // WVALUESET_H
