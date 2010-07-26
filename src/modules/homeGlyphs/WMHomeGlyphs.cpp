//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#include <teem/elf.h>

#include <string>

#include <osg/LightModel>

#include "../../common/WAssert.h"
#include "../../kernel/WKernel.h"
#include "home.xpm"

#include "WMHomeGlyphs.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMHomeGlyphs )

WMHomeGlyphs::WMHomeGlyphs():
    WModule(),
    m_recompute( boost::shared_ptr< WCondition >( new WCondition() ) )
{
}

WMHomeGlyphs::~WMHomeGlyphs()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMHomeGlyphs::factory() const
{
    return boost::shared_ptr< WModule >( new WMHomeGlyphs() );
}

const char** WMHomeGlyphs::getXPMIcon() const
{
    return home_xpm;
}


const std::string WMHomeGlyphs::getName() const
{
    return "HOME Glyphs";
}

const std::string WMHomeGlyphs::getDescription() const
{
    return "Higher-Order Tensor Glyphs as described at http://www.ci.uchicago.edu/~schultz/sphinx/home-glyph.html";
}

void WMHomeGlyphs::connectors()
{
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSphericalHarmonics > >(
        new WModuleInputData< WDataSetSphericalHarmonics >( shared_from_this(), "in", "The input dataset." ) );
    addConnector( m_input );


    // call WModules initialization
    WModule::connectors();
}

void WMHomeGlyphs::properties()
{
    m_glyphIdProp = m_properties->addProperty( "Glyph Id", "Number of the glyph to display", 0, m_recompute );
    m_glyphIdProp->setMin( 0 );
    m_glyphIdProp->setMax( 1000000 );
    m_usePolarPlotProp = m_properties->addProperty( "Use Polar Plot", "Use polar plot for glyph instead of HOME?", false, m_recompute );
}

void WMHomeGlyphs::moduleMain()
{
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_recompute );

    ready();

    // loop until the module container requests the module to quit
    while ( !m_shutdownFlag() )
    {
        if ( !m_input->getData().get() )
        {
            // OK, the output has not yet sent data
            debugLog() << "Waiting for data ...";
            m_moduleState.wait();
            continue;
        }
        execute();

        m_moduleState.wait();
    }
}

void WMHomeGlyphs::execute()
{
    // Please look here  http://www.ci.uchicago.edu/~schultz/sphinx/home-glyph.htm
     if( m_glyphsGeode )
     {
         m_moduleNode->remove( m_glyphsGeode );
     }

    boost::shared_ptr< WDataSetSphericalHarmonics > dataSet = boost::shared_dynamic_cast< WDataSetSphericalHarmonics >( m_input->getData() );
    wmath::WValue< double > coeffs = dataSet->getSphericalHarmonicAt( m_glyphIdProp->get() ).getCoefficients();

    WAssert( coeffs.size() == 15, "This module can handle only 4th order spherical harmonics. Thus the input has to 15 dimensional vectors." );

    unsigned int level = 3;
    unsigned int infoBitFlag = ( 1 << limnPolyDataInfoNorm ) | ( 1 << limnPolyDataInfoRGBA );
    limnPolyData *sphere = limnPolyDataNew();
    limnPolyDataIcoSphere( sphere, infoBitFlag, level );

    const tijk_type *type = tijk_4o3d_sym;

    float* ten = new float[type->num];
    float* res = new float[type->num];
    float* esh = new float[type->num];

    for( size_t i = 0; i < 15; i++ )
    {
        esh[i] = coeffs[i];
    }


    /* convert even-order spherical harmonics to higher-order tensor */
    tijk_esh_to_3d_sym_f( ten, esh, 4 );

    /* create positive approximation of the tensor */
    tijk_refine_rankk_parm *parm = tijk_refine_rankk_parm_new();
    parm->pos = 1;
    tijk_approx_rankk_3d_f( NULL, NULL, res, ten, type, 6, parm );
    parm = tijk_refine_rankk_parm_nix( parm );
    tijk_sub_f( ten, ten, res, type );



    const char normalize = 0;
    limnPolyData *glyph = limnPolyDataNew();
    limnPolyDataCopy( glyph, sphere );

    float radius;
    if( m_usePolarPlotProp->get() )
    {
        radius = elfGlyphPolar( glyph, 1, ten, type, NULL, NULL, normalize, NULL, NULL );
    }
    else
    {
        radius = elfGlyphHOME( glyph, 1, ten, type, NULL, normalize );
    }

    //
    // One can insert per-peak coloring here (see http://www.ci.uchicago.edu/~schultz/sphinx/home-glyph.html )
    //

    //-------------------------------
    // DRAW
    m_moduleNode = new WGEGroupNode();
    osg::Geometry* glyphGeometry = new osg::Geometry();
    m_glyphsGeode = osg::ref_ptr< osg::Geode >( new osg::Geode );

    m_glyphsGeode->setName( "glyphs" );

    osg::ref_ptr< osg::Vec3Array > vertArray = new osg::Vec3Array( glyph->xyzwNum );
    wmath::WPosition glyphPos = boost::shared_dynamic_cast< WGridRegular3D >( dataSet->getGrid() )->getPosition(  m_glyphIdProp->get() );
    m_glyphIdProp->setMax( dataSet->getGrid()->size() - 1 );

    for( unsigned int vertId = 0; vertId < glyph->xyzwNum; ++vertId )
    {
        ( *vertArray )[vertId][0] = glyph->xyzw[4*vertId  ] / radius + glyphPos[0];
        ( *vertArray )[vertId][1] = glyph->xyzw[4*vertId+1] / radius + glyphPos[1];
        ( *vertArray )[vertId][2] = glyph->xyzw[4*vertId+2] / radius + glyphPos[2];
    }
    glyphGeometry->setVertexArray( vertArray );

    osg::DrawElementsUInt* glyphElement;

    glyphElement = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );

    glyphElement->reserve( glyph->indxNum );


    for( unsigned int vertId = 0; vertId < glyph->indxNum; ++vertId )
    {
        glyphElement->push_back( glyph->indx[vertId] );
    }

    glyphGeometry->addPrimitiveSet( glyphElement );

    // ------------------------------------------------
    // normals
    osg::ref_ptr< osg::Vec3Array > normals = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array( glyph->normNum ) );


    for( unsigned int vertId = 0; vertId < glyph->normNum; ++vertId )
    {
        ( *normals )[vertId][0] = glyph->norm[3*vertId];
        ( *normals )[vertId][1] = glyph->norm[3*vertId+1];
        ( *normals )[vertId][2] = glyph->norm[3*vertId+2];
        ( *normals )[vertId].normalize();
    }

    glyphGeometry->setNormalArray( normals );
    glyphGeometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

    // ------------------------------------------------
    // colors
    osg::Vec4Array* colors = new osg::Vec4Array;

    WColor c( 0.0, 0.0, 1.0 );
    colors->push_back( osg::Vec4( c.getRed(), c.getGreen(), c.getBlue(), 1.0f ) );
    glyphGeometry->setColorArray( colors );
    glyphGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    m_glyphsGeode->addDrawable( glyphGeometry );
    m_moduleNode->insert( m_glyphsGeode );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_moduleNode );

    // free memory
    glyph = limnPolyDataNix( glyph );

    // free memory
    sphere = limnPolyDataNix( sphere );

    delete[] ten;
    delete[] res;
    delete[] esh;
}
