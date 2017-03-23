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

#ifndef WUNIONFIND_H
#define WUNIONFIND_H

#include <set>
#include <vector>

#include <boost/shared_ptr.hpp>


/**
 * Implements a very simple union-find datastructure aka disjoint_sets.
 * \note I know there is a boost solution on that, but I didn't get it to work and I don't know how fast it is:
 * http://www.boost.org/doc/libs/1_42_0/libs/disjoint_sets/disjoint_sets.html
 *
 * And you may use it like this:
   \verbatim
   typedef std::vector< SizeType >          RankVector;
   typedef RankVector::iterator             RankRAIter;
   typedef std::vector< VertexDescriptor >  ParentVector;
   typedef ParentVector::iterator           ParentRAIter;

   typedef boost::iterator_property_map< RankRAIter,
                                         IndexMap,
                                         std::iterator_traits< RankRAIter >::value_type,
                                         std::iterator_traits< RankRAIter >::reference > RankMap;

   typedef boost::iterator_property_map< ParentRAIter,
                                         IndexMap,
                                         std::iterator_traits< ParentRAIter >::value_type,
                                         std::iterator_traits< ParentRAIter >::reference > ParentMap;

   RankVector   ranks( d_numOfVertices );
   ParentVector parents( d_numOfVertices );
   boost::disjoint_sets< RankMap, ParentMap > dset( boost::make_iterator_property_map( ranks.begin(),
                                                                                       boost::get( boost::vertex_index, g ),
                                                                                       ranks[0] ),
                                                    boost::make_iterator_property_map( parents.begin(),
                                                                                       boost::get( boost::vertex_index, g ),
                                                                                       parents[0] )
                                                  );

   // After the disjoint set dset is construct, we can use

   dset.make_set( u ); // make u a set
   dset.link( u, v );  // u and v are belonging to the same set.
   dset.find_set( u ); // find the set owning u. A representative of the set is returned
   \endverbatim
 */
class WUnionFind
{
friend class WUnionFindTest; //!< Access for test class.
public:
    /**
     * Creates a new union find datastructure with size elements where each
     * element is initialized as a single set.
     *
     * \param size Number of elements
     */
    explicit WUnionFind( size_t size );

    /**
     * Find the canonical element of the given element and do path compression
     *
     * http://en.wikipedia.org/wiki/Disjoint-set_data_structure
     *
     * depth of recursion is said to be small, therefore, recursion
     * works fine for large dataset, too.
     *
     * \throw WOutOfBounds If x is bigger than the number of elements available
     *
     * \param x The x'th element
     * \return The canonical element of that component which x is also member of
     */
    size_t find( size_t x );

    /**
     * Computes the set with maximum number of elements. If there are more than one candidate one is
     * picked arbitrarily.
     *
     * \return Reference to the set of indices all beloning to a set of maximal size.
     */
    boost::shared_ptr< std::set< size_t > > getMaxSet();

    /**
     * Merges two components (iow: makes a union) where the given elements are
     * members of.
     *
     * \throw WOutOfBounds If i or j are bigger than the number of elements available
     *
     * \param i Element of some component
     * \param j Element of some (maybe other) component
     */
    void merge( size_t i, size_t j );

private:
    std::vector< size_t > m_component; //!< Stores for each index its ID
};


#endif  // WUNIONFIND_H
