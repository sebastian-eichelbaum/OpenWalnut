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

#include "../../common/WThreadedRunner.h"

struct lessy
{
    float const * const data;
    const int pos;
    lessy( float const * const data, const int pos ) :
        data( data ), pos( pos )
    {
    }

    bool operator()( const unsigned int& a, const unsigned int& b ) const
    {
        return data[3* a + pos] < data[3* b + pos];
    }
};

class WKdTreeThread : public WThreadedRunner
{
public:
    WKdTreeThread( float*, std::vector< unsigned int >*, int, int, int );

    void buildTree( int, int, int );
    virtual void threadMain();

    std::vector< unsigned int >* m_tree;
    float *m_pointArray;

    int m_left;
    int m_right;
    int m_axis;
};


/**
 * TODO(schurade): Document this!
 */
class WKdTree
{
public:
        WKdTree(int size, float* pointArray, bool );
        WKdTree(int size, float* pointArray );
        ~WKdTree();

        std::vector<unsigned int>m_tree;

private:
        void buildTree(int, int, int);
        int m_size;
        unsigned int m_root;
        float *m_pointArray;
};

#endif  // WKDTREE_H
