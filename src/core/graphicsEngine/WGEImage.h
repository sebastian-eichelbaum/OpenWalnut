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

#ifndef WGEIMAGE_H
#define WGEIMAGE_H

#include <boost/filesystem.hpp>

#include <osg/Image>

/**
 * Image data object. Encapsulate 1D, 2D, and 3D images. Also provides static load function.
 */
class WGEImage: public osg::Image
{
public:
    /**
     * Convenience typedef for a osg::ref_ptr< WGEImage >.
     */
    typedef osg::ref_ptr< WGEImage > SPtr;

    /**
     * Convenience typedef for a osg::ref_ptr< const WGEImage >.
     */
    typedef osg::ref_ptr< const WGEImage > ConstSPtr;

    /**
     * Default constructor.
     */
    WGEImage();

    /**
     * Copy construct from a given osg::Image.
     *
     * \param image the image
     */
    WGEImage( const osg::Image& image );

    /**
     * Destructor.
     */
    virtual ~WGEImage();

    /**
     * Load an image from a file. As we use the osgDB::readImageFile functions and the DOC is horrible, we cannot tell you whether this throws an
     * exception or not. But please check the returned pointer for validity.
     *
     * \param file the file to load
     *
     * \return the image.
     */
    static WGEImage* loadFromFile( boost::filesystem::path file );

protected:
private:
};

#endif  // WGEIMAGE_H

