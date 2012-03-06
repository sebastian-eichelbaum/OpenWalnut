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

#include <sstream>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "../WTransferable.h"



/**
 * Hirachical binary tree datastructure with spatial layout information called dendrogram. Please note that there are some
 * limitations of this implementation: First of all there are exactly <dfn>n-1</dfn> many inner nodes, and only inner nodes may
 * have a height. In order to use this class for your objects ensure that the objects are labeled from <dfn>0,...,n-1</dfn>.
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
 */
class WDendrogram : public WTransferable // NOLINT
{
friend class WDendrogramTest;
public:
    /**
     * Gets the name of this prototype.
     *
     * \return the name.
     */
    virtual const std::string getName() const;

    /**
     * Gets the description for this prototype.
     *
     * \return the description
     */
    virtual const std::string getDescription() const;

    /**
     * Returns a prototype instantiated with the true type of the deriving class.
     *
     * \return the prototype.
     */
    static boost::shared_ptr< WPrototyped > getPrototype();


    /**
     * Constructs a new dendrogram for \c n many objects.
     *
     * \param n The number of leafs.
     */
    explicit WDendrogram( size_t n );

    /**
     * Default constructs an empty dendrogram.
     */
    WDendrogram();

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

    /**
     * Transform this dendrogram into a string, where each leaf or inner node is mapped to a special string.
     * <dfn>"(level, (all leafs incorporated by this node), (the two direct predecessors), height if available )"</dfn>
     *
     * \return The special string as constructed from the scheme above.
     */
    std::string toString() const;

protected:
    static boost::shared_ptr< WPrototyped > m_prototype; //!< The prototype as singleton.

private:
    /**
     * Resets the whole dendrogram to the number of elements it should be used for.
     *
     * \param n number of leafs
     */
    void reset( size_t n );

    /**
     * Checks if this instance is initialized. If not, it throws an exception.
     * \throw WOutOfBounds
     * \param caller A string identifying the class member function.
     */
    void checkAndThrowExceptionIfUsedUninitialized( std::string caller ) const;

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

inline const std::string WDendrogram::getName() const
{
    return "WDendrogram";
}

inline const std::string WDendrogram::getDescription() const
{
    return "A Dendrogram as a array with additional heights for each inner node.";
}


#endif  // WDENDROGRAM_H
