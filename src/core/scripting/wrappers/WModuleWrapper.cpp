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

#include "core/kernel/WModuleInputConnector.h"
#include "core/kernel/WModuleOutputConnector.h"

#include "WModuleWrapper.h"

WModuleWrapper::WModuleWrapper( boost::shared_ptr< WModule > module )
    : m_module( module )
{
}

WModuleWrapper::~WModuleWrapper()
{
}

std::string WModuleWrapper::getName() const
{
    return m_module->getName();
}

std::string WModuleWrapper::getDescription() const
{
    return m_module->getDescription();
}

boost::shared_ptr< WModule > WModuleWrapper::getModulePtr()
{
    return m_module;
}

WPropertyGroupWrapper WModuleWrapper::getProperties()
{
    return WPropertyGroupWrapper( m_module->getProperties() );
}

WPropertyGroupWrapper WModuleWrapper::getInformationProperties()
{
    return WPropertyGroupWrapper( m_module->getInformationProperties() );
}

WInputConnectorWrapper WModuleWrapper::getInputConnector( std::string const& name )
{
    return WInputConnectorWrapper( m_module->getInputConnector( name ) );
}

WOutputConnectorWrapper WModuleWrapper::getOutputConnector( std::string const& name )
{
    return WOutputConnectorWrapper( m_module->getOutputConnector( name ) );
}
