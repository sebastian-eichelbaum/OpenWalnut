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

#include <osg/Geode>
#include <osg/Group>
#include <osg/Material>
#include <osg/ShapeDrawable>
#include <osg/StateAttribute>

#include "core/common/WColor.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WSelectionManager.h"
#include "WMVectorPlot.h"
#include "WMVectorPlot.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMVectorPlot )

WMVectorPlot::WMVectorPlot():
    WModule()
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
    m_xSlice           = m_properties->addProperty( "X Pos of the slice", "Description.", 80. );
    m_ySlice           = m_properties->addProperty( "Y Pos of the slice", "Description.", 100. );
    m_zSlice           = m_properties->addProperty( "Z Pos of the slice", "Description.", 80. );

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

    m_showOnSagittal        = m_properties->addProperty( "Show sagittal", "Show vectors on sagittal slice.", true );
    m_showOnCoronal        = m_properties->addProperty( "Show coronal", "Show vectors on coronal slice.", true );
    m_showOnAxial        = m_properties->addProperty( "Show axial", "Show vectors on axial slice.", true );

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
            m_rootNode->addUpdateCallback( new WGEFunctorCallback< osg::Node >( boost::bind( &WMVectorPlot::updateCallback, this ) ) );

            m_shader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMVectorPlot", m_localPath ) );
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

    WPosition current = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();

    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
    boost::shared_ptr< WValueSet< float > > vals = boost::shared_dynamic_cast< WValueSet<float> >( m_dataSet->getValueSet() );

    int maxX = grid->getNbCoordsX() - 1;
    int maxY = grid->getNbCoordsY() - 1;
    int maxZ = grid->getNbCoordsZ() - 1;
    int nbX = maxX + 1;
    int nbY = maxY + 1;
    int nbZ = maxZ + 1;

    m_xSlice->setMax( grid->getNbCoordsX() - 1 );
    m_ySlice->setMax( grid->getNbCoordsY() - 1 );
    m_zSlice->setMax( grid->getNbCoordsZ() - 1 );

    WPosition texPos = grid->worldCoordToTexCoord( current );
    double xSlice = texPos[0] * grid->getNbCoordsX() - 0.5;
    double ySlice = texPos[1] * grid->getNbCoordsY() - 0.5;
    double zSlice = texPos[2] * grid->getNbCoordsZ() - 0.5;

    m_xSlice->set( xSlice );
    m_ySlice->set( ySlice );
    m_zSlice->set( zSlice );

    // When working with the OpenSceneGraph, always use ref_ptr to store pointers to OSG objects. This allows OpenSceneGraph to manage
    // its resources automatically.

    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    ref_ptr< osg::Vec4Array > colors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    ref_ptr< osg::Geometry > geometry = ref_ptr< osg::Geometry >( new osg::Geometry );

    WColor color = m_aColor->get( true );

    if( ( ( *m_dataSet ).getValueSet()->order() == 1 ) && ( ( *m_dataSet ).getValueSet()->dimension() == 3 )
            && ( ( *m_dataSet ).getValueSet()->getDataType() == 16 ) )
    {
        if( m_showOnAxial->get( true ) )
        {
            for( int x = 0; x < nbX; ++x )
            {
                for( int y = 0; y < nbY; ++y )
                {
                    float vecCompX = vals->getScalar( ( x + y * nbX + static_cast< int >( zSlice ) * nbX * nbY ) * 3 ) / 2.;
                    float vecCompY = vals->getScalar( ( x + y * nbX + static_cast< int >( zSlice ) * nbX * nbY ) * 3 + 1 ) / 2.;
                    float vecCompZ = vals->getScalar( ( x + y * nbX + static_cast< int >( zSlice ) * nbX * nbY ) * 3 + 2 ) / 2.;

                    if( !m_projectOnSlice->get( true ) )
                    {
                        vertices->push_back( osg::Vec3( x - vecCompX, y - vecCompY, zSlice - vecCompZ ) );
                        vertices->push_back( osg::Vec3( x + vecCompX, y + vecCompY, zSlice + vecCompZ ) );
                        if( m_coloringMode->get( true ) )
                        {
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                        }
                        else
                        {
                            colors->push_back( color );
                            colors->push_back( color );
                        }
                    }
                    else
                    {
                        vertices->push_back( osg::Vec3( x - vecCompX, y - vecCompY, zSlice - 0.01f ) );
                        vertices->push_back( osg::Vec3( x + vecCompX, y + vecCompY, zSlice - 0.01f ) );
                        vertices->push_back( osg::Vec3( x - vecCompX, y - vecCompY, zSlice + 0.01f ) );
                        vertices->push_back( osg::Vec3( x + vecCompX, y + vecCompY, zSlice + 0.01f ) );
                        if( m_coloringMode->get( true ) )
                        {
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                        }
                        else
                        {
                            colors->push_back( color );
                            colors->push_back( color );
                            colors->push_back( color );
                            colors->push_back( color );
                        }
                    }
                }
            }
        }

        ++*progress;

        if( m_showOnCoronal->get( true ) )
        {
            for( int x = 0; x < nbX; ++x )
            {
                for( int z = 0; z < nbZ; ++z )
                {
                    float vecCompX = vals->getScalar( ( x + static_cast< int >( ySlice ) * nbX + z * nbX * nbY ) * 3 ) / 2.;
                    float vecCompY = vals->getScalar( ( x + static_cast< int >( ySlice ) * nbX + z * nbX * nbY ) * 3 + 1 ) / 2.;
                    float vecCompZ = vals->getScalar( ( x + static_cast< int >( ySlice ) * nbX + z * nbX * nbY ) * 3 + 2 ) / 2.;

                    if( !m_projectOnSlice->get( true ) )
                    {
                        vertices->push_back( osg::Vec3( x - vecCompX, ySlice - vecCompY, z - vecCompZ ) );
                        vertices->push_back( osg::Vec3( x + vecCompX, ySlice + vecCompY, z + vecCompZ ) );
                        if( m_coloringMode->get( true ) )
                        {
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                        }
                        else
                        {
                            colors->push_back( color );
                            colors->push_back( color );
                        }
                    }
                    else
                    {
                        vertices->push_back( osg::Vec3( x - vecCompX, ySlice - 0.01f, z - vecCompZ ) );
                        vertices->push_back( osg::Vec3( x + vecCompX, ySlice - 0.01f, z + vecCompZ ) );
                        vertices->push_back( osg::Vec3( x - vecCompX, ySlice + 0.01f, z - vecCompZ ) );
                        vertices->push_back( osg::Vec3( x + vecCompX, ySlice + 0.01f, z + vecCompZ ) );
                        if( m_coloringMode->get( true ) )
                        {
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                        }
                        else
                        {
                            colors->push_back( color );
                            colors->push_back( color );
                            colors->push_back( color );
                            colors->push_back( color );
                        }
                    }
                }
            }
        }

        ++*progress;

        if( m_showOnSagittal->get( true ) )
        {
            for( int y = 0; y < nbY; ++y )
            {
                for( int z = 0; z < nbZ; ++z )
                {
                    float vecCompX = vals->getScalar( ( static_cast< int >( xSlice ) + y * nbX + z * nbX * nbY ) * 3 ) / 2.;
                    float vecCompY = vals->getScalar( ( static_cast< int >( xSlice ) + y * nbX + z * nbX * nbY ) * 3 + 1 ) / 2.;
                    float vecCompZ = vals->getScalar( ( static_cast< int >( xSlice ) + y * nbX + z * nbX * nbY ) * 3 + 2 ) / 2.;

                    if( !m_projectOnSlice->get( true ) )
                    {
                        vertices->push_back( osg::Vec3( xSlice + vecCompX, y + vecCompY, z + vecCompZ ) );
                        vertices->push_back( osg::Vec3( xSlice - vecCompX, y - vecCompY, z - vecCompZ ) );
                        if( m_coloringMode->get( true ) )
                        {
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                        }
                        else
                        {
                            colors->push_back( color );
                            colors->push_back( color );
                        }
                    }
                    else
                    {
                        vertices->push_back( osg::Vec3( xSlice - 0.01f, y + vecCompY, z + vecCompZ ) );
                        vertices->push_back( osg::Vec3( xSlice - 0.01f, y - vecCompY, z - vecCompZ ) );
                        vertices->push_back( osg::Vec3( xSlice + 0.01f, y + vecCompY, z + vecCompZ ) );
                        vertices->push_back( osg::Vec3( xSlice + 0.01f, y - vecCompY, z - vecCompZ ) );
                        if( m_coloringMode->get( true ) )
                        {
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                        }
                        else
                        {
                            colors->push_back( color );
                            colors->push_back( color );
                            colors->push_back( color );
                            colors->push_back( color );
                        }
                    }
                }
            }
        }

        ++*progress;

        // Transform the vertices according to the matrix given in the grid.
        transformVerts( vertices );

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


void WMVectorPlot::updateCallback()
{
    WPosition current = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();

    if( ( m_oldPos != current ) || m_coloringMode->changed() || m_aColor->changed() || m_projectOnSlice->changed() ||
            m_showOnSagittal->changed() || m_showOnCoronal->changed() || m_showOnAxial->changed() )
    {
        m_oldPos = current; // for next run
        osg::ref_ptr<osg::Drawable> old = osg::ref_ptr<osg::Drawable>( m_rootNode->getDrawable( 0 ) );
        m_rootNode->removeDrawable( old );
        m_rootNode->addDrawable( buildPlotSlices() );
    }
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
    WMatrix< double > mat = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() )->getTransformationMatrix();

    for( size_t i = 0; i < verts->size(); ++i )
    {
        std::vector< double > resultPos4D( 4 );
        resultPos4D[0] = mat( 0, 0 ) * ( *verts )[i][0] + mat( 0, 1 ) * ( *verts )[i][1] + mat( 0, 2 ) * ( *verts )[i][2] + mat( 0, 3 ) * 1;
        resultPos4D[1] = mat( 1, 0 ) * ( *verts )[i][0] + mat( 1, 1 ) * ( *verts )[i][1] + mat( 1, 2 ) * ( *verts )[i][2] + mat( 1, 3 ) * 1;
        resultPos4D[2] = mat( 2, 0 ) * ( *verts )[i][0] + mat( 2, 1 ) * ( *verts )[i][1] + mat( 2, 2 ) * ( *verts )[i][2] + mat( 2, 3 ) * 1;
        resultPos4D[3] = mat( 3, 0 ) * ( *verts )[i][0] + mat( 3, 1 ) * ( *verts )[i][1] + mat( 3, 2 ) * ( *verts )[i][2] + mat( 3, 3 ) * 1;

        ( *verts )[i][0] = resultPos4D[0] / resultPos4D[3];
        ( *verts )[i][1] = resultPos4D[1] / resultPos4D[3];
        ( *verts )[i][2] = resultPos4D[2] / resultPos4D[3];
    }
}
