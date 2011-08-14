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

#ifndef WKDTREE_H
#define WKDTREE_H

#include <algorithm>
#include <vector>

#include "../common/WThreadedRunner.h"

/**
 * implements the compare function for std::nth_element on a point array
 */
struct lessy
{
    float const * const data; //!< stores the pointer to the data array
    const int pos; //!< stores the axis at which the array is sorted

    /**
     * constructor
     * \param data pointer to the array
     * \param pos x,y or z axis
     */
    lessy( float const * const data, const int pos ) :
        data( data ), pos( pos )
    {
    }

    /**
     * comparison operator less
     * \param lhs
     * \param rhs
     * \return is lhs smaller than rhs
     */
    bool operator()( const unsigned int& lhs, const unsigned int& rhs ) const //NOLINT
    {
        return data[3 * lhs + pos] < data[3 * rhs + pos];
    }
};

/**
 * class to provide threaded computation of parts of the kd tree
 */
class WKdTreeThread: public WThreadedRunner
{
public:
    /**
     * constructor
     *
     * \param pointArray
     * \param tree pointer to the tree
     * \param left boundary of the part to compute
     * \param right boundary of the part to compute
     * \param axis starting axis
     */
    WKdTreeThread( float* pointArray, std::vector< unsigned int >* tree, int left, int right, int axis );

    /**
     *  recursive function to compute a part of the kd tree
     *
     *  \param left
     *  \param right
     *  \param axis
     */
    void buildTree( int left, int right, int axis );

    /**
     * entry for the run command
     */
    virtual void threadMain();

    std::vector< unsigned int >* m_tree; //!< stores a pointer to the tree
    float *m_pointArray; //!< stores a pointer to the vertex array

    int m_left; //!< stores left boundary, since the threadMain method has no arguments
    int m_right; //!< stores left boundary, since the threadMain method has no arguments
    int m_axis; //!< stores axis, since the threadMain method has no arguments
};

/**
 * implements the computation of a kd tree on a point array
 */
class WKdTree
{
public:
    /**
     * constructor
     *
     * \param size
     * \param pointArray
     */
    WKdTree( int size, float* pointArray );

    /**
     * destructor
     */
    ~WKdTree();

    std::vector< unsigned int > m_tree; //!< stores the tree

private:
    /**
     *  recursive function to compute a part of the kd tree
     *
     *  \param left
     *  \param right
     *  \param axis
     */
    void buildTree( int left, int right, int axis );
    int m_size; //!< size of the tree
    unsigned int m_root; //!< index of the root point
    float *m_pointArray; //!< stores a pointer to the vertex array
};

#endif  // WKDTREE_H
