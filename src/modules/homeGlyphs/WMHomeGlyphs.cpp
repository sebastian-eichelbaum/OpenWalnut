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

#include <sstream>
#include <string>

#include <osg/LightModel>

#include "../../common/WAssert.h"
#include "../../common/WPropertyHelper.h"
#include "../../common/WLimits.h"
#include "../../common/WThreadedFunction.h"
#include "../../common/WConditionOneShot.h"
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

namespace
{
// Helper routine to estimate normals in a triangle soup in which antipodal
// vertices are subsequent
// taken from teem (glyphElf.c)
void estimateNormalsAntipodal( limnPolyData *glyph, const char normalize )
{
    unsigned int faceno = glyph->indxNum/3;
    unsigned int *faces = glyph->indx;
    unsigned int f;
    memset( glyph->norm, 0, sizeof( float )*3*glyph->xyzwNum );
    for( f = 0; f < faceno/2; f++ )
    {
        float diff1[3];
        float diff2[3];
        float cross[3];
        ELL_3V_SUB( diff1, glyph->xyzw+4*faces[3*f+1],
                   glyph->xyzw+4*faces[3*f] );
        ELL_3V_SUB( diff2, glyph->xyzw+4*faces[3*f+2],
                   glyph->xyzw+4*faces[3*f] );
        ELL_3V_CROSS( cross, diff1, diff2 );
        ELL_3V_INCR( glyph->norm+3*faces[3*f], cross );
        ELL_3V_INCR( glyph->norm+3*faces[3*f+1], cross );
        ELL_3V_INCR( glyph->norm+3*faces[3*f+2], cross );
        /* same for anti-face */
        if ( faces[3*f]%2 == 0 )
        {
            ELL_3V_SUB( glyph->norm+3*faces[3*f]+3, glyph->norm+3*faces[3*f]+3, cross );
        }
        else
        {
            ELL_3V_SUB( glyph->norm+3*faces[3*f]-3, glyph->norm+3*faces[3*f]-3, cross );
        }
        if ( faces[3*f+1]%2 == 0 )
        {
            ELL_3V_SUB( glyph->norm+3*faces[3*f+1]+3, glyph->norm+3*faces[3*f+1]+3, cross );
        }
        else
        {
            ELL_3V_SUB( glyph->norm+3*faces[3*f+1]-3, glyph->norm+3*faces[3*f+1]-3, cross );
        }
        if ( faces[3*f+2]%2 == 0 )
        {
            ELL_3V_SUB( glyph->norm+3*faces[3*f+2]+3, glyph->norm+3*faces[3*f+2]+3, cross );
        }
        else
        {
            ELL_3V_SUB( glyph->norm+3*faces[3*f+2]-3, glyph->norm+3*faces[3*f+2]-3, cross );
        }
    }
    if ( normalize )
    {
        float len;
        unsigned int i;
        for ( i = 0; i < glyph->normNum; i++ )
        {
            ELL_3V_NORM_TT( glyph->norm + 3*i, float, glyph->norm + 3*i, len );
        }
    }
}
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
    m_sliceOrientations = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_sliceOrientations->addItem( "x", "x-slice" );
    m_sliceOrientations->addItem( "y", "y-slice" );
    m_sliceOrientations->addItem( "z", "z-slice" );
    m_sliceOrientationSelection = m_properties->addProperty( "Slice orientation",
                                                             "Which slice will be shown?",
                                                             m_sliceOrientations->getSelector( 1 ),
                                                             m_recompute );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_sliceOrientationSelection );

    m_glyphSizeProp = m_properties->addProperty( "Glyph size", "Size of the displayed glyphs.", 0.5, m_recompute );
    m_glyphSizeProp->setMin( 0 );
    m_glyphSizeProp->setMax( 100. );

    m_sliceIdProp = m_properties->addProperty( "Slice ID", "Number of the slice to display", 0, m_recompute );
    m_sliceIdProp->setMin( 0 );
    m_sliceIdProp->setMax( 128 );

    m_usePolarPlotProp = m_properties->addProperty( "Use polar plot", "Use polar plot for glyph instead of HOME?", true, m_recompute );
    m_useNormalizationProp = m_properties->addProperty( "Radius normalization", "Scale the radius of each glyph to be in [0,1].", true, m_recompute );
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
        debugLog() << "-------- Enter renderSlice...";
        renderSlice( m_sliceIdProp->get() );
        debugLog() << "-------- Exit renderSlice...";

        m_moduleState.wait();
    }
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_moduleNode );
}

void  WMHomeGlyphs::renderSlice( size_t sliceId )
{
    boost::shared_ptr< WProgress > progress;
    progress = boost::shared_ptr< WProgress >( new WProgress( "Glyph Generation", 2 ) );
    m_progress->addSubProgress( progress );

    size_t sliceType = m_sliceOrientationSelection->get( true ).getItemIndexOfSelected( 0 );

    debugLog() << "Rendering slice ... " << sliceId;
    // Please look here  http://www.ci.uchicago.edu/~schultz/sphinx/home-glyph.htm
    if( m_moduleNode )
    {
       WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_moduleNode );
    }

    //-----------------------------------------------------------
    // run through the positions in the slice and draw the glyphs
    boost::shared_ptr< GlyphGeneration > generator;
    generator = boost::shared_ptr< GlyphGeneration >(
                  new GlyphGeneration( boost::shared_dynamic_cast< WDataSetSphericalHarmonics >( m_input->getData() ),
                                       sliceId,
                                       sliceType,
                                       m_usePolarPlotProp->get(),
                                       m_glyphSizeProp->get(),
                                       m_useNormalizationProp->get() ) );
    WThreadedFunction< GlyphGeneration > generatorThreaded( 0, generator );
    generatorThreaded.run();
    generatorThreaded.wait();

    ++*progress;

    m_moduleNode = new WGEGroupNode();
    osg::ref_ptr< osg::Geode > glyphsGeode = generator->getGraphics();
    m_moduleNode->insert( glyphsGeode );


    debugLog() << "end loop ... " << sliceId;

    m_shader = osg::ref_ptr< WShader > ( new WShader( "WMHomeGlyphs", m_localPath ) );
    m_shader->apply( glyphsGeode );


    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_moduleNode );

    progress->finish();
}


void WMHomeGlyphs::activate()
{
    if ( m_moduleNode )
    {
        if ( m_active->get() )
        {
            m_moduleNode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_moduleNode->setNodeMask( 0x0 );
        }
    }
    WModule::activate();
}

void WMHomeGlyphs::GlyphGeneration::minMaxNormalization( limnPolyData *glyph, const size_t& nbVertCoords )
{
    // double min = wlimits::MAX_DOUBLE;
    // double max = -wlimits::MAX_DOUBLE;
    double min = 1e15;
    double max = -1e15;
    for( size_t vertID = 0; vertID < glyph->xyzwNum; ++vertID )
    {
        wmath::WPosition pos( glyph->xyzw[nbVertCoords*vertID], glyph->xyzw[nbVertCoords*vertID+1],  glyph->xyzw[nbVertCoords*vertID+2] );
        double norm = pos.norm();
        if( norm < min )
        {
            min = norm;
        }
        if( norm > max )
        {
            max = norm;
        }
    }
    double dist = max - min;

    if( dist != 0 )
    {
        WAssert( dist > 0, "Max has to be larger than min." );

         for( size_t i = 0; i < glyph->xyzwNum; ++i )
         {
             size_t coordIdBase = nbVertCoords * i;
             wmath::WPosition pos( glyph->xyzw[coordIdBase], glyph->xyzw[coordIdBase+1],  glyph->xyzw[coordIdBase+2] );
             double norm = pos.norm();
             const double epsilon = 1e-9;
             wmath::WPosition newPos;
//             newPos = ( ( ( norm - min ) / dist ) + epsilon ) * pos.normalized();
             newPos = ( ( ( norm - min ) / dist ) + epsilon ) * pos / norm;
             glyph->xyzw[coordIdBase] = newPos[0];
             glyph->xyzw[coordIdBase+1] = newPos[1];
             glyph->xyzw[coordIdBase+2] = newPos[2];
         }
    }
    // else do nothing because all values are equal.
}

WMHomeGlyphs::GlyphGeneration::GlyphGeneration( boost::shared_ptr< WDataSetSphericalHarmonics > dataSet,
                                                const size_t& sliceId,
                                                const size_t& sliceType,
                                                const bool& usePolar,
                                                const float& scale,
                                                const bool& useNormalization ) :
    m_dataSet( dataSet ),
    m_grid( boost::shared_dynamic_cast< WGridRegular3D >( dataSet->getGrid() ) ),
    m_sliceType( sliceType ),
    m_usePolar( usePolar ),
    m_scale( scale ),
    m_useNormalization( useNormalization )
{
    enum sliceTypeEnum
    {
        xSlice = 0,
        ySlice,
        zSlice
    };

    // convenience names for grid dimensions
    m_nX =  m_grid->getNbCoordsX();
    m_nY =  m_grid->getNbCoordsY();
    m_nZ =  m_grid->getNbCoordsZ();
    WAssert( sliceId < m_nX, "Slice id to large." );
    m_sliceId = sliceId;

    // preparation of osg stuff for the glyphs
    m_generatorNode = new WGEGroupNode();

    switch( sliceType )
    {
        case xSlice:
            m_nA = m_nY;
            m_nB = m_nZ;
            break;
        case ySlice:
            m_nA = m_nX;
            m_nB = m_nZ;
            break;
        case zSlice:
            m_nA = m_nX;
            m_nB = m_nY;
            break;
    }

    m_glyphGeometry = new osg::Geometry();
    m_glyphsGeode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    m_glyphsGeode->setName( "glyphs" );
    osg::StateSet* state = m_glyphsGeode->getOrCreateStateSet();
    osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel();
    lightModel->setTwoSided( true );
    state->setAttributeAndModes( lightModel.get(), osg::StateAttribute::ON );
    state->setMode(  GL_BLEND, osg::StateAttribute::ON  );

    size_t nbGlyphs = m_nA * m_nB;

    const unsigned int level = 3; // subdivision level of sphere
    unsigned int infoBitFlag = ( 1 << limnPolyDataInfoNorm ) | ( 1 << limnPolyDataInfoRGBA );

    m_sphere = limnPolyDataNew();
    limnPolyDataIcoSphere( m_sphere, infoBitFlag, level );
    size_t nbVerts = m_sphere->xyzwNum;

    m_vertArray = new osg::Vec3Array( nbVerts * nbGlyphs );
    m_normals = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array( nbVerts * nbGlyphs ) );
    m_colors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array( nbVerts * nbGlyphs ) );

    m_glyphElements = osg::ref_ptr< osg::DrawElementsUInt >( new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 ) );
    m_glyphElements->resize( m_sphere->indxNum * nbGlyphs );
}

WMHomeGlyphs::GlyphGeneration::~GlyphGeneration()
{
    // free memory
    m_sphere = limnPolyDataNix( m_sphere );
}

void WMHomeGlyphs::GlyphGeneration::operator()( size_t id, size_t numThreads, WBoolFlag& /*b*/ )
{
    const size_t nbVertCoords = 4; //The teem limn data structure has 4 values for a coordinate: x, y, z, w.
    limnPolyData *localSphere = limnPolyDataNew();
    limnPolyDataCopy( localSphere, m_sphere );
    enum sliceTypeEnum
    {
        xSlice = 0,
        ySlice,
        zSlice
    };


    WAssert( m_sphere->xyzwNum == m_sphere->normNum, "Wrong size of arrays." );
    WAssert( m_sphere->xyzwNum == m_sphere->rgbaNum, "Wrong size of arrays." );
    size_t nbVerts = m_sphere->xyzwNum;

    const tijk_type *type = tijk_4o3d_sym;

    // memory for the tensor and spherical harmonics data.
    float* ten = new float[type->num];
    float* res = new float[type->num];
    float* esh = new float[type->num];

    size_t chunkSize = m_nA / numThreads;
    size_t first = id * chunkSize;
    size_t last = ( id + 1 ) * chunkSize - 1;
    if( id == numThreads - 1 )
    {
        last = m_nA - 1;
    }

    std::stringstream ss;
    ss << id << "/" << numThreads <<" (" << first << " ... " << last << ")[" << chunkSize << "/" << m_nA << "]" << std::endl;
    WLogger::getLogger()->addLogMessage( ss.str(), "______", LL_DEBUG );

    for( size_t aId = first; aId <= last; ++aId )
    {
        for( size_t bId = 0; bId < m_nB; ++bId )
        {
            size_t glyphId = aId * m_nB + bId;
            size_t vertsUpToCurrentIteration = glyphId * nbVerts;
            size_t idsUpToCurrentIteration = glyphId * m_sphere->indxNum;

            size_t posId = 0; // initialized to quiet compiler
            switch( m_sliceType )
            {
                case xSlice:
                    posId = m_sliceId + aId * m_nX + bId * m_nX * m_nY;
                    break;
                case ySlice:
                    posId = aId + m_sliceId * m_nX + bId * m_nX * m_nY;
                    break;
                case zSlice:
                    posId = aId + bId * m_nX + m_sliceId * m_nX * m_nY;
                    break;
            }

            wmath::WValue< double > coeffs = m_dataSet->getSphericalHarmonicAt( posId ).getCoefficients();
            WAssert( coeffs.size() == 15,
                     "This module can handle only 4th order spherical harmonics."
                     "Thus the input has to be 15 dimensional vectors." );

            for( size_t coeffId = 0; coeffId < 15; coeffId++ )
            {
                esh[coeffId] = coeffs[coeffId];
            }
            // convert even-order spherical harmonics to higher-order tensor
            tijk_esh_to_3d_sym_f( ten, esh, 4 );

            // create positive approximation of the tensor
            tijk_refine_rankk_parm *parm = tijk_refine_rankk_parm_new();
            parm->pos = 1;
            int ret = tijk_approx_rankk_3d_f( NULL, NULL, res, ten, type, 6, parm );
            WAssert( ret == 0, "Error condition in call to tijk_approx_rankk_3d_f." );
            parm = tijk_refine_rankk_parm_nix( parm );
            tijk_sub_f( ten, ten, res, type );

            const char normalize = 0;

            limnPolyData *glyph = limnPolyDataNew();
            limnPolyDataCopy( glyph, localSphere );

            double radius = 1.0; // some initialization
            if( m_usePolar )
            {
                char isdef = 3; // some initialization
                radius = elfGlyphPolar( glyph, 1, ten, type, &isdef, 0, normalize, NULL, NULL );
                WAssert( isdef != 0, "Tensor is non positive definite. Think about that." );
            }
            else
            {
                radius = elfGlyphHOME( glyph, 1, ten, type, NULL, normalize );
            }

            // -------------------------------------------------------------------------------------------------------
            // One can insert per-peak coloring here (see http://www.ci.uchicago.edu/~schultz/sphinx/home-glyph.html )
            // -------------------------------------------------------------------------------------------------------


            if( m_useNormalization )
            {
                    minMaxNormalization( glyph, nbVertCoords );
            }
            else
            {
                m_scale = m_scale / radius;
            }
            estimateNormalsAntipodal( glyph, normalize );

            wmath::WPosition glyphPos = m_grid->getPosition( posId );

            //-------------------------------
            // vertex indices
            for( unsigned int vertId = 0; vertId < glyph->indxNum; ++vertId )
            {
                 ( *m_glyphElements )[idsUpToCurrentIteration+vertId] = ( vertsUpToCurrentIteration + glyph->indx[vertId] );
            }


            for( unsigned int vertId = 0; vertId < glyph->xyzwNum; ++vertId )
            {
                size_t globalVertexId = vertsUpToCurrentIteration + vertId;
                //-------------------------------
                // vertices
                ( *m_vertArray )[globalVertexId][0] = glyph->xyzw[nbVertCoords*vertId  ] * m_scale + glyphPos[0];
                ( *m_vertArray )[globalVertexId][1] = glyph->xyzw[nbVertCoords*vertId+1] * m_scale + glyphPos[1];
                ( *m_vertArray )[globalVertexId][2] = glyph->xyzw[nbVertCoords*vertId+2] * m_scale + glyphPos[2];

                // ------------------------------------------------
                // normals
                ( *m_normals )[globalVertexId][0] = glyph->norm[3*vertId];
                ( *m_normals )[globalVertexId][1] = glyph->norm[3*vertId+1];
                ( *m_normals )[globalVertexId][2] = glyph->norm[3*vertId+2];
                ( *m_normals )[globalVertexId].normalize();

                // ------------------------------------------------
                // colors
                const size_t nbColCoords = 4;
                ( *m_colors )[globalVertexId][0] = glyph->rgba[nbColCoords*vertId] / 255.0;
                ( *m_colors )[globalVertexId][1] = glyph->rgba[nbColCoords*vertId+1] / 255.0;
                ( *m_colors )[globalVertexId][2] = glyph->rgba[nbColCoords*vertId+2] / 255.0;
                ( *m_colors )[globalVertexId][3] = glyph->rgba[nbColCoords*vertId+3] / 255.0;
            }

            // free memory
            glyph = limnPolyDataNix( glyph );
        }
    }

    // free memory
    localSphere = limnPolyDataNix( localSphere );

    delete[] ten;
    delete[] res;
    delete[] esh;
}

osg::ref_ptr< osg::Geode > WMHomeGlyphs::GlyphGeneration::getGraphics()
{
    m_glyphGeometry->setVertexArray( m_vertArray );
    m_glyphGeometry->addPrimitiveSet( m_glyphElements );
    m_glyphGeometry->setNormalArray( m_normals );
    m_glyphGeometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
    m_glyphGeometry->setColorArray( m_colors );
    m_glyphGeometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

    m_glyphsGeode->addDrawable( m_glyphGeometry );
    return m_glyphsGeode;
}
