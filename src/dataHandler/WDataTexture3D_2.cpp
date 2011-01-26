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

#include "WValueSet.h"

#include "../graphicsEngine/WGETextureUtils.h"

#include "WDataTexture3D_2.h"

WDataTexture3D_2::WDataTexture3D_2( boost::shared_ptr< WValueSetBase > valueSet, boost::shared_ptr< WGridRegular3D > grid ):
    WGETexture3D( static_cast< float >( valueSet->getMaximumValue() - valueSet->getMinimumValue() ),
                  static_cast< float >( valueSet->getMinimumValue() ) ),
    m_valueSet( valueSet ),
    m_grid( grid )
{
    // initialize members
    setTextureSize( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ() );

    // data textures do not repeat or something
    setWrap( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER );
    setWrap( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER );
    setWrap( osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_BORDER );

    threshold()->setMin( valueSet->getMinimumValue() );
    threshold()->setMax( valueSet->getMaximumValue() );
    threshold()->set( valueSet->getMinimumValue() );

    // subscribe transformation update callback
    m_transformationUpdateConnection = m_grid->getTransformationUpdateCondition()->subscribeSignal(
        boost::bind( &WDataTexture3D_2::updateTransform, this )
    );
    transformation()->set( m_grid->getWorldToTexMatrix() );
}

WDataTexture3D_2::~WDataTexture3D_2()
{
    // cleanup
}

void WDataTexture3D_2::updateTransform()
{
    transformation()->set( m_grid->getWorldToTexMatrix() );
}

void WDataTexture3D_2::create()
{
    osg::ref_ptr< osg::Image > ima;

    if ( m_valueSet->getDataType() == W_DT_UINT8 )
    {
        wlog::debug( "WDataTexture3D_2" ) << "Creating Texture of type W_DT_UINT8";
        boost::shared_ptr< WValueSet< uint8_t > > vs = boost::shared_dynamic_cast< WValueSet< uint8_t > >( m_valueSet );
        uint8_t* source = const_cast< uint8_t* > ( vs->rawData() );
        ima = createTexture( source, m_valueSet->dimension() );
    }
    else if ( m_valueSet->getDataType() == W_DT_INT8 )
    {
        wlog::debug( "WDataTexture3D_2" ) << "Creating Texture of type W_DT_INT8";
        boost::shared_ptr< WValueSet< int8_t > > vs = boost::shared_dynamic_cast< WValueSet< int8_t > >( m_valueSet );
        int8_t* source = const_cast< int8_t* > ( vs->rawData() );
        ima = createTexture( source, m_valueSet->dimension() );
    }
    else if ( m_valueSet->getDataType() == W_DT_INT16 )
    {
        wlog::debug( "WDataTexture3D_2" ) << "Creating Texture of type W_DT_INT16";
        boost::shared_ptr< WValueSet< int16_t > > vs = boost::shared_dynamic_cast< WValueSet< int16_t > >( m_valueSet );
        int16_t* source = const_cast< int16_t* > ( vs->rawData() );
        ima = createTexture( source, m_valueSet->dimension() );
    }
    else if ( m_valueSet->getDataType() == W_DT_UINT16 )
    {
        wlog::debug( "WDataTexture3D_2" ) << "Creating Texture of type W_DT_UINT16";
        boost::shared_ptr< WValueSet< uint16_t > > vs = boost::shared_dynamic_cast< WValueSet< uint16_t > >( m_valueSet );
        uint16_t* source = const_cast< uint16_t* > ( vs->rawData() );
        ima = createTexture( source, m_valueSet->dimension() );
    }
    else if ( m_valueSet->getDataType() == W_DT_SIGNED_INT )
    {
        wlog::debug( "WDataTexture3D_2" ) << "Creating Texture of type W_DT_SIGNED_INT";
        boost::shared_ptr< WValueSet< int32_t > > vs = boost::shared_dynamic_cast< WValueSet< int32_t > >( m_valueSet );
        int* source = const_cast< int* > ( vs->rawData() );
        ima = createTexture( source, m_valueSet->dimension() );
    }
    else if ( m_valueSet->getDataType() == W_DT_FLOAT )
    {
        wlog::debug( "WDataTexture3D_2" ) << "Creating Texture of type W_DT_FLOAT";
        boost::shared_ptr< WValueSet< float > > vs = boost::shared_dynamic_cast< WValueSet< float > >( m_valueSet );
        float* source = const_cast< float* > ( vs->rawData() );
        ima = createTexture( source, m_valueSet->dimension() );
    }
    else if ( m_valueSet->getDataType() == W_DT_DOUBLE )
    {
        wlog::debug( "WDataTexture3D_2" ) << "Creating Texture of type W_DT_DOUBLE";
        boost::shared_ptr< WValueSet< double > > vs = boost::shared_dynamic_cast< WValueSet< double > >( m_valueSet );
        double* source = const_cast< double* > ( vs->rawData() );
        ima = createTexture( source, m_valueSet->dimension() );
    }
    else
    {
        wlog::debug( "WDataTexture3D_2" ) << "Creating Texture of type" << m_valueSet->getDataType();
        wlog::error( "WDataTexture3D_2" ) << "Conversion of this data type to texture not supported yet.";
    }

    setImage( ima );
    dirtyTextureObject();
}

void wge::bindTexture( osg::ref_ptr< osg::Node > node, osg::ref_ptr< WDataTexture3D_2 > texture, size_t unit, std::string prefix )
{
    wge::bindTexture( node, osg::ref_ptr< WGETexture3D >( texture ), unit, prefix );
}

boost::shared_ptr< WGridRegular3D > WDataTexture3D_2::getGrid() const
{
    return m_grid;
}

