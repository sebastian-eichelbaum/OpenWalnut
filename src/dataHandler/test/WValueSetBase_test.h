//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#ifndef WVALUESETBASE_TEST_H
#define WVALUESETBASE_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WValueSetBase.h"


/**
 * Dummy class for testing the abstract class WValueSetBase
 */
class Dummy : public WValueSetBase
{
friend class WValueSetBaseTest;

public:
    Dummy()
        : WValueSetBase( 0, 1 )
    {
    }

    virtual ~Dummy()
    {
    }

    virtual size_t size() const
    {
        return 255;
    }

    virtual size_t rawSize() const
    {
        return 255;
    }
};

/**
 * Testing abstract class via a Dummy class derived esp. for this purpose.
 */
class WValueSetBaseTest : public CxxTest::TestSuite
{
public:
    /**
     *  Checks if the Dummy is instanceable.
     */
    void testInstanziation( void )
    {
        Dummy d;
    }
};

#endif  // WVALUESETBASE_TEST_H
