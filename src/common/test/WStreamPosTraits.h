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

#ifndef WSTREAMPOSTRAITS_H
#define WSTREAMPOSTRAITS_H

#include <sstream>

#include "WTraitsBase.h"

#ifdef CXXTEST_RUNNING
namespace CxxTest
{
CXXTEST_TEMPLATE_INSTANTIATION
/**
 * Enables better UnitTest OutPut if something fails with stream positions, so
 * you see immedeatly what is failing.
 */
class ValueTraits< std::streampos > : public WTraitsBase
{
public:
    /**
     * Constructs a new ValueTrait of a streampos for better test output
     */
    explicit ValueTraits( const std::streampos &pos )
    {
        std::stringstream ss;
        ss << pos;
        m_s = ss.str();
    }
};
}
#endif  // CXXTEST_RUNNING
#endif  // WSTREAMPOSTRAITS_H
