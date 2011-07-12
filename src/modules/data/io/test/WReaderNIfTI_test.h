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

#ifndef WREADERNIFTI_TEST_H
#define WREADERNIFTI_TEST_H

#include <vector>

#include <boost/shared_ptr.hpp>

#include <cxxtest/TestSuite.h>

#include "core/common/WLogger.h"
#include "core/dataHandler/exceptions/WDHNoSuchFile.h"
#include "../WReaderNIfTI.h"
#include "../WReaderNIfTI.cpp" //need this to be able instatiate template function

/**
 * test class the nifti reader class
 */
class WReaderNIfTITest : public CxxTest::TestSuite
{
public:
    /**
     * Per test initialization routines
     */
    void setUp( void )
    {
        WLogger::startup();
    }

    /**
     * For each test the tidy up function.
     */
    void tearDown( void )
    {
    }

    /**
     * Test instantiation with non existing file
     */
    void testInstantiationNonExisting( void )
    {
        TS_ASSERT_THROWS( WReaderNIfTI( "no such file" ), const WDHNoSuchFile &e );
    }

    /**
     * Test instantiation
     */
    void testInstantiation( void )
    {
        TS_ASSERT_THROWS_NOTHING( WReaderNIfTI( W_FIXTURE_PATH + "scalar_signed_short.nii.gz" ) );
        TS_ASSERT_THROWS_NOTHING( WReaderNIfTI( W_FIXTURE_PATH + "scalar_unsigned_char.nii.gz" ) );
        TS_ASSERT_THROWS_NOTHING( WReaderNIfTI( W_FIXTURE_PATH + "scalar_float.nii.gz" ) );
        TS_ASSERT_THROWS_NOTHING( WReaderNIfTI( W_FIXTURE_PATH + "vector_float.nii.gz" ) );
        TS_ASSERT_THROWS_NOTHING( WReaderNIfTI( W_FIXTURE_PATH + "symmetric_2nd_order_tensor_float.nii.gz" ) );
        TS_ASSERT_THROWS_NOTHING( WReaderNIfTI( W_FIXTURE_PATH + "vector_unsigned_char.nii.gz" ) );
    }

    /**
     * Test if the loaded files are really loaded
     */
    void testLoading1( void )
    {
        WReaderNIfTI reader1( W_FIXTURE_PATH + "scalar_signed_short.nii.gz" );
        TS_ASSERT( reader1.load() );
    }

    /**
     * Test if the loaded files are really loaded
     */
    void testLoading2( void )
    {
        WReaderNIfTI reader2( W_FIXTURE_PATH + "scalar_unsigned_char.nii.gz" );
        TS_ASSERT( reader2.load() );
    }

    /**
     * Test if the loaded files are really loaded
     */
    void testLoading3( void )
    {
        WReaderNIfTI reader3( W_FIXTURE_PATH + "scalar_float.nii.gz" );
        TS_ASSERT( reader3.load() );
    }

    /**
     * Test if the loaded files are really loaded
     */
    void testLoading4( void )
    {
        WReaderNIfTI reader4( W_FIXTURE_PATH + "vector_float.nii.gz" );
        TS_ASSERT( reader4.load() );
    }

    /**
     * Test if the loaded files are really loaded
     */
    void testLoading5( void )
    {
        WReaderNIfTI reader5( W_FIXTURE_PATH + "symmetric_2nd_order_tensor_float.nii.gz" );
        TS_ASSERT( reader5.load() );
    }

    /**
     * Test if the loaded files are really loaded
     */
    void testLoading6( void )
    {
        WReaderNIfTI reader6( W_FIXTURE_PATH + "vector_unsigned_char.nii.gz" );
        TS_ASSERT( reader6.load() );
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

        // need this for calling the function
        WReaderNIfTI reader1( W_FIXTURE_PATH + "scalar_signed_short.nii.gz" );

        WMatrix< double >  result = reader1.convertMatrix( dummy );

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

    /**
     * Test the function copying an array into a vector
     */
    void testCopyArray( void )
    {
        // need this for calling the function
        WReaderNIfTI reader1( W_FIXTURE_PATH + "scalar_signed_short.nii.gz" );

        const size_t nbVoxels = 10;
        const size_t vDim = 3;
        double* dataArray = new double[nbVoxels * vDim];

        for( unsigned int voxId = 0; voxId < nbVoxels; ++voxId )
        {
            for( unsigned int dim = 0; dim < vDim; ++dim )
            {
                unsigned int i = ( voxId * vDim + dim );
                dataArray[i] = 1.1 * i;
            }
        }
        boost::shared_ptr< std::vector< double > > vec = reader1.copyArray( dataArray, nbVoxels, vDim );

        TS_ASSERT_EQUALS( vec->size(), nbVoxels * vDim );

        double delta = 1e-16;
        for( unsigned int voxId = 0; voxId < nbVoxels; ++voxId )
        {
            for( unsigned int dim = 0; dim < vDim; ++dim )
            {
                // The following two test exactly the same thing.
                TS_ASSERT_DELTA( vec->at( voxId * vDim + dim ), dataArray[voxId + nbVoxels * dim], delta );
                TS_ASSERT_DELTA( vec->at( voxId * vDim + dim ), 1.1 * ( voxId + nbVoxels * dim ), delta );
            }
        }
        delete[] dataArray;
    }
};

#endif  // WREADERNIFTI_TEST_H
