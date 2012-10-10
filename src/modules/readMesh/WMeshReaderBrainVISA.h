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

#ifndef WMESHREADERBRAINVISA_H
#define WMESHREADERBRAINVISA_H

#include <core/common/WObjectNDIP.h>

#include "WMeshReaderInterface.h"

/**
 * Load BrainVISA meshes
 */
class WMeshReaderBrainVISA: public WObjectNDIP< WMeshReaderInterface >
{
public:
    /**
     * Constructor
     */
    WMeshReaderBrainVISA();

    /**
     * Destructor
     */
    virtual ~WMeshReaderBrainVISA();

    /**
     * Load the dataset. Throws exceptions if appropriate. Refer to the implementing class.
     *
     * \param parentProgress the progress indicator
     * \param file path to the file to load
     *
     * \return the loaded mesh
     */
    virtual WTriangleMesh::SPtr operator()( WProgressCombiner::SPtr parentProgress,
                                            boost::filesystem::path file );
private:
    WPropInt m_propDatasetSizeX; //!< Size of the dataset (X)
    WPropInt m_propDatasetSizeY; //!< Size of the dataset (Y)
    WPropInt m_propDatasetSizeZ; //!< Size of the dataset (Z)
};

#endif  // WMESHREADERBRAINVISA_H

