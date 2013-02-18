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

#include <string>
#include <vector>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/StateAttribute>
#include <osg/PolygonMode>
#include <osg/LightModel>

#include "core/kernel/WKernel.h"

#include "core/graphicsEngine/WGEUtils.h"
#include "core/graphicsEngine/shaders/WGEPropertyUniform.h"
#include "core/graphicsEngine/postprocessing/WGEPostprocessingNode.h"

#include "WMSuperquadricGlyphs.h"
#include "WMSuperquadricGlyphs.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMSuperquadricGlyphs )

WMSuperquadricGlyphs::WMSuperquadricGlyphs():
    WModule()
{
    // initialize members
}

WMSuperquadricGlyphs::~WMSuperquadricGlyphs()
{
    // Cleanup!
    removeConnectors();
}

boost::shared_ptr< WModule > WMSuperquadricGlyphs::factory() const
{
    // create prototype
    return boost::shared_ptr< WModule >( new WMSuperquadricGlyphs() );
}

const char** WMSuperquadricGlyphs::getXPMIcon() const
{
    return superquadricglyphs_xpm;
}

const std::string WMSuperquadricGlyphs::getName() const
{
    return "Superquadric Glyphs";
}

const std::string WMSuperquadricGlyphs::getDescription() const
{
    return "GPU based ray-tracing of second order, superquadric tensor glyphs.";
}

void WMSuperquadricGlyphs::connectors()
{
    // The input second order tensor dataset
    m_input = boost::shared_ptr< WModuleInputData< WDataSetDTI > >( new WModuleInputData< WDataSetDTI >( shared_from_this(),
        "tensor input", "An input set of 2nd-order tensors on a regular 3D-grid." )
    );
    addConnector( m_input );

    // call WModule's initialization
    WModule::connectors();
}

void WMSuperquadricGlyphs::properties()
{
    // The condition fires on property updates
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // The slice positions. These get update externally
    m_xPos           = m_properties->addProperty( "Sagittal position", "Slice X position.", 0, m_propCondition );
    m_yPos           = m_properties->addProperty( "Coronal position", "Slice Y position.", 0, m_propCondition );
    m_zPos           = m_properties->addProperty( "Axial position", "Slice Z position.", 0, m_propCondition );
    m_xPos->setMin( 0 );
    m_xPos->setMax( 1 );
    m_yPos->setMin( 0 );
    m_yPos->setMax( 1 );
    m_zPos->setMin( 0 );
    m_zPos->setMax( 1 );

    // Flags denoting whether the glyphs should be shown on the specific slice
    // NOTE: the showon* properties do not need to notify m_propCondition as they get handled by the slice's osg node.
    m_showonX        = m_properties->addProperty( "Show sagittal", "Show vectors on sagittal slice.", true );
    m_showonY        = m_properties->addProperty( "Show coronal", "Show vectors on coronal slice.", true );
    m_showonZ        = m_properties->addProperty( "Show axial", "Show vectors on axial slice.", true );

    // Thresholding for filtering glyphs
    m_evThreshold = m_properties->addProperty( "Eigenvalue threshold",
                                               "Clip Glyphs whose smallest eigenvalue is below the given threshold.", 0.01 );
    m_evThreshold->setMin( 0.0 );
    m_evThreshold->setMax( 1.0 );
    m_faThreshold = m_properties->addProperty( "FA threshold",
                                               "Clip Glyphs whose fractional anisotropy is below the given threshold.", 0.01 );
    m_faThreshold->setMin( 0.0 );
    m_faThreshold->setMax( 1.0 );

    m_gamma = m_properties->addProperty( "Gamma", "Sharpness Parameter of the SuperQuadrics.", 10.0 );
    m_gamma->setMin( 0.0 );
    m_gamma->setMax( 10.0 );

    m_scaling = m_properties->addProperty( "Scaling", "Scaling of Glyphs.", 0.5 );
    m_scaling->setMin( 0.0 );
    m_scaling->setMax( 2.0 );
}

inline void WMSuperquadricGlyphs::addGlyph( osg::Vec3 position, osg::ref_ptr< osg::Vec3Array > vertices, osg::ref_ptr< osg::Vec3Array > orientation )
{
    // Front Face
    vertices->push_back( position );
    orientation->push_back( osg::Vec3( -1.0, -1.0, -1.0 ) );
    vertices->push_back( position );
    orientation->push_back( osg::Vec3(  1.0, -1.0, -1.0 ) );
    vertices->push_back( position );
    orientation->push_back( osg::Vec3(  1.0,  1.0, -1.0 ) );
    vertices->push_back( position );
    orientation->push_back( osg::Vec3( -1.0,  1.0, -1.0 ) );

    // Back Face
    vertices->push_back( position );
    orientation->push_back( osg::Vec3( -1.0, -1.0, 1.0 ) );
    vertices->push_back( position );
    orientation->push_back( osg::Vec3(  1.0, -1.0, 1.0 ) );
    vertices->push_back( position );
    orientation->push_back( osg::Vec3(  1.0,  1.0, 1.0 ) );
    vertices->push_back( position );
    orientation->push_back( osg::Vec3( -1.0,  1.0, 1.0 ) );

    // Top Face
    vertices->push_back( position );
    orientation->push_back( osg::Vec3( -1.0,  1.0, -1.0 ) );
    vertices->push_back( position );
    orientation->push_back( osg::Vec3(  1.0,  1.0, -1.0 ) );
    vertices->push_back( position );
    orientation->push_back( osg::Vec3(  1.0,  1.0, 1.0 ) );
    vertices->push_back( position );
    orientation->push_back( osg::Vec3( -1.0,  1.0, 1.0 ) );

    // Bottom Face
    vertices->push_back( position );
    orientation->push_back( osg::Vec3( -1.0, -1.0, -1.0 ) );
    vertices->push_back( position );
    orientation->push_back( osg::Vec3(  1.0,  -1.0, -1.0 ) );
    vertices->push_back( position );
    orientation->push_back( osg::Vec3(  1.0,  -1.0, 1.0 ) );
    vertices->push_back( position );
    orientation->push_back( osg::Vec3( -1.0,  -1.0, 1.0 ) );

    // Left Face
    vertices->push_back( position );
    orientation->push_back( osg::Vec3( -1.0, -1.0, -1.0 ) );
    vertices->push_back( position );
    orientation->push_back( osg::Vec3(  -1.0, 1.0, -1.0 ) );
    vertices->push_back( position );
    orientation->push_back( osg::Vec3(  -1.0, 1.0, 1.0 ) );
    vertices->push_back( position );
    orientation->push_back( osg::Vec3( -1.0, -1.0, 1.0 ) );

    // Right Face
    vertices->push_back( position );
    orientation->push_back( osg::Vec3( 1.0, -1.0, -1.0 ) );
    vertices->push_back( position );
    orientation->push_back( osg::Vec3( 1.0, 1.0, -1.0 ) );
    vertices->push_back( position );
    orientation->push_back( osg::Vec3( 1.0, 1.0, 1.0 ) );
    vertices->push_back( position );
    orientation->push_back( osg::Vec3( 1.0, -1.0, 1.0 ) );
}

void WMSuperquadricGlyphs::addTensor( size_t idx, osg::Vec3Array* diag, osg::Vec3Array* offdiag )
{
    osg::Vec3 d = osg::Vec3( m_dataSetValueSet->getScalarDouble( idx * 6 + 0 ),
                             m_dataSetValueSet->getScalarDouble( idx * 6 + 3 ),
                             m_dataSetValueSet->getScalarDouble( idx * 6 + 5 ) );
    osg::Vec3 o = osg::Vec3( m_dataSetValueSet->getScalarDouble( idx * 6 + 1 ),
                             m_dataSetValueSet->getScalarDouble( idx * 6 + 2 ),
                             m_dataSetValueSet->getScalarDouble( idx * 6 + 4 ) );

    // we need to add it for every vertex per glyph!
    for( size_t c = 0; c < 6 * 4; ++c )
    {
        diag->push_back( d );
        offdiag->push_back( o );
    }
}

void WMSuperquadricGlyphs::initOSG()
{
    // The Idea behind all this is to have fast updates when the navigation slices move. By presetting glyph vertex and tex coords -> fast change
    // of diag and offdiag tex arrays possible. The slices are moved using the transform nodes

    debugLog() << "Grid Size: " << m_maxX << "x" << m_maxY << "x" << m_maxZ;
    debugLog() << "Creating " << m_nbGlyphsX + m_nbGlyphsY + m_nbGlyphsZ << " glyphs.";

    // remove the old slices
    m_output->remove( m_xSlice );
    m_output->remove( m_ySlice );
    m_output->remove( m_zSlice );

    // create all the transformation nodes
    m_xSlice = new WGEManagedGroupNode( m_showonX );
    m_xSlice->setMatrix( osg::Matrixd::identity() );
    m_ySlice = new WGEManagedGroupNode( m_showonY );
    m_ySlice->setMatrix( osg::Matrixd::identity() );
    m_zSlice = new WGEManagedGroupNode( m_showonZ );
    m_zSlice->setMatrix( osg::Matrixd::identity() );

    // init the vertex arrays
    osg::ref_ptr< osg::Geode > geode;
    osg::ref_ptr< osg::Geometry > geometry;
    osg::ref_ptr< osg::DrawArrays > da;

    // NOTE: it would be nice to use one vertex and tex array for all the three but this disallows us to replace the tensor diag/offdiag arrays
    // for each slide separately.
    osg::ref_ptr< osg::Vec3Array > vertices;
    osg::ref_ptr< osg::Vec3Array > texcoords0;

    ///////////////
    // X Slice

    // fill the geode with glyph proxy geometry for a slice
    vertices = new osg::Vec3Array;
    vertices->reserve( m_nbGlyphsX * 6 * 4 );
    texcoords0 = new osg::Vec3Array;
    texcoords0->reserve( m_nbGlyphsX * 6 * 4 );
    geometry = new osg::Geometry();
    geometry->setDataVariance( osg::Object::DYNAMIC );
    geometry->setVertexArray( vertices );
    geometry->setUseVertexBufferObjects( true );
    geometry->setTexCoordArray( 0, texcoords0 );
    da = new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, m_nbGlyphsX * 6 * 4 );
    geometry->addPrimitiveSet( da );

    // create a new geode containing the glyphs proxy geometry
    geode = new osg::Geode();
    geode->addDrawable( geometry );

    // add a glyph at every position
    for( size_t z = 0; z < m_maxZ; ++z )
    {
        for( size_t y = 0; y < m_maxY; ++y )
        {
            addGlyph( osg::Vec3d( 0.0, y, z ), vertices, texcoords0 );
        }
    }

    // set some callbacks
    m_xSliceGlyphCallback = new GlyphGeometryNodeCallback( geometry );
    geometry->setUpdateCallback( m_xSliceGlyphCallback );
    m_xSlice->addUpdateCallback( new WGELinearTranslationCallback< WPropInt >( osg::Vec3( 1.0, 0.0, 0.0 ), m_xPos ) );
    m_xSlice->addChild( geode );

    ///////////////
    // Y Slice, sorry for code duplication.

    // fill the geode with glyph proxy geometry for a slice
    vertices = new osg::Vec3Array;
    vertices->reserve( m_nbGlyphsX * 6 * 4 );
    texcoords0 = new osg::Vec3Array;
    texcoords0->reserve( m_nbGlyphsX * 6 * 4 );
    geometry = new osg::Geometry();
    geometry->setDataVariance( osg::Object::DYNAMIC );
    geometry->setVertexArray( vertices );
    geometry->setUseVertexBufferObjects( true );
    geometry->setTexCoordArray( 0, texcoords0 );
    da = new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, m_nbGlyphsY * 6 * 4 );
    geometry->addPrimitiveSet( da );

    // create a new geode containing the glyphs proxy geometry
    geode = new osg::Geode();
    geode->addDrawable( geometry );
    // add a glyph at every position
    for( size_t z = 0; z < m_maxZ; ++z )
    {
        for( size_t x = 0; x < m_maxX; ++x )
        {
            addGlyph( osg::Vec3d( x, 0.0, z ), vertices, texcoords0 );
        }
    }

    // set some callbacks
    m_ySliceGlyphCallback = new GlyphGeometryNodeCallback( geometry );
    geometry->setUpdateCallback( m_ySliceGlyphCallback );
    m_ySlice->addUpdateCallback( new WGELinearTranslationCallback< WPropInt >( osg::Vec3( 0.0, 1.0, 0.0 ), m_yPos ) );
    m_ySlice->addChild( geode );

    ///////////////
    // Z Slice, sorry for code duplication.

    // fill the geode with glyph proxy geometry for a slice
    vertices = new osg::Vec3Array;
    vertices->reserve( m_nbGlyphsX * 6 * 4 );
    texcoords0 = new osg::Vec3Array;
    texcoords0->reserve( m_nbGlyphsX * 6 * 4 );
    geometry = new osg::Geometry();
    geometry->setDataVariance( osg::Object::DYNAMIC );
    geometry->setVertexArray( vertices );
    geometry->setUseVertexBufferObjects( true );
    geometry->setTexCoordArray( 0, texcoords0 );
    da = new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, m_nbGlyphsZ * 6 * 4 );
    geometry->addPrimitiveSet( da );

    // create a new geode containing the glyphs proxy geometry
    geode = new osg::Geode();
    geode->addDrawable( geometry );
    // add a glyph at every position
    for( size_t y = 0; y < m_maxY; ++y )
    {
        for( size_t x = 0; x < m_maxX; ++x )
        {
            addGlyph( osg::Vec3d( x, y, 0.0 ), vertices, texcoords0 );
        }
    }

    // set some callbacks
    m_zSliceGlyphCallback = new GlyphGeometryNodeCallback( geometry );
    geometry->setUpdateCallback( m_zSliceGlyphCallback );
    m_zSlice->addUpdateCallback( new WGELinearTranslationCallback< WPropInt >( osg::Vec3( 0.0, 0.0, 1.0 ), m_zPos ) );
    m_zSlice->addChild( geode );

    // add the transformation nodes to the output group
    m_output->insert( m_xSlice );
    m_output->insert( m_ySlice );
    m_output->insert( m_zSlice );
}

void WMSuperquadricGlyphs::GlyphGeometryNodeCallback::update( osg::NodeVisitor* /*nv*/, osg::Drawable* /*d*/ )
{
    if( m_dirty )
    {
        m_dirty = false;

        // update the tex array
        m_geometry->setTexCoordArray( 1, m_tensorDiag );
        m_geometry->setTexCoordArray( 2, m_tensorOffDiag );
    }
}

void WMSuperquadricGlyphs::GlyphGeometryNodeCallback::setNewTensorData( osg::ref_ptr< osg::Vec3Array > diag, osg::ref_ptr< osg::Vec3Array > offdiag )
{
    m_tensorDiag = diag;
    m_tensorOffDiag = offdiag;

    m_dirty = true;
}

void WMSuperquadricGlyphs::moduleMain()
{
    // use the m_input "data changed" flag
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    // use property condition to wake up on property changes
    m_moduleState.add( m_propCondition );

    // signal ready state
    ready();

    // postproc node
    osg::ref_ptr< WGEPostprocessingNode > postNode = new WGEPostprocessingNode(
        WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCamera()
    );
    postNode->addUpdateCallback( new WGENodeMaskCallback( m_active ) ); // disable the postNode with m_active
    // provide the properties of the post-processor to the user
    m_properties->addProperty( postNode->getProperties() );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( postNode );

    // create all these geodes we need
    m_output = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_active ) );
    osg::ref_ptr< osg::StateSet > sset = m_output->getOrCreateStateSet();
    sset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );


    // add shader and postproc
    m_shader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMSuperquadricGlyphs", m_localPath ) );
    m_shader->apply( m_output );
    postNode->insert( m_output, m_shader );

    // set uniform callbacks and uniforms
    osg::ref_ptr< osg::Uniform > evThreshold = new WGEPropertyUniform< WPropDouble >( "u_evThreshold", m_evThreshold );
    osg::ref_ptr< osg::Uniform > faThreshold = new WGEPropertyUniform< WPropDouble >( "u_faThreshold", m_faThreshold );
    osg::ref_ptr< osg::Uniform > scaling = new WGEPropertyUniform< WPropDouble >( "u_scaling", m_scaling );
    osg::ref_ptr< osg::Uniform > gamma = new WGEPropertyUniform< WPropDouble >( "u_gamma", m_gamma );

    sset->addUniform( evThreshold );
    sset->addUniform( faThreshold );
    sset->addUniform( scaling );
    sset->addUniform( gamma );

    bool initialTensorUpload = true;

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // After waking up, the module has to check whether the shutdownFlag fired. If yes, simply quit the module.

        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Collect changes

        // has the data changes? And even more important: is it valid?
        boost::shared_ptr< WDataSetDTI > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        bool dataValid   = ( newDataSet );

        // if data is invalid, remove rendering
        if( !dataValid )
        {
            debugLog() << "Resetting.";
            m_output->clear();
            m_dataSet.reset();
            continue;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Handle changes

        if( dataChanged && dataValid )
        {
            // The data is different. Copy it to our internal data variable:
            debugLog() << "Received Data.";
            initialTensorUpload = true;

            // also provide progress information
            boost::shared_ptr< WProgress > progress1 = boost::shared_ptr< WProgress >( new WProgress( "Building Glyph Geometry" ) );
            m_progress->addSubProgress( progress1 );

            // get pointers for the new data
            m_dataSet = newDataSet;
            m_dataSetGrid = boost::dynamic_pointer_cast< WGridRegular3D >( m_dataSet->getGrid() );
            WAssert( m_dataSetGrid, "Dataset does not have a regular 3D grid." );
            m_maxX = m_dataSetGrid->getNbCoordsX();
            m_maxY = m_dataSetGrid->getNbCoordsY();
            m_maxZ = m_dataSetGrid->getNbCoordsZ();
            m_dataSetValueSet = m_dataSet->getValueSet();
            m_nbGlyphsX = m_maxY * m_maxZ;
            m_nbGlyphsY = m_maxX * m_maxZ;
            m_nbGlyphsZ = m_maxX * m_maxY;

            // update properties
            m_xPos->setMax( m_maxX );
            m_xPos->set( m_maxX / 2 );
            m_yPos->setMax( m_maxY );
            m_yPos->set( m_maxY / 2 );
            m_zPos->setMax( m_maxZ );
            m_zPos->set( m_maxZ / 2 );

            m_output->setMatrix( static_cast< WMatrix4d >( m_dataSetGrid->getTransform() ) );

            // new data -> update OSG Stuff
            initOSG();
            progress1->finish();
        }

        if( dataValid && ( m_xPos->changed() || initialTensorUpload ) )
        {
            // also provide progress information
            boost::shared_ptr< WProgress > progress1 = boost::shared_ptr< WProgress >( new WProgress( "Building Glyph Geometry" ) );
            m_progress->addSubProgress( progress1 );

            // grab some mem
            osg::ref_ptr< osg::Vec3Array > diag = new osg::Vec3Array;
            diag->reserve( m_nbGlyphsX * 6 * 4 );
            osg::ref_ptr< osg::Vec3Array > offdiag = new osg::Vec3Array;
            offdiag->reserve( m_nbGlyphsX * 6 * 4 );

            // this updates the diag/offdiag texture coordinate arrays of the xSlice
            // x = const -> handle xPos property
            size_t fixedX = m_xPos->get( true );
            for( size_t z = 0; z < m_maxZ; ++z )
            {
                // Calculate the current position along the z direction
                size_t zPre = fixedX + z * m_maxX * m_maxY;

                for( size_t y = 0; y < m_maxY; ++y )
                {
                    // add glyph
                    addTensor( zPre + y * m_maxX, diag, offdiag );
                }
            }

            m_xSliceGlyphCallback->setNewTensorData( diag, offdiag );
            progress1->finish();
        }

        if( dataValid && ( m_yPos->changed() || initialTensorUpload ) )
        {
            // also provide progress information
            boost::shared_ptr< WProgress > progress1 = boost::shared_ptr< WProgress >( new WProgress( "Building Glyph Geometry" ) );
            m_progress->addSubProgress( progress1 );

            // grab some mem
            osg::ref_ptr< osg::Vec3Array > diag = new osg::Vec3Array;
            diag->reserve( m_nbGlyphsX * 6 * 4 );
            osg::ref_ptr< osg::Vec3Array > offdiag = new osg::Vec3Array;
            offdiag->reserve( m_nbGlyphsX * 6 * 4 );

            // y = const -> handle yPos property
            size_t fixedY = m_yPos->get( true );
            size_t fixedYOffset = fixedY * m_maxX;
            for( size_t z = 0; z < m_maxZ; ++z )
            {
                // Calculate current offset for value index
                size_t zPre = fixedYOffset + ( z * m_maxX * m_maxY );

                for( size_t x = 0; x < m_maxX; ++x )
                {
                    // add glyph
                    addTensor( zPre + x, diag, offdiag );
                }
            }

            m_ySliceGlyphCallback->setNewTensorData( diag, offdiag );
            progress1->finish();
        }

        if( dataValid && ( m_zPos->changed() || initialTensorUpload ) )
        {
            // also provide progress information
            boost::shared_ptr< WProgress > progress1 = boost::shared_ptr< WProgress >( new WProgress( "Building Glyph Geometry" ) );
            m_progress->addSubProgress( progress1 );

            // grab some mem
            osg::ref_ptr< osg::Vec3Array > diag = new osg::Vec3Array;
            diag->reserve( m_nbGlyphsX * 6 * 4 );
            osg::ref_ptr< osg::Vec3Array > offdiag = new osg::Vec3Array;
            offdiag->reserve( m_nbGlyphsX * 6 * 4 );

            // z = const -> handle zPos property
            size_t fixedZ = static_cast< size_t >( m_zPos->get( true ) );
            size_t fixedZOffset = fixedZ * m_maxX * m_maxY;
            for( size_t y = 0; y < m_maxY; ++y )
            {
                // Calculate current offset for value index
                size_t yPre = fixedZOffset + ( y * m_maxX );

                for( size_t x = 0; x < m_maxX; ++x )
                {
                    // add glyph
                    addTensor( yPre + x, diag, offdiag );
                }
            }

            m_zSliceGlyphCallback->setNewTensorData( diag, offdiag );
            progress1->finish();
        }

        initialTensorUpload = false;
    }

    // At this point, the container managing this module signalled to shutdown. The main loop has ended and you should clean up. Always remove
    // allocated memory and remove all OSG nodes.
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( postNode );
}

