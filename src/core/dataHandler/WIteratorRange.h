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

#ifndef WITERATORRANGE_H_
#define WITERATORRANGE_H_

#include <utility>

/**
 * \class WIteratorRange
 *
 * \brief A range of iterators.
 *
 * Stores a begin and an end iterator to define a range for iteration.
 *
 * \tparam IteratorType The type of the iterators to store.
 */
template< typename IteratorType >
class WIteratorRange
{
public:
    /**
     * Construct a range from a begin and end iterator.
     *
     * \param begin The iterator to act as the begin iterator.
     * \param end The iterator to act as the end iterator.
     */
    WIteratorRange( IteratorType const& begin, IteratorType const& end );

    /**
     * Destructor.
     */
    ~WIteratorRange();

    /**
     * Copy constructor.
     *
     * \param range The range to copy from.
     */
    WIteratorRange( WIteratorRange const& range ); // NOLINT will be used implicitly

    /**
     * Assignment.
     *
     * \param range The range to assign from.
     * \return *this
     */
    WIteratorRange& operator=( WIteratorRange const& range );

    /**
     * Returns the begin of the range.
     *
     * \return The begin iterator.
     */
    IteratorType const& begin() const;

    /**
     * Returns the end of the range.
     *
     * \return The end iterator.
     */
    IteratorType const& end() const;

private:
    /**
     * The standard constructor is not allowed.
     */
    WIteratorRange();

    //! The begin of the range.
    IteratorType m_begin;

    //! The end of the range.
    IteratorType m_end;
};

template< typename IteratorType >
WIteratorRange< IteratorType >::WIteratorRange( IteratorType const& begin, IteratorType const& end )
    : m_begin( begin ),
      m_end( end )
{
}

template< typename IteratorType >
WIteratorRange< IteratorType >::~WIteratorRange()
{
}

template< typename IteratorType >
WIteratorRange< IteratorType >::WIteratorRange( WIteratorRange const& range )
    : m_begin( range.m_begin ),
      m_end( range.m_end )
{
}

template< typename IteratorType >
WIteratorRange< IteratorType >& WIteratorRange< IteratorType >::operator=( WIteratorRange const& range )
{
    if( this == &range )
        return *this;

    m_begin = range.m_begin;
    m_end = range.m_end;

    return *this;
}

template< typename IteratorType >
IteratorType const& WIteratorRange< IteratorType >::begin() const
{
    return m_begin;
}

template< typename IteratorType >
IteratorType const& WIteratorRange< IteratorType >::end() const
{
    return m_end;
}

/**
 * \class WIteratorRangeUnpacker
 *
 * \brief A temporary used to unpack an iterator range into two iterators.
 *
 * \tparam IteratorType THe type of the iterators used.
 */
template< typename IteratorType >
class WIteratorRangeUnpacker
{
public:
    /**
     * Constructs the unpacker.
     *
     * \param begin A reference to the iterator to unpack the begin iterator into.
     * \param end A reference to the iterator to unpack the end iterator into.
     */
    WIteratorRangeUnpacker( IteratorType& begin, IteratorType& end ); // NOLINT non-const intended

    /**
     * Destructor.
     */
    ~WIteratorRangeUnpacker();

    /**
     * Unpacks the provided range into the iterators referenced by this object.
     * The lack of a return type is intended.
     *
     * \param range The renge to unpack.
     */
    void operator=( WIteratorRange< IteratorType > const& range );

    /**
     * Copy constructor.
     */
    WIteratorRangeUnpacker( WIteratorRangeUnpacker const& ); // NOLINT used implicitly

private:
    /**
     * Assignment is not allowed.
     *
     * \return Not used.
     */
    WIteratorRangeUnpacker& operator=( WIteratorRangeUnpacker const& );

    //! A reference to the iterator that will be assigned the beginning of the range.
    IteratorType& m_begin;

    //! A reference to the iterator that will be assigned the end of the range.
    IteratorType& m_end;
};

template< typename IteratorType >
WIteratorRangeUnpacker< IteratorType >::WIteratorRangeUnpacker( IteratorType& begin, IteratorType& end ) // NOLINT non-const intended
    : m_begin( begin ),
      m_end( end )
{
}

template< typename IteratorType >
WIteratorRangeUnpacker< IteratorType >::~WIteratorRangeUnpacker()
{
}

template< typename IteratorType >
WIteratorRangeUnpacker< IteratorType >::WIteratorRangeUnpacker( WIteratorRangeUnpacker const& other )
    : m_begin( other.m_begin ),
      m_end( other.m_end )
{
}

template< typename IteratorType >
void WIteratorRangeUnpacker< IteratorType >::operator=( WIteratorRange< IteratorType > const& range )
{
    m_begin = range.begin();
    m_end = range.end();
}

#endif  // WITERATORRANGE_H_

