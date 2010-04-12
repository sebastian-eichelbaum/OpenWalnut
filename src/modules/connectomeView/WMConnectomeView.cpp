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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <cmath>

#include "WMConnectomeView.h"

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/StateAttribute>
#include <osg/PolygonMode>
#include <osg/LightModel>
#include <osgDB/WriteFile>

#include "../../common/WProgress.h"
#include "../../common/WPreferences.h"
#include "../../common/math/WVector3D.h"
#include "../../dataHandler/WSubject.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../kernel/WKernel.h"
#include "../../graphicsEngine/WShader.h"

WMConnectomeView::WMConnectomeView():
    WModuleContainer( "Connectome View", "Connectome View allows connectome data to be displayed in the context of MRI data." ),
    m_dataSet()
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // NOTE: Do not use the module factory inside this constructor. This will cause a dead lock as the module factory is locked
    // during construction of this instance and can then not be used to create another instance (Isosurface in this case). If you
    // want to initialize some modules using the module factory BEFORE the moduleMain() call, overwrite WModule::initialize().
}

WMConnectomeView::~WMConnectomeView()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMConnectomeView::factory() const
{
    return boost::shared_ptr< WModule >( new WMConnectomeView() );
}

const std::string WMConnectomeView::getName() const
{
    return WModuleContainer::getName();
}

const std::string WMConnectomeView::getDescription() const
{
    return WModuleContainer::getDescription();
}

void WMConnectomeView::moduleMain()
{
    // signal ready state
    ready();

    waitForStop();
}

void WMConnectomeView::connectors()
{
    // this is the scalar field input
    m_mrtInput = boost::shared_ptr< WModuleInputForwardData< WDataSetSingle > >(
        new WModuleInputForwardData< WDataSetSingle >( shared_from_this(),
                              "context", "The context dataset used to visualize the context in the brain." )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_mrtInput );

    // this is the scalar field input
    m_fiberInput = boost::shared_ptr< WModuleInputForwardData< WDataSetFibers > >(
        new WModuleInputForwardData< WDataSetFibers >( shared_from_this(),
                              "fibers", "The fiber dataset used to find connection path." )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_fiberInput );

    // call WModules initialization
    WModule::connectors();
}

void WMConnectomeView::properties()
{
    m_test = m_properties->addProperty( "Test", "A test value for testing test stuff", 1, boost::bind(
                &WMConnectomeView::slotPropertyChanged, this ) );
}

void WMConnectomeView::slotPropertyChanged()
{
}

