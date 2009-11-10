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

#ifndef WLOADERNIFTI_H
#define WLOADERNIFTI_H

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "../WLoader.h"
#include "../../math/WMatrix.hpp"

#include "nifti/nifti1_io.h"

class WDataHandler;

/**
 * Loader for the NIfTI file format. For NIfTI just see http://nifti.nimh.nih.gov/.
 * \ingroup dataHandler
 */
class WLoaderNIfTI : public WLoader
{
public:
    /**
     * Constructs a loader to be executed in its own thread and ets the data needed
     * for the loader when executed in its own thread.
     */
    WLoaderNIfTI( std::string fileName, boost::shared_ptr< WDataHandler > dataHandler );

    /**
     * Loads the dataset.
     * 
     * \return the dataset loaded.
     */
    virtual boost::shared_ptr< WDataSet > load();

protected:
private:
    /**
     * This function allows to copy the data given as a T*
     * by niftilibio into a std::vector< T >
     */
    template < typename T > std::vector< T > copyArray( const T* dataArray, const size_t countVoxels, const size_t vDim );

    /**
     * This function converts a 4x4 matrix from the NIfTI libs into the format
     * used by OpenWalnut.
     */
    wmath::WMatrix< double > convertMatrix( const mat44& in );
};

#endif  // WLOADERNIFTI_H
