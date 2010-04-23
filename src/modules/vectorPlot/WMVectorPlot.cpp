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

#include <string>

#include <osg/ShapeDrawable>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Material>
#include <osg/StateAttribute>

#include "../../kernel/WKernel.h"
#include "../../common/WColor.h"

#include "WMVectorPlot.h"
#include "vectorplot.xpm"

WMVectorPlot::WMVectorPlot():
    WModule()
{
    m_shader = osg::ref_ptr< WShader > ( new WShader( "vectorplot" ) );
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
    m_xPos           = m_properties->addProperty( "X Pos of the slice", "Description.", 80 );
    m_yPos           = m_properties->addProperty( "Y Pos of the slice", "Description.", 100 );
    m_zPos           = m_properties->addProperty( "Z Pos of the slice", "Description.", 80 );

    m_xPos->setHidden( true );
    m_yPos->setHidden( true );
    m_zPos->setHidden( true );

    m_projectOnSlice = m_properties->addProperty( "Projection",
                                                   "If active, the vectors are projected into the surface "
                                                   "used to place them. Thus their "
                                                   "representation is tangential to the surface.",
                                                   false );
    m_coloringMode   = m_properties->addProperty( "Direction Coloring",
                                                   "Draw each vector in a color indicating its direction. ", false );
    m_aColor         = m_properties->addProperty( "Color",
                                                   "This color is used if direction coloring is deactivated.",
                                                   WColor( 1.0, 0.0, 0.0, 1.0 ) );

    m_showonX        = m_properties->addProperty( "Show Sagittal", "Show vectors on sagittal slice.", true );
    m_showonY        = m_properties->addProperty( "Show Coronal", "Show vectors on coronal slice.", true );
    m_showonZ        = m_properties->addProperty( "Show Axial", "Show vectors on axial slice.", true );

    m_xPos->setMin( 0 );
    m_xPos->setMax( 160 );
    m_yPos->setMin( 0 );
    m_yPos->setMax( 200 );
    m_zPos->setMin( 0 );
    m_zPos->setMax( 160 );
}

void WMVectorPlot::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );

    ready();

    while ( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if ( m_shutdownFlag() )
        {
            break;
        }

        boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Vector Plot", 2 ) );
        m_progress->addSubProgress( progress );

        boost::shared_ptr< WDataSetVector > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        bool dataValid   = ( newDataSet );

        if ( dataChanged && dataValid )
        {
            m_dataSet = newDataSet;
        }

        if ( m_xPos->changed() || m_yPos->changed() || m_zPos->changed() )
        {
            osg::ref_ptr< osg::Geode > newRootNode = new osg::Geode();

            newRootNode->addDrawable( buildPlotSlices() );
            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );

            ++*progress;

            m_rootNode = newRootNode;
            m_rootNode->setNodeMask( m_active->get() ? 0xFFFFFFFF : 0x0 );
            m_rootNode->addUpdateCallback( new SafeUpdateCallback( this ) );

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

    int xPos = current[0];
    int yPos = current[1];
    int zPos = current[2];

    m_xPos->set( xPos );
    m_yPos->set( yPos );
    m_zPos->set( zPos );

    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
    boost::shared_ptr< WValueSet< float > > vals = boost::shared_dynamic_cast< WValueSet<float> >( m_dataSet->getValueSet() );

    m_xPos->setMax( grid->getNbCoordsX() );
    m_yPos->setMax( grid->getNbCoordsY() );
    m_zPos->setMax( grid->getNbCoordsZ() );


    // When working with the OpenSceneGraph, always use ref_ptr to store pointers to OSG objects. This allows OpenSceneGraph to manage
    // its resources automatically.

    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    ref_ptr< osg::Vec4Array > colors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    ref_ptr< osg::Geometry > geometry = ref_ptr< osg::Geometry >( new osg::Geometry );

    WColor c = m_aColor->get( true );
    osg::Vec4 cc( c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha() );

    if ( ( ( *m_dataSet ).getValueSet()->order() == 1 ) && ( ( *m_dataSet ).getValueSet()->dimension() == 3 )
            && ( ( *m_dataSet ).getValueSet()->getDataType() == 16 ) )
    {
        int maxX = m_xPos->getMax()->getMax();
        int maxY = m_yPos->getMax()->getMax();
        int maxZ = m_zPos->getMax()->getMax();

        if ( m_showonZ->get( true ) )
        {
            for ( int x = 0; x < maxX; ++x )
            {
                for ( int y = 0; y < maxY; ++y )
                {
                    float xx = vals->getScalar( ( x + y * maxX + zPos * maxX * maxY ) * 3 ) / 2.;
                    float yy = vals->getScalar( ( x + y * maxX + zPos * maxX * maxY ) * 3 + 1 ) / 2.;
                    float zz = vals->getScalar( ( x + y * maxX + zPos * maxX * maxY ) * 3 + 2 ) / 2.;

                    if ( !m_projectOnSlice->get( true ) )
                    {
                        vertices->push_back( osg::Vec3( x + 0.5f - xx, y + 0.5f - yy, zPos + 0.5f - zz ) );
                        vertices->push_back( osg::Vec3( x + 0.5f + xx, y + 0.5f + yy, zPos + 0.5f + zz ) );
                        if ( m_coloringMode->get( true ) )
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
                        vertices->push_back( osg::Vec3( x + 0.5f - xx, y + 0.5f - yy, zPos + 0.4f ) );
                        vertices->push_back( osg::Vec3( x + 0.5f + xx, y + 0.5f + yy, zPos + 0.4f ) );
                        vertices->push_back( osg::Vec3( x + 0.5f - xx, y + 0.5f - yy, zPos + 0.6f ) );
                        vertices->push_back( osg::Vec3( x + 0.5f + xx, y + 0.5f + yy, zPos + 0.6f ) );
                        if ( m_coloringMode->get( true ) )
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

        if ( m_showonY->get( true ) )
        {
            for ( int x = 0; x < maxX; ++x )
            {
                for ( int z = 0; z < maxZ; ++z )
                {
                    float xx = vals->getScalar( ( x + yPos * maxX + z * maxX * maxY ) * 3 ) / 2.;
                    float yy = vals->getScalar( ( x + yPos * maxX + z * maxX * maxY ) * 3 + 1 ) / 2.;
                    float zz = vals->getScalar( ( x + yPos * maxX + z * maxX * maxY ) * 3 + 2 ) / 2.;

                    if ( !m_projectOnSlice->get( true ) )
                    {
                        vertices->push_back( osg::Vec3( x + 0.5f - xx, yPos + 0.5f - yy, z + 0.5f - zz ) );
                        vertices->push_back( osg::Vec3( x + 0.5f + xx, yPos + 0.5f + yy, z + 0.5f + zz ) );
                        if ( m_coloringMode->get( true ) )
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
                        vertices->push_back( osg::Vec3( x + 0.5f - xx, yPos + 0.4f, z + 0.5f - zz ) );
                        vertices->push_back( osg::Vec3( x + 0.5f + xx, yPos + 0.4f, z + 0.5f + zz ) );
                        vertices->push_back( osg::Vec3( x + 0.5f - xx, yPos + 0.6f, z + 0.5f - zz ) );
                        vertices->push_back( osg::Vec3( x + 0.5f + xx, yPos + 0.6f, z + 0.5f + zz ) );
                        if ( m_coloringMode->get( true ) )
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

        if ( m_showonX->get( true ) )
        {
            for ( int y = 0; y < maxY; ++y )
            {
                for ( int z = 0; z < maxZ; ++z )
                {
                    float xx = vals->getScalar( ( xPos + y * maxX + z * maxX * maxY ) * 3 ) / 2.;
                    float yy = vals->getScalar( ( xPos + y * maxX + z * maxX * maxY ) * 3 + 1 ) / 2.;
                    float zz = vals->getScalar( ( xPos + y * maxX + z * maxX * maxY ) * 3 + 2 ) / 2.;

                    if ( !m_projectOnSlice->get( true ) )
                    {
                        vertices->push_back( osg::Vec3( xPos + 0.5f + xx, y + 0.5f + yy, z + 0.5f + zz ) );
                        vertices->push_back( osg::Vec3( xPos + 0.5f - xx, y + 0.5f - yy, z + 0.5f - zz ) );
                        if ( m_coloringMode->get( true ) )
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
                        vertices->push_back( osg::Vec3( xPos + 0.4f, y + 0.5f + yy, z + 0.5f + zz ) );
                        vertices->push_back( osg::Vec3( xPos + 0.4f, y + 0.5f - yy, z + 0.5f - zz ) );
                        vertices->push_back( osg::Vec3( xPos + 0.6f, y + 0.5f + yy, z + 0.5f + zz ) );
                        vertices->push_back( osg::Vec3( xPos + 0.6f, y + 0.5f - yy, z + 0.5f - zz ) );
                        if ( m_coloringMode->get( true ) )
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

        for ( size_t i = 0; i < vertices->size(); ++i )
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
    wmath::WPosition old( m_module->m_xPos->get(), m_module->m_yPos->get(), m_module->m_zPos->get() );

    if ( ( old != current ) || m_module->m_coloringMode->changed() || m_module->m_aColor->changed() || m_module->m_projectOnSlice->changed() ||
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
    if ( m_rootNode )   // always ensure the root node exists
    {
        if ( m_active->get() )
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

