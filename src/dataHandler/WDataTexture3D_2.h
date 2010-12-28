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

#ifndef WDATATEXTURE3D_2_H
#define WDATATEXTURE3D_2_H

#include <algorithm>

#include <boost/shared_ptr.hpp>

#include "../graphicsEngine/WGETexture.h"
#include "../graphicsEngine/WGETypeTraits.h"
#include "../common/WProperties.h"

#include "WValueSetBase.h"
#include "WGridRegular3D.h"

#include "WExportDataHandler.h"

/**
 * This class allows simple creation of WGETexture3D by using a specified grid and value-set. It also automates grid changes. Ony advantage: the
 * first call to the texture's update callback ensures texture creation. It is not created earlier.
 */
class OWDATAHANDLER_EXPORT WDataTexture3D_2: public WGETexture3D
{
public:

    /**
     * Constructor. Creates the texture. Just run it after graphics engine was initialized.
     *
     * \param valueSet  the value set to use
     * \param grid the grid to use
     */
     WDataTexture3D_2( boost::shared_ptr< WValueSetBase > valueSet, boost::shared_ptr< WGridRegular3D > grid );

    /**
     * Destructor.
     */
    virtual ~WDataTexture3D_2();

    /**
     * The texture got created using this grid. It can help to translate coordinates into the texture coordinate space relating to
     * the grid.
     *
     * \return the grid.
     */
    boost::shared_ptr< WGridRegular3D > getGrid() const;

    /**
     * Returns the matrix used for transforming the texture coordinates to match the texture. This method calculates the correct texture matrix
     * by using the grid's transformation.
     *
     * \return the matrix allowing direct application to osg::TexMat.
     */
    virtual osg::Matrix getTexMatrix() const;

protected:

    /**
     * Scales the specified value to the interval [0,1] using m_min and m_scale. As the method is inline, the additional parameters are no
     * problem.
     *
     * \param value the value to scale
     * \param minimum the min value
     * \param maximum the max value
     * \param scale the scaler
     *
     * \return the value scaled to [0,1]
     */
    template < typename T >
    T scaleInterval( T value, T minimum, T maximum, float scale ) const;

    /**
     * Creates the texture data. This method creates the texture during the first update traversal using the value set and grid.
     */
    virtual void create();

private:

    /**
     * The value set from which the texture gets created.
     */
    boost::shared_ptr<WValueSetBase> m_valueSet;

    /**
     * The grid used to set up the texture.
     */
    boost::shared_ptr< WGridRegular3D > m_grid;

    /**
     * The lock for securing createTexture.
     */
    boost::shared_mutex m_creationLock;

    /**
     * Creates a properly sized osg::Image from the specified source data.
     *
     * \param source the source data
     * \param components number of components
     * \tparam T the type of source data
     *
     * \return
     */
    template < typename T >
    osg::ref_ptr< osg::Image > createTexture( T* source, int components = 1 );
};

template < typename T >
osg::ref_ptr< osg::Image > WDataTexture3D_2::createTexture( T* source, int components )
{
    // get lock
    boost::unique_lock< boost::shared_mutex > lock( m_creationLock );

    // get the current scaling info
    T min = static_cast< T >( minimum()->get() );
    float scale = static_cast< float >( minimum()->get() );
    T max = min + static_cast< T >( scale );

    typedef typename wge::GLType< T >::Type TexType;
    GLenum type = wge::GLType< T >::TypeEnum;

    wlog::debug( "WDataTexture3D_2" ) << "Resolution: " << m_grid->getNbCoordsX() << "x" << m_grid->getNbCoordsY() << "x" << m_grid->getNbCoordsZ();
    wlog::debug( "WDataTexture3D_2" ) << "Colordepth: " << components;

    osg::ref_ptr< osg::Image > ima = new osg::Image;
    if ( components == 1)
    {
        // OpenGL just supports float textures
        ima->allocateImage( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(), GL_LUMINANCE, type );
        TexType* data = reinterpret_cast< TexType* >( ima->data() );

        // Copy the data pixel wise and convert to float
        for ( unsigned int i = 0; i < m_grid->getNbCoordsX() * m_grid->getNbCoordsY() * m_grid->getNbCoordsZ() ; ++i )
        {
            data[i] = scaleInterval( source[i], min, max, scale );
        }
    }
    else if ( components == 3)
    {
        // OpenGL just supports float textures
        ima->allocateImage( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(), GL_RGBA, type );
        ima->setInternalTextureFormat( GL_RGBA );
        TexType* data = reinterpret_cast< TexType* >( ima->data() );

        // Copy the data pixel wise and convert to float
        for ( unsigned int i = 0; i < m_grid->getNbCoordsX() * m_grid->getNbCoordsY() * m_grid->getNbCoordsZ() ; ++i )
        {
            data[ ( 4 * i ) ]     = scaleInterval( source[ ( 3 * i ) ], min, max, scale );
            data[ ( 4 * i ) + 1 ] = scaleInterval( source[ ( 3 * i ) + 1 ], min, max, scale );
            data[ ( 4 * i ) + 2 ] = scaleInterval( source[ ( 3 * i ) + 2 ], min, max, scale );
            data[ ( 4 * i ) + 3 ] = 1.0;
        }
    }
    else if ( components == 4)
    {
        // OpenGL just supports float textures
        ima->allocateImage( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(), GL_RGBA, type );
        ima->setInternalTextureFormat( GL_RGBA );
        TexType* data = reinterpret_cast< TexType* >( ima->data() );

        // Copy the data pixel wise and convert to float
        for ( unsigned int i = 0; i < m_grid->getNbCoordsX() * m_grid->getNbCoordsY() * m_grid->getNbCoordsZ() ; ++i )
        {
            data[ ( 4 * i ) ]     = scaleInterval( source[ ( 4 * i ) ], min, max, scale );
            data[ ( 4 * i ) + 1 ] = scaleInterval( source[ ( 4 * i ) + 1 ], min, max, scale );
            data[ ( 4 * i ) + 2 ] = scaleInterval( source[ ( 4 * i ) + 2 ], min, max, scale );
            data[ ( 4 * i ) + 3 ] = scaleInterval( source[ ( 4 * i ) + 3 ], min, max, scale );
        }
    }
    else
    {
        wlog::error( "WDataTexture3D" ) << "Did not handle dataset ( components != 1,3 or 4 ).";
    }

    // done, unlock
    lock.unlock();

    return ima;
}

template < typename T >
inline T WDataTexture3D_2::scaleInterval( T value, T minimum, T maximum, float scale ) const
{
    return static_cast< float >( std::min( std::max( value, minimum ), maximum ) - minimum ) / scale;
}

#endif  // WDATATEXTURE3D_2_H

