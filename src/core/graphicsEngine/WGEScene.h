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

#ifndef WGESCENE_H
#define WGESCENE_H

#include "WGEGroupNode.h"
#include "WExportWGE.h"

/**
 * Class for managing the OpenSceneGraph root node. It can handle new nodes, removing nodes and so on.
 * \ingroup ge
 */
class WGE_EXPORT WGEScene: public WGEGroupNode
{
public:
    /**
     * Default constructor.
     */
    WGEScene();

    /**
     * Indicates whether we want the widget showing the scene to be
     * tranferred to its home position. This is the case if we have the
     * <b>first</b> child added to the scene.
     *
     * \return true if reset to bbox is requested
     */
    bool isHomePositionRequested();

protected:
    /**
     * Destructor. Protected. To avoid accidental destruction of these root nodes.
     */
    virtual ~WGEScene();

private:
    bool firstHomePositionRequest; //!< Indicates whether we still have the first request for home position (isHomePositionRequested()).
};



#endif  // WGESCENE_H

