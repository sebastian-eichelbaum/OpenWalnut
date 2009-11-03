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

#include <cassert>
#include <cmath>
#include <string>

#include "WStatusReport.h"

WStatusReport::WStatusReport( unsigned int totalSteps )
    : m_totalSteps( totalSteps ),
      m_finishedSteps( 0 )
{
}

double WStatusReport::operator++()
{
    assert( m_totalSteps > m_finishedSteps );
    m_finishedSteps++;
    return progress();
}

double WStatusReport::operator+=( unsigned int numSteps )
{
    assert( m_totalSteps > m_finishedSteps );
    if( m_totalSteps - m_finishedSteps < numSteps )
    {
        m_finishedSteps = m_totalSteps;
    }
    else
    {
        m_finishedSteps += numSteps;
    }
    return progress();
}

std::string WStatusReport::stringBar( char symbol, unsigned int finalNumOfSymbols ) const
{
    unsigned int numSymbols = progress() * finalNumOfSymbols;
    return std::string( numSymbols, symbol );
}
