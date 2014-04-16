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

#include <algorithm>
#include <string>

#include <osgDB/ReadFile>

#include "core/common/WLogger.h"

#include "WGEImage.h"

WGEImage::WGEImage():
    m_image()
{
    // initialize members
}

WGEImage::WGEImage( const osg::Image& image ):
    m_image( new osg::Image( image, osg::CopyOp::DEEP_COPY_IMAGES ) )
{
}

WGEImage::WGEImage( const WGEImage& image ):
    m_image( new osg::Image( *image.m_image, osg::CopyOp::DEEP_COPY_IMAGES ) )
{
}

WGEImage& WGEImage::operator= ( WGEImage other )
{
    std::swap( m_image, other.m_image );
    return *this;
}

WGEImage::~WGEImage()
{
    // cleanup
}

WGEImage::SPtr WGEImage::createFromFile( std::string file )
{
    try
    {
        osg::ref_ptr< osg::Image > image = osgDB::readImageFile( file );
        if( !image )
        {
            wlog::error( "WGEImage" ) << "Failed to load image \"" << file << "\".";
            return WGEImage::SPtr();
        }
        return WGEImage::SPtr( new WGEImage( *image ) );
    }
    catch( ... )
    {
        wlog::error( "WGEImage" ) << "Failed to load image \"" << file << "\".";
    }
    return WGEImage::SPtr();
}

WGEImage::SPtr WGEImage::createFromFile( boost::filesystem::path file )
{
    return WGEImage::createFromFile( file.string() );
}

osg::ref_ptr< osg::Image > WGEImage::getAsOSGImage() const
{
    return m_image;
}

int WGEImage::getWidth() const
{
    return m_image->s();
}

int WGEImage::getHeight() const
{
    return m_image->t();
}

int WGEImage::getDepth() const
{
    return m_image->r();
}

unsigned char* WGEImage::data()
{
    return m_image->data();
}

const unsigned char* WGEImage::data() const
{
    return m_image->data();
}

WColor WGEImage::getColor( unsigned int x, unsigned int y, unsigned int z )
{
    return m_image->getColor( x, y, z );
}

WGEImage::Origin WGEImage::getOrigin() const
{
    switch( m_image->getOrigin() )
    {
        case osg::Image::BOTTOM_LEFT:
            return BOTTOM_LEFT;
        case osg::Image::TOP_LEFT:
            return TOP_LEFT;
    }

    return BOTTOM_LEFT;
}
