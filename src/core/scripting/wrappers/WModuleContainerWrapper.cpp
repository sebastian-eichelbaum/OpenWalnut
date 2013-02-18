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

#include "core/kernel/WModuleFactory.h"

#include "WModuleContainerWrapper.h"

WModuleContainerWrapper::WModuleContainerWrapper()
    : m_mc()
{
}

WModuleContainerWrapper::WModuleContainerWrapper( boost::shared_ptr< WModuleContainer > mc )
    : m_mc( mc )
{
}

WModuleContainerWrapper::~WModuleContainerWrapper()
{
}

WModuleWrapper WModuleContainerWrapper::create( std::string const& name )
{
    WModuleWrapper mw( m_mc->createAndAdd( name ) );
    return mw;
}

WModuleWrapper WModuleContainerWrapper::createDataModule( std::string const& filename )
{
    boost::shared_ptr< WDataModule > dataModule = boost::static_pointer_cast< WDataModule >(
                                                    WModuleFactory::getModuleFactory()->create(
                                                      WModuleFactory::getModuleFactory()->getPrototypeByName( "Data Module" ) ) );
    dataModule->setSuppressColormaps( true );
    dataModule->setFilename( filename );

    m_mc->add( dataModule );

    dataModule->isReadyOrCrashed().wait();
    return WModuleWrapper( dataModule );
}

void WModuleContainerWrapper::remove( WModuleWrapper module )
{
    module.getModulePtr()->wait( true );
    m_mc->remove( module.getModulePtr() );
}
