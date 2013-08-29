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

#include <iostream>

#include "../../graphicsEngine/WGEUtils.h"

// Compatibility between OSG 3.2 and earlier versions
#include "core/graphicsEngine/WOSG.h"

#include "WDendrogramGeode.h"

/**
 * Class implements a dendrogram as an osg geode
 */
WDendrogramGeode::WDendrogramGeode( WHierarchicalTree* tree, size_t cluster, bool useLevel, size_t minClusterSize,
                                    float xSize, float ySize, float xOffset, float yOffset ) :
    osg::Geode(),
    m_tree( tree ),
    m_rootCluster( cluster ),
    m_minClusterSize( minClusterSize ),
    m_xSize( xSize ),
    m_ySize( ySize ),
    m_xOff( xOffset ),
    m_yOff( yOffset ),
    m_useLevel( useLevel )
{
    create();
}

WDendrogramGeode::~WDendrogramGeode()
{
}

void WDendrogramGeode::create()
{
    m_colors = osg::ref_ptr<osg::Vec4Array>( new osg::Vec4Array );

    m_vertexArray = new osg::Vec3Array;

    m_lineArray = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );

    float xMax = static_cast<float>( m_tree->size( m_rootCluster ) - 1 );

    m_xMult = m_xSize / xMax;


    if( m_useLevel )
    {
        layoutLevel( m_rootCluster, 0.0f, static_cast<float>( m_tree->size( m_rootCluster ) - 1 ) );
        float yMax = m_tree->getLevel( m_rootCluster );
        m_yMult = m_ySize / yMax;
    }
    else
    {
        layoutValue( m_rootCluster, 0.0f, static_cast<float>( m_tree->size( m_rootCluster ) - 1 ) );
        m_yMult = m_ySize;
    }

    for( size_t i = 0; i < m_vertexArray->size(); ++i )
    {
        (*m_vertexArray)[i].x() = (*m_vertexArray)[i].x() * m_xMult + m_xOff;
        (*m_vertexArray)[i].y() = (*m_vertexArray)[i].y() * m_yMult + m_yOff;
    }

    osg::ref_ptr< wosg::Geometry > geometry = osg::ref_ptr< wosg::Geometry >( new wosg::Geometry() );

    geometry->setVertexArray( m_vertexArray );

    geometry->addPrimitiveSet( m_lineArray );

    geometry->setColorArray( m_colors );
    geometry->setColorBinding( wosg::Geometry::BIND_PER_VERTEX );

    osg::StateSet* state = geometry->getOrCreateStateSet();
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

    addDrawable( geometry );
}

void WDendrogramGeode::layoutLevel( size_t cluster, float left, float right )
{
    float height = m_tree->getLevel( cluster );

    float size = right - left;

    if( m_tree->getLevel( cluster ) > 0 )
    {
        size_t leftCluster = m_tree->getChildren( cluster ).first;
        size_t rightCluster = m_tree->getChildren( cluster ).second;

        float leftHeight = m_tree->getLevel( leftCluster );
        float leftSize = static_cast<float>( m_tree->size( leftCluster ) );

        float rightHeight = m_tree->getLevel( rightCluster );
        float rightSize = static_cast<float>( m_tree->size( rightCluster ) );

        if( ( leftSize >= m_minClusterSize ) &&  ( rightSize < m_minClusterSize ) )
        //if( rightSize < 2 )
        {
            // left cluster is much bigger, draw only left
            m_vertexArray->push_back( osg::Vec3( ( left + size / 2.0 ), height, 0 ) );
            m_vertexArray->push_back( osg::Vec3( ( left + size / 2.0 ), leftHeight, 0 ) );

            m_lineArray->push_back( m_vertexArray->size() - 2 );
            m_lineArray->push_back( m_vertexArray->size() - 1 );

            m_colors->push_back( m_tree->getColor( cluster ) );
            m_colors->push_back( m_tree->getColor( cluster ) );

            layoutLevel( leftCluster, left, right );
        }
        else if( ( rightSize >= m_minClusterSize ) &&  ( leftSize < m_minClusterSize ) )
        //else if( leftSize < 2 )
        {
            // right cluster is much bigger, draw only right
            m_vertexArray->push_back( osg::Vec3( ( left + size / 2.0 ), height, 0 ) );
            m_vertexArray->push_back( osg::Vec3( ( left + size / 2.0 ), rightHeight, 0 ) );

            m_lineArray->push_back( m_vertexArray->size() - 2 );
            m_lineArray->push_back( m_vertexArray->size() - 1 );

            m_colors->push_back( m_tree->getColor( cluster ) );
            m_colors->push_back( m_tree->getColor( cluster ) );

            layoutLevel( rightCluster, left, right );
        }
        else
        {
            float mult = size / ( leftSize + rightSize );

            m_vertexArray->push_back( osg::Vec3( ( left  + leftSize * mult  / 2.0 ), height, 0 ) );
            m_vertexArray->push_back( osg::Vec3( ( right - rightSize * mult / 2.0 ), height, 0 ) );

            m_lineArray->push_back( m_vertexArray->size() - 2 );
            m_lineArray->push_back( m_vertexArray->size() - 1 );

            m_vertexArray->push_back( osg::Vec3( ( left  + leftSize * mult  / 2.0 ), leftHeight, 0 ) );
            m_vertexArray->push_back( osg::Vec3( ( right - rightSize * mult / 2.0 ), rightHeight, 0 ) );

            m_colors->push_back( m_tree->getColor( cluster ) );
            m_colors->push_back( m_tree->getColor( cluster ) );
            m_colors->push_back( m_tree->getColor( cluster ) );
            m_colors->push_back( m_tree->getColor( cluster ) );

            m_lineArray->push_back( m_vertexArray->size() - 4 );
            m_lineArray->push_back( m_vertexArray->size() - 2 );
            m_lineArray->push_back( m_vertexArray->size() - 3 );
            m_lineArray->push_back( m_vertexArray->size() - 1 );

            layoutLevel( leftCluster, left, left + leftSize * mult );
            layoutLevel( rightCluster, right - rightSize * mult, right );
        }
    }
}

void WDendrogramGeode::layoutValue( size_t cluster, float left, float right )
{
    float height = m_tree->getCustomData( cluster );

    float size = right - left;

    if( m_tree->getLevel( cluster ) > 0 )
    {
        size_t leftCluster = m_tree->getChildren( cluster ).first;
        size_t rightCluster = m_tree->getChildren( cluster ).second;

        float leftHeight = m_tree->getCustomData( leftCluster );
        float leftSize = static_cast<float>( m_tree->size( leftCluster ) );

        float rightHeight = m_tree->getCustomData( rightCluster );
        float rightSize = static_cast<float>( m_tree->size( rightCluster ) );

        if( ( leftSize >= m_minClusterSize ) &&  ( rightSize < m_minClusterSize ) )
        //if( rightSize < 2 )
        {
            // left cluster is much bigger, draw only left
            m_vertexArray->push_back( osg::Vec3( ( left + size / 2.0 ), height, 0 ) );
            m_vertexArray->push_back( osg::Vec3( ( left + size / 2.0 ), leftHeight, 0 ) );

            m_lineArray->push_back( m_vertexArray->size() - 2 );
            m_lineArray->push_back( m_vertexArray->size() - 1 );

            m_colors->push_back( m_tree->getColor( cluster ) );
            m_colors->push_back( m_tree->getColor( cluster ) );

            layoutValue( leftCluster, left, right );
        }
        else if( ( rightSize >= m_minClusterSize ) &&  ( leftSize < m_minClusterSize ) )
        //else if( leftSize < 2 )
        {
            // right cluster is much bigger, draw only right
            m_vertexArray->push_back( osg::Vec3( ( left + size / 2.0 ), height, 0 ) );
            m_vertexArray->push_back( osg::Vec3( ( left + size / 2.0 ), rightHeight, 0 ) );

            m_lineArray->push_back( m_vertexArray->size() - 2 );
            m_lineArray->push_back( m_vertexArray->size() - 1 );

            m_colors->push_back( m_tree->getColor( cluster ) );
            m_colors->push_back( m_tree->getColor( cluster ) );

            layoutValue( rightCluster, left, right );
        }
        else
        {
            float mult = size / ( leftSize + rightSize );

            m_vertexArray->push_back( osg::Vec3( ( left  + leftSize * mult  / 2.0 ), height, 0 ) );
            m_vertexArray->push_back( osg::Vec3( ( right - rightSize * mult / 2.0 ), height, 0 ) );

            m_lineArray->push_back( m_vertexArray->size() - 2 );
            m_lineArray->push_back( m_vertexArray->size() - 1 );

            m_vertexArray->push_back( osg::Vec3( ( left  + leftSize * mult  / 2.0 ), leftHeight, 0 ) );
            m_vertexArray->push_back( osg::Vec3( ( right - rightSize * mult / 2.0 ), rightHeight, 0 ) );

            m_colors->push_back( m_tree->getColor( cluster ) );
            m_colors->push_back( m_tree->getColor( cluster ) );
            m_colors->push_back( m_tree->getColor( cluster ) );
            m_colors->push_back( m_tree->getColor( cluster ) );

            m_lineArray->push_back( m_vertexArray->size() - 4 );
            m_lineArray->push_back( m_vertexArray->size() - 2 );
            m_lineArray->push_back( m_vertexArray->size() - 3 );
            m_lineArray->push_back( m_vertexArray->size() - 1 );

            layoutValue( leftCluster, left, left + leftSize * mult );
            layoutValue( rightCluster, right - rightSize * mult, right );
        }
    }
}

size_t WDendrogramGeode::getClickedCluster( int xClick, int yClick )
{
    m_xClicked = ( xClick - m_xOff ) / m_xSize * ( m_tree->size( m_rootCluster ) );

    m_clickedCluster = m_rootCluster;

    if( m_useLevel )
    {
        m_yClicked = ( yClick - m_yOff ) / m_ySize * ( m_tree->getLevel( m_rootCluster ) );
        getClickClusterRecursive( m_rootCluster, 0.0f, static_cast<float>( m_tree->size( m_rootCluster ) - 1 ) );
    }
    else
    {
        m_yClicked = ( yClick - m_yOff );
        getClickClusterRecursive2( m_rootCluster, 0.0f, static_cast<float>( m_tree->size( m_rootCluster ) - 1 ) );
    }

    return m_clickedCluster;
}

void WDendrogramGeode::getClickClusterRecursive2( size_t cluster, float left, float right )
{
    int height = static_cast<int>( m_tree->getCustomData( cluster ) * m_ySize );

    if( abs( height - m_yClicked ) < 2 )
    {
        m_clickedCluster = cluster;
        return;
    }

    int size = right - left;

    if( m_tree->getLevel( cluster ) > 0 )
    {
        size_t leftCluster = m_tree->getChildren( cluster ).first;
        size_t rightCluster = m_tree->getChildren( cluster ).second;

        float leftSize = static_cast<float>( m_tree->size( leftCluster ) );
        float rightSize = static_cast<float>( m_tree->size( rightCluster ) );

        if( ( leftSize >= m_minClusterSize ) &&  ( rightSize < m_minClusterSize ) )
        {
            // left cluster is much bigger, draw only left
            getClickClusterRecursive2( leftCluster, left, right );
        }
        else if( ( rightSize >= m_minClusterSize ) &&  ( leftSize < m_minClusterSize ) )
        {
            // right cluster is much bigger, draw only right
            getClickClusterRecursive2( rightCluster, left, right );
        }
        else
        {
            float mult = size / ( leftSize + rightSize );

            if( m_xClicked < left + leftSize * mult )
            {
                getClickClusterRecursive2( leftCluster, left, left + leftSize * mult );
            }
            else
            {
                getClickClusterRecursive2( rightCluster, right - rightSize * mult, right );
            }
        }
    }
}

void WDendrogramGeode::getClickClusterRecursive( size_t cluster, float left, float right )
{
    int height = m_tree->getLevel( cluster );

    if( height == m_yClicked )
    {
        m_clickedCluster = cluster;
        return;
    }

    int size = right - left;

    if( m_tree->getLevel( cluster ) > 0 )
    {
        size_t leftCluster = m_tree->getChildren( cluster ).first;
        size_t rightCluster = m_tree->getChildren( cluster ).second;

        float leftSize = static_cast<float>( m_tree->size( leftCluster ) );
        float rightSize = static_cast<float>( m_tree->size( rightCluster ) );

        if( ( leftSize >= m_minClusterSize ) &&  ( rightSize < m_minClusterSize ) )
        {
            // left cluster is much bigger, draw only left
            getClickClusterRecursive( leftCluster, left, right );
        }
        else if( ( rightSize >= m_minClusterSize ) &&  ( leftSize < m_minClusterSize ) )
        {
            // right cluster is much bigger, draw only right
            getClickClusterRecursive( rightCluster, left, right );
        }
        else
        {
            float mult = size / ( leftSize + rightSize );

            if( m_xClicked < left + leftSize * mult )
            {
                getClickClusterRecursive( leftCluster, left, left + leftSize * mult );
            }
            else
            {
                getClickClusterRecursive( rightCluster, right - rightSize * mult, right );
            }
        }
    }
}
