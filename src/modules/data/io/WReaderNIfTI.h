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

#ifndef WREADERNIFTI_H
#define WREADERNIFTI_H

#include <nifti1_io.h>

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "core/dataHandler/io/WReader.h"
#include "core/dataHandler/WDataSet.h"
#include "core/common/math/WMatrix.h"

/**
 * Reader for the NIfTI file format. For NIfTI just see http://nifti.nimh.nih.gov/.
 * \ingroup dataHandler
 */
class WReaderNIfTI : public WReader // NOLINT
{
/**
 * Only UnitTests may be friends.
 */
friend class WReaderNIfTITest;

public:
    /**
     * Constructs a loader to be executed in its own thread and ets the data needed
     * for the loader when executed in its own thread.
     * \param fileName this file will be loaded
     */
    explicit WReaderNIfTI( std::string fileName );

    /**
     * Loads the dataset.
     *
     * \param dataSetType This parameter can be used to tell the  function in advance how it should interprete the data.
     *
     * \return the dataset loaded.
     */
    virtual boost::shared_ptr< WDataSet > load( DataSetType dataSetType = W_DATASET_NONE  );

    /**
     * Returns a standard transformation.
     *
     * \return A Wmatrix that represents the dataset's transformation.
     */
    WMatrix< double > getStandardTransform() const;

    /**
     * Returns the SForm transformation stored in the nifti file's header.
     *
     * \return A Wmatrix that represents the dataset's transformation.
     */
    WMatrix< double > getSFormTransform() const;

    /**
     * Returns the QForm transformation stored in the nifti file's header.
     *
     * \return A Wmatrix that represents the dataset's transformation.
     */
    WMatrix< double > getQFormTransform() const;

protected:
    /**
     * Shorthand type for a vector of gradients.
     */
    typedef boost::shared_ptr< std::vector< WVector3d > > GradVec;

    /** Shorthand type for a vector of bvalues. The comopents are typically given as floats, as partical b-values start
     * at 0 and are at maximum around 17000 (see: https://dx.doi.org/10.1002/mrm.20642).
     */
    typedef boost::shared_ptr< std::vector< float > > BValues;

    /**
     * Reads the additional bval file if available. The file format should be (ASCII file):
     *  - for each (x,y,z) component the bvalue must be repeated in such a form:
     *
     *  bval for_x-component of the first gradient followed by a newline
     *  bval for x-component of the second gradient followed by a newline
     *  ...
     *  bval for x-compoennt of the last gradient followed by a newline
     *  bval for_y-component of the first gradient followed by a newline
     *  ...
     *  bval for y-compoennt of the last gradient followed by a newline
     *  bval for_z-component of the first gradient followed by a newline
     *  ...
     *  bval for z-compoennt of the last gradient followed by a newline
     *
     *  For example: three gradients, with bvalues: 0, 1000, and 1000.
     *
     *  0
     *  1000
     *  1000
     *  0
     *  1000
     *  1000
     *  0
     *  1000
     *  1000
     *
     *  for three gradients, where first all bvalues for all x-components must be given, each in a separate line, then all bavlues
     *  for all y-components, and finally the same for the z-components. Thus there should be three times of the number of
     *  gradients be present in such a file.
     *
     * \param vDim Nifti dimension
     *
     * \return Null ptr if not available or in case of error during read, otherwise the pointer of the vector of BValues.
     */
    BValues readBValuesIfAvailable( unsigned int vDim );

    /**
     * Reads the additional gradient file if available. The file format should be (ASCII file):
     * There are three lines, each containing the x-, y-, and z-components of the gradients.
     * The first line contains all x-components of all gradients. Thus each lines should have the same number of values, each
     * separated by whitespace.
     *
     * x-comp._of_the_first_gradient x-comp._of_the_second_gradient ... x-comp._of_the_last_gradient followed by a newline
     * y-comp._of_the_first_gradient y-comp._of_the_second_gradient ... y-comp._of_the_last_gradient followed by a newline
     * z-comp._of_the_first_gradient z-comp._of_the_second_gradient ... z-comp._of_the_last_gradient followed by a newline
     *
     * For example: three gradients:
     *
     * 0 -0.530466 -0.294864
     * 0 -0.558441 -0.285613
     * 0 0.637769 -0.911856
     *
     * \param vDim Nifti dimension
     *
     * \return Null ptr if not available or in case of error during read, otherwise the pointer of the vector of gradients.
     */
    GradVec readGradientsIfAvailable( unsigned int vDim );

private:
    /**
     * This function allows one to copy the data given as a T*
     * by niftilibio into a std::vector< T >
     * \param dataArray data to copy
     * \param countVoxels number of voxels stored in dataArray
     * \param vDim number of values per voxel
     *
     * \return the copy
     */
    template < typename T > boost::shared_ptr< std::vector< T > > copyArray( const T* dataArray, const size_t countVoxels, const size_t vDim );

    /**
     * This function converts a 4x4 matrix from the NIfTI libs into the format
     * used by OpenWalnut.
     * \param in this matrix will be converted.
     *
     * \return the new matrix
     */
    WMatrix< double > convertMatrix( const mat44& in );

    //! the sform transform stored in the file header
    WMatrix< double > m_sform;

    //! the qform transform stored in the file header
    WMatrix< double > m_qform;
};

#endif  // WREADERNIFTI_H
