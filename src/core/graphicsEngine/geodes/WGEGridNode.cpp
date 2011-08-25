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

#include <sstream>

#include <osg/LineWidth>

#include "../callbacks/WGEFunctorCallback.h"
#include "../../common/math/linearAlgebra/WLinearAlgebra.h"
#include "../WGEGeodeUtils.h"

#include "WGEGridNode.h"

WGEGridNode::WGEGridNode( WGridRegular3D::ConstSPtr grid ):
    m_boundaryGeode( new osg::Geode() ),
    m_innerGridGeode( new osg::Geode() ),
    m_labelGeode( new osg::Geode() ),
    m_gridUpdate( true ),
    m_gridGeometryUpdate( true ),
    m_showLabels( true ),
    m_bbColor( WColor( 0.3, 0.3, 0.3, 1.0 ) ),
    m_gridColor( WColor( 0.1, 0.1, 0.1, 1.0 ) )
{
    m_grid.getWriteTicket()->get() = grid;

    // init the boundary geometry
    m_boundaryGeode->addDrawable( wge::createUnitCubeAsLines( m_bbColor ) );

    // init labels
    // Therefore: create prototype
    WGELabel::SPtr label = new WGELabel();
    label->setAlignment( osgText::Text::CENTER_TOP );
    label->setColor( osg::Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );

    // add several copies and set position accordingly

    // Front face ( z = 0 )
    // bottom left
    label->setPosition( osg::Vec3( 0.0, 0.0, 0.0 ) );
    label->setCharacterSize( 6 );
    m_labelGeode->addDrawable( label );
    m_borderLabels[0] = label;

    // bottom right
    label = new WGELabel( *label );
    label->setPosition( osg::Vec3( 1.0, 0.0, 0.0 ) );
    m_labelGeode->addDrawable( label );
    m_borderLabels[1] = label;

    // top right
    label = new WGELabel( *label );
    label->setPosition( osg::Vec3( 1.0, 1.0, 0.0 ) );
    m_labelGeode->addDrawable( label );
    m_borderLabels[2] = label;

    // top left
    label = new WGELabel( *label );
    label->setPosition( osg::Vec3( 0.0, 1.0, 0.0 ) );
    m_labelGeode->addDrawable( label );
    m_borderLabels[3] = label;

    // Back face ( z = 1 )
    // bottom left
    label = new WGELabel( *label );
    label->setPosition( osg::Vec3( 0.0, 0.0, 1.0 ) );
    m_labelGeode->addDrawable( label );
    m_borderLabels[4] = label;

    // bottom right
    label = new WGELabel( *label );
    label->setPosition( osg::Vec3( 1.0, 0.0, 1.0 ) );
    m_labelGeode->addDrawable( label );
    m_borderLabels[5] = label;

    // top right
    label = new WGELabel( *label );
    label->setPosition( osg::Vec3( 1.0, 1.0, 1.0 ) );
    m_labelGeode->addDrawable( label );
    m_borderLabels[6] = label;

    // top left
    label = new WGELabel( *label );
    label->setPosition( osg::Vec3( 0.0, 1.0, 1.0 ) );
    m_labelGeode->addDrawable( label );
    m_borderLabels[7] = label;

    // add the others too
    addChild( m_boundaryGeode );
    addChild( m_innerGridGeode );
    addChild( m_labelGeode );

    m_boundaryGeode->getOrCreateStateSet()->setAttributeAndModes( new osg::LineWidth( 4.0 ), osg::StateAttribute::ON );

    addUpdateCallback( new WGEFunctorCallback< osg::Node >( boost::bind( &WGEGridNode::callback, this, _1 ) ) );

    // no blending
    getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::OFF );
    // disable light for this node
    getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
}

WGEGridNode::~WGEGridNode()
{
    // cleanup
}

void WGEGridNode::setGrid( WGridRegular3D::ConstSPtr grid )
{
    m_grid.getWriteTicket()->get() = grid;
    m_gridUpdate = true;
    m_gridGeometryUpdate = true;
}

WGridRegular3D::ConstSPtr WGEGridNode::getGrid() const
{
    return m_grid.getReadTicket()->get();
}

bool WGEGridNode::getEnableLabels() const
{
    return m_showLabels;
}

void WGEGridNode::setEnableLabels( bool enable )
{
    m_showLabels = enable;
    m_gridUpdate = true;
}

bool WGEGridNode::getEnableBBox() const
{
    return m_showBBox;
}

void WGEGridNode::setEnableBBox( bool enable )
{
    m_showBBox = enable;
    m_gridUpdate = true;
}

bool WGEGridNode::getEnableGrid() const
{
    return m_showGrid;
}

void WGEGridNode::setEnableGrid( bool enable )
{
    m_showGrid = enable;
    m_gridUpdate = true;
}

const WColor& WGEGridNode::getBBoxColor() const
{
    return m_bbColor;
}

void WGEGridNode::setBBoxColor( const WColor& color )
{
    m_bbColor = color;
    m_gridUpdate = true;
}

const WColor& WGEGridNode::getGridColor() const
{
    return m_gridColor;
}

void WGEGridNode::setGridColor( const WColor& color )
{
    m_gridColor = color;
    m_gridUpdate = true;
}

/**
 * Simply converts the vector to an string.
 *
 * \param vec the vector
 *
 * \return string representation
 */
std::string vec2str( osg::Vec3 vec )
{
    std::ostringstream os;
    os.precision( 5 );
    os << "(" << vec[0] << "," << vec[1] << "," << vec[2] << ")";
    return os.str();
}

void WGEGridNode::callback( osg::Node* /*node*/ )
{
    if( m_gridUpdate )
    {
        // grab the grid
        WGridRegular3D::ConstSPtr grid = m_grid.getReadTicket()->get();

        // apply the grid transformation
        osg::Matrix m = osg::Matrix::scale( grid->getNbCoordsX() - 1, grid->getNbCoordsY() - 1, grid->getNbCoordsZ() - 1 ) *
                        static_cast< osg::Matrixd >( static_cast< WMatrix4d >( grid->getTransform() ) );
        setMatrix( m );

        // set the labels correspondingly
        for( size_t i = 0; i < 8; ++i )
        {
            m_borderLabels[i]->setText( vec2str( m_borderLabels[i]->getPosition() * m ) );
        }

        // set node mask of labels, bbox and grid
        m_labelGeode->setNodeMask( 0xFFFFFFFF * m_showLabels );
        m_boundaryGeode->setNodeMask( 0xFFFFFFFF * m_showBBox );
        m_innerGridGeode->setNodeMask( 0xFFFFFFFF * m_showGrid );

        // color
        osg::ref_ptr< osg::Vec4Array > colors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
        colors->push_back( m_bbColor );
        m_boundaryGeode->getDrawable( 0 )->asGeometry()->setColorArray( colors );

        // set color for grid too
        if( m_innerGridGeode->getNumDrawables() )
        {
            osg::ref_ptr< osg::Vec4Array > colors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
            colors->push_back( m_gridColor );
            m_innerGridGeode->getDrawable( 0 )->asGeometry()->setColorArray( colors );
        }

        m_gridUpdate = false;
    }

    // recreate grid?
    if( m_gridGeometryUpdate && m_showGrid )
    {
        // grab the grid
        WGridRegular3D::ConstSPtr grid = m_grid.getReadTicket()->get();

        osg::Geometry* gridGeometry = new osg::Geometry();
        osg::ref_ptr< osg::Vec3Array > vertArray = new osg::Vec3Array( grid->size() );

        osg::DrawElementsUInt* gridElement = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );
        gridElement->reserve( grid->size() * 2 );

        size_t sx = grid->getNbCoordsX();
        size_t sy = grid->getNbCoordsY();
        size_t sz = grid->getNbCoordsZ();
        for( unsigned int vertIdX = 0; vertIdX < sx; ++vertIdX )
        {
            for( unsigned int vertIdY = 0; vertIdY < sy; ++vertIdY )
            {
                for( unsigned int vertIdZ = 0; vertIdZ < sz; ++vertIdZ )
                {
                    size_t id = vertIdX + vertIdY * sx + vertIdZ * sx * sy;

                    ( *vertArray )[id][0] = static_cast< float >( vertIdX ) / static_cast< float >( sx - 1 );
                    ( *vertArray )[id][1] = static_cast< float >( vertIdY ) / static_cast< float >( sy - 1 );
                    ( *vertArray )[id][2] = static_cast< float >( vertIdZ ) / static_cast< float >( sz - 1 );

                    if( vertIdX < sx - 1 )
                    {
                        gridElement->push_back( id );
                        gridElement->push_back( id + 1 );
                    }

                    if( vertIdY < sy - 1 )
                    {
                        gridElement->push_back( id );
                        gridElement->push_back( id + sx );
                    }

                    if( vertIdZ < sz - 1 )
                    {
                        gridElement->push_back( id );
                        gridElement->push_back( id + sx * sy );
                    }
                }
            }
        }

        // done. Add it
        gridGeometry->setVertexArray( vertArray );
        gridGeometry->addPrimitiveSet( gridElement );

        osg::ref_ptr< osg::Vec4Array > colors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
        // finally, the colors
        colors->push_back( m_gridColor );
        gridGeometry->setColorArray( colors );
        gridGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

        if( m_innerGridGeode->getNumDrawables() )
        {
            m_innerGridGeode->setDrawable( 0, gridGeometry );
        }
        else
        {
            m_innerGridGeode->addDrawable( gridGeometry );
        }

        // we create a unit cube here as the transformation matrix already contains the proper scaling.
        m_gridGeometryUpdate = false;
    }
}

