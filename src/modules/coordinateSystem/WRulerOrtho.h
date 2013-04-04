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

#include <osgText/Text>

#include "core/common/math/linearAlgebra/WVectorFixed.h"

#include "WCoordConverter.h"
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
     * \param coordConverter
     * \param origin
     * \param mode
     * \param showNumbers
     */
    WRulerOrtho( boost::shared_ptr<WCoordConverter>coordConverter, osg::Vec3 origin, scaleMode mode, bool showNumbers = true );

    /**
     * destructor
     */
    ~WRulerOrtho();


protected:
private:
    boost::shared_ptr<WCoordConverter>m_coordConverter; //!< stores pointer to a coordinate converter

    /**
     * Origin of the ruler, it will be drawn in the positive direction
     */
    osg::Vec3 m_origin;

    /**
     * orientation of ruler
     */
    scaleMode m_scaleMode;

    bool m_showNumbers; //!< flag to indicate wether to show number labels

    WVector3d m_lb; //!< = m_coordConverter->getBoundingBox().first;
    WVector3d m_ub; //!< = m_coordConverter->getBoundingBox().second;


    /**
     * creates the osg node for the ruler representation
     */
    void create();

    /**
     * helper function to add a label to the ruler
     *
     * \param position position of thelabel
     * \param text text
     */
    void addLabel( osg::Vec3 position, std::string text );

    /**
     * Helper function to create the ruler along the x axis
     *
     * \return A part of the geometry of the ruler
     */
    osg::ref_ptr< osg::Geometry > createXY();

    /**
     * Helper function to create the ruler along the x axis
     *
     * \return A part of the geometry of the ruler
     */
    osg::ref_ptr< osg::Geometry > createXZ();

    /**
     * Helper function to create the ruler along the y axis
     *
     * \return A part of the geometry of the ruler
     */
    osg::ref_ptr< osg::Geometry > createYX();

    /**
     * Helper function to create the ruler along the y axis
     *
     * \return A part of the geometry of the ruler
     */
    osg::ref_ptr< osg::Geometry > createYZ();

    /**
     * Helper function to create the ruler along the z axis
     *
     * \return A part of the geometry of the ruler
     */
    osg::ref_ptr< osg::Geometry > createZX();

    /**
     * Helper function to create the ruler along the z axis
     *
     * \return A part of the geometry of the ruler
     */
    osg::ref_ptr< osg::Geometry > createZY();

    /**
     * converts a number into a string according to the currently selected coordinate system
     * \param number
     * \return string
     */
    std::string numberToString( int number );
};

#endif  // WRULERORTHO_H
