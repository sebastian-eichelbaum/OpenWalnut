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

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LightModel>
#include <osg/LineWidth>
#include <osg/PolygonMode>
#include <osg/StateAttribute>
#include <osg/StateSet>

#include "../../common/WStringUtils.h"
#include "../../graphicsEngine/WROIBox.h"
#include "../../kernel/WKernel.h"
#include "../../math/WPosition.h"
#include "../../math/WVector3D.h"
#include "../data/WMData.h"
#include "WMPrototypeBoxManipulation.h"

WMPrototypeBoxManipulation::WMPrototypeBoxManipulation():
    WModule()
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.
}

WMPrototypeBoxManipulation::~WMPrototypeBoxManipulation()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMPrototypeBoxManipulation::factory() const
{
    return boost::shared_ptr< WModule >( new WMPrototypeBoxManipulation() );
}

const std::string WMPrototypeBoxManipulation::getName() const
{
    return "Box Manipulation";
}

const std::string WMPrototypeBoxManipulation::getDescription() const
{
    return "Just a prtotype for box manipulation";
}

void WMPrototypeBoxManipulation::moduleMain()
{
    // use the m_input "data changed" flag
    m_moduleState.add( m_input->getDataChangedCondition() );

    // signal ready state
    ready();

    m_ROI = boost::shared_ptr< WROIBox >( new WROIBox( wmath::WPosition(), wmath::WPosition( 30., 30., 30 ) ) );

    // loop until the module container requests the module to quit
    while ( !m_shutdownFlag() )
    {
        // this waits for m_moduleState to fire. By default, this is only the m_shutdownFlag condition.
        // NOTE: you can add your own conditions to m_moduleState using m_moduleState.add( ... )
        m_moduleState.wait();
    }
}

void WMPrototypeBoxManipulation::connectors()
{
    // initialize connectors
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(),
                                                               "in", "Dummy in." )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMPrototypeBoxManipulation::properties()
{
    // m_active gets initialized in WModule and is available for all modules. Overwrite activate() to have a special callback for m_active
    // changes or add a callback manually.
}

