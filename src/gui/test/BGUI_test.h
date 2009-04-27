//---------------------------------------------------------------------------
//
// Project: BrainCognize
//
//---------------------------------------------------------------------------

#ifndef BGUI_TEST_H
#define BGUI_TEST_H

#include <cxxtest/TestSuite.h>

#include "../BGUI.h"

class BGUITest : public CxxTest::TestSuite
{
    public:
        // Since BGUI is abstrac no test are here to define
        void testInstanziation( void )
        {
            TS_ASSERT( true );
        }
};

#endif  // BGUI_TEST_H
