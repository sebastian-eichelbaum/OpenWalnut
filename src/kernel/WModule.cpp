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

#include <set>
#include <string>

#include <boost/shared_ptr.hpp>

#include "WModuleInputConnector.h"
#include "WModuleOutputConnector.h"

#include "WModule.h"

WModule::WModule():
    WThreadedRunner()
{
    // initialize members
}

WModule::~WModule()
{
    // cleanup
}

void WModule::addConnector( boost::shared_ptr<WModuleInputConnector> con )
{
    m_InputConnectors.insert( con );
}

void WModule::addConnector( boost::shared_ptr<WModuleOutputConnector> con )
{
    m_OutputConnectors.insert( con );
}

const std::set<boost::shared_ptr<WModuleInputConnector> >& WModule::getInputConnectors() const
{
    return m_InputConnectors;
}

const std::set<boost::shared_ptr<WModuleOutputConnector> >& WModule::getOutputConnectors() const
{
    return m_OutputConnectors;
}

void WModule::notifyConnectionEstablished( boost::shared_ptr<WModuleConnector> /*here*/,
                                           boost::shared_ptr<WModuleConnector> /*there*/ )
{
    // By default this callback does nothing. Overwrite it in your module.
}


void WModule::notifyConnectionClosed( boost::shared_ptr<WModuleConnector> /*here*/,
                                      boost::shared_ptr<WModuleConnector> /*there*/ )
{
    // By default this callback does nothing. Overwrite it in your module.
}

void WModule::notifyDataChange( boost::shared_ptr<WModuleInputConnector> /*input*/,
                                boost::shared_ptr<WModuleOutputConnector> /*output*/ )
{
    // By default this callback does nothing. Overwrite it in your module.
}

