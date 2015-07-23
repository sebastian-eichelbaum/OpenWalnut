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

//---------------------------------------------------------------------------
//
// Project: hClustering
//
// Whole-Brain Connectivity-Based Hierarchical Parcellation Project
// David Moreno-Dominguez
// d.mor.dom@gmail.com
// moreno@cbs.mpg.de
// www.cbs.mpg.de/~moreno//
// This file is also part of OpenWalnut ( http://www.openwalnut.org ).
//
// hClustering is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// http://creativecommons.org/licenses/by-nc/3.0
//
// hClustering is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
//---------------------------------------------------------------------------
#include <iostream>
#include <vector>

#include "core/graphicsEngine/WGEUtils.h"

#include "WNonBinDendroGeode.h"

/**
 * Class implements a dendrogram as an osg geode
 */
WNonBinDendroGeode::WNonBinDendroGeode( const WHtree& tree, const std::vector< WColor > &displayColors, size_t cluster,
                float xSize, float ySize, float xOffset, float yOffset, bool useHLevel, bool triangleLeaves, float hozLine ) :
    osg::Geode(), m_tree( tree ), m_rootCluster( cluster ), m_displayColors( displayColors ), m_xSize( xSize ), m_ySize( ySize ),
                    m_xOff( xOffset ), m_yOff( yOffset ), m_useHLevel( useHLevel ), m_triangleLeaves( triangleLeaves ),
                    m_hozLine( hozLine )

{
    create();
} // end dendrogramGeode()

WNonBinDendroGeode::~WNonBinDendroGeode()
{
}

void WNonBinDendroGeode::create()
{
    m_lineColors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );

    m_vertexArray = new osg::Vec3Array;

    m_lineArray = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );

    const WHnode& currentRoot( m_tree.getNode( m_rootCluster ) );

    float subTreeSize( static_cast< float > ( currentRoot.getSize() ) );
    m_xUnit = m_xSize / subTreeSize;

    float topLevel( 0 ), topRoot( 0 );

    if( m_useHLevel )
    {
        topLevel = ( currentRoot.getHLevel() );
        m_yUnit = m_ySize / topLevel;

        topRoot = topLevel + ( 20 / m_yUnit );
    }
    else
    {
        topLevel = ( currentRoot.getDistLevel() );

        if( m_rootCluster == m_tree.getRoot().getID() )
        {
            m_yUnit = m_ySize;
            topRoot = 1;
        }
        else
        {
            m_yUnit = m_ySize / topLevel;
            topRoot = topLevel + ( 20 / m_yUnit );
        }
    }

    // first line (trunk), first draw 2 vertices and then indicate that a line goes between them
    m_vertexArray->push_back( osg::Vec3( subTreeSize / 2.0, topRoot, 0 ) );
    m_vertexArray->push_back( osg::Vec3( subTreeSize / 2.0, topLevel, 0 ) );
    m_lineArray->push_back( m_vertexArray->size() - 2 );
    m_lineArray->push_back( m_vertexArray->size() - 1 );
    m_lineColors->push_back( WColor( 0.3, 0.3, 0.3, 1 ) );
    m_lineColors->push_back( WColor( 0.3, 0.3, 0.3, 1 ) );

    // Draw a red horizontal line if indicated
    if( m_hozLine != 0 && !m_useHLevel )
    {
        m_vertexArray->push_back( osg::Vec3( 0, m_hozLine, 0 ) );
        m_vertexArray->push_back( osg::Vec3( subTreeSize, m_hozLine, 0 ) );
        m_lineArray->push_back( m_vertexArray->size() - 2 );
        m_lineArray->push_back( m_vertexArray->size() - 1 );
        m_lineColors->push_back( WColor( 1, 0, 0, 1 ) );
        m_lineColors->push_back( WColor( 1, 0, 0, 1 ) );
    }

    // draw rest of the tree
    layout( currentRoot, 0.0f, static_cast< float > ( subTreeSize ) );

    for( size_t i = 0; i < m_vertexArray->size(); ++i )
    {
        ( *m_vertexArray )[i].x() = ( *m_vertexArray )[i].x() * m_xUnit + m_xOff;
        ( *m_vertexArray )[i].y() = ( *m_vertexArray )[i].y() * m_yUnit + m_yOff;
    }

    osg::ref_ptr< osg::Geometry > geometry = osg::ref_ptr< osg::Geometry >( new osg::Geometry() );

    geometry->setVertexArray( m_vertexArray );

    geometry->addPrimitiveSet( m_lineArray );

    geometry->setColorArray( m_lineColors );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

    osg::StateSet* state = geometry->getOrCreateStateSet();
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

    addDrawable( geometry );
} // end create()

void WNonBinDendroGeode::layout( const WHnode& cluster, const float leftLimit, const float rightLimit )
{
    // get height of current branching depending
    float height( 0 );
    if( m_useHLevel )
    {
        height = cluster.getHLevel();
    }
    else
    {
        height = cluster.getDistLevel();
    }

    float leftStart( leftLimit );

    // order children nodes by size
    std::vector< nodeID_t > kids( cluster.getChildren() );
    m_tree.sortBySize( &kids );
    bool horizontalNotDone( true );
    for( unsigned int i = 0; i < kids.size(); ++i )
    {
        const WHnode& thisKid( m_tree.getNode( kids[i] ) );

        // if its a leaf and the triangle leaves option is activated
        if( thisKid.isLeaf() && m_triangleLeaves && !m_useHLevel )
        {
            m_vertexArray->push_back( osg::Vec3( ( leftLimit + rightLimit ) / 2.0, height, 0 ) );
            m_vertexArray->push_back( osg::Vec3( leftStart + 0.5, 0, 0 ) );
            m_lineArray->push_back( m_vertexArray->size() - 2 );
            m_lineArray->push_back( m_vertexArray->size() - 1 );
            m_lineColors->push_back( m_displayColors[cluster.getID()] );
            m_lineColors->push_back( m_displayColors[cluster.getID()] );

            ++leftStart;
            continue;
        }

        // draw the horizontal line
        if( horizontalNotDone )
        {
            float leftHozPoint( leftStart + ( thisKid.getSize() * 0.5 ) );
            if( leftHozPoint > ( ( leftLimit + rightLimit ) / 2.0 ) )
                leftHozPoint = ( leftLimit + rightLimit ) / 2.0;
            const WHnode& lastKid( m_tree.getNode( kids.back() ) );

            m_vertexArray->push_back( osg::Vec3( leftHozPoint, height, 0 ) );
            m_vertexArray->push_back( osg::Vec3( rightLimit - ( lastKid.getSize() * 0.5 ), height, 0 ) );
            m_lineArray->push_back( m_vertexArray->size() - 2 );
            m_lineArray->push_back( m_vertexArray->size() - 1 );
            m_lineColors->push_back( m_displayColors[cluster.getID()] );
            m_lineColors->push_back( m_displayColors[cluster.getID()] );

            horizontalNotDone = false;
        }

        // any other case

        float kidHeight( 0 );
        if( m_useHLevel )
        {
            kidHeight = thisKid.getHLevel();
        }
        else
        {
            kidHeight = thisKid.getDistLevel();
        }
        m_vertexArray->push_back( osg::Vec3( leftStart + ( thisKid.getSize() * 0.5 ), height, 0 ) );
        m_vertexArray->push_back( osg::Vec3( leftStart + ( thisKid.getSize() * 0.5 ), kidHeight, 0 ) );
        m_lineArray->push_back( m_vertexArray->size() - 2 );
        m_lineArray->push_back( m_vertexArray->size() - 1 );
        m_lineColors->push_back( m_displayColors[cluster.getID()] );
        m_lineColors->push_back( m_displayColors[cluster.getID()] );

        // if its not a leaf continue recursively
        if( thisKid.isNode() )
            layout( thisKid, leftStart, leftStart + ( thisKid.getSize() ) );

        // update undrawn dimensions
        leftStart += thisKid.getSize();
    }
} // end layout()


size_t WNonBinDendroGeode::getClickedCluster( int xClick, int yClick )
{
    m_clickedCluster = m_rootCluster+1;
    // translate real coordinates to scaled coordinates of the dendrogram
    m_xClicked = ( static_cast< float > ( xClick ) - m_xOff ) / m_xUnit;
    m_yClicked = ( static_cast< float > ( yClick ) - m_yOff ) / m_yUnit;

    findClickedCluster( m_rootCluster, 0.0f, static_cast< float > ( m_tree.getNode( m_rootCluster ).getSize() ) );

    return m_clickedCluster;
} // end getClickedCluster()


void WNonBinDendroGeode::findClickedCluster( size_t cluster, float leftLimit, float rightLimit )
{
    float diffX( m_xClicked - ( ( leftLimit + rightLimit ) * 0.5 ) );
    if( diffX < 0 )
        diffX = -diffX;

    float diffY( 0 );
    if( m_useHLevel )
    {
        diffY = m_yClicked - ( static_cast< float > ( m_tree.getNode( cluster ).getHLevel() ) );
    }
    else
    {
        diffY = m_yClicked - ( m_tree.getNode( cluster ).getDistLevel() );
    }
    if( diffY < 0 )
        diffY = -diffY;

    if( diffX <= ( 5.0 / m_xUnit ) && diffY <= ( 5.0 / m_yUnit ) )
    {
        m_clickedCluster = cluster;
        return;
    }
    else if( m_tree.getNode( cluster ).getHLevel() > 0 )
    {
        float leftStart( leftLimit );

        std::vector< nodeID_t > kids( m_tree.getNode( cluster ).getChildren() );
        m_tree.sortBySize( &kids );

        for( size_t i = 0; i < kids.size(); ++i )
        {
            const WHnode& thisKid( m_tree.getNode( kids[i] ) );
            if( m_xClicked < leftStart + thisKid.getSize() )
            {
                if( thisKid.isNode() )
                {
                    findClickedCluster( thisKid.getID(), leftStart, leftStart + thisKid.getSize() );
                    return;
                }
                else
                {
                    return;
                }
            }
            else
            {
                leftStart += thisKid.getSize();
            }
        }
    }
    return;
} // end findClickedCluster()

bool WNonBinDendroGeode::inDendrogramArea( int xClick, int yClick ) const
{
    return ( xClick >= m_xOff && xClick <= m_xOff + m_xSize &&
             yClick >= m_yOff && yClick <= m_yOff + m_ySize );
}

