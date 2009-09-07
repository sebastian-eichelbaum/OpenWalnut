//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#include <osg/Group>

/**
 * \par Description:
 * Class for managing the OpenSceneGraph root node. It can handle new nodes, removing nodes and so on.
 * \ingroup ge
 */
class WGEScene: public osg::Group
{
public:

    /**
     * \par Description
     * Default constructor.
     */
    WGEScene();

    /**
     * \par Description
     * Destructor.
     */
    virtual ~WGEScene();

protected:

private:
};

#endif  // WGESCENE_H

