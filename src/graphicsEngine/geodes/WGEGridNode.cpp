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

#include "../callbacks/WGEFunctorCallback.h"
#include "../WGEGeodeUtils.h"

#include "WGEGridNode.h"

WGEGridNode::WGEGridNode( WGridRegular3D::ConstSPtr grid ):
    m_boundaryGeode( new osg::Geode() ),
    m_innerGridGeode( new osg::Geode() ),
    m_labelGeode( new osg::Geode() ),
    m_gridUpdate( true ),
    m_showLabels( true )
{
    m_grid.getWriteTicket()->get() = grid;

    // init the boundary geometry
    m_boundaryGeode->addDrawable( wge::createUnitCubeAsLines( WColor( 0.3, 0.3, 0.3, 1.0 ) ) );

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
    if ( m_gridUpdate )
    {
        // grab the grid
        WGridRegular3D::ConstSPtr grid = m_grid.getReadTicket()->get();

        // apply the grid transformation
        osg::Matrix m = osg::Matrix::scale( grid->getNbCoordsX() - 1, grid->getNbCoordsY() - 1, grid->getNbCoordsZ() - 1 ) *
                        grid->getTransform();
        setMatrix( m );

        // set the labels correspondingly
        for ( size_t i = 0; i < 8; ++i )
        {
            m_borderLabels[i]->setText( vec2str( m_borderLabels[i]->getPosition() * m ) );
        }

        // set node mask of labels
        m_labelGeode->setNodeMask( 0xFFFFFFFF * m_showLabels );

        m_gridUpdate = false;
    }
}

