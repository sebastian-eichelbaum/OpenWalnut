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

#include <algorithm>
#include <vector>

#include "../common/WAssert.h"
#include "../common/WLogger.h"

#include "WKdTree.h"

WKdTree::WKdTree( int size, float *pointArray ) :
    m_size( size ), m_pointArray( pointArray )
{
    // WAssert( m_size > 2, "The current kD-tree implementation works only with at least 3 vertices." );
    m_tree.clear();
    m_tree.resize( m_size );

    wlog::debug( "KdTree" ) << " Start building KdTree";

    for( int i = 0; i < m_size; ++i )
        m_tree[i] = i;

    int root = ( m_size - 1 ) / 2;

    if( m_size == 0 )
        m_root = 0;

    if( m_size > 0 )
    {
        m_root = root;
        std::nth_element( m_tree.begin(), m_tree.begin() + root, m_tree.end(), lessy( m_pointArray, 0 ) );
    }

    int rootLeft = ( root - 1 ) / 2;
    if( m_size > 2 )
    {
        std::nth_element( m_tree.begin(), m_tree.begin() + rootLeft, m_tree.begin() + root - 1, lessy( m_pointArray, 1 ) );
    }

    int rootRight = ( m_size + root ) / 2;
    if( m_size > 1 )
    {
        std::nth_element( m_tree.begin() + root + 1, m_tree.begin() + rootRight, m_tree.end(), lessy( m_pointArray, 1 ) );
    }

    if( m_size > 2 )
    {
        WKdTreeThread *thread1 = new WKdTreeThread( m_pointArray, &m_tree, 0, rootLeft - 1, 2 );
        WKdTreeThread *thread2 = new WKdTreeThread( m_pointArray, &m_tree, rootLeft + 1, root - 1, 2 );
        WKdTreeThread *thread3 = new WKdTreeThread( m_pointArray, &m_tree, root + 1, rootRight - 1, 2 );
        WKdTreeThread *thread4 = new WKdTreeThread( m_pointArray, &m_tree, rootRight + 1, m_size - 1, 2 );

        wlog::debug( "KdTree" ) << "Start threads";

        thread1->run();
        thread2->run();
        thread3->run();
        thread4->run();

        wlog::debug( "KdTree" ) << "All threads started";

        thread1->wait();
        thread2->wait();
        thread3->wait();
        thread4->wait();

        wlog::debug( "KdTree" ) << "All threads finished";

        delete thread1;
        delete thread2;
        delete thread3;
        delete thread4;
    }
}

WKdTree::~WKdTree()
{
}

void WKdTree::buildTree( int left, int right, int axis )
{
    if( left >= right )
        return;

    int div = ( left + right ) / 2;
    std::nth_element( m_tree.begin() + left, m_tree.begin() + div, m_tree.begin() + right, lessy( m_pointArray, axis ) );

    buildTree( left, div - 1, ( axis + 1 ) % 3 );
    buildTree( div + 1, right, ( axis + 1 ) % 3 );
}

WKdTreeThread::WKdTreeThread( float* pointArray, std::vector< unsigned int >* tree, int left, int right, int axis ) :
    WThreadedRunner(), m_tree( tree ), m_pointArray( pointArray ), m_left( left ), m_right( right ), m_axis( axis )
{
}

void WKdTreeThread::threadMain()
{
    buildTree( m_left, m_right, m_axis );
    wlog::debug( "KdTree" ) << "thread finished";
}

void WKdTreeThread::buildTree( int left, int right, int axis )
{
    if( left >= right )
        return;

    int div = ( left + right ) / 2;
    std::nth_element( m_tree->begin() + left, m_tree->begin() + div, m_tree->begin() + right, lessy( m_pointArray, axis ) );

    buildTree( left, div - 1, ( axis + 1 ) % 3 );
    buildTree( div + 1, right, ( axis + 1 ) % 3 );
}
