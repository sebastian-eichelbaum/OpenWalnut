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

#include <string>

#include <boost/filesystem.hpp>

#include <osg/Image>

#include "core/common/WColor.h"

/**
 * Image data object. Encapsulate 1D, 2D, and 3D images. Also provides static load function. This basically encapsulates an osg::Image. The
 * osg::Image can contain a huge variety of image data of different data types, formats, and so on.
 *
 * \note If the API of WGEImage is not sufficient, you can use getAsOSGImage() and use theirs.
 */
class WGEImage
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WGEImage >.
     */
    typedef boost::shared_ptr< WGEImage > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WGEImage >.
     */
    typedef boost::shared_ptr< const WGEImage > ConstSPtr;

    /**
     * Default constructor.
     */
    WGEImage();

    /**
     * Construct from a given osg::Image.
     *
     * \param image the image
     */
    explicit WGEImage( const osg::Image& image );

    /**
     * Copy construct from given image
     *
     * \param image the image
     */
    explicit WGEImage( const WGEImage& image );

    /**
     * Destructor.
     */
    virtual ~WGEImage();

    /**
     * Copy assignment operator.
     *
     * \param other the other instance
     *
     * \return this
     */
    WGEImage& operator= ( WGEImage other );

    /**
     * Load an image from a file. This is very fault tolerant. Just returns NULL on error.
     *
     * \param file the file to load
     *
     * \return the image. Can be NULL on error.
     */
    static WGEImage::SPtr createFromFile( boost::filesystem::path file );

    /**
     * Load an image from a file. This is very fault tolerant. Just returns NULL on error.
     *
     * \param file the file to load
     *
     * \return the image. Can be NULL on error.
     */
    static WGEImage::SPtr createFromFile( std::string file );

    /**
     * Get size in X direction.
     *
     * \return the width
     */
    int getWidth() const;

    /**
     * Get size in Y direction. This is 1 for 1D images.
     *
     * \return the height
     */
    int getHeight() const;

    /**
     * Get size in Z direction. This is 1 for 2D & 1D images.
     *
     * \return the depth
     */
    int getDepth() const;

    /**
     * Return the underlying osg::Image. Should rarely be used and is mostly useful when working directly with OSG.
     *
     * \return the osg::Image instance
     */
    osg::ref_ptr< osg::Image > getAsOSGImage() const;

    /**
     * Get the raw image data.
     *
     * \return the raw data.
     */
    unsigned char* data();

    /**
     * Get the raw image data.
     *
     * \return the raw data.
     */
    const unsigned char* data() const;

    /**
     * Grab color at specified pixel/voxel. Please note that you should consider the image's origin. Query with \ref getOrigin().
     *
     * \param x X coord
     * \param y Y coord, optional in 1D images
     * \param z Z coord, optional in 1D,2D images.
     *
     * \return color at given position
     */
    WColor getColor( unsigned int x, unsigned int y = 0, unsigned int z = 0 );

    /**
     * Where is the origin?
     */
    enum Origin
    {
        BOTTOM_LEFT = 0,    //!< bottom left origin
        TOP_LEFT            //!< top left origin
    };

    /**
     * Query origin.
     *
     * \return the origin.
     */
    Origin getOrigin() const;
protected:
private:
    /**
     * The osg image we use.
     */
    osg::ref_ptr< osg::Image > m_image;
};

#endif  // WGEIMAGE_H

