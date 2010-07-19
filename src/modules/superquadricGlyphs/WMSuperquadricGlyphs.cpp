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

#include "../../kernel/WKernel.h"

#include "../../graphicsEngine/WGEUtils.h"

#include "WMSuperquadricGlyphs.h"
#include "superquadricglyphs.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMSuperquadricGlyphs )

WMSuperquadricGlyphs::WMSuperquadricGlyphs():
    WModule()
{
    m_shader = osg::ref_ptr< WShader > ( new WShader( "WMSuperquadricGlyphs", m_localPath ) );
    m_output = osg::ref_ptr< WGEGroupNode > ( new WGEGroupNode() );
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
    return "GPU based raytracing of second order, superquadric tensor glyphs.";
}

void WMSuperquadricGlyphs::connectors()
{
    // The input second order tensor dataset
    m_input = boost::shared_ptr< WModuleInputData< WDataSetSingle > >( new WModuleInputData< WDataSetSingle >( shared_from_this(),
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
    m_xPos           = m_properties->addProperty( "Sagittal Position", "Slice X position.", 80, m_propCondition );
    m_yPos           = m_properties->addProperty( "Coronal Position", "Slice Y position.", 100, m_propCondition );
    m_zPos           = m_properties->addProperty( "Axial Position", "Slice Z position.", 80, m_propCondition );
    m_xPos->setMin( 0 );
    m_xPos->setMax( 159 );
    m_yPos->setMin( 0 );
    m_yPos->setMax( 199 );
    m_zPos->setMin( 0 );
    m_zPos->setMax( 159 );

    // Flags denoting whether the glyphs should be shown on the specific slice
    m_showonX        = m_properties->addProperty( "Show Sagittal", "Show vectors on sagittal slice.", true, m_propCondition );
    m_showonY        = m_properties->addProperty( "Show Coronal", "Show vectors on coronal slice.", true, m_propCondition );
    m_showonZ        = m_properties->addProperty( "Show Axial", "Show vectors on axial slice.", true, m_propCondition );

    // Thresholding for filtering glyphs
    m_evThreshold = m_properties->addProperty( "Eigenvalue Threshold",
                                               "Clip Glyphs whose smallest eigenvalue is below the given threshold.", 0.01 );
    m_evThreshold->setMin( 0.0 );
    m_evThreshold->setMax( 1.0 );
    m_faThreshold = m_properties->addProperty( "FA Threshold",
                                               "Clip Glyphs whose fractional anisotropy is below the given threshold.", 0.01 );
    m_faThreshold->setMin( 0.0 );
    m_faThreshold->setMax( 1.0 );

    m_gamma = m_properties->addProperty( "Gamma", "Sharpness Parameter of the SuperQuadrics.", 10.0 );
    m_gamma->setMin( 0.0 );
    m_gamma->setMax( 10.0 );

    m_scaling = m_properties->addProperty( "Scaling", "Scaling of Glyphs.", 0.5 );
    m_scaling->setMin( 0.0 );
    m_scaling->setMax( 2.0 );

    m_unifyEV = m_properties->addProperty( "Unify Eigenvalues", "Unify the eigenvalues?.", false );
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
    for ( size_t c = 0; c < 6 * 4; ++c )
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
    m_xSlice = new osg::MatrixTransform();
    m_xSlice->setMatrix( osg::Matrixd::identity() );
    m_ySlice = new osg::MatrixTransform();
    m_ySlice->setMatrix( osg::Matrixd::identity() );
    m_zSlice = new osg::MatrixTransform();
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
    for ( size_t z = 0; z < m_maxZ; ++z )
    {
        for ( size_t y = 0; y < m_maxY; ++y )
        {
            addGlyph( osg::Vec3d( 0.0, y, z ), vertices, texcoords0 );
        }
    }

    // set some callbacks
    m_xSliceGlyphCallback = new GlyphGeometryNodeCallback( geometry );
    geometry->setUpdateCallback( m_xSliceGlyphCallback );
    m_xSliceCallback = new SliceNodeCallback( m_xSlice, osg::Vec3( 1.0, 0.0, 0.0 ), m_xPos );
    m_xSlice->addUpdateCallback( m_xSliceCallback );
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
    for ( size_t z = 0; z < m_maxZ; ++z )
    {
        for ( size_t x = 0; x < m_maxX; ++x )
        {
            addGlyph( osg::Vec3d( x, 0.0, z ), vertices, texcoords0 );
        }
    }

    // set some callbacks
    m_ySliceGlyphCallback = new GlyphGeometryNodeCallback( geometry );
    geometry->setUpdateCallback( m_ySliceGlyphCallback );
    m_ySliceCallback = new SliceNodeCallback( m_ySlice, osg::Vec3( 0.0, 1.0, 0.0 ), m_yPos );
    m_ySlice->addUpdateCallback( m_ySliceCallback );
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
    for ( size_t y = 0; y < m_maxY; ++y )
    {
        for ( size_t x = 0; x < m_maxX; ++x )
        {
            addGlyph( osg::Vec3d( x, y, 0.0 ), vertices, texcoords0 );
        }
    }

    // set some callbacks
    m_zSliceGlyphCallback = new GlyphGeometryNodeCallback( geometry );
    geometry->setUpdateCallback( m_zSliceGlyphCallback );
    m_zSliceCallback = new SliceNodeCallback( m_zSlice, osg::Vec3( 0.0, 0.0, 1.0 ), m_zPos );
    m_zSlice->addUpdateCallback( m_zSliceCallback );
    m_zSlice->addChild( geode );

    m_xSlice->setNodeMask( m_showonX->get( true ) ? 0xFFFFFFFF : 0x0 );
    m_ySlice->setNodeMask( m_showonY->get( true ) ? 0xFFFFFFFF : 0x0 );
    m_zSlice->setNodeMask( m_showonZ->get( true ) ? 0xFFFFFFFF : 0x0 );

    // add the transformation nodes to the output group
    m_output->insert( m_xSlice );
    m_output->insert( m_ySlice );
    m_output->insert( m_zSlice );
}

void WMSuperquadricGlyphs::SliceNodeCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    // this node is a MatrixTransform
    int newPos = m_pos->get();
    if ( newPos != m_oldPos )
    {
        m_oldPos = m_pos->get();
        m_slice->setMatrix( osg::Matrix::translate( m_axe * static_cast< double >( m_oldPos ) ) );
    }

    traverse( node, nv );
}

void WMSuperquadricGlyphs::GlyphGeometryNodeCallback::update( osg::NodeVisitor* /*nv*/, osg::Drawable* /*d*/ )
{
    if ( m_dirty )
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

    // create all these geodes we need
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_output );
    osg::ref_ptr< osg::StateSet > sset = m_output->getOrCreateStateSet();
    sset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    // add shader
    m_shader->apply( m_output );

    // set uniform callbacks and uniforms
    osg::ref_ptr< osg::Uniform > evThreshold = new osg::Uniform( "u_evThreshold", static_cast< float >( m_evThreshold->get() ) );
    evThreshold->setUpdateCallback( new SafeUniformCallback( this ) );
    osg::ref_ptr< osg::Uniform > faThreshold = new osg::Uniform( "u_faThreshold", static_cast< float >( m_faThreshold->get() ) );
    faThreshold->setUpdateCallback( new SafeUniformCallback( this ) );
    osg::ref_ptr< osg::Uniform > unifyEV = new osg::Uniform( "u_unifyEV", m_unifyEV->get() );
    unifyEV->setUpdateCallback( new SafeUniformCallback( this ) );
    osg::ref_ptr< osg::Uniform > scaling = new osg::Uniform( "u_scaling", static_cast< float >( m_scaling->get() ) );
    scaling->setUpdateCallback( new SafeUniformCallback( this ) );
    osg::ref_ptr< osg::Uniform > gamma = new osg::Uniform( "u_gamma", static_cast< float >( m_gamma->get() ) );
    gamma->setUpdateCallback( new SafeUniformCallback( this ) );

    sset->addUniform( evThreshold );
    sset->addUniform( faThreshold );
    sset->addUniform( unifyEV );
    sset->addUniform( scaling );
    sset->addUniform( gamma );

    // loop until the module container requests the module to quit
    while ( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // After waking up, the module has to check whether the shutdownFlag fired. If yes, simply quit the module.

        // woke up since the module is requested to finish
        if ( m_shutdownFlag() )
        {
            break;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Collect changes

        // has the data changes? And even more important: is it valid?
        boost::shared_ptr< WDataSetSingle > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        bool dataValid   = ( newDataSet );
        // TODO(ebaum): as long as we do not have a proper second order tensor field:
        if ( ( newDataSet->getValueSet()->order() != 1 ) && ( newDataSet->getValueSet()->dimension() != 6 ) )
        {
            warnLog() << "Received data with order=" <<  newDataSet->getValueSet()->order() <<
                         " and dimension=" << newDataSet->getValueSet()->dimension() << " not compatible with this module. Ignoring!";
            dataValid = false;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Handle changes

        if ( dataChanged && dataValid )
        {
            // The data is different. Copy it to our internal data variable:
            debugLog() << "Received Data.";

            // also provide progress information
            boost::shared_ptr< WProgress > progress1 = boost::shared_ptr< WProgress >( new WProgress( "Building Glyph Geometry" ) );
            m_progress->addSubProgress( progress1 );

            // get pointers for the new data
            m_dataSet = newDataSet;
            m_dataSetGrid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
            WAssert( m_dataSetGrid, "Dataset does not have a regular 3D grid." );
            m_maxX = m_dataSetGrid->getNbCoordsX();
            m_maxY = m_dataSetGrid->getNbCoordsY();
            m_maxZ = m_dataSetGrid->getNbCoordsZ();
            m_dataSetValueSet = m_dataSet->getValueSet();
            m_nbGlyphsX = m_maxY * m_maxZ;
            m_nbGlyphsY = m_maxX * m_maxZ;
            m_nbGlyphsZ = m_maxX * m_maxY;

            // new data -> update OSG Stuff
            initOSG();
            progress1->finish();
        }

        if ( dataValid && m_showonX->changed() )
        {
            m_xSlice->setNodeMask( m_showonX->get( true ) ? 0xFFFFFFFF : 0x0 );
        }
        if ( dataValid && m_showonY->changed() )
        {
            m_ySlice->setNodeMask( m_showonY->get( true ) ? 0xFFFFFFFF : 0x0 );
        }
        if ( dataValid && m_showonZ->changed() )
        {
            m_zSlice->setNodeMask( m_showonZ->get( true ) ? 0xFFFFFFFF : 0x0 );
        }

        if ( dataValid && m_xPos->changed() )
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
            for ( size_t z = 0; z < m_maxZ; ++z )
            {
                // Calculate the current position along the z direction
                size_t zPre = fixedX + z * m_maxX * m_maxY;

                for ( size_t y = 0; y < m_maxY; ++y )
                {
                    // add glyph
                    addTensor( zPre + y * m_maxX, diag, offdiag );
                }
            }

            m_xSliceGlyphCallback->setNewTensorData( diag, offdiag );
            progress1->finish();
        }

        if ( dataValid && m_yPos->changed() )
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
            for ( size_t z = 0; z < m_maxZ; ++z )
            {
                // Calculate current offset for value index
                size_t zPre = fixedYOffset + ( z * m_maxX * m_maxY );

                for ( size_t x = 0; x < m_maxX; ++x )
                {
                    // add glyph
                    addTensor( zPre + x, diag, offdiag );
                }
            }

            m_ySliceGlyphCallback->setNewTensorData( diag, offdiag );
            progress1->finish();
        }

        if ( dataValid && m_zPos->changed() )
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
            size_t fixedZ = m_zPos->get( true );
            size_t fixedZOffset = fixedZ * m_maxX * m_maxY;
            for ( size_t y = 0; y < m_maxY; ++y )
            {
                // Calculate current offset for value index
                size_t yPre = fixedZOffset + ( y * m_maxX );

                for ( size_t x = 0; x < m_maxX; ++x )
                {
                    // add glyph
                    addTensor( yPre + x, diag, offdiag );
                }
            }

            m_zSliceGlyphCallback->setNewTensorData( diag, offdiag );
            progress1->finish();
        }
    }

    // At this point, the container managing this module signalled to shutdown. The main loop has ended and you should clean up. Always remove
    // allocated memory and remove all OSG nodes.
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_output );
}

void WMSuperquadricGlyphs::activate()
{
    if ( m_output )   // always ensure the root node exists
    {
        if ( m_active->get() )
        {
            m_output->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_output->setNodeMask( 0x0 );
        }
    }

    // Always call WModule's activate!
    WModule::activate();
}

void WMSuperquadricGlyphs::SafeUniformCallback::operator()( osg::Uniform* uniform, osg::NodeVisitor* /* nv */ )
{
    // update some uniforms:
    if ( m_module->m_evThreshold->changed()  && ( uniform->getName() == "u_evThreshold" ) )
    {
        uniform->set( static_cast< float >( m_module->m_evThreshold->get( true ) ) );
    }
    if ( m_module->m_faThreshold->changed()  && ( uniform->getName() == "u_faThreshold" ) )
    {
        uniform->set( static_cast< float >( m_module->m_faThreshold->get( true ) ) );
    }
    if ( m_module->m_gamma->changed()  && ( uniform->getName() == "u_gamma" ) )
    {
        uniform->set( static_cast< float >( m_module->m_gamma->get( true ) ) );
    }
    if ( m_module->m_scaling->changed()  && ( uniform->getName() == "u_scaling" ) )
    {
        uniform->set( static_cast< float >( m_module->m_scaling->get( true ) ) );
    }
    if ( m_module->m_unifyEV->changed()  && ( uniform->getName() == "u_unifyEV" ) )
    {
        uniform->set( m_module->m_unifyEV->get( true ) );
    }
}

