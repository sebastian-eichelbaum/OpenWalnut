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

// PLEASE NOTE THAT THIS IS A COPY FROM THE OSG PROJECT: <osg/MixinVector>
// http://www.openscenegraph.org/projects/osg/attachment/wiki/Legal/LICENSE.txt

#ifndef WMIXINVECTOR_H
#define WMIXINVECTOR_H

#include <algorithm>
#include <iostream>
#include <vector>

#include "WStringUtils.h"

/**
 * This is taken from OpenSceneGraph <osg/MixinVector> but copy and pasted in
 * order to reduce dependency to OSG. It follows the orignal documentation:
 *
 * WMixinVector is a base class that allows inheritance to be used to easily
 * emulate derivation from std::vector but without introducing undefined
 * behaviour through violation of virtual destructor rules.
 *
 * @author Neil Groves
 **/
template< class ValueT > class WMixinVector
{
    /**
     * Handy shortcut for the vector type
     */
    typedef typename std::vector< ValueT > vector_type;

public:
    /**
     * Compares to std::vector type
     */
    typedef typename vector_type::allocator_type allocator_type;

    /**
     * Compares to std::vector type
     */
    typedef typename vector_type::value_type value_type;
    /**
     * Compares to std::vector type
     */
    typedef typename vector_type::const_pointer const_pointer;

    /**
     * Compares to std::vector type
     */
    typedef typename vector_type::pointer pointer;

    /**
     * Compares to std::vector type
     */
    typedef typename vector_type::const_reference const_reference;

    /**
     * Compares to std::vector type
     */
    typedef typename vector_type::reference reference;

    /**
     * Compares to std::vector type
     */
    typedef typename vector_type::const_iterator const_iterator;

    /**
     * Compares to std::vector type
     */
    typedef typename vector_type::iterator iterator;

    /**
     * Compares to std::vector type
     */
    typedef typename vector_type::const_reverse_iterator const_reverse_iterator;

    /**
     * Compares to std::vector type
     */
    typedef typename vector_type::reverse_iterator reverse_iterator;

    /**
     * Compares to std::vector type
     */

    /**
     * Compares to std::vector type
     */
    typedef typename vector_type::size_type size_type;

    /**
     * Compares to std::vector type
     */
    typedef typename vector_type::difference_type difference_type;

    /**
     * Empty standard constructor
     */
    explicit WMixinVector() : _impl()
    {
    }

    /**
     * Constructs a vector of initial_size size where every emlement has its
     * default value or the given value.
     *
     *
     * \param initial_size The initial size
     * \param fill_value The default value for every element
     */
    explicit WMixinVector( size_type initial_size, const value_type& fill_value = value_type() )
        : _impl( initial_size, fill_value )
    {
    }


    /**
     * Constructs a new vector out of an iterator of another vector.
     *
     * \param first Begin of the iterator
     * \param last End of the iterator
     */
    template< class InputIterator > WMixinVector( InputIterator first, InputIterator last )
        : _impl( first, last )
    {
    }

    /**
     * Copy constructor for the appropriate vector type
     *
     * \param other Other std::vector of type vector_type
     */
    explicit WMixinVector( const vector_type& other )
        : _impl( other )
    {
    }

    /**
     * Copy constructor for the WMixinVector itself
     *
     * \param other Other WMixinVector
     */
    WMixinVector( const WMixinVector& other )
        : _impl( other._impl )
    {
    }

    /**
     * Assignment operator for the appropriate vector type
     *
     * \param other Other std::vector
     *
     * \return Reference to the assigned mixin
     */
    WMixinVector& operator=( const vector_type& other )
    {
        _impl = other;
        return *this;
    }

    /**
     * Assigment operator for the WMixinVector itself
     *
     * \param other Other WMixinVector
     *
     * \return Reference to the assigned mixin
     */
    WMixinVector& operator=( const WMixinVector& other )
    {
        _impl = other._impl;
        return *this;
    }

    /**
     * Virtual Destructor
     */
    virtual ~WMixinVector()
    {
    }

    /**
     * Wrapper around std::vector member function.
     */
    void clear()
    {
        _impl.clear();
    }

    /**
     * Wrapper around std::vector member function.
     *
     * \param new_size
     * \param fill_value
     */
    void resize( size_type new_size, const value_type& fill_value = value_type() )
    {
        _impl.resize( new_size, fill_value );
    }

    /**
     * Wrapper around std::vector member function.
     *
     * \param new_capacity How many elements will be in this vector
     */
    void reserve( size_type new_capacity )
    {
        _impl.reserve( new_capacity );
    }

    /**
     * Allow also swap with vectors of an appropriate type
     *
     * \param other To swap with
     */
    void swap( vector_type& other )
    {
        _impl.swap( other );
    }

    /**
     * Wrapper around std::vector member function.
     *
     * \param other
     */
    void swap( WMixinVector& other )
    {
        _impl.swap( other._impl );
    }

    /**
     * Wrapper around std::vector member function.
     *
     * \return True if empty otherwise false.
     */
    bool empty() const
    {
        return _impl.empty();
    }

    /**
     * Wrapper around std::vector member function.
     *
     * \return How many elements this vector has
     */
    size_type size() const
    {
        return _impl.size();
    }

    /**
     * Wrapper around std::vector member function.
     *
     * \return Its capacity
     */
    size_type capacity() const
    {
        return _impl.capacity();
    }

    /**
     * Wrapper around std::vector member function.
     *
     * \return Its maximal size
     */
    size_type max_size() const
    {
        return _impl.max_size();
    }

    /**
     * Returns its allocator
     *
     * \return Its allocator
     */
    allocator_type get_allocator() const
    {
        return _impl.get_allocator();
    }

    /**
     * Wrapper around std::vector member function.
     *
     * \return Const iterator starting a the first element
     */
    const_iterator begin() const
    {
        return _impl.begin();
    }

    /**
     * Wrapper around std::vector member function.
     *
     * \return Iterator starting a the first element
     */
    iterator begin()
    {
        return _impl.begin();
    }

    /**
     * Wrapper around std::vector member function.
     *
     * \return Const iterator starting a the last element
     */
    const_iterator end() const
    {
        return _impl.end();
    }

    /**
     * Wrapper around std::vector member function.
     *
     * \return Iterator starting a the last element
     */
    iterator end()
    {
        return _impl.end();
    }

    /**
     *  Wrapper around std::vector member function.
     *
     * \return Const reverse iterator starting a the last element
     */
    const_reverse_iterator rbegin() const
    {
        return _impl.rbegin();
    }

    /**
     *  Wrapper around std::vector member function.
     *
     * \return Reverse iterator starting a the last element
     */
    reverse_iterator rbegin()
    {
        return _impl.rbegin();
    }

    /**
     *  Wrapper around std::vector member function.
     *
     * \return Const reverse iterator starting a the first element
     */
    const_reverse_iterator rend() const
    {
        return _impl.rend();
    }

    /**
     *  Wrapper around std::vector member function.
     *
     * \return Reverse iterator starting a the first element
     */
    reverse_iterator rend()
    {
        return _impl.rend();
    }

    /**
     *  Wrapper around std::vector member function.
     *
     * \param index Index of the element that is returned
     *
     * \return Const referenece to the index'th element
     */
    const_reference operator[]( size_type index ) const
    {
        return _impl[index];
    }

    /**
     *  Wrapper around std::vector member function.
     *
     * \param index Index of the element that is returned
     *
     * \return Referenece to the index'th element
     */
    reference operator[]( size_type index )
    {
        return _impl[index];
    }

    /**
     *  Wrapper around std::vector member function.
     *
     * \param index Index of the element that is returned
     *
     * \return Const referenece to the index'th element
     */
    const_reference at( size_type index ) const
    {
        return _impl.at( index );
    }

    /**
     *  Wrapper around std::vector member function.
     *
     * \param index Index of the element that is returned
     *
     * \return Referenece to the index'th element
     */
    reference at( size_type index )
    {
        return _impl.at( index );
    }

    /**
     * Wrapper around std::vector member function.
     *
     * \param count
     * \param value
     */
    void assign( size_type count, const value_type& value )
    {
        _impl.assign( count, value );
    }

    /**
     * Wrapper around std::vector member function.
     *
     * \param first
     * \param last
     */
    template< class Iter > void assign( Iter first, Iter last )
    {
        _impl.assign( first, last );
    }

    /**
     * Wrapper around std::vector member function.
     *
     * \param value Value to append
     */
    void push_back( const value_type& value )
    {
        _impl.push_back( value );
    }

    /**
     * Wrapper around std::vector member function.
     */
    void pop_back()
    {
        _impl.pop_back();
    }

    /**
     * Wrapper around std::vector member function.
     *
     * \param where Position where to erase
     *
     * \return Iterator from the erase
     */
    iterator erase( iterator where )
    {
        return _impl.erase( where );
    }

    /**
     * Wrapper around std::vector member function.
     *
     * \param first Start from where to erase
     * \param last End unti to erase
     *
     * \return Iterator from erase
     */
    iterator erase( iterator first, iterator last )
    {
        return _impl.erase( first, last );
    }

    /**
     * Wrapper around std::vector member function.
     *
     * \param where Position where to insert
     * \param value Value of the element to insert
     *
     * \return Iterator from insert
     */
    iterator insert( iterator where, const value_type& value )
    {
        return _impl.insert( where, value );
    }

    /**
     * Wrapper around std::vector member function.
     *
     * \param where Position where to insert
     * \param first Position where to start insert ( First element that should be copied )
     * \param last Position where to end insert ( Last element that should be copied )
     */
    template< class InputIterator > void insert( iterator where, InputIterator first, InputIterator last )
    {
        _impl.insert( where, first, last );
    }

    /**
     * Wrapper around std::vector member function.
     *
     * \param where Position where to insert
     * \param count How many elements to insert
     * \param value Which value is inserted
     */
    void insert( iterator where, size_type count, const value_type& value )
    {
        _impl.insert( where, count, value );
    }

    /**
     *  Wrapper around std::vector member function.
     *
     * \return Const reference to last element
     */
    const_reference back() const
    {
        return _impl.back();
    }

    /**
     *  Wrapper around std::vector member function.
     *
     * \return Reference to last element
     */
    reference back()
    {
        return _impl.back();
    }

    /**
     *  Wrapper around std::vector member function.
     *
     * \return Const reference to first element
     */
    const_reference front() const
    {
        return _impl.front();
    }

    /**
     *  Wrapper around std::vector member function.
     *
     * \return Reference to first element
     */
    reference front()
    {
        return _impl.front();
    }

    /**
     * Return this Mixin as its underlying real vector type.
     * \warning Use with caution!
     *
     * \return Reference to its private vector.
     */
    vector_type& asVector()
    {
        return _impl;
    }

    /**
     * Return this Mixin as its underlying real vector type.
     *
     * \return Const reference to its private vector.
     */
    const vector_type& asVector() const
    {
        return _impl;
    }

    /**
     *  Wrapper around std::vector operator
     *
     * \param left Left hand side
     * \param right Right hand side
     *
     * \return True if and only if std::vector operator is true
     */
    friend inline bool operator==( const WMixinVector< ValueT >& left, const WMixinVector< ValueT >& right )
    {
        return left._impl == right._impl;
    }

    /**
     *  Wrapper around std::vector operator
     *
     * \param left Left hand side
     * \param right Right hand side
     *
     * \return True if and only if std::vector operator is true
     */
    friend inline bool operator==( const WMixinVector< ValueT >& left, const std::vector< ValueT >& right )
    {
        return left._impl == right;
    }

    /**
     *  Wrapper around std::vector operator
     *
     * \param left Left hand side
     * \param right Right hand side
     *
     * \return True if and only if std::vector operator is true
     */
    friend inline bool operator==( const std::vector< ValueT >& left, const WMixinVector< ValueT >& right )
    {
        return left == right._impl;
    }


    /**
     *  Wrapper around std::vector operator
     *
     * \param left Left hand side
     * \param right Right hand side
     *
     * \return True if and only if std::vector operator is true
     */
    friend inline bool operator!=( const WMixinVector< ValueT >& left, const WMixinVector< ValueT >& right )
    {
        return left._impl != right._impl;
    }

    /**
     *  Wrapper around std::vector operator
     *
     * \param left Left hand side
     * \param right Right hand side
     *
     * \return True if and only if std::vector operator is true
     */
    friend inline bool operator!=( const WMixinVector< ValueT >& left, const std::vector< ValueT >& right )
    {
        return left._impl != right;
    }

    /**
     *  Wrapper around std::vector operator
     *
     * \param left Left hand side
     * \param right Right hand side
     *
     * \return True if and only if std::vector operator is true
     */
    friend inline bool operator!=( const std::vector< ValueT >& left, const WMixinVector< ValueT >& right )
    {
        return left != right._impl;
    }


    /**
     *  Wrapper around std::vector operator
     *
     * \param left Left hand side
     * \param right Right hand side
     *
     * \return True if and only if std::vector operator is true
     */
    friend inline bool operator<( const WMixinVector< ValueT >& left, const WMixinVector< ValueT >& right )
    {
        return left._impl < right._impl;
    }

    /**
     *  Wrapper around std::vector operator
     *
     * \param left Left hand side
     * \param right Right hand side
     *
     * \return True if and only if std::vector operator is true
     */
    friend inline bool operator<( const WMixinVector< ValueT >& left, const std::vector< ValueT >& right )
    {
        return left._impl < right;
    }

    /**
     *  Wrapper around std::vector operator
     *
     * \param left Left hand side
     * \param right Right hand side
     *
     * \return True if and only if std::vector operator is true
     */
    friend inline bool operator<( const std::vector< ValueT >& left, const WMixinVector< ValueT >& right )
    {
        return left < right._impl;
    }


    /**
     *  Wrapper around std::vector operator
     *
     * \param left Left hand side
     * \param right Right hand side
     *
     * \return True if and only if std::vector operator is true
     */
    friend inline bool operator >( const WMixinVector< ValueT >& left, const WMixinVector< ValueT >& right )
    {
        return left._impl > right._impl;
    }

    /**
     *  Wrapper around std::vector operator
     *
     * \param left Left hand side
     * \param right Right hand side
     *
     * \return True if and only if std::vector operator is true
     */
    friend inline bool operator >( const WMixinVector< ValueT >& left, const std::vector< ValueT >& right )
    {
        return left._impl > right;
    }

    /**
     *  Wrapper around std::vector operator
     *
     * \param left Left hand side
     * \param right Right hand side
     *
     * \return True if and only if std::vector operator is true
     */
    friend inline bool operator >( const std::vector< ValueT >& left, const WMixinVector< ValueT >& right )
    {
        return left > right._impl;
    }


    /**
     *  Wrapper around std::vector operator
     *
     * \param left Left hand side
     * \param right Right hand side
     *
     * \return True if and only if std::vector operator is true
     */
    friend inline bool operator<=( const WMixinVector< ValueT >& left, const WMixinVector< ValueT >& right )
    {
        return left._impl <= right._impl;
    }

    /**
     *  Wrapper around std::vector operator
     *
     * \param left Left hand side
     * \param right Right hand side
     *
     * \return True if and only if std::vector operator is true
     */
    friend inline bool operator<=( const WMixinVector< ValueT >& left, const std::vector< ValueT >& right )
    {
        return left._impl <= right;
    }

    /**
     *  Wrapper around std::vector operator
     *
     * \param left Left hand side
     * \param right Right hand side
     *
     * \return True if and only if std::vector operator is true
     */
    friend inline bool operator<=( const std::vector< ValueT >& left, const WMixinVector< ValueT >& right )
    {
        return left <= right._impl;
    }


    /**
     *  Wrapper around std::vector operator
     *
     * \param left Left hand side
     * \param right Right hand side
     *
     * \return True if and only if std::vector operator is true
     */
    friend inline bool operator>=( const WMixinVector< ValueT >& left, const WMixinVector< ValueT >& right )
    {
        return left._impl >= right._impl;
    }

    /**
     *  Wrapper around std::vector operator
     *
     * \param left Left hand side
     * \param right Right hand side
     *
     * \return True if and only if std::vector operator is true
     */
    friend inline bool operator>=( const WMixinVector< ValueT >& left, const std::vector< ValueT >& right )
    {
        return left._impl >= right;
    }

    /**
     *  Wrapper around std::vector operator
     *
     * \param left Left hand side
     * \param right Right hand side
     *
     * \return True if and only if std::vector operator is true
     */
    friend inline bool operator>=( const std::vector< ValueT >& left, const WMixinVector< ValueT >& right )
    {
        return left >= right._impl;
    }

private:
    /**
     * Encapsulated internal vector from which derivation is simulated.
     */
    vector_type _impl;
};

/**
 * Standard non member wrapper function to swap
 *
 * \param left Left hand side
 * \param right Right hand side
 */
template< class ValueT > inline void swap( WMixinVector< ValueT >& left, WMixinVector< ValueT >& right )
{
    std::swap( left.asVector(), right.asVector() );
}

/**
 * Standard non member wrapper function to swap
 *
 * \param left Left hand side
 * \param right Right hand side
 */
template< class ValueT > inline void swap( WMixinVector< ValueT >& left, std::vector< ValueT >& right )
{
    std::swap( left.asVector(), right );
}

/**
 * Standard non member wrapper function to swap
 *
 * \param left Left hand side
 * \param right Right hand side
 */
template< class ValueT > inline void swap( std::vector< ValueT >& left, WMixinVector< ValueT >& right )
{
    std::swap( left, right.asVector() );
}

/**
 * Writes every mixin vector to an output stream such as cout, if its
 * elements have an output operator defined.
 *
 * \param os The output stream where the elements are written to
 * \param v MixinVector containing the elements
 * \return The output stream again.
 */
template< class ValueT > inline std::ostream& operator<<( std::ostream& os, const WMixinVector< ValueT >& v )
{
    using string_utils::operator<<;
    os << v.asVector();
    return os;
}

#endif  // WMIXINVECTOR_H
