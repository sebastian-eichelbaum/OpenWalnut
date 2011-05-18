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

#ifndef WTRIANGLEMESHTRAITS_H
#define WTRIANGLEMESHTRAITS_H

#include <sstream>

#include <cxxtest/TestSuite.h>
#include <cxxtest/ValueTraits.h>

#include "../../common/test/WTraitsBase.h"
#include "../WTriangleMesh.h"

#ifdef CXXTEST_RUNNING
namespace CxxTest
{
CXXTEST_TEMPLATE_INSTANTIATION
/**
 * Enables better UnitTest OutPut if something fails with WTriangleMeshs, so you see
 * immedeatly what is failing.
 */
class ValueTraits< WTriangleMesh > : public WTraitsBase
{
public:
    /**
     * Constructs a new ValueTrait of a WTriangleMesh for better test output
     */
    explicit ValueTraits( const WTriangleMesh &mesh )
    {
        std::stringstream ss;
        using tm_utils::operator<<;
        ss << mesh;
        m_s = ss.str();
    }
};
}
#endif  // CXXTEST_RUNNING
#endif  // WTRIANGLEMESHTRAITS_H
