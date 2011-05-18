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

#ifndef WCUSTOMWIDGET_H
#define WCUSTOMWIDGET_H

#include <boost/shared_ptr.hpp>

#include <osg/ref_ptr>

#include "../graphicsEngine/WGEViewer.h"
class WGEGroupNode;

/**
 * Custom widget which is created by a module to display custom information.
 */
class WCustomWidget
{
public:
    /**
     * Destructor
     */
    virtual ~WCustomWidget();

    /**
     * Get the scene which is displayed
     *
     * \return the scene as osg::ref_ptr
     */
    virtual osg::ref_ptr< WGEGroupNode > getScene() const = 0;

    /**
     * Get the viewer which is used
     *
     * \return the viewer as boost::shard_ptr
     */
    virtual boost::shared_ptr< WGEViewer > getViewer() const = 0;

protected:
private:
};

#endif  // WCUSTOMWIDGET_H
