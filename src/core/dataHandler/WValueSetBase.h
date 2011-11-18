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

#ifndef WVALUESETBASE_H
#define WVALUESETBASE_H

#include <cstddef>
#include <cmath>

#include <boost/variant.hpp>

#include "../common/math/WValue.h"
#include "WDataHandlerEnums.h"
#include "WExportDataHandler.h"

//! forward declaration
template< typename T >
class WValueSet;

//! declare a boost::variant of all possible valuesets
typedef boost::variant< WValueSet< uint8_t > const*,
                        WValueSet< int8_t > const*,
                        WValueSet< uint16_t > const*,
                        WValueSet< int16_t > const*,
                        WValueSet< uint32_t > const*,
                        WValueSet< int32_t > const*,
                        WValueSet< uint64_t > const*,
                        WValueSet< int64_t > const*,
                        WValueSet< float > const*,
                        WValueSet< double > const*,
                        WValueSet< long double > const* > WValueSetVariant;

/**
 * Abstract base class to all WValueSets. This class doesn't provide any genericness.
 * \ingroup dataHandler
 */
class OWDATAHANDLER_EXPORT WValueSetBase // NOLINT
{
public:
    /**
     * Despite this is an abstract class all subclasses should have an order
     * and dimension.
     * \param order the tensor order of the values stored in this WValueSet
     * \param dimension the tensor dimension of the values stored in this WValueSet
     * \param inDataType indication of the primitive data type used to store the values
     */
    WValueSetBase( size_t order, size_t dimension, dataType inDataType );

    /**
     * Dummy since each class with virtual member functions needs one.
     */
    virtual ~WValueSetBase() = 0;

    /**
     * \return The number of tensors in this ValueSet.
     */
    virtual size_t size() const = 0;

    /**
     * \return The number of integrals (POD like int, double) in this ValueSet.
     */
    virtual size_t rawSize() const = 0;

    /**
     * \param i id of the scalar to retrieve
     * \return The i-th scalar stored in this value set. There are rawSize() such scalars.
     */
    virtual double getScalarDouble( size_t i ) const = 0;

    /**
     * \param i id of the WValue to retrieve
     * \return The i-th WValue stored in this value set. There are size() such scalars.
     */
    virtual WValue< double > getWValueDouble( size_t i ) const = 0;

    /**
     * \param i id of the WVector to retrieve
     * \return The i-th WValue (stored in this value set) as WVector. There are size() such scalars.
     */
    virtual WVector_2 getWVector( size_t i ) const = 0;

    /**
     * \return Dimension of the values in this ValueSet
     */
    virtual size_t dimension() const
    {
        return m_dimension;
    }

    /**
     * \return Order of the values in this ValueSet
     */
    virtual size_t order() const
    {
        return m_order;
    }

    /**
     * Returns the number of elements of type T per value.
     * \note this is dimension to the power of order.
     * \return number of elements per value
     */
    virtual size_t elementsPerValue() const
    {
        // Windows Hack: the MSVC obviously does not support ( oh wonder, which wonder ) pow with integers.
        return static_cast< size_t >( std::pow( static_cast< double >( m_dimension ), static_cast< int >( m_order ) ) );
    }

    /**
     * \return Dimension of the values in this ValueSet
     */
    virtual dataType getDataType() const
    {
        return m_dataType;
    }

    /**
     * This method returns the smallest value in the valueset. It does not handle vectors, matrices and so on well. It simply returns the
     * smallest value in the data array. This is especially useful for texture scaling or other statistic tools (histograms).
     *
     * \return the smallest value in the data.
     */
    virtual double getMinimumValue() const = 0;

    /**
     * This method returns the largest value in the valueset. It does not handle vectors, matrices and so on well. It simply returns the
     * largest value in the data array. This is especially useful for texture scaling or other statistic tools (histograms).
     *
     * \return the largest value in the data.
     */
    virtual double getMaximumValue() const = 0;

    /**
     * Apply a function object to this valueset.
     *
     * \tparam Func_T The type of the function object, should be derived from the boost::static_visitor template.
     *
     * \param func The function object to apply.
     * \return The result of the operation.
     */
    template< typename Func_T >
    typename Func_T::result_type applyFunction( Func_T const& func )
    {
        return boost::apply_visitor( func, getVariant() );
    }

protected:
    /**
     * The order of the tensors for this ValueSet
     */
    const size_t m_order;

    /**
     * The dimension of the tensors for this ValueSet
     */
    const size_t m_dimension;

    /**
     * The data type of the values' elements.
     */
    const dataType m_dataType;

private:
    /**
     * Creates a boost::variant reference.
     *
     * \return var A pointer to a variant reference to the valueset.
     */
    virtual WValueSetVariant const getVariant() const
    {
        return WValueSetVariant();
    }
};

#endif  // WVALUESETBASE_H
