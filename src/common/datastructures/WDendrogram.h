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
 * dendron "tree", -gramma "drawing") is a tree diagram frequently used to illustrate the arrangement of the
 * clusters produced by hierarchical clustering.
 *
 * This implementation is based on three arrays (\ref m_objectOrder, \ref m_branching, \ref m_levelHeight) and
 * requires implicitly object lables from <dfn>0..n-1</dfn>. The idea is very similar to the idea described in
 * the paper: <em>F.J. Rohlf, Algorithm 76: Hierarchical clustering using the minimum spanning tree. Comput.
 * J. 16 (1973), pp. 93–95.</em>
 */
class WDendrogram
{
friend class WDendrogramTest;
public:
    typedef boost::shared_ptr< std::vector< size_t > > LabelArray;
    typedef LabelArray LevelArray;
    typedef boost::shared_ptr< std::vector< double > > HeightArray;

//    WDendrogram( LabelArray objectOrder, LevelArray branches, HeightArray heights );
protected:

private:
    /**
     * Since the dendrogram has nonintersecting edges as this:
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

        2        0   3  1     4

      \endverbatim
     *
     * we need to arrage the objects from left to right so merging will not produce intersections. For this
     * ordering this array is used and contains the object labels from left to right and provide thus the
     * special ordering.
     */
    LabelArray            m_objectOrder;

    /**
     * This array stores when the nodes will join or branch. Just imaging we rotate the dendrogram as follows:
     *
      \verbatim

      2  ----.
             |
      0  ----'-----------.
                         |
      3  ----------------'-----.
      1  ---------.            |
                  |            |
      4  ---------'------------'-----

         ----+----+------+-----+--------> levels
            1st  2nd    3rd   4th

     \endverbatim
     * so the array for the example above would be: <dfn>[ 1, 3, 4, 2, - ]</dfn>
     */
    LevelArray            m_branching;

    /**
     * Stores for each join level its height which may be used for spatial layouting.
     */
    std::vector< double > m_levelHeight;
};

#endif  // WDENDROGRAM_H
