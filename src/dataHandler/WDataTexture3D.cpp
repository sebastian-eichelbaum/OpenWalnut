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
#include <limits>
#include <vector>

#include "WDataSetSingle.h"
#include "../common/WLogger.h"
#include "../common/WLimits.h"
#include "../common/WCondition.h"

#include "../graphicsEngine/WGETexture.h"
#include "WDataTexture3D.h"

WDataTexture3D::WDataTexture3D( boost::shared_ptr<WValueSetBase> valueSet, boost::shared_ptr<WGrid> grid ):
    m_properties( boost::shared_ptr< WProperties >( new WProperties( "Data Texture Properties", "Properties of a texture." ) ) ),
    m_infoProperties( boost::shared_ptr< WProperties >( new WProperties( "Data Texture Info Properties", "Texture's information properties." ) ) ),
    m_alpha( 1.0 ),
    m_threshold( 0.0 ),
    m_texture( osg::ref_ptr< WGETexture3D >() ),
    m_valueSet( valueSet ),
    m_grid( boost::shared_dynamic_cast< WGridRegular3D >( grid ) ),
    m_changeCondition( new WCondition() ),
    m_globalActive( true ),
    m_interpolation( true ),
    m_cmap( 0 ),
    m_minValue( static_cast< float >( valueSet->getMinimumValue() ) ),
    m_maxValue( static_cast< float >( valueSet->getMaximumValue() ) ),
    m_scale( m_maxValue - m_minValue )
{
    // initialize members
    wlog::debug( "WDataTexture3D" ) << "Texture scaling information for data in [" << m_minValue << ", "<< m_maxValue <<
                                       "]: scaling factor=" << m_scale;
    wlog::debug( "WDataTexture3D" ) << "Resolution is " <<  m_grid->getNbCoordsX() << "x" << m_grid->getNbCoordsY() << "x" << m_grid->getNbCoordsZ();
    WAssert( m_grid->getNbCoordsX() <= wlimits::MAX_TEXTURE_DIMENSION, "Cannot create a texture with more"
                                                                       " than 2048 pixels/voxels in one dimension." );
    WAssert( m_grid->getNbCoordsY() <= wlimits::MAX_TEXTURE_DIMENSION, "Cannot create a texture with more"
                                                                       " than 2048 pixels/voxels in one dimension." );
    WAssert( m_grid->getNbCoordsZ() <= wlimits::MAX_TEXTURE_DIMENSION, "Cannot create a texture with more"
                                                                       " than 2048 pixels/voxels in one dimension." );
}

WDataTexture3D::~WDataTexture3D()
{
    // cleanup
}


float WDataTexture3D::getAlpha() const
{
    return m_alpha;
}

void WDataTexture3D::setOpacity( float opacity )
{
    setAlpha( opacity / 100.0 );
}

void WDataTexture3D::setAlpha( float alpha )
{
    if ( ( alpha > 1.0 ) || ( alpha < 0.0 ) )
    {
        return;
    }

    m_alpha = alpha;
    notifyChange();
}

float WDataTexture3D::getThreshold() const
{
    return m_threshold;
}

void WDataTexture3D::setThreshold( float threshold )
{
    m_threshold = threshold;
    notifyChange();
}

bool WDataTexture3D::isGloballyActive()
{
    return m_globalActive;
}

void WDataTexture3D::setGloballyActive( bool active )
{
    if ( active != m_globalActive )
    {
        m_globalActive = active;
        notifyChange();
    }
}

boost::shared_ptr< WGridRegular3D > WDataTexture3D::getGrid() const
{
    return m_grid;
}

osg::ref_ptr< WGETexture3D > WDataTexture3D::getTexture()
{
    createTexture();
    return m_texture;
}

osg::ref_ptr< osg::Image > WDataTexture3D::createTexture3D( unsigned char* source, int components )
{
    osg::ref_ptr< osg::Image > ima = new osg::Image;

    if ( components == 1 )
    {
        wlog::debug( "WDataTexture3D" ) << "Texture for scalar char data set.";
        ima->allocateImage( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(), GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE );

        unsigned char* data = ima->data();

        for ( unsigned int i = 0; i < m_grid->getNbCoordsX() * m_grid->getNbCoordsY() * m_grid->getNbCoordsZ(); ++i )
        {
            data[2*i] = source[i];
            if( source[i] == m_minValue )
            {
                data[2*i + 1] =  0;
            }
            else
            {
                data[2*i + 1] =  255;
            }
        }
    }
    else if ( components == 3 )
    {
        wlog::debug( "WDataTexture3D" ) << "Texture for vector char data set.";
        ima->allocateImage( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(), GL_RGB, GL_UNSIGNED_BYTE );

        unsigned char* data = ima->data();

        for ( unsigned int i = 0; i < m_grid->getNbCoordsX() * m_grid->getNbCoordsY() * m_grid->getNbCoordsZ() * 3; ++i )
        {
            data[i] = source[i];
        }
    }
    else
    {
        // TODO(schurade): throw exception if components!=1 or 3
        wlog::error( "WDataTexture3D" ) << "Did not handle ( components != 1 ||  components != 3 ).";
    }

    return ima;
}

osg::ref_ptr< osg::Image > WDataTexture3D::createTexture3D( int16_t* source, int components )
{
    osg::ref_ptr< osg::Image > ima = new osg::Image;
    if( components == 1 )
    {
        wlog::debug( "WDataTexture3D" ) << "Texture for scalar int16 data set.";
        int nSize = m_grid->getNbCoordsX() * m_grid->getNbCoordsY() * m_grid->getNbCoordsZ();

        std::vector<int16_t> tempSource( nSize );

        //compute maximum of tempSource and build its histogram.
        for ( int i = 0; i < nSize; ++i )
        {
            tempSource[i] = source[i];
        }

        int max = 0;
        std::vector< int > histo( 65536, 0 );
        for ( int i = 0; i < nSize; ++i )
        {
            if ( max < tempSource[i])
            {
                max = tempSource[i];
            }
            ++histo[tempSource[i]];
        }
        int percentage = static_cast< int >( nSize * 0.001 );

        int newMax = 65535; // lower bound of the highest "percentage" values
        int adder = 0;

        // Find lower bound of the highest "percentage" values
        for ( int i = 65535; i > 0; --i )
        {
            adder += histo[i];
            newMax = i;
            if ( adder > percentage )
                break;
        }

        // Set all values that are above the newMax bound to newMax
        for ( int i = 0; i < nSize; ++i )
        {
            if ( tempSource[i] > newMax )
            {
                tempSource[i] = newMax;
            }
        }

        int mult = 65535 / newMax;
        for ( int i = 0; i < nSize; ++i )
        {
            tempSource[i] *= mult;
        }

        ima->allocateImage( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(), GL_LUMINANCE_ALPHA, GL_UNSIGNED_SHORT );

        unsigned char* data = ima->data();

        unsigned char* charSource = reinterpret_cast< unsigned char* >( &tempSource[0] );

        for ( int i = 0; i < nSize ; ++i )
        {
            data[4*i] = charSource[2*i];
            data[4*i+1] = charSource[2*i+1];
            if( source[i] == m_minValue )
            {
                data[4*i + 2] =  0;
                data[4*i + 3] =  0;
            }
            else
                data[4*i + 2] =  255;
                data[4*i + 3] =  255;
        }
    }
    else
    {
        // TODO(schurade): throw exception if components!=1
        wlog::error( "WDataTexture3D" ) << "Did not handle ( components != 1 ).";
    }
    return ima;
}

osg::ref_ptr< osg::Image > WDataTexture3D::createTexture3D( int* source, int components )
{
    osg::ref_ptr< osg::Image > ima = new osg::Image;
    if( components == 1 )
    {
        wlog::debug( "WDataTexture3D" ) << "Texture for scalar int data set.";
        int nSize = m_grid->getNbCoordsX() * m_grid->getNbCoordsY() * m_grid->getNbCoordsZ();

        std::vector<float> tempSource( nSize );

        for ( int i = 0; i < nSize; ++i )
        {
            tempSource[i] = static_cast<float>( source[i] );
        }

        // OpenGL just supports float textures
        ima->allocateImage( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(), GL_LUMINANCE_ALPHA, GL_FLOAT );
        float* data = reinterpret_cast< float* >( ima->data() );

        // Copy the data pixel wise and convert to float
        for ( unsigned int i = 0; i < m_grid->getNbCoordsX() * m_grid->getNbCoordsY() * m_grid->getNbCoordsZ() ; ++i )
        {
            data[2*i] = scaleInterval( tempSource[i] );
            if( tempSource[i] == m_minValue )
            {
                data[2*i + 1] =  0.0;
            }
            else
                data[2*i + 1] =  1.0;
        }
    }
    else
    {
        // TODO(schurade): throw exception if components!=1
        wlog::error( "WDataTexture3D" ) << "Did not handle ( components != 1 ).";
    }
    return ima;
}


osg::ref_ptr< osg::Image > WDataTexture3D::createTexture3D( float* source, int components )
{
    osg::ref_ptr< osg::Image > ima = new osg::Image;
    if ( components == 1)
    {
        wlog::debug( "WDataTexture3D" ) << "Texture for scalar float data set.";
        // OpenGL just supports float textures
        ima->allocateImage( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(), GL_LUMINANCE_ALPHA, GL_FLOAT );
        float* data = reinterpret_cast< float* >( ima->data() );

        // Copy the data pixel wise and convert to float
        for ( unsigned int i = 0; i < m_grid->getNbCoordsX() * m_grid->getNbCoordsY() * m_grid->getNbCoordsZ() ; ++i )
        {
            data[2*i] = scaleInterval( source[i] );
            if( source[i] == m_minValue )
            {
                data[2*i + 1] =  0.0;
            }
            else
            {
                data[2*i + 1] =  1.0;
            }
        }
    }
    else if ( components == 3 )
    {
        wlog::debug( "WDataTexture3D" ) << "Texture for 3-vector float data set.";
        // OpenGL just supports float textures
        ima->allocateImage( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(), GL_RGBA, GL_FLOAT );
        ima->setInternalTextureFormat( GL_RGBA );
        float* data = reinterpret_cast< float* >( ima->data() );

        // Copy the data pixel wise and convert to float
        for ( unsigned int i = 0; i < m_grid->getNbCoordsX() * m_grid->getNbCoordsY() * m_grid->getNbCoordsZ() ; ++i )
        {
            data[ ( 4 * i ) ]     = fabs( scaleInterval( source[ ( 3 * i ) ] ) );
            data[ ( 4 * i ) + 1 ] = fabs( scaleInterval( source[ ( 3 * i ) + 1 ] ) );
            data[ ( 4 * i ) + 2 ] = fabs( scaleInterval( source[ ( 3 * i ) + 2 ] ) );
            data[ ( 4 * i ) + 3 ] = 1.0;
        }
    }
    else if ( components == 4 )
    {
        wlog::debug( "WDataTexture3D" ) << "Texture for 4-vector float data set.";
        // OpenGL just supports float textures
        ima->allocateImage( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(), GL_RGBA, GL_FLOAT );
        ima->setInternalTextureFormat( GL_RGBA );
        float* data = reinterpret_cast< float* >( ima->data() );

        // Copy the data pixel wise and convert to float
        for ( unsigned int i = 0; i < m_grid->getNbCoordsX() * m_grid->getNbCoordsY() * m_grid->getNbCoordsZ() ; ++i )
        {
            data[ ( 4 * i ) ]     = scaleInterval( source[ ( 4 * i ) ] );
            data[ ( 4 * i ) + 1 ] = scaleInterval( source[ ( 4 * i ) + 1 ] );
            data[ ( 4 * i ) + 2 ] = scaleInterval( source[ ( 4 * i ) + 2 ] );
            data[ ( 4 * i ) + 3 ] = scaleInterval( source[ ( 4 * i ) + 3 ] );
        }
    }
    else
    {
        // TODO(schurade): throw exception if components!=1
        wlog::error( "WDataTexture3D" ) << "Did not handle dataset ( components != 1,3 or 4 ).";
    }

    return ima;
}

osg::ref_ptr< osg::Image > WDataTexture3D::createTexture3D( double* source, int components )
{
    osg::ref_ptr< osg::Image > ima = new osg::Image;
    if ( components == 1)
    {
        // OpenGL just supports float textures
        ima->allocateImage( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(), GL_LUMINANCE_ALPHA, GL_FLOAT );
        float* data = reinterpret_cast< float* >( ima->data() );

        // Copy the data pixel wise and convert to float
        for ( unsigned int i = 0; i < m_grid->getNbCoordsX() * m_grid->getNbCoordsY() * m_grid->getNbCoordsZ() ; ++i )
        {
            data[2*i] = scaleInterval( static_cast< float >( source[i] ) );
            if( source[i] == m_minValue )
            {
                data[2*i + 1] =  0.0;
            }
            else
            {
                data[2*i + 1] =  1.0;
            }
            //std::cout << static_cast< float >( source[i] ) << " - " << data[i] << std::endl;
        }
    }
    else if ( components == 3)
    {
        // OpenGL just supports float textures
        ima->allocateImage( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(), GL_RGBA, GL_FLOAT );
        ima->setInternalTextureFormat( GL_RGBA );
        float* data = reinterpret_cast< float* >( ima->data() );

        // Copy the data pixel wise and convert to float
        for ( unsigned int i = 0; i < m_grid->getNbCoordsX() * m_grid->getNbCoordsY() * m_grid->getNbCoordsZ() ; ++i )
        {
            data[ ( 4 * i ) ]     = scaleInterval( static_cast< float >( source[ ( 3 * i ) ] ) );
            data[ ( 4 * i ) + 1 ] = scaleInterval( static_cast< float >( source[ ( 3 * i ) + 1 ] ) );
            data[ ( 4 * i ) + 2 ] = scaleInterval( static_cast< float >( source[ ( 3 * i ) + 2 ] ) );
            data[ ( 4 * i ) + 3 ] = 1.0;
        }
    }
    else if ( components == 4)
    {
        // OpenGL just supports float textures
        ima->allocateImage( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(), GL_RGBA, GL_FLOAT );
        ima->setInternalTextureFormat( GL_RGBA );
        float* data = reinterpret_cast< float* >( ima->data() );

        // Copy the data pixel wise and convert to float
        for ( unsigned int i = 0; i < m_grid->getNbCoordsX() * m_grid->getNbCoordsY() * m_grid->getNbCoordsZ() ; ++i )
        {
            data[ ( 4 * i ) ]     = scaleInterval( static_cast< float >( source[ ( 4 * i ) ] ) );
            data[ ( 4 * i ) + 1 ] = scaleInterval( static_cast< float >( source[ ( 4 * i ) + 1 ] ) );
            data[ ( 4 * i ) + 2 ] = scaleInterval( static_cast< float >( source[ ( 4 * i ) + 2 ] ) );
            data[ ( 4 * i ) + 3 ] = scaleInterval( static_cast< float >( source[ ( 4 * i ) + 3 ] ) );
        }
    }
    else
    {
        // TODO(schurade): throw exception if components!=1
        wlog::error( "WDataTexture3D" ) << "Did not handle dataset ( components != 1,3 or 4 ).";
    }

    return ima;
}

void WDataTexture3D::createTexture()
{
    WAssert( m_minValue <= m_maxValue, "" );
    boost::unique_lock< boost::shared_mutex > lock( m_creationLock );
    if ( !m_texture )
    {
        osg::ref_ptr< osg::Image > ima;

        if ( m_valueSet->getDataType() == W_DT_UINT8 )
        {
            wlog::debug( "WDataTexture3D" ) << "Handling W_DT_UINT8";
            boost::shared_ptr< WValueSet< uint8_t > > vs = boost::shared_dynamic_cast< WValueSet< uint8_t > >( m_valueSet );
            uint8_t* source = const_cast< uint8_t* > ( vs->rawData() );
            ima = createTexture3D( source, m_valueSet->dimension() );
        }
        else if ( m_valueSet->getDataType() == W_DT_INT16 )
        {
            wlog::debug( "WDataTexture3D" ) << "Handling W_DT_INT16";
            boost::shared_ptr< WValueSet< int16_t > > vs = boost::shared_dynamic_cast< WValueSet< int16_t > >( m_valueSet );
            int16_t* source = const_cast< int16_t* > ( vs->rawData() );
            ima = createTexture3D( source, m_valueSet->dimension() );
        }
        else if ( m_valueSet->getDataType() == W_DT_SIGNED_INT )
        {
            wlog::debug( "WDataTexture3D" ) << "Handling W_DT_SIGNED_INT";
            boost::shared_ptr< WValueSet< int32_t > > vs = boost::shared_dynamic_cast< WValueSet< int32_t > >( m_valueSet );
            int* source = const_cast< int* > ( vs->rawData() );
            ima = createTexture3D( source, m_valueSet->dimension() );
        }
        else if ( m_valueSet->getDataType() == W_DT_FLOAT )
        {
            wlog::debug( "WDataTexture3D" ) << "Handling W_DT_FLOAT";
            boost::shared_ptr< WValueSet< float > > vs = boost::shared_dynamic_cast< WValueSet< float > >( m_valueSet );
            float* source = const_cast< float* > ( vs->rawData() );
            ima = createTexture3D( source, m_valueSet->dimension() );
        }
        else if ( m_valueSet->getDataType() == W_DT_DOUBLE )
        {
            wlog::debug( "WDataTexture3D" ) << "Handling W_DT_DOUBLE";
            boost::shared_ptr< WValueSet< double > > vs = boost::shared_dynamic_cast< WValueSet< double > >( m_valueSet );
            double* source = const_cast< double* > ( vs->rawData() );
            ima = createTexture3D( source, m_valueSet->dimension() );
        }
        else
        {
            wlog::debug( "WDataTexture3D" ) << "Handling" << m_valueSet->getDataType();
            wlog::error( "WDataTexture3D" ) << "Conversion of this data type to texture not supported yet.";
        }

        m_texture = osg::ref_ptr< WGETexture3D >( new WGETexture3D( m_scale, m_minValue ) );
        m_texture->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
        m_texture->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );
        m_texture->setWrap( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER );
        m_texture->setWrap( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER );
        m_texture->setWrap( osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_BORDER );
        m_texture->setImage( ima );
        m_texture->setResizeNonPowerOfTwoHint( false );
        m_texture->setTextureSize( ima->s(), ima->t(), ima->r() );
    }
    lock.unlock();
}

dataType WDataTexture3D::getDataType()
{
    return m_valueSet->getDataType();
}

boost::shared_ptr< WCondition > WDataTexture3D::getChangeCondition()
{
    return m_changeCondition;
}

void WDataTexture3D::notifyChange()
{
    m_changeCondition->notify();
}

float WDataTexture3D::getMinValue()
{
    createTexture();
    return m_minValue;
}

float WDataTexture3D::getMaxValue()
{
    createTexture();
    return m_maxValue;
}

void WDataTexture3D::setMinValue( float min )
{
    m_minValue = min;
    m_scale = m_maxValue - m_minValue;
}

void WDataTexture3D::setMaxValue( float max )
{
    m_maxValue = max;
    m_scale = m_maxValue - m_minValue;
}

float WDataTexture3D::getMinMaxScale()
{
    createTexture();
    return m_scale;
}

float WDataTexture3D::scaleInterval( float value ) const
{
    //return value;
    return ( std::min( std::max( value, m_minValue ), m_maxValue ) - m_minValue ) / m_scale;
}

bool WDataTexture3D::isInterpolated()
{
    return m_interpolation;
}

void WDataTexture3D::setInterpolation( bool interpol )
{
    m_interpolation = interpol;
    notifyChange();
}

int WDataTexture3D::getSelectedColormap()
{
    return m_cmap;
}

void WDataTexture3D::setSelectedColormap( int cmap )
{
    m_cmap = cmap;
    notifyChange();
}

boost::shared_ptr< WProperties > WDataTexture3D::getProperties() const
{
    return m_properties;
}

boost::shared_ptr< WProperties > WDataTexture3D::getInformationProperties() const
{
    return m_infoProperties;
}

