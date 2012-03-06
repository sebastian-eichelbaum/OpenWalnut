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

#ifndef WMIXINVECTORTRAITS_H
#define WMIXINVECTORTRAITS_H

#include <sstream>

#include "../WMixinVector.h"
#include "WTraitsBase.h"

#ifdef CXXTEST_RUNNING
namespace CxxTest
{
CXXTEST_TEMPLATE_INSTANTIATION
/**
 * Enables better UnitTest OutPut if something fails with a mixin vector, so
 * you see immedeatly what is failing.
 */
template< class T > class ValueTraits< WMixinVector< T > > : public WTraitsBase
{
public:
    /**
     * Constructs a new ValueTrait of a mixin vector for better test output
     */
    explicit ValueTraits( const WMixinVector< T > & v )
    {
        std::stringstream ss;
        ss << v;
        m_s = ss.str();
    }
};
}
#endif  // CXXTEST_RUNNING
#endif  // WMIXINVECTORTRAITS_H
