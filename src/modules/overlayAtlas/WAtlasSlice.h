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

#ifndef WATLASSLICE_H
#define WATLASSLICE_H

#include <string>

#include <osg/Image>
#include <osg/Texture3D>
#include <osgDB/ReadFile>

#include "../../dataHandler/WGridRegular3D.h"

typedef enum
{
    OA_AXIAL = 0,
    OA_CORONAL,
    OA_SAGITTAL
}
WOASliceOrientation;


/**
 * class encapsulates one slice of an overlay collection
 */
class WAtlasSlice
{
public:
    /**
     * standard constructor
     *
     * this loads an image from a file and creates a 3D texture object which is aligned in spase and positioned
     * according to additional info given
     *
     * \param path to the image file
     * \param orientation slice orientation ( axial, coronal or sagittal )
     * \param position
     * \param left boundary
     * \param bottom boundary
     * \param right boundary
     * \param top boundary
     */
    WAtlasSlice( std::string path, WOASliceOrientation orientation, float position, float left, float bottom, float right, float top );

    /**
     * destructor
     */
    ~WAtlasSlice();

    /**
     * getter
     *
     * \return the osg texture object
     */
    osg::ref_ptr<osg::Texture3D> getImage();

    /**
     * getter
     * \return the grid for the texture
     */
    boost::shared_ptr<WGridRegular3D>getGrid();

    /**
     * getter
     *
     * \return path to the image
     */
    std::string path();

    /**
     * getter
     *
     * \return
     */
    float position();

    /**
     * getter
     */
    float left();

    /**
     * getter
     */
    float bottom();

    /**
     * getter
     */
    float right();

    /**
     * getter
     */
    float top();

    /**
     * setter
     * \param pos position of the slice
     */
    void setPosition( float pos );

    /**
     * setter
     * \param left left boundary
     */
    void setLeft( float left );

    /**
     * setter
     * \param bottom lower boundary
     */
    void setBottom( float bottom );

    /**
     * setter
     * \param right right boundary
     */
    void setRight( float right );

    /**
     * setter
     * \param top upper boundary
     */
    void setTop( float top );



protected:
private:
    /**
     * helper function for loading an image from file
     * \return true if succesful
     */
    bool loadImage();

    /**
     * creates a 3D texture from the loaded image
     */
    void createTextureCoronal();

    /**
     * creates a grid for the texture
     */
    void createGridCoronal();

    std::string m_path; //!< path to the gfx

    osg::Image* m_image; //!< osg image object for the loaded image

    osg::Image* m_newImage; //!< osg image object for texture generation,

    WOASliceOrientation m_orientation; //!< slice orientation

    float m_position; //!< position

    float m_left; //!< left boundary

    float m_bottom; //!< lower boundary

    float m_right; //!< right boundary

    float m_top; //!< upper boundary

    boost::shared_ptr<WGridRegular3D>m_grid; //!< grid for the texture

    /**
     * stores a pointer to the texture we paint in
     */
    osg::ref_ptr<osg::Texture3D>m_texture;

    bool m_dirty; //!< dirty flag
};

inline std::string WAtlasSlice::path()
{
    return m_path;
}


inline float WAtlasSlice::position()
{
    return m_position;
}

inline float WAtlasSlice::left()
{
    return m_left;
}

inline float WAtlasSlice::bottom()
{
    return m_bottom;
}

inline float WAtlasSlice::right()
{
    return m_right;
}

inline float WAtlasSlice::top()
{
    return m_top;
}


#endif  // WATLASSLICE_H
