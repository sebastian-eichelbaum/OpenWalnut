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

#ifndef WGEVIEWERBASE_H
#define WGEVIEWERBASE_H

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "../common/WColor.h"
#include "../common/WFlag.h"

#include "WGEViewerEffectHorizon.h"
#include "WGEViewerEffectVignette.h"
#include "WGEViewerEffectImageOverlay.h"

#include "WGEScreenCapture.h"

class WGEGroupNode;
class WPickHandler;

/**
 * Class for managing one view to the scene. This includes viewport, camera, and scene. It is a base
 * class and implements no rendering functionality.
 *
 * \ingroup ge
 */
class WGEViewerBase: public boost::enable_shared_from_this< WGEViewerBase >
{
public:
    /**
     * Convenience typedef
     */
    typedef boost::shared_ptr< WGEViewer > SPtr;

    /**
     * Convenience typedef
     */
    typedef boost::shared_ptr< const WGEViewer > ConstSPtr;

    /**
     * Default constructor. Create the viewer. The name needs to be unique.
     *
     * \param name the name of the viewer. Needs to be unique. "Main View" is reserved.
     * \param x X coordinate of widget where to create the context.
     * \param y Y coordinate of widget where to create the context.
     * \param width Width of the widget.
     * \param height Height of the Widget.
     * \param projectionMode Projection mode of the viewer. Currently only the orthographic mode allows the use of a pick handler.
     *
     * \exception WGEInitFailed thrown if initialization of graphics context or graphics window has failed.
     */
    WGEViewerBase( std::string name, int x, int y, int width, int height, WGECamera::ProjectionMode projectionMode = WGECamera::ORTHOGRAPHIC );

    /**
     * Destructor. Cleans up.
     */
    virtual ~WGEViewerBase();

    /**
     * Returns the name of the viewer.
     *
     * \return the name
     */
    std::string getName() const;

protected:
private:
    /**
     * The name of the viewer.
     */
    std::string m_name;

};

#endif  // WGEVIEWERBASE_H
