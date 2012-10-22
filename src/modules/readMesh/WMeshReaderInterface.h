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

#ifndef WMESHREADERINTERFACE_H
#define WMESHREADERINTERFACE_H

#include <limits>
#include <string>
#include <vector>
#include <fstream>

#include <boost/filesystem/path.hpp>

#include "core/common/WIOTools.h"
#include "core/common/WProgressCombiner.h"
#include "core/common/WStringUtils.h"
#include "core/common/WLogger.h"
#include "core/dataHandler/exceptions/WDHIOFailure.h"
#include "core/dataHandler/exceptions/WDHNoSuchFile.h"
#include "core/dataHandler/exceptions/WDHParseError.h"
#include "core/graphicsEngine/WTriangleMesh.h"

/**
 * Define the interface which is injected into an \ref WObjectNDIP. Remember that \ref WObjectNDIP is a template class deriving from its
 * template type. This way we can inject methods into the base class. It avoids derivation from \ref WObjectNDIP.
 *
 * This defines the caller-interface common to each mesh loader
 */
class WMeshReaderInterface
{
public:
    /**
     * Load the dataset. Throws exceptions if appropriate. Refer to the implementing class.
     *
     * \param progress the progress indicator
     * \param file path to the file to load
     *
     * \return the loaded mesh
     */
    virtual WTriangleMesh::SPtr operator()( WProgressCombiner::SPtr progress,
                                            boost::filesystem::path file ) = 0;

    /**
     * Destructor
     */
    virtual ~WMeshReaderInterface();

    /**
     * Read a line from the given input stream and throw an exception on error.
     *
     * \param ifs the stream to read from
     * \param desc the description for the exception. Provide some useful info here.
     *
     * \return the line
     */
    std::string getLine( boost::shared_ptr< std::ifstream > ifs, const std::string& desc ) const;

    /**
     * Read a line from the given input stream and throw an exception on error.
     *
     * \param ifs the stream to read from
     * \param desc the description for the exception. Provide some useful info here.
     *
     * \return the line
     */
    std::string getLine( std::ifstream& ifs, const std::string& desc ) const;
};

#endif  // WMESHREADERINTERFACE_H

