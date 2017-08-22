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

#ifndef WPROTOTYPED_TEST_H
#define WPROTOTYPED_TEST_H

#include <string>

#include <cxxtest/TestSuite.h>

#include "../WPrototyped.h"

/**
 * Helper class derived from WPrototyped to check WPrototypes functionality.
 */
class SomePrototypeClass1: public WPrototyped
{
public:
    /**
     * Gets the name of this prototype.
     *
     * \return the name.
     */
    virtual const std::string getName() const
    {
        return "test1";
    };

    /**
     * Gets the description for this prototype.
     *
     * \return the description
     */
    virtual const std::string getDescription() const
    {
        return "test1";
    };
};


/**
 * Another helper class derived from \ref SomePrototypeClass1. Used to check against \ref SomePrototypeClass1.
 */
class SomePrototypeClass2: public SomePrototypeClass1
{
public:
    /**
     * Gets the name of this prototype.
     *
     * \return the name.
     */
    virtual const std::string getName() const
    {
        return "test2";
    };

    /**
     * Gets the description for this prototype.
     *
     * \return the description
     */
    virtual const std::string getDescription() const
    {
        return "test2";
    };
};

/**
 * Another helper class derived from WPrototyped. Used to check against \ref SomePrototypeClass1.
 */
class SomePrototypeClass3: public WPrototyped
{
public:
    /**
     * Gets the name of this prototype.
     *
     * \return the name.
     */
    virtual const std::string getName() const
    {
        return "test3";
    };

    /**
     * Gets the description for this prototype.
     *
     * \return the description
     */
    virtual const std::string getDescription() const
    {
        return "test3";
    };
};

/**
 * Test WPrototyped
 */
class WPrototypedTest : public CxxTest::TestSuite
{
public:
    /**
     * Test the runtime type check
     */
    void testType( void )
    {
        SomePrototypeClass1 a;
        SomePrototypeClass2 b;
        SomePrototypeClass3 c;

        // check the type checking mechanism in WPrototyped

        // these should be true
        TS_ASSERT( a.isA< SomePrototypeClass1 >() );
        TS_ASSERT( b.isA< SomePrototypeClass2 >() );
        TS_ASSERT( b.isA< SomePrototypeClass1 >() );

        // The following lines would cause a warning
        // "nonnull argument 'this' compared to NULL"
        // because they would be always true,
        // thus they are commented out.
        //TS_ASSERT( a.isA< WPrototyped >() );
        //TS_ASSERT( b.isA< WPrototyped >() );
        //TS_ASSERT( c.isA< WPrototyped >() );

        // check against other types not in polymorphic relation to each other (except the base class)
        TS_ASSERT( !a.isA< SomePrototypeClass2 >() );
        TS_ASSERT( !a.isA< SomePrototypeClass3 >() );
        TS_ASSERT( !b.isA< SomePrototypeClass3 >() );
        TS_ASSERT( !c.isA< SomePrototypeClass1 >() );
        TS_ASSERT( !c.isA< SomePrototypeClass2 >() );
    }
};

#endif  // WPROTOTYPED_TEST_H
