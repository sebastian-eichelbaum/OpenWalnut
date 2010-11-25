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

#ifndef WVEC3TRAITS_H
#define WVEC3TRAITS_H

#include <sstream>

#include <cxxtest/TestSuite.h>
#include <cxxtest/ValueTraits.h>

#include <osg/Vec3>
#include <osg/io_utils>

#include "../../common/test/WTraitsBase.h"

#ifdef CXXTEST_RUNNING
namespace CxxTest
{
CXXTEST_TEMPLATE_INSTANTIATION
/**
 * Enables better UnitTest OutPut if something fails with osg::Vec3, so you see
 * immedeatly what is failing.
 */
class ValueTraits< osg::Vec3 > : public WTraitsBase
{
public:
    /**
     * Constructor for class allowing usable output of osg::Vec3 in tests
     *
     * \param c the Vec3 to print
     */
    explicit ValueTraits( const osg::Vec3 &c )
    {
        std::stringstream tmp;
        tmp.precision( 16 );
        tmp << "Vec3( " << c << ")";
        m_s = tmp.str();
    }
};
}
#endif  // CXXTEST_RUNNING
#endif  // WVEC3TRAITS_H
