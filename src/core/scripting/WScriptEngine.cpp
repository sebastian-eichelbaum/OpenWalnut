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

#include "python/WScriptInterpreterPython.h"

#include "WScriptEngine.h"

WScriptEngine::WScriptEngine( boost::shared_ptr<WModuleContainer> const& rootContainer )
{
#ifdef PYTHON_FOUND // this is defined in the CMake scripts
    m_interpreters.push_back( boost::shared_ptr< WScriptInterpreter >( new WScriptInterpreterPython( rootContainer ) ) );
#endif
    for( std::size_t k = 0; k < m_interpreters.size(); ++k )
    {
        m_interpreters[ k ]->initBindings();
    }
}

WScriptEngine::~WScriptEngine()
{
}

boost::shared_ptr< WScriptInterpreter > WScriptEngine::getInterpreterByFileExtension( std::string const& ext )
{
    for( std::size_t k = 0; k < m_interpreters.size(); ++k )
    {
        if( ext == m_interpreters[ k ]->getExtension() )
        {
            return m_interpreters[ k ];
        }
    }
    return boost::shared_ptr< WScriptInterpreter >();
}

boost::shared_ptr< WScriptInterpreter > WScriptEngine::getInterpreter( std::string const& name )
{
    for( std::size_t k = 0; k < m_interpreters.size(); ++k )
    {
        if( name == m_interpreters[ k ]->getName() )
        {
            return m_interpreters[ k ];
        }
    }
    return boost::shared_ptr< WScriptInterpreter >();
}

std::size_t WScriptEngine::getNumInterpreters() const
{
    return m_interpreters.size();
}

boost::shared_ptr< WScriptInterpreter > WScriptEngine::getInterpreter( std::size_t index )
{
    if( index < m_interpreters.size() )
    {
        return m_interpreters[ index ];
    }
    return boost::shared_ptr< WScriptInterpreter >();
}
