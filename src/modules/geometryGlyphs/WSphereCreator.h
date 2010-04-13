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

#ifndef WSPHERECREATOR_H
#define WSPHERECREATOR_H

#include <boost/shared_ptr.hpp>

#include "../../graphicsEngine/WTriangleMesh2.h"

/**
 * TODO(schurade): Document this!
 */
class WSphereCreator
{
public:
    /**
     * constructor, does nothing
     */
    WSphereCreator();
    /**
     * destructor
     */
    ~WSphereCreator();


    /**
     * creates a sphere mesh by executing a given number of loopsubd on a
     *
     * \param resolution
     * \param zoom
     * \param xOff
     * \param yOff
     * \param zOff
     */
    boost::shared_ptr<WTriangleMesh2>createSphere( int resolution, float zoom = 1., float xOff = 0., float yOff = 0., float zOff = 0. );



protected:
private:
    /**
     * creates a sphere mesh by executing a given number of loopsubd on a icosahedrom
     *
     * \param iterations
     */
    boost::shared_ptr<WTriangleMesh2>createIcosahedronSphere( int iterations );
};

#endif  // WSPHERECREATOR_H
