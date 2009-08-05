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

#ifndef WGRIDREGULAR3D_H
#define WGRIDREGULAR3D_H

#include "WGrid.h"

/**
 * A grid that has axis aligned cuboid cells which all have the same width,
 * the same height and the same depth. I.e. the samples along a single axis
 * are equidistant. The distance of samples may vary between axes.
 */
class WGridRegular3D : public WGrid
{
public:
    /**
     * Defines the position of the origin of the grid, the number of
     * samples in each coordinate direction and the offset between the
     * samples in the different coordinate directions.
     */
    WGridRegular3D(
                   double originX, double originY, double originZ,
                   unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                   double offsetX, double offsetY, double offsetZ );

    /**
     * Convenience constructor that does the same as the one above but
     * uses the origin (0,0,0) as default.
     */
    WGridRegular3D(
                   unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                   double offsetX, double offsetY, double offsetZ );
protected:
private:
    double m_originX;
    double m_originY;
    double m_originZ;
    unsigned int m_nbPosX;
    unsigned int m_nbPosY;
    unsigned int m_nbPosZ;
    double m_offsetX;
    double m_offsetY;
    double m_offsetZ;
};

#endif  // WGRIDREGULAR3D_H
