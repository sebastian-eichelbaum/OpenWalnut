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

#ifndef WRULERORTHO_H
#define WRULERORTHO_H

#include <string>

#include "WRuler.h"

typedef enum
{
    RULER_ALONG_X_AXIS_SCALE_Y = 0,
    RULER_ALONG_X_AXIS_SCALE_Z,
    RULER_ALONG_Y_AXIS_SCALE_X,
    RULER_ALONG_Y_AXIS_SCALE_Z,
    RULER_ALONG_Z_AXIS_SCALE_X,
    RULER_ALONG_Z_AXIS_SCALE_Y
}
scaleMode;

/**
 * class to implement rulers orthogonally
 */
class WRulerOrtho : public WRuler
{
public:
    /**
     * standard constructor
     */
    WRulerOrtho();

    /**
     * destructor
     */
    ~WRulerOrtho();

    /**
     *
     * \param start start point in space
     * \param length lenght of ruler in milimeter
     * \param mode orientation of the ruler
     */
    void create( osg::Vec3 start, float length, scaleMode mode );

protected:
private:
    /**
     * helper function to add a label to the ruler
     *
     * \param position position of thelabel
     * \param text text
     */
    void addLabel( osg::Vec3 position, std::string text );

    /**
     * orientation of ruler
     */
    scaleMode m_scaleMode;
};

#endif  // WRULERORTHO_H
