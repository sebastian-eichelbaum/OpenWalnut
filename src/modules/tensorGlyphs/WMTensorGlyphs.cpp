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

#include "../../kernel/WKernel.h"

#include "WMTensorGlyphs.xpm"
#include "WMTensorGlyphs.h"

W_LOADABLE_MODULE( WMTensorGlyphs )

/*-------------------------------------------------------------------------------------------------------------------*/

WMTensorGlyphs::WMTensorGlyphs(): WModule()
{}

/*-------------------------------------------------------------------------------------------------------------------*/

WMTensorGlyphs::~WMTensorGlyphs()
{
    removeConnectors();
}

/*-------------------------------------------------------------------------------------------------------------------*/

boost::shared_ptr< WModule > WMTensorGlyphs::factory() const
{
    return boost::shared_ptr< WModule >( new WMTensorGlyphs() );
}

/*-------------------------------------------------------------------------------------------------------------------*/

const char** WMTensorGlyphs::getXPMIcon() const
{
    return glyph_xpm;
}

/*-------------------------------------------------------------------------------------------------------------------*/

const std::string WMTensorGlyphs::getName() const
{
    return "Tensor Glyphs";
}

/*-------------------------------------------------------------------------------------------------------------------*/

const std::string WMTensorGlyphs::getDescription() const
{
    return "GPU based raytracing of high order tensor glyphs.";
}

/*-------------------------------------------------------------------------------------------------------------------*/

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

/*-------------------------------------------------------------------------------------------------------------------*/

void WMTensorGlyphs::properties()
{
    m_propertyChanged = boost::shared_ptr< WCondition >( new WCondition() );

    m_slices[ 0 ] = m_properties->addProperty( "Sagittal Position", "Slice X position.", 0, m_propertyChanged, true );
    m_slices[ 1 ] = m_properties->addProperty( "Coronal Position", "Slice Y position.", 0, m_propertyChanged, true );
    m_slices[ 2 ] = m_properties->addProperty( "Axial Position", "Slice Z position.", 0, m_propertyChanged, true );

    m_slices[ 0 ]->setMin( 0 );
    m_slices[ 1 ]->setMin( 0 );
    m_slices[ 2 ]->setMin( 0 );

    m_sliceEnabled[ 0 ] = m_properties->addProperty( "Show Sagittal", "Show vectors on sagittal slice.", true, m_propertyChanged, true );
    m_sliceEnabled[ 1 ] = m_properties->addProperty( "Show Coronal", "Show vectors on coronal slice.", true, m_propertyChanged, true );
    m_sliceEnabled[ 2 ] = m_properties->addProperty( "Show Axial", "Show vectors on axial slice.", true, m_propertyChanged, true );
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WMTensorGlyphs::moduleMain()
{
    // set conditions for data and property changes

    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propertyChanged );

    ready();

    boost::shared_ptr< WDataSetSingle > dataSet;
    boost::shared_ptr< WDataSetSingle > newDataSet;
    WValueSetBase* newValueSet;
    int newDimension;
    int newOrder;

    bool dataChanged;
    bool dataValid;

    // main loop

    while ( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if ( m_shutdownFlag() )
        {
            break;
        }

        // test for data changes

        newDataSet = m_input->getData();
        dataChanged = dataSet != newDataSet;

        // test for data validity

        newValueSet = newDataSet->getValueSet().get();

        dataValid = ( dynamic_cast< WValueSet< float >* >( newValueSet ) != 0 );

        if ( !dataValid )
        {
            warnLog() << "Dataset does not have a valid data set. Ignoring Data.";
        }

        if ( dataValid )
        {
            // check the order of the new tensor data
            // TODO: substitute this bullshit as soon as there is a working tensor data set class

            newOrder = 0;
            newDimension = newValueSet->dimension();

            while (true)
            {
                if ( newDimension == ( ( ( newOrder + 1 ) * ( newOrder + 2 ) ) / 2 ) )
                {
                    break;
                }

                if ( newDimension < ( ( ( newOrder + 1 ) * ( newOrder + 2 ) ) / 2) )
                {
                    dataValid = false;

                    break;
                }

                newOrder += 2;
            }

            if ( newOrder == 0 )
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

            // check the grid validity

            dataValid = ( dynamic_cast< WGridRegular3D* >( newDataSet->getGrid().get() ) != 0 );

            if (!dataValid)
            {
                warnLog() << "Dataset does not have a regular 3D grid. Ignoring Data.";
            }
        }

        if ( dataValid )
        {
            // handle data

            if ( dataChanged )
            {
                debugLog() << "Received Data.";

                dataSet = newDataSet;

                WGridRegular3D* grid = static_cast< WGridRegular3D* >( dataSet->getGrid().get() );

                int numOfTensorsX = grid->getNbCoordsX();
                int numOfTensorsY = grid->getNbCoordsY();
                int numOfTensorsZ = grid->getNbCoordsZ();

                m_slices[ 0 ]->setMax( numOfTensorsX - 1 );
                m_slices[ 1 ]->setMax( numOfTensorsY - 1 );
                m_slices[ 2 ]->setMax( numOfTensorsZ - 1 );

                m_slices[ 0 ]->set( ( numOfTensorsX / 2 ), true );
                m_slices[ 1 ]->set( ( numOfTensorsY / 2 ), true );
                m_slices[ 2 ]->set( ( numOfTensorsZ / 2 ), true );

                m_sliceEnabled[ 0 ]->set( true, true );
                m_sliceEnabled[ 1 ]->set( true, true );
                m_sliceEnabled[ 2 ]->set( true, true );

                if ( renderNode.valid() )
                {
                    renderNode->setTensorData
                    (
                        dataSet, newOrder, 
                        m_slices[0]->get(), m_slices[1]->get(), m_slices[2]->get(), 
                        m_sliceEnabled[0]->get(), m_sliceEnabled[1]->get(), m_sliceEnabled[2]->get() 
                    );
                }
                else
                {
                    renderNode = new WGlyphRenderNode
                    (
                        dataSet, newOrder, 
                        m_slices[0]->get(), m_slices[1]->get(), m_slices[2]->get(), 
                        m_sliceEnabled[0]->get(), m_sliceEnabled[1]->get(), m_sliceEnabled[2]->get(), 
                        m_localPath 
                    );

                    if ( !renderNode->isSourceRead() )
                    {
                        return;
                    }

                    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( renderNode );

                    m_slices[ 0 ]->setHidden( false );
                    m_slices[ 1 ]->setHidden( false );
                    m_slices[ 2 ]->setHidden( false );

                    m_sliceEnabled[ 0 ]->setHidden( false );
                    m_sliceEnabled[ 1 ]->setHidden( false );
                    m_sliceEnabled[ 2 ]->setHidden( false );
                }

                continue;
            }

            // property changes

            if ( m_slices[ 0 ]->changed() || m_slices[ 1 ]->changed() || m_slices[ 2 ]->changed() || 
                 m_sliceEnabled[ 0 ]->changed() || m_sliceEnabled[ 1 ]->changed() || m_sliceEnabled[ 2 ]->changed() )
            {
                renderNode->setSlices
                (
                    m_slices[0]->get( true ), m_slices[1]->get( true ), m_slices[2]->get( true ), 
                    m_sliceEnabled[0]->get( true ), m_sliceEnabled[1]->get( true ), m_sliceEnabled[2]->get( true ) 
                );
            }
        }
    }

    if ( renderNode.valid() )
    {
        WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( renderNode );
    }
}

/*-------------------------------------------------------------------------------------------------------------------*/

void WMTensorGlyphs::activate()
{
    if ( renderNode.valid() )
    {
        if ( m_active->get() )
        {
            renderNode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            renderNode->setNodeMask( 0x0 );
        }
    }

    WModule::activate();
}

/*-------------------------------------------------------------------------------------------------------------------*/
