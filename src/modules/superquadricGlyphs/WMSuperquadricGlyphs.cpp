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

WMSuperquadricGlyphs::WMSuperquadricGlyphs():
    WModule()
{
    m_shader = osg::ref_ptr< WShader > ( new WShader( "gpuSuperQuadrics" ) );
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
    m_xPos           = m_properties->addProperty( "X Pos of the slice", "Slice X position.", 80, m_propCondition );
    m_yPos           = m_properties->addProperty( "Y Pos of the slice", "Slice Y position.", 100, m_propCondition );
    m_zPos           = m_properties->addProperty( "Z Pos of the slice", "Slice Z position.", 80, m_propCondition );
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

inline void WMSuperquadricGlyphs::addGlyph( osg::Vec3 position, size_t i,
                                                                         osg::ref_ptr< osg::Vec3Array > vertices,
                                                                         osg::ref_ptr< osg::Vec3Array > orientation,
                                                                         osg::ref_ptr< osg::Vec3Array > diags,
                                                                         osg::ref_ptr< osg::Vec3Array > offdiags )
{
    // the diag and offdiag elements
    osg::Vec3 diag = osg::Vec3( m_dataSetValueSet->getScalarDouble( i * 6 + 0 ),
                                m_dataSetValueSet->getScalarDouble( i * 6 + 3 ),
                                m_dataSetValueSet->getScalarDouble( i * 6 + 5 ) );
    osg::Vec3 offdiag = osg::Vec3( m_dataSetValueSet->getScalarDouble( i * 6 + 1 ),
                                   m_dataSetValueSet->getScalarDouble( i * 6 + 2 ),
                                   m_dataSetValueSet->getScalarDouble( i * 6 + 4 ) );

    // Front Face
    vertices->push_back( position );
    orientation->push_back( osg::Vec3( -1.0, -1.0, -1.0 ) );
    diags->push_back( diag );
    offdiags->push_back( offdiag );

    vertices->push_back( position );
    orientation->push_back( osg::Vec3(  1.0, -1.0, -1.0 ) );
    diags->push_back( diag );
    offdiags->push_back( offdiag );

    vertices->push_back( position );
    orientation->push_back( osg::Vec3(  1.0,  1.0, -1.0 ) );
    diags->push_back( diag );
    offdiags->push_back( offdiag );

    vertices->push_back( position );
    orientation->push_back( osg::Vec3( -1.0,  1.0, -1.0 ) );
    diags->push_back( diag );
    offdiags->push_back( offdiag );

    // Back Face
    vertices->push_back( position );
    orientation->push_back( osg::Vec3( -1.0, -1.0, 1.0 ) );
    diags->push_back( diag );
    offdiags->push_back( offdiag );

    vertices->push_back( position );
    orientation->push_back( osg::Vec3(  1.0, -1.0, 1.0 ) );
    diags->push_back( diag );
    offdiags->push_back( offdiag );

    vertices->push_back( position );
    orientation->push_back( osg::Vec3(  1.0,  1.0, 1.0 ) );
    diags->push_back( diag );
    offdiags->push_back( offdiag );

    vertices->push_back( position );
    orientation->push_back( osg::Vec3( -1.0,  1.0, 1.0 ) );
    diags->push_back( diag );
    offdiags->push_back( offdiag );

    // Top Face
    vertices->push_back( position );
    orientation->push_back( osg::Vec3( -1.0,  1.0, -1.0 ) );
    diags->push_back( diag );
    offdiags->push_back( offdiag );

    vertices->push_back( position );
    orientation->push_back( osg::Vec3(  1.0,  1.0, -1.0 ) );
    diags->push_back( diag );
    offdiags->push_back( offdiag );

    vertices->push_back( position );
    orientation->push_back( osg::Vec3(  1.0,  1.0, 1.0 ) );
    diags->push_back( diag );
    offdiags->push_back( offdiag );

    vertices->push_back( position );
    orientation->push_back( osg::Vec3( -1.0,  1.0, 1.0 ) );
    diags->push_back( diag );
    offdiags->push_back( offdiag );

    // Bottom Face
    vertices->push_back( position );
    orientation->push_back( osg::Vec3( -1.0, -1.0, -1.0 ) );
    diags->push_back( diag );
    offdiags->push_back( offdiag );

    vertices->push_back( position );
    orientation->push_back( osg::Vec3(  1.0,  -1.0, -1.0 ) );
    diags->push_back( diag );
    offdiags->push_back( offdiag );

    vertices->push_back( position );
    orientation->push_back( osg::Vec3(  1.0,  -1.0, 1.0 ) );
    diags->push_back( diag );
    offdiags->push_back( offdiag );

    vertices->push_back( position );
    orientation->push_back( osg::Vec3( -1.0,  -1.0, 1.0 ) );
    diags->push_back( diag );
    offdiags->push_back( offdiag );

    // Left Face
    vertices->push_back( position );
    orientation->push_back( osg::Vec3( -1.0, -1.0, -1.0 ) );
    diags->push_back( diag );
    offdiags->push_back( offdiag );

    vertices->push_back( position );
    orientation->push_back( osg::Vec3(  -1.0, 1.0, -1.0 ) );
    diags->push_back( diag );
    offdiags->push_back( offdiag );

    vertices->push_back( position );
    orientation->push_back( osg::Vec3(  -1.0, 1.0, 1.0 ) );
    diags->push_back( diag );
    offdiags->push_back( offdiag );

    vertices->push_back( position );
    orientation->push_back( osg::Vec3( -1.0, -1.0, 1.0 ) );
    diags->push_back( diag );
    offdiags->push_back( offdiag );

    // Right Face
    vertices->push_back( position );
    orientation->push_back( osg::Vec3( 1.0, -1.0, -1.0 ) );
    diags->push_back( diag );
    offdiags->push_back( offdiag );

    vertices->push_back( position );
    orientation->push_back( osg::Vec3( 1.0, 1.0, -1.0 ) );
    diags->push_back( diag );
    offdiags->push_back( offdiag );

    vertices->push_back( position );
    orientation->push_back( osg::Vec3( 1.0, 1.0, 1.0 ) );
    diags->push_back( diag );
    offdiags->push_back( offdiag );

    vertices->push_back( position );
    orientation->push_back( osg::Vec3( 1.0, -1.0, 1.0 ) );
    diags->push_back( diag );
    offdiags->push_back( offdiag );
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

    // loop until the module container requests the module to quit
    while ( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
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

        bool posChanged = m_xPos->changed() || m_yPos->changed() || m_zPos->changed();

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Handle changes

        // data changes
        if ( ( dataChanged || posChanged ) && dataValid  )
        {
            // The data is different. Copy it to our internal data variable:
            debugLog() << "Received Data.";

            // get pointers for the new data
            m_dataSet = newDataSet;
            m_dataSetGrid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
            WAssert( m_dataSetGrid, "Dataset does not have a regular 3D grid." );
            size_t maxX = m_dataSetGrid->getNbCoordsX();
            size_t maxY = m_dataSetGrid->getNbCoordsY();
            size_t maxZ = m_dataSetGrid->getNbCoordsZ();
            m_dataSetValueSet = m_dataSet->getValueSet();
            size_t valueSetSize = m_dataSetValueSet->size();
            size_t nbGlyphs = ( maxY * maxZ ) + ( maxX * maxZ ) + ( maxX * maxY );

            // create a new geode containing the glyphs proxy geometry
            osg::ref_ptr< osg::Geode > newRootNode = new osg::Geode();

            // fill the geode with glyph proxy geometry
            osg::ref_ptr< osg::Geometry > geometry = new osg::Geometry();
            newRootNode->addDrawable( geometry );

            // as we use quads -> four verts per quad, 6 quad per qlyph
            // NOTE: the vertices are only the position of the glyph. The shader actually spans them up according to the proper bbox
            osg::ref_ptr< osg::Vec3Array > vertices = new osg::Vec3Array;
            vertices->reserve( nbGlyphs * 6 * 4 );
            osg::ref_ptr< osg::Vec3Array > texcoords0 = new osg::Vec3Array;
            texcoords0->reserve( nbGlyphs * 6 * 4 );
            osg::ref_ptr< osg::Vec3Array > texcoords1 = new osg::Vec3Array;
            texcoords1->reserve( nbGlyphs * 6 * 4 );
            osg::ref_ptr< osg::Vec3Array > texcoords2 = new osg::Vec3Array;
            texcoords2->reserve( nbGlyphs * 6 * 4 );

            debugLog() << "Grid Size: " << maxX << "x" << maxY << "x" << maxZ << " (" << valueSetSize << ")";
            debugLog() << "Creating " << nbGlyphs << " glyphs.";

            // also provide progress information
            boost::shared_ptr< WProgress > progress1 = boost::shared_ptr< WProgress >( new WProgress( "Building Glyph Geometry", 3 ) );
            m_progress->addSubProgress( progress1 );

            // NOTE: the following code is surely ugly but it should be faster

            // to speed up calculations, implement WGridRegular3D::getPosition
            osg::Vec3d origin = wge::osgVec3( m_dataSetGrid->getOrigin() );
            osg::Vec3d dirX = wge::osgVec3( m_dataSetGrid->getDirectionX() );
            osg::Vec3d dirY = wge::osgVec3( m_dataSetGrid->getDirectionY() );
            osg::Vec3d dirZ = wge::osgVec3( m_dataSetGrid->getDirectionZ() );
            // NOTE: this line is from WGridRegular3D.cpp
            //return m_origin + iX * m_directionX + iY * m_directionY + iZ * m_directionZ;

            // x = const -> handle xPos property
            size_t fixedX = m_xPos->get( true );
            osg::Vec3 posAlongZ = origin + dirX * fixedX;
            for ( size_t z = 0; z < maxZ; ++z )
            {
                // Calculate the current position along the z direction
                posAlongZ += dirZ;
                osg::Vec3 posAlongY = posAlongZ;
                size_t zPre = fixedX + z * maxX * maxY;

                for ( size_t y = 0; y < maxY; ++y )
                {
                    posAlongY += dirY;
                    // add glyph
                    addGlyph( posAlongY, zPre + y * maxX, vertices, texcoords0, texcoords1, texcoords2 );
                }
            }
            ++( *progress1 );

            // y = const -> handle yPos property
            size_t fixedY = m_yPos->get( true );
            size_t fixedYOffset = fixedY * maxX;
            posAlongZ = origin + dirY * fixedY;
            for ( size_t z = 0; z < maxZ; ++z )
            {
                // Calculate the current position along the z direction
                posAlongZ += dirZ;
                osg::Vec3 posAlongX = posAlongZ;

                // Calculate current offset for value index
                size_t zPre = fixedYOffset + ( z * maxX * maxY );

                for ( size_t x = 0; x < maxX; ++x )
                {
                    posAlongX += dirX;
                    // add glyph
                    addGlyph( posAlongX, zPre + x, vertices, texcoords0, texcoords1, texcoords2 );
                }
            }
            ++( *progress1 );

            // z = const -> handle zPos property
            size_t fixedZ = m_zPos->get( true );
            size_t fixedZOffset = fixedZ * maxX * maxY;
            osg::Vec3 posAlongY = origin + dirZ * fixedZ;
            for ( size_t y = 0; y < maxY; ++y )
            {
                // Calculate the current position along the y direction
                posAlongY += dirY;
                osg::Vec3 posAlongX = posAlongY;

                // Calculate current offset for value index
                size_t yPre = fixedZOffset + ( y * maxX );

                for ( size_t x = 0; x < maxX; ++x )
                {
                    posAlongX += dirX;
                    // add glyph
                    addGlyph( posAlongX, yPre + x, vertices, texcoords0, texcoords1, texcoords2 );
                }
            }
            ++( *progress1 );
            progress1->finish();

            // add arrays
            geometry->setVertexArray( vertices );
            geometry->setUseVertexBufferObjects( true );
            geometry->setTexCoordArray( 0, texcoords0 );
            geometry->setTexCoordArray( 1, texcoords1 );
            geometry->setTexCoordArray( 2, texcoords2 );
            osg::ref_ptr< osg::DrawArrays > da = new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, nbGlyphs * 6 * 4 );
            geometry->addPrimitiveSet( da );

            // update stateset
            osg::ref_ptr< osg::StateSet > sset = newRootNode->getOrCreateStateSet();
            sset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

            // add shader
            m_shader->apply( newRootNode );

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

            // remove old node
            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_outputGeode );
            m_outputGeode = newRootNode;

            // please also ensure that, according to m_active, the node is active or not. Setting it here allows the user to deactivate modules
            // in project files for example.
            m_outputGeode->setNodeMask( m_active->get() ? 0xFFFFFFFF : 0x0 );

            // set the update callback

            // add new node
            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_outputGeode );
        }
    }

    // At this point, the container managing this module signalled to shutdown. The main loop has ended and you should clean up. Always remove
    // allocated memory and remove all OSG nodes.
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_outputGeode );
}

void WMSuperquadricGlyphs::activate()
{
    if ( m_outputGeode )   // always ensure the root node exists
    {
        if ( m_active->get() )
        {
            m_outputGeode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_outputGeode->setNodeMask( 0x0 );
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

