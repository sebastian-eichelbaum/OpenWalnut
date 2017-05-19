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

#ifndef WGEBORDERLAYOUT_H
#define WGEBORDERLAYOUT_H

#include <osg/Drawable>
#include <osg/Geode>
#include <osgText/Text>
#include <osg/Projection>

#include "../../WGEGroupNode.h"
#include "WGELabel.h"

/**
 * This class contains a label layout for positioning labels at the side of the screen. There is a line from the point of interest to the label
 * on the side of the screen.
 */
class WGEBorderLayout: public WGEGroupNode
{
friend class SafeUpdateCallback; //!< Grant access for its inner class
public:
    /**
     * Default constructor.
     */
    WGEBorderLayout();

    /**
     * Destructor.
     */
    virtual ~WGEBorderLayout();

    /**
     * Adds the specified object to the list of layouted objects.
     *
     * \param obj the layoutable object.
     * \todo(ebaum): this is ugly, use a common baseclass instead
     */
    virtual void addLayoutable( osg::ref_ptr< WGELabel > obj );

protected:
    /**
     * The geode containing all drawables.
     */
    osg::ref_ptr< osg::Geode > m_geode;

    /**
     * Contains all lead lines.
     */
    osg::ref_ptr< osg::Geode > m_lineGeode;

    /**
     * The projection to the screen
     */
    osg::ref_ptr< osg::Projection > m_screen;

    /**
     * Node callback to actually layout the objects.
     */
    class SafeUpdateCallback : public osg::NodeCallback
    {
    public: // NOLINT
        /**
         * Constructor.
         *
         * \param layout just set the creating layout as pointer for later reference.
         */
        explicit SafeUpdateCallback( osg::ref_ptr< WGEBorderLayout > layout ): m_layouter( layout )
        {
        };

        /**
         * operator () - called during the update traversal.
         *
         * \param node the osg node
         * \param nv the node visitor
         */
        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

        /**
         * The layouter owning this callback
         * \todo(ebaum) the layoiter insance will never be freed -> cyclic dependency
         */
        osg::ref_ptr< WGEBorderLayout > m_layouter;
    };

private:
};

#endif  // WGEBORDERLAYOUT_H

