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

#ifndef WROIBOX_TEST_H
#define WROIBOX_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WROIBox.h"

/**
 * Test for WROIBox
 */
class WROIBoxTest : public CxxTest::TestSuite
{
public:
    /**
     * I would like to test instantiation here, but WROIBox needs nearly a complete
     * running GUI and GE with the "main" viewer. This seemed to much effort for
     * this small test to me.
     */
    void testInstantiation( void )
    {
//         WPosition maxPos;
//         WPosition minPos;
//         WLogger logger;
//         boost::shared_ptr< WGUI > gui = boost::shared_ptr< WGUI >( new WQt4Gui( 0, 0 ) );
//         boost::shared_ptr< WGraphicsEngine > ge = boost::shared_ptr< WGraphicsEngine >( new WGraphicsEngine );
//         TS_ASSERT_THROWS_NOTHING( WROIBox( minPos, maxPos ) );
    }
};

#endif  // WROIBOX_TEST_H
