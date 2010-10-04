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

#include "WDataTexture3D_2.h"

WDataTexture3D_2::WDataTexture3D_2( boost::shared_ptr< WValueSetBase > valueSet, boost::shared_ptr< WGridRegular3D > grid ):
    WGETexture3D( static_cast< float >( valueSet->getMinimumValue() ),
                  static_cast< float >( valueSet->getMaximumValue() - valueSet->getMinimumValue() ) ),
    m_valueSet( valueSet ),
    m_grid( grid )
{
    // initialize members

    // data textures do not repeat or something
    setWrap( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER );
    setWrap( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER );
    setWrap( osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_BORDER );

    osg::ref_ptr< osg::Image > ima;

    if ( m_valueSet->getDataType() == W_DT_UINT8 )
    {
        wlog::debug( "WDataTexture3D_2" ) << "Handling W_DT_UINT8";
        boost::shared_ptr< WValueSet< unsigned char > > vs = boost::shared_dynamic_cast< WValueSet< unsigned char > >( m_valueSet );
        unsigned char* source = const_cast< unsigned char* > ( vs->rawData() );
        ima = createTexture( source, m_valueSet->dimension() );
    }
    else if ( m_valueSet->getDataType() == W_DT_INT16 )
    {
        wlog::debug( "WDataTexture3D_2" ) << "Handling W_DT_INT16";
        boost::shared_ptr< WValueSet< int16_t > > vs = boost::shared_dynamic_cast< WValueSet< int16_t > >( m_valueSet );
        int16_t* source = const_cast< int16_t* > ( vs->rawData() );
        ima = createTexture( source, m_valueSet->dimension() );
    }
    else if ( m_valueSet->getDataType() == W_DT_SIGNED_INT )
    {
        wlog::debug( "WDataTexture3D_2" ) << "Handling W_DT_SIGNED_INT";
        boost::shared_ptr< WValueSet< int32_t > > vs = boost::shared_dynamic_cast< WValueSet< int32_t > >( m_valueSet );
        int* source = const_cast< int* > ( vs->rawData() );
        ima = createTexture( source, m_valueSet->dimension() );
    }
    else if ( m_valueSet->getDataType() == W_DT_FLOAT )
    {
        wlog::debug( "WDataTexture3D_2" ) << "Handling W_DT_FLOAT";
        boost::shared_ptr< WValueSet< float > > vs = boost::shared_dynamic_cast< WValueSet< float > >( m_valueSet );
        float* source = const_cast< float* > ( vs->rawData() );
        ima = createTexture( source, m_valueSet->dimension() );
    }
    else if ( m_valueSet->getDataType() == W_DT_DOUBLE )
    {
        wlog::debug( "WDataTexture3D_2" ) << "Handling W_DT_DOUBLE";
        boost::shared_ptr< WValueSet< double > > vs = boost::shared_dynamic_cast< WValueSet< double > >( m_valueSet );
        double* source = const_cast< double* > ( vs->rawData() );
        ima = createTexture( source, m_valueSet->dimension() );
    }
    else
    {
        wlog::debug( "WDataTexture3D_2" ) << "Handling" << m_valueSet->getDataType();
        wlog::error( "WDataTexture3D_2" ) << "Conversion of this data type to texture not supported yet.";
    }
}

WDataTexture3D_2::~WDataTexture3D_2()
{
    // cleanup
}


