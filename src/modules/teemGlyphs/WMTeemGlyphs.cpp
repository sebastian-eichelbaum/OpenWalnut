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

#include <algorithm>
#include <sstream>
#include <string>

#include <osg/LightModel>
#include <osg/Geometry>

#include <teem/elf.h> // NOLINT false positive C system header

#include "core/common/WStringUtils.h"
#include "core/common/WAssert.h"
#include "core/common/WConditionOneShot.h"
#include "core/common/WLimits.h"
#include "core/common/WPropertyHelper.h"
#include "core/common/WThreadedFunction.h"
#include "core/kernel/WKernel.h"

// Compatibility between OSG 3.2 and earlier versions
#include "core/graphicsEngine/WOSG.h"

#include "WMTeemGlyphs.h"
#include "WMTeemGlyphs.xpm"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMTeemGlyphs )

WMTeemGlyphs::WMTeemGlyphs():
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
        if( faces[3*f]%2 == 0 )
        {
            ELL_3V_SUB( glyph->norm+3*faces[3*f]+3, glyph->norm+3*faces[3*f]+3, cross );
        }
        else
        {
            ELL_3V_SUB( glyph->norm+3*faces[3*f]-3, glyph->norm+3*faces[3*f]-3, cross );
        }
        if( faces[3*f+1]%2 == 0 )
        {
            ELL_3V_SUB( glyph->norm+3*faces[3*f+1]+3, glyph->norm+3*faces[3*f+1]+3, cross );
        }
        else
        {
            ELL_3V_SUB( glyph->norm+3*faces[3*f+1]-3, glyph->norm+3*faces[3*f+1]-3, cross );
        }
        if( faces[3*f+2]%2 == 0 )
        {
            ELL_3V_SUB( glyph->norm+3*faces[3*f+2]+3, glyph->norm+3*faces[3*f+2]+3, cross );
        }
        else
        {
            ELL_3V_SUB( glyph->norm+3*faces[3*f+2]-3, glyph->norm+3*faces[3*f+2]-3, cross );
        }
    }
    if( normalize )
    {
        float len;
        unsigned int i;
        for( i = 0; i < glyph->normNum; i++ )
        {
            ELL_3V_NORM_TT( glyph->norm + 3*i, float, glyph->norm + 3*i, len );
        }
    }
}
}

WMTeemGlyphs::~WMTeemGlyphs()
{
}

boost::shared_ptr< WModule > WMTeemGlyphs::factory() const
{
    return boost::shared_ptr< WModule >( new WMTeemGlyphs() );
}

const char** WMTeemGlyphs::getXPMIcon() const
{
    return teemGlyphs_xpm;
}


const std::string WMTeemGlyphs::getName() const
{
    return "Teem Glyphs";
}

const std::string WMTeemGlyphs::getDescription() const
{
    return "Higher-Order Tensor Glyphs as described at http://www.ci.uchicago.edu/~schultz/sphinx/home-glyph.html";
}

void WMTeemGlyphs::connectors()
{
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSphericalHarmonics > >(
        new WModuleInputData< WDataSetSphericalHarmonics >( shared_from_this(), "in", "The input dataset." ) );
    addConnector( m_input );

    m_inputGFA = boost::shared_ptr< WModuleInputData< WDataSetScalar > >( new WModuleInputData< WDataSetScalar >( shared_from_this(),
                "inGFA", "Generalized fractional anisotropy." )
            );
    addConnector( m_inputGFA );


    // call WModules initialization
    WModule::connectors();
}

void WMTeemGlyphs::properties()
{
    m_exceptionCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_sliceOrientations = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_sliceOrientations->addItem( "x", "x-slice" );
    m_sliceOrientations->addItem( "y", "y-slice" );
    m_sliceOrientations->addItem( "z", "z-slice" );
    m_sliceOrientationSelectionProp = m_properties->addProperty( "Slice orientation",
                                                             "Which slice will be shown?",
                                                                 m_sliceOrientations->getSelector( 1 ), m_recompute );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_sliceOrientationSelectionProp );

    m_sliceIdProp = m_properties->addProperty( "Slice ID", "Number of the slice to display", 100, m_recompute );
    m_sliceIdProp->setMin( 0 );
    m_sliceIdProp->setMax( 128 );

    m_orders = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_orders->addItem( "2", "Order 2" );
    m_orders->addItem( "4", "Order 4" );
    m_orders->addItem( "6", "Order 6" );

    m_orderProp = m_properties->addProperty( "Order",
                                             "Order of the displayed spherical harmonics."
                                             " If actual order is higer, the additional coefficients are ignored.",
                                             m_orders->getSelector( 1 ),
                                             m_recompute );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_orderProp );

    m_GFAThresholdProp = m_properties->addProperty( "GFA threshold", "Show only glyphs at voxels above the given generalized fractional"
                                                    " anisotropy (GFA) threshold"
                                                    " (if GFA data is present at input connector).",
                                                    0.0,
                                                    m_recompute );
    m_GFAThresholdProp->setMin( 0.0 );
    m_GFAThresholdProp->setMax( 1.0 );

    m_glyphSizeProp = m_properties->addProperty( "Glyph size", "Size of the displayed glyphs.", 1.0, m_recompute );
    m_glyphSizeProp->setMin( 0 );
    m_glyphSizeProp->setMax( 100. );


    m_moduloProp = m_properties->addProperty( "Modulo", "Shows only every Modulo-th glyph in the two slice directions", 2, m_recompute );
    m_moduloProp->setMin( 1 );
    m_moduloProp->setMax( 10 );

    m_subdivisionLevelProp = m_properties->addProperty( "Subdivision level",
                                                        "Determines the glyph resolution. Subdivision level of"
                                                        " icosahadra use as sphere approximations.",
                                                        2,
                                                        m_recompute );
    m_subdivisionLevelProp->setMin( 0 );
    m_subdivisionLevelProp->setMax( 5 );

    m_usePolarPlotProp = m_properties->addProperty( "Use polar plot", "Use polar plot for glyph instead of HOME?", true, m_recompute );
    m_useNormalizationProp = m_properties->addProperty( "Min-max normalization", "Scale the radius of each glyph to be in [0,1]."
                                                        " Do <b>not</b> use with \"Hide negative lobes\"!",
                                                        true,
                                                        m_recompute );
    m_useRadiusNormalizationProp = m_properties->addProperty( "Radius normalization", "Make all glyphs have similar size.",
                                                        true,
                                                        m_recompute );
    m_hideNegativeLobesProp = m_properties->addProperty( "Hide negative lobes", "Hide glyph lobes that have negative radius."
                                                        " Do <b>not</b> use with \"Min-max normalization\"!",
                                                        false,
                                                        m_recompute );

    WModule::properties();
}

void WMTeemGlyphs::handleException( WException const& e )
{
    m_lastException = boost::shared_ptr< WException >( new WException( e ) );
    m_exceptionCondition->notify();
}

void WMTeemGlyphs::moduleMain()
{
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_recompute );
    m_moduleState.add( m_exceptionCondition );

    ready();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        if( !m_input->getData().get() )
        {
            // OK, the output has not yet sent data
            debugLog() << "Waiting for data ...";
            m_moduleState.wait();
            continue;
        }
        if( m_input->getData().get() )
        {
            bool dataChanged = false;
            if( m_dataSet != m_input->getData() )
            {
                // acquire data from the input connector
                m_dataSet = m_input->getData();
                dataChanged = true;
            }

            boost::shared_ptr< WGridRegular3D > gridReg = boost::dynamic_pointer_cast< WGridRegular3D >( m_input->getData().get()->getGrid() );
            switch( m_sliceOrientationSelectionProp->get( true ).getItemIndexOfSelected( 0 ) )
            {
                case 0:
                    m_sliceIdProp->setMax( gridReg->getNbCoordsX() - 1 );
                    break;
                case 1:
                    m_sliceIdProp->setMax( gridReg->getNbCoordsY() - 1 );
                    break;
                case 2:
                    m_sliceIdProp->setMax( gridReg->getNbCoordsZ() - 1 );
                    break;
            }

            if( dataChanged )
            {
                m_sliceIdProp->set( m_sliceIdProp->getMax()->getMax() / 2 );
            }

            boost::shared_ptr< WDataSetScalar > gfa = m_inputGFA->getData();
            if( gfa )
            {
                m_GFAThresholdProp->setMax( gfa->getMax() );
                m_GFAThresholdProp->setMin( gfa->getMin() );
            }

            renderSlice( m_sliceIdProp->get( true ) );
        }

        if( !( m_sliceIdProp->changed()
               || m_sliceOrientationSelectionProp->changed()
               || m_orderProp->changed()
               || m_GFAThresholdProp->changed()
               || m_glyphSizeProp->changed()
               || m_subdivisionLevelProp->changed()
               || m_moduloProp->changed()
               || m_usePolarPlotProp->changed()
               || m_useNormalizationProp->changed()
               || m_useRadiusNormalizationProp->changed()
               || m_hideNegativeLobesProp->changed()
                )
            )
        {
            m_moduleState.wait();
        }
    }

    {
        boost::unique_lock< boost::mutex > lock( m_moduleNodeLock );

        WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_moduleNode );

        lock.unlock();
    }
}

void  WMTeemGlyphs::renderSlice( size_t sliceId )
{
    // Please look here  http://www.ci.uchicago.edu/~schultz/sphinx/home-glyph.htm

    debugLog() << "Rendering Slice " << sliceId;
    boost::shared_ptr< WProgress > progress;
    progress = boost::shared_ptr< WProgress >( new WProgress( "Glyph Generation", 2 ) );
    m_progress->addSubProgress( progress );

    size_t sliceType = m_sliceOrientationSelectionProp->get( true ).getItemIndexOfSelected( 0 );
    size_t order =
        string_utils::fromString< float >( m_orders->getSelector( m_orderProp->get( true ).getItemIndexOfSelected( 0 ) ) .at( 0 )->getName() );

    {
        boost::unique_lock< boost::mutex > lock( m_moduleNodeLock );

        if( m_moduleNode )
        {
            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_moduleNode );
        }

        lock.unlock();
    }

    //-----------------------------------------------------------
    // run through the positions in the slice and draw the glyphs
    boost::shared_ptr< GlyphGeneration > generator;
    generator = boost::shared_ptr< GlyphGeneration >(
                  new GlyphGeneration( boost::dynamic_pointer_cast< WDataSetSphericalHarmonics >( m_input->getData() ),
                                       boost::dynamic_pointer_cast< WDataSetScalar >( m_inputGFA->getData() ),
                                       m_GFAThresholdProp->get( true ),
                                       sliceId,
                                       order,
                                       m_subdivisionLevelProp->get( true ),
                                       m_moduloProp->get( true ),
                                       sliceType,
                                       m_usePolarPlotProp->get( true ),
                                       m_glyphSizeProp->get( true ),
                                       m_useNormalizationProp->get( true ),
                                       m_useRadiusNormalizationProp->get( true ),
                                       m_hideNegativeLobesProp->get( true ) ) );
    WThreadedFunction< GlyphGeneration > generatorThreaded( W_AUTOMATIC_NB_THREADS, generator );
    generatorThreaded.run();
    generatorThreaded.wait();

    ++*progress;

    {
        boost::unique_lock< boost::mutex > lock( m_moduleNodeLock );

        m_moduleNode = osg::ref_ptr< WGEGroupNode >( new WGEGroupNode() );
        osg::ref_ptr< osg::Geode > glyphsGeode = generator->getGraphics();
        m_moduleNode->insert( glyphsGeode );
        m_moduleNode->setName( "teem glyphs module node" );

        m_shader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMTeemGlyphs", m_localPath ) );
        m_shader->apply( glyphsGeode );

        WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_moduleNode );

        lock.unlock();

        activate(); // sets active state of moduleNode correctly
    }

    progress->finish();
}


void WMTeemGlyphs::activate()
{
    boost::unique_lock< boost::mutex > lock( m_moduleNodeLock );

    if( m_moduleNode )
    {
        if( m_active->get() )
        {
            m_moduleNode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_moduleNode->setNodeMask( 0x0 );
        }
    }

    lock.unlock();

    WModule::activate();
}

void WMTeemGlyphs::GlyphGeneration::minMaxNormalization( limnPolyData *glyph, const size_t& nbVertCoords )
{
    // double min = wlimits::MAX_DOUBLE;
    // double max = -wlimits::MAX_DOUBLE;
    double min = 1e15;
    double max = -1e15;
    for( size_t vertID = 0; vertID < glyph->xyzwNum; ++vertID )
    {
        WPosition pos( glyph->xyzw[nbVertCoords*vertID], glyph->xyzw[nbVertCoords*vertID+1],  glyph->xyzw[nbVertCoords*vertID+2] );
        double norm = length( pos );

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

    if( dist != 0.0 )
    {
        WAssert( dist > 0.0, "Max has to be larger than min." );

         for( size_t i = 0; i < glyph->xyzwNum; ++i )
         {
             size_t coordIdBase = nbVertCoords * i;
             WPosition pos( glyph->xyzw[coordIdBase], glyph->xyzw[coordIdBase+1],  glyph->xyzw[coordIdBase+2] );
             double norm = length( pos );
             const double epsilon = 1e-9;
             WPosition newPos;
//             newPos = ( ( ( norm - min ) / dist ) + epsilon ) * normalize( pos );
            newPos = ( ( ( norm - min ) / dist ) + epsilon ) * pos / norm;
            glyph->xyzw[coordIdBase] = newPos[0];
            glyph->xyzw[coordIdBase+1] = newPos[1];
            glyph->xyzw[coordIdBase+2] = newPos[2];
        }
    }
    // else do nothing because all values are equal.
}

WMTeemGlyphs::GlyphGeneration::GlyphGeneration( boost::shared_ptr< WDataSetSphericalHarmonics > dataSet,
                                                boost::shared_ptr< WDataSetScalar > dataGFA,
                                                double thresholdGFA,
                                                const size_t& sliceId,
                                                const size_t& order,
                                                const size_t& subdivisionLevel,
                                                const size_t& modulo,
                                                const size_t& sliceType,
                                                const bool& usePolar,
                                                const float& scale,
                                                const bool& useNormalization,
                                                const bool& useRadiusNormalization,
                                                const bool& hideNegativeLobes ) :
    m_dataSet( dataSet ),
    m_dataGFA( dataGFA ),
    m_grid( boost::dynamic_pointer_cast< WGridRegular3D >( dataSet->getGrid() ) ),
    m_thresholdGFA( thresholdGFA ),
    m_order( order ),
    m_sliceType( sliceType ),
    m_subdivisionLevel( subdivisionLevel ),
    m_modulo( modulo ),
    m_usePolar( usePolar ),
    m_scale( scale ),
    m_useNormalization( useNormalization ),
    m_useRadiusNormalization( useRadiusNormalization ),
    m_hideNegativeLobes( hideNegativeLobes )
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
    m_sliceId = sliceId;

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
    size_t nbGlyphs = ( ( m_nA + ( m_modulo - 1 ) ) / m_modulo ) * ( ( m_nB + ( m_modulo - 1 ) ) / m_modulo );

    const unsigned int level = m_subdivisionLevel; // subdivision level of sphere
    unsigned int infoBitFlag = ( 1 << limnPolyDataInfoNorm ) | ( 1 << limnPolyDataInfoRGBA );

    m_sphere = limnPolyDataNew();
    limnPolyDataIcoSphere( m_sphere, infoBitFlag, level );
    size_t nbVerts = m_sphere->xyzwNum;

    m_vertArray = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array() );
    m_normals = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array() );
    m_colors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array() );
    m_vertArray->resize( nbVerts * nbGlyphs );
    m_normals->resize( nbVerts * nbGlyphs );
    m_colors->resize( nbVerts * nbGlyphs, osg::Vec4( 0, 0, 0, 1.0 ) );

    m_glyphElements = osg::ref_ptr< osg::DrawElementsUInt >( new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES ) );
    m_glyphElements->resize( m_sphere->indxNum * nbGlyphs );
}

WMTeemGlyphs::GlyphGeneration::~GlyphGeneration()
{
    // free memory
    m_sphere = limnPolyDataNix( m_sphere );
}

void WMTeemGlyphs::GlyphGeneration::operator()( size_t id, size_t numThreads, WBoolFlag& /*b*/ )
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

    const tijk_type *type = 0; // Initialized to quiet compiler
    const tijk_type *typeOrder2 = tijk_2o3d_sym;
    const tijk_type *typeOrder4 = tijk_4o3d_sym;
    const tijk_type *typeOrder6 = tijk_6o3d_sym;

    // memory for the tensor and spherical harmonics data.
    float* ten = new float[ typeOrder6->num ];
    float* res = new float[ typeOrder6->num ];
    float* esh = new float[ typeOrder6->num ];


    size_t chunkSize = m_nA / ( numThreads - 1 );
    size_t first = id * chunkSize;

    size_t lastPlusOne = ( id + 1 ) * chunkSize;

    if( id == numThreads - 1 )
    {
        lastPlusOne = m_nA;
    }

    std::stringstream ss;
    ss << id << "/" << numThreads <<" (" << first << " ... " << lastPlusOne - 1 << ")[" << chunkSize << "/" << m_nA << "]" << std::endl;
    WLogger::getLogger()->addLogMessage( ss.str(), "______", LL_DEBUG );

    for( size_t aId = first; aId < lastPlusOne; ++aId )
    {
        for( size_t bId = 0; bId < m_nB; ++bId )
        {
            if( ( aId % m_modulo != 0) || ( bId % m_modulo != 0 ) )
            {
                continue;
            }
            size_t glyphId = ( aId / m_modulo ) * ( ( m_nB + ( m_modulo - 1 ) ) / m_modulo ) + ( bId / m_modulo );

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

            //-------------------------------
            // vertex indices
            // We have to set them also if we do not draw the glyph because otherwise we would leave their
            // to be zero. If many indices are zero, they block each other because of synchronized
            // memory access to the same memory address.
            for( unsigned int vertId = 0; vertId < localSphere->indxNum; ++vertId )
            {
                ( *m_glyphElements )[idsUpToCurrentIteration+vertId] = ( vertsUpToCurrentIteration + localSphere->indx[vertId] );
            }

            // do not compute positions of vertices if GFA below threshold
            if(  m_dataGFA && boost::static_pointer_cast< WDataSetSingle >( m_dataGFA )->getValueAt( posId ) < m_thresholdGFA )
            {
                continue;
            }

            WValue<double> coeffs( m_dataSet->getSphericalHarmonicAt( posId ).getCoefficients() );
            int countOfCoeffsToUse = 0;
            switch( m_order )
            {
                case 2:
                    countOfCoeffsToUse = 6;
                    break;
                case 4:
                    countOfCoeffsToUse = 15;
                    break;
                case 6:
                    countOfCoeffsToUse = 28;
                    break;
                default:
                    WAssert( false, "order above 6 not supported yet." );
            }
            countOfCoeffsToUse = std::min( static_cast< int >( coeffs.size() ), countOfCoeffsToUse );

            // determine order to use
            int useOrder = 0;
            switch( countOfCoeffsToUse )
            {
                case 6:
                    useOrder = 2;
                    type = typeOrder2;
                    break;
                case 15:
                    useOrder = 4;
                    type = typeOrder4;
                    break;
                case 28:
                    useOrder = 6;
                    type = typeOrder6;
                    break;
                default:
                    WAssert( false, "order above 6 not supported yet." );
            }

            for( int coeffId = 0; coeffId < countOfCoeffsToUse; ++coeffId )
            {
                esh[ coeffId ] = coeffs[ coeffId ];
            }

            // change Descoteaux basis to Schulz/teem basis
            // Descoteaux basis: see his PhD thesis page 66
            // Schultz basis: see PDF "Real Spherical Harmonic basis" - Luke Bloy 9. December 2009
            size_t k = 0;
            for( int l = 0; l <= useOrder; l += 2 )
            {
                for( int m = -l; m <= l; ++m )
                {
                    if( m < 0 && ( ( -m ) % 2 == 1 ) )
                    {
                        esh[k] *= -1.0;
                    }
                    else if( m > 0 && ( m % 2 == 0 ) )
                    {
                        esh[k] *= -1.0;
                    }
                    ++k;
                }
            }

            // convert even-order spherical harmonics to higher-order tensor
            tijk_esh_to_3d_sym_f( ten, esh, useOrder );

            const char normalize = 0;

            limnPolyData *glyph = limnPolyDataNew();
            limnPolyDataCopy( glyph, localSphere );

            double radius = 1.0; // some initialization
            if( m_usePolar )
            {
                char isdef = 3; // some initialization
                radius = elfGlyphPolar( glyph, 1, ten, type, &isdef,  m_hideNegativeLobes, normalize, NULL, NULL );
            }
            else
            {
                // create positive approximation of the tensor
                tijk_refine_rankk_parm *parm = tijk_refine_rankk_parm_new();
                parm->pos = 1;
                int ret = tijk_approx_rankk_3d_f( NULL, NULL, res, ten, type, 6, parm );
                WAssert( ret == 0, "Error condition in call to tijk_approx_rankk_3d_f." );
                parm = tijk_refine_rankk_parm_nix( parm );
                tijk_sub_f( ten, ten, res, type );

                radius = elfGlyphHOME( glyph, 1, ten, type, NULL, normalize );
            }

            // -------------------------------------------------------------------------------------------------------
            // One can insert per-peak coloring here (see http://www.ci.uchicago.edu/~schultz/sphinx/home-glyph.html )
            // -------------------------------------------------------------------------------------------------------

            float scale = m_scale;

            if( m_useNormalization )
            {
                minMaxNormalization( glyph, nbVertCoords );
                if( !m_useRadiusNormalization )
                {
                    scale = m_scale * radius;
                }
            }
            else
            {
                if( m_useRadiusNormalization )
                {
                    if( radius != 0 )
                    {
                        scale = m_scale / radius;
                    }
                    else
                    {
                        scale = 0.0;
                    }
                }
            }
            estimateNormalsAntipodal( glyph, normalize );

            WPosition glyphPos = m_grid->getPosition( posId );


            for( unsigned int vertId = 0; vertId < glyph->xyzwNum; ++vertId )
            {
                size_t globalVertexId = vertsUpToCurrentIteration + vertId;
                //-------------------------------
                // vertices
                ( *m_vertArray )[globalVertexId][0] = glyph->xyzw[nbVertCoords*vertId  ] * scale + glyphPos[0];
                ( *m_vertArray )[globalVertexId][1] = glyph->xyzw[nbVertCoords*vertId+1] * scale + glyphPos[1];
                ( *m_vertArray )[globalVertexId][2] = glyph->xyzw[nbVertCoords*vertId+2] * scale + glyphPos[2];

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

osg::ref_ptr< osg::Geode > WMTeemGlyphs::GlyphGeneration::getGraphics()
{
    osg::ref_ptr< wosg::Geometry > glyphGeometry = new wosg::Geometry();
    glyphGeometry->setVertexArray( m_vertArray );
    glyphGeometry->addPrimitiveSet( m_glyphElements );
    glyphGeometry->setNormalArray( m_normals );
    glyphGeometry->setNormalBinding( wosg::Geometry::BIND_PER_VERTEX );
    glyphGeometry->setColorArray( m_colors );
    glyphGeometry->setColorBinding( wosg::Geometry::BIND_PER_VERTEX );


    osg::ref_ptr< osg::Geode > glyphsGeode;
    glyphsGeode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    glyphsGeode->setName( "teem glyphs" );
    osg::StateSet* state = glyphsGeode->getOrCreateStateSet();
    state->setMode(  GL_BLEND, osg::StateAttribute::ON  );

    glyphsGeode->addDrawable( glyphGeometry );
    return glyphsGeode;
}
