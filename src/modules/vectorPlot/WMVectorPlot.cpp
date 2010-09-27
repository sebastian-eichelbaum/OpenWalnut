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
#include <string>
#include <vector>

#include <osg/ShapeDrawable>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Material>
#include <osg/StateAttribute>

#include "../../kernel/WKernel.h"
#include "../../common/WColor.h"

#include "WMVectorPlot.h"
#include "WMVectorPlot.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMVectorPlot )

WMVectorPlot::WMVectorPlot():
    WModule(), m_mat( 4, 4 )
{
}

WMVectorPlot::~WMVectorPlot()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMVectorPlot::factory() const
{
    return boost::shared_ptr< WModule >( new WMVectorPlot() );
}

const char** WMVectorPlot::getXPMIcon() const
{
    return vectorplot_xpm;
}

const std::string WMVectorPlot::getName() const
{
    return "Vector Plot";
}

const std::string WMVectorPlot::getDescription() const
{
    return "This module displays vector data as small vector representations on navigation slices and other surfaces.";
}

void WMVectorPlot::connectors()
{
    m_input = boost::shared_ptr< WModuleInputData < WDataSetVector > >(
        new WModuleInputData< WDataSetVector >( shared_from_this(), "in", "The dataset to display" )
        );

    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMVectorPlot::properties()
{
    m_xSlice           = m_properties->addProperty( "X Pos of the slice", "Description.", 80 );
    m_ySlice           = m_properties->addProperty( "Y Pos of the slice", "Description.", 100 );
    m_zSlice           = m_properties->addProperty( "Z Pos of the slice", "Description.", 80 );

    m_xSlice->setHidden( true );
    m_ySlice->setHidden( true );
    m_zSlice->setHidden( true );

    m_projectOnSlice = m_properties->addProperty( "Projection",
                                                   "If active, the vectors are projected into the surface "
                                                   "used to place them. Thus their "
                                                   "representation is tangential to the surface.",
                                                   false );
    m_coloringMode   = m_properties->addProperty( "Direction coloring",
                                                   "Draw each vector in a color indicating its direction. ", false );
    m_aColor         = m_properties->addProperty( "Color",
                                                   "This color is used if direction coloring is deactivated.",
                                                   WColor( 1.0, 0.0, 0.0, 1.0 ) );

    m_showonX        = m_properties->addProperty( "Show sagittal", "Show vectors on sagittal slice.", true );
    m_showonY        = m_properties->addProperty( "Show coronal", "Show vectors on coronal slice.", true );
    m_showonZ        = m_properties->addProperty( "Show axial", "Show vectors on axial slice.", true );

    m_xSlice->setMin( 0 );
    m_xSlice->setMax( 160 );
    m_ySlice->setMin( 0 );
    m_ySlice->setMax( 200 );
    m_zSlice->setMin( 0 );
    m_zSlice->setMax( 160 );

    WModule::properties();
}

void WMVectorPlot::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );

    ready();

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Vector Plot", 2 ) );
        m_progress->addSubProgress( progress );

        boost::shared_ptr< WDataSetVector > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        bool dataValid   = ( newDataSet );

        if( dataChanged && dataValid )
        {
            m_dataSet = newDataSet;
        }

        if( m_xSlice->changed() || m_ySlice->changed() || m_zSlice->changed() )
        {
            osg::ref_ptr< osg::Geode > newRootNode = new osg::Geode();

            newRootNode->addDrawable( buildPlotSlices() );
            if( m_rootNode )
            {
                WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
            }

            ++*progress;

            m_rootNode = newRootNode;
            m_rootNode->setNodeMask( m_active->get() ? 0xFFFFFFFF : 0x0 );
            m_rootNode->addUpdateCallback( new SafeUpdateCallback( this ) );

            m_shader = osg::ref_ptr< WShader > ( new WShader( "WMVectorPlot", m_localPath ) );
            m_shader->apply( m_rootNode );

            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );
        }
        progress->finish();
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

osg::ref_ptr<osg::Geometry> WMVectorPlot::buildPlotSlices()
{
    boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Vector Plot", 4 ) );
    m_progress->addSubProgress( progress );

    wmath::WPosition current = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();

    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
    boost::shared_ptr< WValueSet< float > > vals = boost::shared_dynamic_cast< WValueSet<float> >( m_dataSet->getValueSet() );

    m_xSlice->setMax( grid->getNbCoordsX() );
    m_ySlice->setMax( grid->getNbCoordsY() );
    m_zSlice->setMax( grid->getNbCoordsZ() );

    wmath::WPosition texPos = grid->worldCoordToTexCoord( current );
    int xSlice = texPos[0] * grid->getNbCoordsX();
    int ySlice = texPos[1] * grid->getNbCoordsY();
    int zSlice = texPos[2] * grid->getNbCoordsZ();

    m_xSlice->set( xSlice );
    m_ySlice->set( ySlice );
    m_zSlice->set( zSlice );



    // When working with the OpenSceneGraph, always use ref_ptr to store pointers to OSG objects. This allows OpenSceneGraph to manage
    // its resources automatically.

    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    ref_ptr< osg::Vec4Array > colors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    ref_ptr< osg::Geometry > geometry = ref_ptr< osg::Geometry >( new osg::Geometry );

    WColor c = m_aColor->get( true );
    osg::Vec4 cc( c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha() );

    if( ( ( *m_dataSet ).getValueSet()->order() == 1 ) && ( ( *m_dataSet ).getValueSet()->dimension() == 3 )
            && ( ( *m_dataSet ).getValueSet()->getDataType() == 16 ) )
    {
        int maxX = m_xSlice->getMax()->getMax();
        int maxY = m_ySlice->getMax()->getMax();
        int maxZ = m_zSlice->getMax()->getMax();

        if( m_showonZ->get( true ) )
        {
            for( int x = 0; x < maxX; ++x )
            {
                for( int y = 0; y < maxY; ++y )
                {
                    float xx = vals->getScalar( ( x + y * maxX + zSlice * maxX * maxY ) * 3 ) / 2.;
                    float yy = vals->getScalar( ( x + y * maxX + zSlice * maxX * maxY ) * 3 + 1 ) / 2.;
                    float zz = vals->getScalar( ( x + y * maxX + zSlice * maxX * maxY ) * 3 + 2 ) / 2.;

                    if( !m_projectOnSlice->get( true ) )
                    {
                        vertices->push_back( osg::Vec3( x - xx, y - yy, zSlice - zz ) );
                        vertices->push_back( osg::Vec3( x + xx, y + yy, zSlice + zz ) );
                        if( m_coloringMode->get( true ) )
                        {
                            colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                        }
                        else
                        {
                            colors->push_back( cc );
                            colors->push_back( cc );
                        }
                    }
                    else
                    {
                        vertices->push_back( osg::Vec3( x - xx, y - yy, zSlice + 0.4f ) );
                        vertices->push_back( osg::Vec3( x + xx, y + yy, zSlice + 0.4f ) );
                        vertices->push_back( osg::Vec3( x - xx, y - yy, zSlice + 0.6f ) );
                        vertices->push_back( osg::Vec3( x + xx, y + yy, zSlice + 0.6f ) );
                        if( m_coloringMode->get( true ) )
                        {
                            colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                        }
                        else
                        {
                            colors->push_back( cc );
                            colors->push_back( cc );
                            colors->push_back( cc );
                            colors->push_back( cc );
                        }
                    }
                }
            }
        }

        ++*progress;

        if( m_showonY->get( true ) )
        {
            for( int x = 0; x < maxX; ++x )
            {
                for( int z = 0; z < maxZ; ++z )
                {
                    float xx = vals->getScalar( ( x + ySlice * maxX + z * maxX * maxY ) * 3 ) / 2.;
                    float yy = vals->getScalar( ( x + ySlice * maxX + z * maxX * maxY ) * 3 + 1 ) / 2.;
                    float zz = vals->getScalar( ( x + ySlice * maxX + z * maxX * maxY ) * 3 + 2 ) / 2.;

                    if( !m_projectOnSlice->get( true ) )
                    {
                        vertices->push_back( osg::Vec3( x - xx, ySlice - yy, z - zz ) );
                        vertices->push_back( osg::Vec3( x + xx, ySlice + yy, z + zz ) );
                        if( m_coloringMode->get( true ) )
                        {
                            colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                        }
                        else
                        {
                            colors->push_back( cc );
                            colors->push_back( cc );
                        }
                    }
                    else
                    {
                        vertices->push_back( osg::Vec3( x - xx, ySlice + 0.4f, z - zz ) );
                        vertices->push_back( osg::Vec3( x + xx, ySlice + 0.4f, z + zz ) );
                        vertices->push_back( osg::Vec3( x - xx, ySlice + 0.6f, z - zz ) );
                        vertices->push_back( osg::Vec3( x + xx, ySlice + 0.6f, z + zz ) );
                        if( m_coloringMode->get( true ) )
                        {
                            colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                        }
                        else
                        {
                            colors->push_back( cc );
                            colors->push_back( cc );
                            colors->push_back( cc );
                            colors->push_back( cc );
                        }
                    }
                }
            }
        }

        ++*progress;

        if( m_showonX->get( true ) )
        {
            for( int y = 0; y < maxY; ++y )
            {
                for( int z = 0; z < maxZ; ++z )
                {
                    float xx = vals->getScalar( ( xSlice + y * maxX + z * maxX * maxY ) * 3 ) / 2.;
                    float yy = vals->getScalar( ( xSlice + y * maxX + z * maxX * maxY ) * 3 + 1 ) / 2.;
                    float zz = vals->getScalar( ( xSlice + y * maxX + z * maxX * maxY ) * 3 + 2 ) / 2.;

                    if( !m_projectOnSlice->get( true ) )
                    {
                        vertices->push_back( osg::Vec3( xSlice + xx, y + yy, z + zz ) );
                        vertices->push_back( osg::Vec3( xSlice - xx, y - yy, z - zz ) );
                        if( m_coloringMode->get( true ) )
                        {
                            colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                        }
                        else
                        {
                            colors->push_back( cc );
                            colors->push_back( cc );
                        }
                    }
                    else
                    {
                        vertices->push_back( osg::Vec3( xSlice + 0.4f, y + yy, z + zz ) );
                        vertices->push_back( osg::Vec3( xSlice + 0.4f, y - yy, z - zz ) );
                        vertices->push_back( osg::Vec3( xSlice + 0.6f, y + yy, z + zz ) );
                        vertices->push_back( osg::Vec3( xSlice + 0.6f, y - yy, z - zz ) );
                        if( m_coloringMode->get( true ) )
                        {
                            colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                        }
                        else
                        {
                            colors->push_back( cc );
                            colors->push_back( cc );
                            colors->push_back( cc );
                            colors->push_back( cc );
                        }
                    }
                }
            }
        }

        ++*progress;

        {
            // Transform the vertices according to the matrix given in the grid.
            m_mat = grid->getTransformationMatrix();
            transformVerts( vertices );
        }

        for( size_t i = 0; i < vertices->size(); ++i )
        {
            osg::DrawElementsUInt* line = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );
            line->push_back( i );
            line->push_back( i + 1 );
            ++i;
            geometry->addPrimitiveSet( line );
        }
    }

    geometry->setVertexArray( vertices );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

    progress->finish();

    return geometry;
}


void WMVectorPlot::SafeUpdateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    wmath::WPosition current = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();
    wmath::WPosition old( m_module->m_xSlice->get(), m_module->m_ySlice->get(), m_module->m_zSlice->get() );

    if( ( old != current ) || m_module->m_coloringMode->changed() || m_module->m_aColor->changed() || m_module->m_projectOnSlice->changed() ||
            m_module->m_showonX->changed() || m_module->m_showonY->changed() || m_module->m_showonZ->changed() )
    {
        osg::ref_ptr<osg::Drawable> old = osg::ref_ptr<osg::Drawable>( m_module->m_rootNode->getDrawable( 0 ) );
        m_module->m_rootNode->removeDrawable( old );
        m_module->m_rootNode->addDrawable( m_module->buildPlotSlices() );
    }

    traverse( node, nv );
}


void WMVectorPlot::activate()
{
    if( m_rootNode )   // always ensure the root node exists
    {
        if( m_active->get() )
        {
            m_rootNode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_rootNode->setNodeMask( 0x0 );
        }
    }

    // Always call WModule's activate!
    WModule::activate();
}

void WMVectorPlot::transformVerts( osg::ref_ptr< osg::Vec3Array > verts )
{
    for( size_t i = 0; i < verts->size(); ++i )
    {
        std::vector< double > resultPos4D( 4 );
        resultPos4D[0] = m_mat( 0, 0 ) * ( *verts )[i][0] + m_mat( 0, 1 ) * ( *verts )[i][1] + m_mat( 0, 2 ) * ( *verts )[i][2] + m_mat( 0, 3 ) * 1;
        resultPos4D[1] = m_mat( 1, 0 ) * ( *verts )[i][0] + m_mat( 1, 1 ) * ( *verts )[i][1] + m_mat( 1, 2 ) * ( *verts )[i][2] + m_mat( 1, 3 ) * 1;
        resultPos4D[2] = m_mat( 2, 0 ) * ( *verts )[i][0] + m_mat( 2, 1 ) * ( *verts )[i][1] + m_mat( 2, 2 ) * ( *verts )[i][2] + m_mat( 2, 3 ) * 1;
        resultPos4D[3] = m_mat( 3, 0 ) * ( *verts )[i][0] + m_mat( 3, 1 ) * ( *verts )[i][1] + m_mat( 3, 2 ) * ( *verts )[i][2] + m_mat( 3, 3 ) * 1;

        ( *verts )[i][0] = resultPos4D[0] / resultPos4D[3];
        ( *verts )[i][1] = resultPos4D[1] / resultPos4D[3];
        ( *verts )[i][2] = resultPos4D[2] / resultPos4D[3];
    }
}
