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
#include <boost/shared_ptr.hpp>

#include "../common/math/WValue.h"
#include "../common/math/linearAlgebra/WLinearAlgebra.h"
#include "../common/WAssert.h"
#include "../common/WLimits.h"
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
     * The type of the single value in this value set.
     */
    typedef T ValueT;

    /**
     * \class SubArray
     *
     * A helper class granting safe access to a certain part of the valueset.
     */
    class SubArray
    {
    public:
        //! make the valueset a friend
        friend class WValueSet;

        /**
         * Destructor.
         */
        ~SubArray()
        {
        }

        /**
         * Safe access. Only the const version is allowed.
         *
         * \param i The relative position of the element in the subarray's range.
         *
         * \note If i is not in ( 0, size - 1 ), the first element will be returned.
         *
         * \return the value
         */
        T const& operator[] ( std::size_t i ) const
        {
            return *( m_ptr + i * static_cast< std::size_t >( i < m_size ) );
        }

        // use the standard copy constructor and operator
    private:
        /**
         * Construct an object that allows safe access.
         * (no access to elements not in the subarray's range).
         * Only a valueset may construct a SubArray.
         *
         * \param p A pointer to the first element.
         * \param size The size of the subarray.
         */
        SubArray( T const* const p, std::size_t size )
            : m_ptr( p ),
              m_size( size )
        {
        }

        //! the pointer to the first element
        T const* const m_ptr;

        //! the size of the subarray
        std::size_t const m_size;
    };

    /**
     * Constructs a value set with values of type T. Sets order and dimension
     * to allow to interpret the values as tensors of a certain order and dimension.
     * \param order tensor order of values stored in the value set
     * \param dimension tensor dimension of values stored in the value set
     * \param data the vector holding the raw data
     * \param inDataType indicator telling us which dataType comes in
     */
    WValueSet( size_t order, size_t dimension, const boost::shared_ptr< std::vector< T > > data, dataType inDataType )
        : WValueSetBase( order, dimension, inDataType ),
          m_data( data )
    {
        // calculate min and max
        // Calculating this once simply ensures that it does not need to be recalculated in textures, histograms ...
        m_minimum = wlimits::MAX_DOUBLE;
        m_maximum = wlimits::MIN_DOUBLE;
        for( typename std::vector< T >::const_iterator iter = data->begin(); iter != data->end(); ++iter )
        {
            m_minimum = m_minimum > *iter ? *iter : m_minimum;
            m_maximum = m_maximum < *iter ? *iter : m_maximum;
        }
    }

    /**
     * Constructs a value set with values of type T. Sets order and dimension
     * to allow to interpret the values as tensors of a certain order and dimension.
     * \param order tensor order of values stored in the value set
     * \param dimension tensor dimension of values stored in the value set
     * \param data the vector holding the raw data
     */
    WValueSet( size_t order, size_t dimension, const boost::shared_ptr< std::vector< T > > data )
        : WValueSetBase( order, dimension, DataType< T >::type ),
          m_data( data )
    {
        // calculate min and max
        // Calculating this once simply ensures that it does not need to be recalculated in textures, histograms ...
        m_minimum = wlimits::MAX_DOUBLE;
        m_maximum = wlimits::MIN_DOUBLE;
        for( typename std::vector< T >::const_iterator iter = data->begin(); iter != data->end(); ++iter )
        {
            m_minimum = m_minimum > *iter ? *iter : m_minimum;
            m_maximum = m_maximum < *iter ? *iter : m_maximum;
        }
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
        return (*m_data.get()).size();
    }

    /**
     * \param i id of the scalar to retrieve
     * \return The i-th scalar stored in this value set. There are rawSize() such scalars.
     */
    virtual T getScalar( size_t i ) const
    {
        return (*m_data.get())[i];
    }

    /**
     * \param i id of the scalar to retrieve
     * \return The i-th scalar stored in this value set. There are rawSize() such scalars.
     */
    virtual double getScalarDouble( size_t i ) const
    {
        return static_cast< double >( (*m_data.get())[i] );
    }

    /**
     * \param i id of the WValue to retrieve
     * \return The i-th WValue stored in this value set. There are size() such scalars.
     */
    virtual WValue< double > getWValueDouble( size_t i ) const
    {
        return WValue< double >( getWValue( i ) );
    }

    /**
     * \param i id of the WVector to retrieve
     * \return The i-th WValue (stored in this value set) as WVector. There are size() such scalars.
     */
    virtual WVector_2 getWVector( size_t i ) const
    {
        return ( WValue< double >( getWValue( i ) ) ).toWVector();
    }

    /**
     * Get the i'th vector
     *
     * \param index the index number of the vector
     *
     * \return the vector
     */
    WVector3d getVector3D( size_t index ) const;


    /**
     * Get the i'th WValue with the dimension of WValueSet
     *
     * \param index the index number of the WValue
     *
     * \return a WValue with the dimension WValueSet
     */
    WValue< T > getWValue( size_t index ) const;

    /**
     * Sometimes we need raw access to the data array, for e.g. OpenGL.
     *
     * \return the raw data pointer
     */
    const T * rawData() const
    {
        return &(*m_data.get())[0];
    }

    /**
     * Sometimes we need raw access to the data vector.
     *
     * \return the data vector
     */
    const std::vector< T >* rawDataVectorPointer() const
    {
        return &(*m_data.get());
    }

    /**
     * Request (read-) access object to a subarray of this valueset.
     * The object returned by this function can be used as an array
     * ( starting at index 0 ), whose elements are the data elements
     * at positions start to ( including ) start + size - 1 of the valueset.
     *
     * \param start The position of the first element of the subarray.
     * \param size The number of elements in the subarray.
     * \return The subarray.
     */
    SubArray const getSubArray( std::size_t start, std::size_t size ) const
    {
        WAssert( start + size <= rawSize(), "" );
        WAssert( size != 0, "" );
        return SubArray( rawData() + start, size );
    }

    /**
     * This method returns the smallest value in the valueset. It does not handle vectors, matrices and so on well. It simply returns the
     * smallest value in the data array. This is especially useful for texture scaling or other statistic tools (histograms).
     *
     * \return the smallest value in the data.
     */
    virtual double getMinimumValue() const
    {
        return m_minimum;
    }

    /**
     * This method returns the largest value in the valueset. It does not handle vectors, matrices and so on well. It simply returns the
     * largest value in the data array. This is especially useful for texture scaling or other statistic tools (histograms).
     *
     * \return the largest value in the data.
     */
    virtual double getMaximumValue() const
    {
        return m_maximum;
    }

protected:
    /**
     * The smallest value in m_data.
     */
    T m_minimum;

    /**
     * The largest value in m_data.
     */
    T m_maximum;

private:
    /**
     * Stores the values of type T as simple array which never should be modified.
     */
    const boost::shared_ptr< std::vector< T > > m_data;  // WARNING: don't remove constness since &m_data[0] won't work anymore!

    /**
     * Get a variant reference to this valueset (the reference is stored in the variant).
     * \note Use this as a temporary object inside a function or something like that.
     * \return var A variant reference.
     */
    virtual WValueSetVariant const getVariant() const
    {
        return WValueSetVariant( this );
    }
};

template< typename T > WVector3d WValueSet< T >::getVector3D( size_t index ) const
{
    WAssert( m_order == 1 && m_dimension == 3, "WValueSet<T>::getVector3D only implemented for order==1, dim==3 value sets" );
    WAssert( ( index + 1 ) * 3 <= m_data->size(), "index in WValueSet<T>::getVector3D too big" );
    size_t offset = index * 3;
    return WVector3d( ( *m_data )[offset], ( *m_data )[offset + 1], ( *m_data )[offset + 2] );
}

template< typename T > WValue< T > WValueSet< T >::getWValue( size_t index ) const
{
    WAssert( m_order == 1, "WValueSet<T>::getWValue only implemented for order==1 value sets" );
    WAssert( ( index + 1 ) * m_dimension <= m_data->size(), "index in WValueSet<T>::getWValue too big" );

    size_t offset = index * m_dimension;

    WValue< T > result( m_dimension );

    // copying values
    for( std::size_t i = 0; i < m_dimension; i++ )
        result[i] = ( *m_data )[offset+i];

    return result;
}

#endif  // WVALUESET_H
