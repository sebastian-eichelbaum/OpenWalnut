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

#ifndef WGESUBDIVIDEDPLANE_H
#define WGESUBDIVIDEDPLANE_H

#include <osg/Geode>

/**
 * Represents a plane which is subdivided into quads where each quad primitive is colorable.
 */
class WGESubdividedPlane : public osg::Geode
{
public:
    /**
     * For each quad primitive, the center point is computed and stored in \ref m_quadCenters.
     *
     * \param centers the vertex array of the center points of each quad in same order as the quads
     */
    void setCenterArray( osg::ref_ptr< osg::Vec3Array > centers );

    /**
     * Gives the reference to the centerpoints of the quads this plane is representing.
     *
     * \return Const reference to the center points
     */
    osg::ref_ptr< const osg::Vec3Array > getCenterArray() const;

protected:
private:
    /**
     * Stores the center points of each quad. This is saved in this geode, so the center points are
     * available before transformation.
     */
    osg::ref_ptr< osg::Vec3Array > m_quadCenters;
};

inline void WGESubdividedPlane::setCenterArray( osg::ref_ptr< osg::Vec3Array > centers )
{
    // TODO(math): do some constraint checks
    m_quadCenters = centers;
}

inline osg::ref_ptr< const osg::Vec3Array > WGESubdividedPlane::getCenterArray() const
{
    return m_quadCenters;
}

#endif  // WGESUBDIVIDEDPLANE_H
