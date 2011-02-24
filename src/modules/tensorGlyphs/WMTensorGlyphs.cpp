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

#include "../../common/WItemSelector.h"
#include "../../common/WPropertyHelper.h"
#include "../../graphicsEngine/OpenCL/WGEManagedRenderNodeCL.h"
#include "../../kernel/WKernel.h"
#include "WGlyphModule.h"
#include "WMTensorGlyphs.xpm"
#include "WMTensorGlyphs.h"

W_LOADABLE_MODULE( WMTensorGlyphs )

WMTensorGlyphs::WMTensorGlyphs(): WModule()
{
    // initialize
}

WMTensorGlyphs::~WMTensorGlyphs()
{
    cleanup();
}

boost::shared_ptr< WModule > WMTensorGlyphs::factory() const
{
    return boost::shared_ptr< WModule >( new WMTensorGlyphs() );
}

const char** WMTensorGlyphs::getXPMIcon() const
{
    return glyph_xpm;
}

const std::string WMTensorGlyphs::getName() const
{
    return "Tensor Glyphs";
}

const std::string WMTensorGlyphs::getDescription() const
{
    return "GPU based raytracing of high order tensor glyphs.";
}

void WMTensorGlyphs::connectors()
{
    m_input = boost::shared_ptr< WModuleInputData< WDataSetSingle > >
    (
        new WModuleInputData< WDataSetSingle >
        (
            shared_from_this(),
            "tensor input",
            "An input set of high order tensors on a regular 3D-grid."
        )
    );

    addConnector( m_input );

    WModule::connectors();
}

void WMTensorGlyphs::properties()
{
    m_changed = boost::shared_ptr< WCondition >( new WCondition() );
    m_coloringList = boost::shared_ptr< WItemSelection >( new WItemSelection() );

    m_coloringList->addItem( "Magnitude", "Glyph coloring by magnitude." );
    m_coloringList->addItem( "Direction", "Glyph coloring by direction." );

    m_coloring = m_properties->addProperty
    (
        "Coloring scheme", "Select the coloring scheme.",
        m_coloringList->getSelectorFirst(), m_changed
    );

    WPropertyHelper::PC_NOTEMPTY::addTo( m_coloring );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_coloring );

    m_slicePosition[ 0 ] = m_properties->addProperty( "Sagittal Position", "Sagittal slice position.", 0, m_changed );
    m_slicePosition[ 1 ] = m_properties->addProperty( "Coronal Position", "Coronal slice position.", 0, m_changed );
    m_slicePosition[ 2 ] = m_properties->addProperty( "Axial Position", "Axial slice position.", 0, m_changed );

    m_slicePosition[ 0 ]->setMin( 0 );
    m_slicePosition[ 1 ]->setMin( 0 );
    m_slicePosition[ 2 ]->setMin( 0 );

    m_sliceVisibility[ 0 ] = m_properties->addProperty( "Show Sagittal", "Show sagittal slice.", true, m_changed );
    m_sliceVisibility[ 1 ] = m_properties->addProperty( "Show Coronal", "Show coronal slice.", true, m_changed );
    m_sliceVisibility[ 2 ] = m_properties->addProperty( "Show Axial", "Show axial slice.", true, m_changed );
}

void WMTensorGlyphs::moduleMain()
{
    // set conditions for data and property changes -------------------------------------------------------------------

    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_changed );

    ready();

    // create graphics node -------------------------------------------------------------------------------------------

    osg::ref_ptr< WGEManagedRenderNodeCL > node = new WGEManagedRenderNodeCL( m_active, true );
    osg::ref_ptr< WGlyphModule > module = new WGlyphModule( m_localPath / "shaders" / "WMTensorGlyphs.cl" );

    if ( !module->isSourceRead() )
    {
        errorLog() << " Couldn't find kernel source file ( WMTensorGlyphs.cl ). " << std::endl;

        return;
    }

    node->setModule( module );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( node );

    boost::shared_ptr< WDataSetSingle > dataSet;

    // main loop ------------------------------------------------------------------------------------------------------

    while ( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if ( m_shutdownFlag() )
        {
            break;
        }

        boost::shared_ptr< WDataSetSingle > newDataSet = m_input->getData();

        if ( ( dataSet != newDataSet ) && ( newDataSet != 0 ) )
        {
            // check the data validity --------------------------------------------------------------------------------

            boost::shared_ptr< WValueSetBase > newValueSet = newDataSet->getValueSet();

            // check the order of the new tensor data -----------------------------------------------------------------

            // TODO(reichenbach): substitute this as soon as there is a working tensor data set class ---------------------
            int dimension = newValueSet->dimension();
            int order = 0;

            bool dataValid = true;

            while ( true )
            {
                if ( dimension == (  ( order + 1 ) * ( order + 2 ) / 2 ) )
                {
                    break;
                }

                if ( dimension < ( ( order + 1 ) * ( order + 2 ) / 2 ) )
                {
                    dataValid = false;

                    break;
                }

                order += 2;
            }

            if ( order == 0 )
            {
                dataValid = false;
            }

            if ( newValueSet->order() != 1 )
            {
                dataValid = false;
            }

            if ( !dataValid )
            {
                warnLog() << "Received data with order=" << newDataSet->getValueSet()->order()
                          << " and dimension=" << newDataSet->getValueSet()->dimension()
                          << " not compatible with this module. Ignoring Data.";
            }

            // check the grid validity --------------------------------------------------------------------------------

            if ( dynamic_cast< WGridRegular3D* >( newDataSet->getGrid().get() ) == 0 )
            {
                dataValid = false;
            }

            if ( !dataValid )
            {
                warnLog() << "Dataset does not have a regular 3D grid. Ignoring Data.";
            }

            // handle new data if valid -------------------------------------------------------------------------------

            if ( dataValid )
            {
                debugLog() << "Received Data.";

                dataSet = newDataSet;

                WGridRegular3D* grid = static_cast< WGridRegular3D* >( dataSet->getGrid().get() );

                int numOfTensorsX = grid->getNbCoordsX();
                int numOfTensorsY = grid->getNbCoordsY();
                int numOfTensorsZ = grid->getNbCoordsZ();

                m_slicePosition[ 0 ]->setMax( numOfTensorsX - 1 );
                m_slicePosition[ 1 ]->setMax( numOfTensorsY - 1 );
                m_slicePosition[ 2 ]->setMax( numOfTensorsZ - 1 );

                m_slicePosition[ 0 ]->set( ( numOfTensorsX / 2 ), true );
                m_slicePosition[ 1 ]->set( ( numOfTensorsY / 2 ), true );
                m_slicePosition[ 2 ]->set( ( numOfTensorsZ / 2 ), true );

                m_sliceVisibility[ 0 ]->set( true, true );
                m_sliceVisibility[ 1 ]->set( true, true );
                m_sliceVisibility[ 2 ]->set( true, true );

                module->setTensorData( dataSet, order, m_coloring->get( true ) );
            }
        }
        else
        {
            // handle property changes --------------------------------------------------------------------------------

            if ( m_coloring->changed() )
            {
                module->setColoring( m_coloring->get( true ) );
            }

            for ( int i = 0; i < 3; i++ )
            {
                if ( m_slicePosition[ i ]->changed() )
                {
                    module->setPosition( i, m_slicePosition[ i ]->get( true ) );
                }
            }

            if ( m_sliceVisibility[ 0 ]->changed() ||
                 m_sliceVisibility[ 1 ]->changed() ||
                 m_sliceVisibility[ 2 ]->changed() )
            {
                module->setVisibility
                (
                    m_sliceVisibility[ 0 ]->get( true ),
                    m_sliceVisibility[ 1 ]->get( true ),
                    m_sliceVisibility[ 2 ]->get( true )
                );
            }
        }
    }

    // remove node ----------------------------------------------------------------------------------------------------
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( node );
}

