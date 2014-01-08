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

#include <map>
#include <string>
#include <vector>

#include "../../common/WStringUtils.h"
#include "WTreeNode.h"

WTreeNode::WTreeNode( size_t index, double level )
  : boost::enable_shared_from_this< WTreeNode >(),
    m_level( level ),
    m_index( index )
{
}

WTreeNode::WTreeNode( const WDendrogram &dendrogram )
  : boost::enable_shared_from_this< WTreeNode >()
{
    const std::vector< size_t >& nodes = dendrogram.getParents();
    const std::vector< double >& heights = dendrogram.getHeights();

    size_t n = heights.size();

    std::map< size_t, WTreeNode::SPtr > map;

    for( size_t index = 0; index < nodes.size(); index++ )
    {
        double height = 0.0;

        if( index > n )
        {
            height = heights[ index - n - 1 ];
        }
        map[ index ] = WTreeNode::SPtr( new WTreeNode( index, height ) );
    }

    for( size_t index = 0; index < nodes.size() - 1; index++ )
    {
        size_t parent = nodes[ index ];
        map[ parent ]->addChild( map[ index ] );
    }

    //Last node is the root node
    WTreeNode::SPtr root = map.at( map.size() - 1 );

    m_index = root->index();
    m_level = root->level();
    m_children = root->getChildren();
}

WTreeNode::~WTreeNode()
{
}

double WTreeNode::level()
{
    return m_level;
}

size_t WTreeNode::index()
{
    return m_index;
}

void WTreeNode::addChild( WTreeNode::SPtr child )
{
    m_children.push_back( child );
    child->m_parent = shared_from_this();
}

WTreeNode::SPtr WTreeNode::getParent()
{
    return m_parent;
}

std::vector< WTreeNode::SPtr > WTreeNode::getChildren()
{
    return m_children;
}
