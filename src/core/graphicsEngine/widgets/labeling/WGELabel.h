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

#ifndef WGELABEL_H
#define WGELABEL_H

#include <osgText/Text>
#include <osgText/FadeText>

#include "WGELayoutableItem.h"
#include "../../WExportWGE.h"

/**
 * Label layout-item. An layoutable item which displays text. This is nice for labeling elements on the screen. It can be used stand-alone (as it
 * simply is an osgText::Text instance or in conjunction with an WGEItemLayouter.
 */
class WGE_EXPORT WGELabel: public osgText::Text,
                                WGELayoutableItem
{
public:

    /**
     * Convenience typedef for a osg::ref_ptr< WGELabel >.
     */
    typedef osg::ref_ptr< WGELabel > SPtr;

    /**
     * Convenience typedef for a osg::ref_ptr< const WGELabel >.
     */
    typedef osg::ref_ptr< const WGELabel > ConstSPtr;

    /**
     * Default constructor.
     */
    WGELabel();

    /**
     * Destructor.
     */
    virtual ~WGELabel();

    /**
     * Gets the current anchor point of the label.
     *
     * \return the anchor.
     */
    virtual osg::Vec3 getAnchor() const;

    /**
     * Sets the anchor point in world coordinates of this labels.
     *
     * \param anchor the anchor point
     */
    virtual void setAnchor( const osg::Vec3& anchor );

protected:

    /**
     * The anchor of the label in world space.
     */
    osg::Vec3 m_anchor;

private:
};

#endif  // WGELABEL_H

