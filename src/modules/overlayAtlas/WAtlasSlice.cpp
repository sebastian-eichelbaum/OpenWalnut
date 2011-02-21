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

#include <iostream>
#include <string>

#include "../../common/WLogger.h"

#include "WAtlasSlice.h"

WAtlasSlice::WAtlasSlice( std::string path, WOASliceOrientation orientation, float position, float left, float bottom, float right, float top ) :
    m_path( path ),
    m_orientation( orientation ),
    m_position( position ),
    m_left( left ),
    m_bottom( bottom ),
    m_right( right ),
    m_top( top ),
    m_dirty( true )
{
    if ( loadImage() )
    {
        switch( orientation )
        {
            case OA_AXIAL:
                break;
            case OA_CORONAL:
                createTextureCoronal();
                break;
            case OA_SAGITTAL:
                break;
        }
    }
}

WAtlasSlice::~WAtlasSlice()
{
}

bool WAtlasSlice::loadImage()
{
    m_image = osgDB::readImageFile( m_path );

    if ( !m_image )
    {
        WLogger::getLogger()->addLogMessage( "Could't find image", "WAtlasSlice", LL_ERROR );
        return false;
    }
    return true;
}

osg::ref_ptr<osg::Texture3D> WAtlasSlice::getImage()
{
    return m_texture;
}

boost::shared_ptr<WGridRegular3D> WAtlasSlice::getGrid()
{
    if ( m_dirty )
    {
        createGridCoronal();
    }
    return m_grid;
}

void WAtlasSlice::createTextureCoronal()
{
    m_newImage = new osg::Image;

    m_newImage->allocateImage( m_image->s(), m_image->r(), m_image->t(), m_image->getPixelFormat(), m_image->getDataType() );

    unsigned char* data = m_image->data();
    unsigned char* newData = m_newImage->data();

    for ( unsigned int i = 0; i < m_image->getTotalSizeInBytes(); ++i )
    {
        newData[i] = data[i];
    }

    m_texture = osg::ref_ptr<osg::Texture3D>( new osg::Texture3D );
    m_texture->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
    m_texture->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );
    m_texture->setWrap( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER );
    m_texture->setWrap( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER );
    m_texture->setWrap( osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_BORDER );
    m_texture->setImage( m_newImage );
    m_texture->setResizeNonPowerOfTwoHint( false );
}

void WAtlasSlice::createGridCoronal()
{
    float sizeX = ( m_right - m_left ) / m_image->s();
    float sizeY = ( m_top - m_bottom ) / m_image->t();

    m_grid = boost::shared_ptr<WGridRegular3D>( new WGridRegular3D( m_newImage->s(), m_newImage->t(), m_newImage->r(),
            m_left, m_position, m_bottom, sizeX, 2.0, sizeY ) );

    m_dirty = false;
}

void WAtlasSlice::setPosition( float pos )
{
    m_position = pos;
    m_dirty = true;
}

void WAtlasSlice::setLeft( float left )
{
    m_left = left;
    m_dirty = true;
}

void WAtlasSlice::setBottom( float bottom )
{
    m_bottom = bottom;
    m_dirty = true;
}

void WAtlasSlice::setRight( float right )
{
    m_right = right;
    m_dirty = true;
}

void WAtlasSlice::setTop( float top )
{
    m_top = top;
    m_dirty = true;
}

