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

#ifndef WDENDROGRAM_H
#define WDENDROGRAM_H

#include <vector>

#include <boost/shared_ptr.hpp>

/**
 * Hirachical binary tree datastructure with spatial layout information called dendrogram.
 *
 * The following description is taken from: http://en.wikipedia.org/wiki/Dendrogram A dendrogram (from Greek
 * dendron "tree", -gramma "drawing") is a tree diagram frequently used to illustrate the arrangement of
 * clusters produced by hierarchical clustering. Please note that each level has its height.
 *
   \verbatim
                     |
              ,------'--.     --- 4th level
              |         |
          |```````|     |     --- 3rd level
          |       |     |
          |       |  ...'...  --- 2nd level
          |       |  |     |
     |''''''''|   |  |     |  --- 1st level
     |        |   |  |     |
     |        |   |  |     |
     o        o   o  o     o  --- 0   level
   \endverbatim
 *
 * In order to use this class for your objects ensure that the objects are labeled from <dfn>0,...,n-1</dfn>.
 */
class WDendrogram
{
friend class WDendrogramTest;
public:
    /**
     * Constructs a new dendrogram for \ref size many objects.
     *
     * \param size The number of leafs.
     */
    explicit WDendrogram( size_t n );

    /**
     * Merges two elements (either inner nodes or leafs) given via the indices \e i and \e j.
     *
     * \param i The index referring either to an inner node or to a leaf.
     * \param j The other index of a leaf or inner node.
     * \param height The height at which those to elements join.
     *
     * \return The number of the inner node now representing now the parent of \e i and \e j.
     */
    size_t merge( size_t i, size_t j, double height );

protected:
private:
    /**
     * Stores the parents of leafs as well as of inner nodes. The first half of the arrary corresponds to the
     * parents of the leafs and the second of the inner nodes. The last inner node is the top of the
     * dendrogram.
     */
    std::vector< size_t > m_parents;

    /**
     * Stores only for the inner nodes their heights.
     */
    std::vector< double > m_heights;
};

#endif  // WDENDROGRAM_H
