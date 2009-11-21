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

#ifndef WLOADERNIFTI_TEST_H
#define WLOADERNIFTI_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WLoaderNIfTI.h"
#include "../../WDataHandler.h"

/**
 * test class the nifti loader class
 */
class WLoaderNIfTITest : public CxxTest::TestSuite
{
private: // private normally comes behind public, but I would like to make it easily visible here.
    /**
     * Dummy DataHandler instance
     */
    boost::shared_ptr< WDataHandler > m_dataHandler;

public:
    /**
     * Create test environment.
     */
    void setUp( void )
    {
        m_dataHandler = boost::shared_ptr< WDataHandler >( new WDataHandler() );
    }

    /**
     * Test instantiation with non existing file
     */
    void testInstantiationNonExisting( void )
    {
        TS_ASSERT_THROWS( WLoaderNIfTI( "no such file", m_dataHandler ), WDHIOFailure );
    }

    /**
     * Test instantiation
     */
    void testInstantiation( void )
    {
        TS_ASSERT_THROWS_NOTHING( WLoaderNIfTI( "../fixtures/scalar_signed_short.nii.gz", m_dataHandler ) );
        TS_ASSERT_THROWS_NOTHING( WLoaderNIfTI( "../fixtures/scalar_unsigned_char.nii.gz", m_dataHandler ) );
        TS_ASSERT_THROWS_NOTHING( WLoaderNIfTI( "../fixtures/scalar_float.nii.gz", m_dataHandler ) );
        TS_ASSERT_THROWS_NOTHING( WLoaderNIfTI( "../fixtures/vector_float.nii.gz", m_dataHandler ) );
        TS_ASSERT_THROWS_NOTHING( WLoaderNIfTI( "../fixtures/symmetric_2nd_order_tensor_float.nii.gz", m_dataHandler ) );
        TS_ASSERT_THROWS_NOTHING( WLoaderNIfTI( "../fixtures/vector_unsigned_char.nii.gz", m_dataHandler ) );

        TS_ASSERT_EQUALS( m_dataHandler->getNumberOfSubjects(), 0 );
    }

    /**
     * Test if the loaded files are really in the dataHandler
     */
    void testLoading( void )
    {
        WLoaderNIfTI loader1( "../fixtures/scalar_signed_short.nii.gz", m_dataHandler );
        WLoaderNIfTI loader2( "../fixtures/scalar_unsigned_char.nii.gz", m_dataHandler );
        WLoaderNIfTI loader3( "../fixtures/scalar_float.nii.gz", m_dataHandler );
        WLoaderNIfTI loader4( "../fixtures/vector_float.nii.gz", m_dataHandler );
        WLoaderNIfTI loader5( "../fixtures/symmetric_2nd_order_tensor_float.nii.gz", m_dataHandler );
        WLoaderNIfTI loader6( "../fixtures/vector_unsigned_char.nii.gz", m_dataHandler );

        TS_ASSERT_EQUALS( m_dataHandler->getNumberOfSubjects(), 0 );

        loader1.load();

        TS_ASSERT_EQUALS( m_dataHandler->getNumberOfSubjects(), 1 );
        TS_ASSERT_EQUALS( m_dataHandler->getSubject( 0 )->getNumberOfDataSets(), 1 );

        loader2.load();

        TS_ASSERT_EQUALS( m_dataHandler->getNumberOfSubjects(), 1 );
        TS_ASSERT_EQUALS( m_dataHandler->getSubject( 0 )->getNumberOfDataSets(), 2 );

        loader3.load();

        TS_ASSERT_EQUALS( m_dataHandler->getNumberOfSubjects(), 1 );
        TS_ASSERT_EQUALS( m_dataHandler->getSubject( 0 )->getNumberOfDataSets(), 3 );

        loader4.load();

        TS_ASSERT_EQUALS( m_dataHandler->getNumberOfSubjects(), 1 );
        TS_ASSERT_EQUALS( m_dataHandler->getSubject( 0 )->getNumberOfDataSets(), 4 );

        loader5.load();

        TS_ASSERT_EQUALS( m_dataHandler->getNumberOfSubjects(), 1 );
        TS_ASSERT_EQUALS( m_dataHandler->getSubject( 0 )->getNumberOfDataSets(), 5 );

        loader6.load();

        TS_ASSERT_EQUALS( m_dataHandler->getNumberOfSubjects(), 1 );
        TS_ASSERT_EQUALS( m_dataHandler->getSubject( 0 )->getNumberOfDataSets(), 6 );
    }

    /**
     * Test conversion of nifti matrix to OW matrix.
     */
    void testMatrixConversion( void )
    {
        mat44 dummy;
        dummy.m[0][0] = 1.1;
        dummy.m[0][1] = 1.2;
        dummy.m[0][2] = 1.3;
        dummy.m[0][3] = 1.4;
        dummy.m[1][0] = 1.5;
        dummy.m[1][1] = 1.6;
        dummy.m[1][2] = 1.7;
        dummy.m[1][3] = 1.8;
        dummy.m[2][0] = 1.9;
        dummy.m[2][1] = 1.11;
        dummy.m[2][2] = 1.12;
        dummy.m[2][3] = 1.13;
        dummy.m[3][0] = 1.14;
        dummy.m[3][1] = 1.15;
        dummy.m[3][2] = 1.16;
        dummy.m[3][3] = 1.17;

        WLoaderNIfTI loader1( "../fixtures/scalar_signed_short.nii.gz", m_dataHandler );
        wmath::WMatrix< double >  result = loader1.convertMatrix( dummy );

        TS_ASSERT_EQUALS( result.getNbRows(), 4 );
        TS_ASSERT_EQUALS( result.getNbCols(), 4 );

        double delta = 1e-7;
        TS_ASSERT_DELTA( result( 0, 0 ), 1.1, delta );
        TS_ASSERT_DELTA( result( 0, 1 ), 1.2, delta );
        TS_ASSERT_DELTA( result( 0, 2 ), 1.3, delta );
        TS_ASSERT_DELTA( result( 0, 3 ), 1.4, delta );
        TS_ASSERT_DELTA( result( 1, 0 ), 1.5, delta );
        TS_ASSERT_DELTA( result( 1, 1 ), 1.6, delta );
        TS_ASSERT_DELTA( result( 1, 2 ), 1.7, delta );
        TS_ASSERT_DELTA( result( 1, 3 ), 1.8, delta );
        TS_ASSERT_DELTA( result( 2, 0 ), 1.9, delta );
        TS_ASSERT_DELTA( result( 2, 1 ), 1.11, delta );
        TS_ASSERT_DELTA( result( 2, 2 ), 1.12, delta );
        TS_ASSERT_DELTA( result( 2, 3 ), 1.13, delta );
        TS_ASSERT_DELTA( result( 3, 0 ), 1.14, delta );
        TS_ASSERT_DELTA( result( 3, 1 ), 1.15, delta );
        TS_ASSERT_DELTA( result( 3, 2 ), 1.16, delta );
        TS_ASSERT_DELTA( result( 3, 3 ), 1.17, delta );
    }
};

#endif  // WLOADERNIFTI_TEST_H
