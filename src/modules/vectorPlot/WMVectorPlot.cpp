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

#include "core/common/exceptions/WTypeMismatch.h"
#include "core/common/WColor.h"
#include "core/dataHandler/WDataHandlerEnums.h"
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

    // NOTE: min/max of these props are set vy buildPlotSlices
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

        if( dataValid && dataChanged )
        {
            debugLog() << "Building Vector Plot";

            if( m_rootNode )
            {
                WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
            }

            osg::ref_ptr< osg::Geode > newRootNode = new osg::Geode();
            newRootNode->addDrawable( buildPlotSlices() );

            ++*progress;

            m_rootNode = newRootNode;
            m_rootNode->setNodeMask( m_active->get() ? 0xFFFFFFFF : 0x0 );
            m_rootNode->addUpdateCallback( new WGEFunctorCallback< osg::Node >( boost::bind( &WMVectorPlot::updateCallback, this ) ) );

            // no light for lines
            m_rootNode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );
        }
        progress->finish();
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

osg::ref_ptr<osg::Geometry> WMVectorPlot::buildPlotSlices()
{
    switch( ( *m_dataSet ).getValueSet()->getDataType() )
    {
        case W_DT_SIGNED_INT:
            return buildPlotSlices< DataTypeRT< W_DT_SIGNED_INT >::type >();
        case W_DT_FLOAT:
            return buildPlotSlices< DataTypeRT< W_DT_FLOAT >::type >();
        case W_DT_DOUBLE:
            return buildPlotSlices< DataTypeRT< W_DT_DOUBLE >::type >();
        case W_DT_INT8:
            return buildPlotSlices< DataTypeRT< W_DT_INT8 >::type >();
        case W_DT_UINT16:
            return buildPlotSlices< DataTypeRT< W_DT_UINT16 >::type >();
        case W_DT_UINT8:
            return buildPlotSlices< DataTypeRT< W_DT_UINT8 >::type >();
        case W_DT_INT16:
            return buildPlotSlices< DataTypeRT< W_DT_INT16 >::type >();
        case W_DT_UINT32:
            return buildPlotSlices< DataTypeRT< W_DT_UINT32 >::type >();
        case W_DT_INT64:
            return buildPlotSlices< DataTypeRT< W_DT_INT64 >::type >();
        case W_DT_UINT64:
            return buildPlotSlices< DataTypeRT< W_DT_UINT64 >::type >();
        default:
            throw WTypeMismatch( "Unknown valueset type." );
            break;
    }
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
