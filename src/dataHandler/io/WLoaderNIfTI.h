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

#ifndef WLOADERNIFTI_H
#define WLOADERNIFTI_H

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "../WLoader.h"
#include "../WMetaInfo.h"

#include "nifti/nifti1_io.h"

class WDataSet;
class WDataHandler;


/**
 * Loader for the NIfTI file format. For NIfTI just see http://nifti.nimh.nih.gov/.
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
     * This function is automatically called when creating a new thread for the
     * loader with boost::thread. It calls the methods of the NIfTI I/O library.
     */
    void operator()();

protected:
private:
    template < typename T > std::vector< T > copyArray( const T* dataArray, const size_t nbValues,
            const size_t vDim );
    void fillMetaInfo( boost::shared_ptr< WMetaInfo > metaInfo, nifti_image* header );

    boost::shared_ptr< WDataHandler > m_dataHandler;
};

#endif  // WLOADERNIFTI_H
