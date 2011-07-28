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

#include "WDisconnectCombiner.h"

WDisconnectCombiner::WDisconnectCombiner( boost::shared_ptr< WModuleContainer > target,
                                          boost::shared_ptr< WModule > srcModule, std::string srcConnector,
                                          boost::shared_ptr< WModule > targetModule, std::string targetConnector ):
    WModuleOneToOneCombiner( target, srcModule, srcConnector, targetModule, targetConnector )
{
}

WDisconnectCombiner::WDisconnectCombiner( boost::shared_ptr< WModule > srcModule, std::string srcConnector,
                                          boost::shared_ptr< WModule > targetModule, std::string targetConnector ):
    WModuleOneToOneCombiner( srcModule, srcConnector, targetModule, targetConnector )
{
}

WDisconnectCombiner::WDisconnectCombiner( boost::shared_ptr< WModuleConnector > srcConnector,
                                          boost::shared_ptr< WModuleConnector > targetConnector ):
    WModuleOneToOneCombiner( srcConnector->getModule(), srcConnector->getName(), targetConnector->getModule(), targetConnector->getName() )
{
}

WDisconnectCombiner::~WDisconnectCombiner()
{
    // cleanup
}

void WDisconnectCombiner::apply()
{
    // get first connector
    boost::shared_ptr< WModuleConnector > c1 = m_srcModule->findConnector( m_srcConnector );
    boost::shared_ptr< WModuleConnector > c2 = m_targetModule->findConnector( m_targetConnector );

    // check if they really existed
    if( !c1 || !c2 )
    {
        return;
    }

    // and do it
    c1->disconnect( c2 );
}

