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

#include "core/common/WCondition.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WDataSetVector.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleFactory.h"
#include "core/kernel/WModuleInputForwardData.h"
#include "core/kernel/WSelectionManager.h"
#include "WMSlices.h"
#include "WMSlices.xpm"

namespace
{
    const size_t NUM_ICS = 9;
}

WMSlices::WMSlices()
    : WModuleContainer( "Schmahmann-Pandya slices", "Combines the probTractDisplaySP and boundaryCurvesWMGM modules into one module." )
{
}

WMSlices::~WMSlices()
{
}

boost::shared_ptr< WModule > WMSlices::factory() const
{
    return boost::shared_ptr< WModule >( new WMSlices() );
}

const char** WMSlices::getXPMIcon() const
{
    return WMSlices_xpm;
}
const std::string WMSlices::getName() const
{
    return "Schmahmann-Pandya slices";
}

const std::string WMSlices::getDescription() const
{
    return "Slice based display of probabilistic tracts using line stipples, gray matter and white matter boundary curves.";
}

void WMSlices::connectors()
{
    // Put the code for your connectors here. See "src/modules/template/" for an extensively documented example.
    for( size_t i = 0; i < NUM_ICS; ++i )
    {
        std::stringstream ss;
        ss << "probTract" << i << "Input";
        m_probICs[i] = WModuleInputForwardData< WDataSetScalar >::createAndAdd( shared_from_this(), ss.str(), "Probabilistic tract to display." ); // NOLINT line length
    }
    m_t1ImageIC = WModuleInputForwardData< WDataSetScalar >::createAndAdd( shared_from_this(), "t1Input", "T1 Image for WM/GM estimation." );
    m_evecsIC = WModuleInputForwardData< WDataSetVector >::createAndAdd( shared_from_this(), "evecInput", "Vector field with principal diffusion direction." ); // NOLINT line length

    WModule::connectors();
}

void WMSlices::properties()
{
    WPropGroup sliceGroup = m_properties->addPropertyGroup( "Slices",  "Group of slices for tract rendering ala Schmahmann and Pandya." );

    m_showSlice[0] = sliceGroup->addProperty( "Show Sagittal", "Show rendering on sagittal slice.", true );
    m_showSlice[1] = sliceGroup->addProperty( "Show Coronal", "Show rendering on coronal slice.", true );
    m_showSlice[2] = sliceGroup->addProperty( "Show Axial", "Show rendering on axial slice.", true );

    m_slicePos[0] = m_properties->addProperty( boost::shared_dynamic_cast< WPropertyBase >(
                WKernel::getRunningKernel()->getSelectionManager()->getPropSagittalPos() ) )->toPropDouble();
    m_slicePos[1] = m_properties->addProperty( boost::shared_dynamic_cast< WPropertyBase >(
                WKernel::getRunningKernel()->getSelectionManager()->getPropCoronalPos() ) )->toPropDouble();
    m_slicePos[2] = m_properties->addProperty( boost::shared_dynamic_cast< WPropertyBase >(
                WKernel::getRunningKernel()->getSelectionManager()->getPropAxialPos() ) )->toPropDouble();

    WModule::properties();
}

void WMSlices::requirements()
{
    // Put the code for your requirements here. See "src/modules/template/" for an extensively documented example.
}

void WMSlices::redrawUntilSlicePosChangingAnymore( unsigned char sliceNum, const std::string &propName )
{
    double pos = 0.0;
    do
    {
        pos = m_slicePos[sliceNum]->get();
        m_boundaryCurvesWMGM->getProperties()->findProperty( propName )->toPropDouble()->set( pos );
        m_probTractDisplaySP->getProperties()->findProperty( propName )->toPropDouble()->set( pos );
    }
    while( m_slicePos[sliceNum]->get() != pos );
}

void WMSlices::moduleMain()
{
    m_moduleState.setResetable( true, true );
    for( size_t i = 0; i < NUM_ICS; ++i )
    {
        m_moduleState.add( m_probICs[i]->getDataChangedCondition() );
    }
    m_moduleState.add( m_t1ImageIC->getDataChangedCondition() );
    m_moduleState.add( m_evecsIC->getDataChangedCondition() );
    m_moduleState.add( m_active->getCondition() );

    for( size_t i = 0; i < 3; ++i )
    {
        m_moduleState.add( m_slicePos[i]->getCondition() );
        m_moduleState.add( m_showSlice[i]->getCondition() );
    }

    initSubModules();

    ready();

    while( !m_shutdownFlag() )
    {
        infoLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish?
        if( m_shutdownFlag() )
        {
            break;
        }

        redrawUntilSlicePosChangingAnymore( 0, "Slices/Sagittal Position" );
        redrawUntilSlicePosChangingAnymore( 1, "Slices/Coronal Position" );
        redrawUntilSlicePosChangingAnymore( 2, "Slices/Axial Position" );

        m_boundaryCurvesWMGM->getProperties()->findProperty( "Slices/Show Sagittal" )->toPropBool()->set( m_showSlice[0]->get( true ) );
        m_boundaryCurvesWMGM->getProperties()->findProperty( "Slices/Show Coronal" )->toPropBool()->set( m_showSlice[1]->get( true ) );
        m_boundaryCurvesWMGM->getProperties()->findProperty( "Slices/Show Axial" )->toPropBool()->set( m_showSlice[2]->get( true ) );
        m_probTractDisplaySP->getProperties()->findProperty( "Slices/Show Sagittal" )->toPropBool()->set( m_showSlice[0]->get( true ) );
        m_probTractDisplaySP->getProperties()->findProperty( "Slices/Show Coronal" )->toPropBool()->set( m_showSlice[1]->get( true ) );
        m_probTractDisplaySP->getProperties()->findProperty( "Slices/Show Axial" )->toPropBool()->set( m_showSlice[2]->get( true ) );

        if( m_active->changed() )
        {
            infoLog() << "En/Disabling submodules graphic context.";
            m_boundaryCurvesWMGM->getProperties()->getProperty( "active" )->toPropBool()->set( m_active->get( true ) );
            m_probTractDisplaySP->getProperties()->getProperty( "active" )->toPropBool()->set( m_active->get( true ) );
        }
    }
}

boost::shared_ptr< WModule > WMSlices::addNewSubmodule( const std::string &prototypeName )
{
    boost::shared_ptr< WModuleFactory > factory = WModuleFactory::getModuleFactory();
    boost::shared_ptr< WModule > result = factory->create( factory->getPrototypeByName( prototypeName ) );
    add( result );
    result->isReady().wait();
    return result;
}

void WMSlices::initSubModules()
{
    m_boundaryCurvesWMGM = addNewSubmodule( "Boundary Curves WMGM" );
    m_probTractDisplaySP = addNewSubmodule( "Prob Tract Display SP" );

    infoLog() << "Add properties from submodules";
    m_properties->addProperty( m_boundaryCurvesWMGM->getProperties()->getProperty( "White Matter" ) );
    m_properties->addProperty( m_boundaryCurvesWMGM->getProperties()->getProperty( "Gray Matter" ) );
    m_properties->addProperty( m_boundaryCurvesWMGM->getProperties()->getProperty( "White Matter Color" ) );
    m_properties->addProperty( m_boundaryCurvesWMGM->getProperties()->getProperty( "Gray Matter Color" ) );
    m_properties->addProperty( m_boundaryCurvesWMGM->getProperties()->getProperty( "Resolution" ) );
    m_properties->addProperty( m_probTractDisplaySP->getProperties()->getProperty( "Vector Group" ) );
    for( size_t i = 0; i < NUM_ICS; ++i )
    {
        std::stringstream ss;
        ss << "Color for " << i << "InputConnector";
        m_properties->addProperty( m_probTractDisplaySP->getProperties()->getProperty( ss.str() ) );
    }

    infoLog() << "Wiring submodules";
    m_evecsIC->forward( m_probTractDisplaySP->getInputConnector( "vectorInput" ) );
    for( size_t i = 0; i < NUM_ICS; ++i )
    {
        m_probICs[i]->forward( m_probTractDisplaySP->getInputConnector( m_probICs[i]->getName() ) );
    }
    m_t1ImageIC->forward( m_boundaryCurvesWMGM->getInputConnector( "textureInput" ) );
}
