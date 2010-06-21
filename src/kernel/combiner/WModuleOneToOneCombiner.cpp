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

#include "WModuleOneToOneCombiner.h"

WModuleOneToOneCombiner::WModuleOneToOneCombiner( boost::shared_ptr< WModuleContainer > target,
                                                  boost::shared_ptr< WModule > srcModule, std::string srcConnector,
                                                  boost::shared_ptr< WModule > prototype, std::string targetConnector ):
    WModuleCombiner( target ),
    m_srcModule( srcModule ),
    m_srcConnector( srcConnector ),
    m_targetModule( prototype ),
    m_targetConnector( targetConnector )
{
}

WModuleOneToOneCombiner::WModuleOneToOneCombiner( boost::shared_ptr< WModule > srcModule, std::string srcConnector,
                                                  boost::shared_ptr< WModule > prototype, std::string targetConnector ):
    WModuleCombiner(),
    m_srcModule( srcModule ),
    m_srcConnector( srcConnector ),
    m_targetModule( prototype ),
    m_targetConnector( targetConnector )
{
}

WModuleOneToOneCombiner::~WModuleOneToOneCombiner()
{
    // cleanup
}

boost::shared_ptr< WModule > WModuleOneToOneCombiner::getSrcModule() const
{
    return m_srcModule;
}

std::string WModuleOneToOneCombiner::getSrcConnector() const
{
    return m_srcConnector;
}

boost::shared_ptr< WModule > WModuleOneToOneCombiner::getTargetModule() const
{
    return m_targetModule;
}

std::string WModuleOneToOneCombiner::getTargetConnector() const
{
    return m_targetConnector;
}

