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

#ifndef WREADER_H
#define WREADER_H

#include <string>

#include "../exceptions/WDHNoSuchFile.h"
#include "../../common/WDefines.h"

/**
 * Read some data from a given file. This base class is just for file
 * management (file exists, etc.) Subclasses may use those mechanisms and
 * specify their file format which is not the purpose of this base class.
 * \ingroup dataHandler
 */
class WReader
{
public:
    /**
     * Creates a read object for file reading.
     *
     * \param fname path to file which should be loaded
     * \throws WDHNoSuchFile
     */
    explicit WReader( std::string fname );

    /**
     * Reset the file name and checks if it exists.
     *
     * \param fname file name
     * \throws WDHNoSuchFile
     * \deprecated use setFilename instead.
     */
    OW_API_DEPRECATED void setFileName( std::string fname );

    /**
     * Reset the file name and checks if it exists.
     *
     * \param fname file name
     * \throws WDHNoSuchFile
     */
    void setFilename( std::string fname );

protected:
    std::string m_fname; //!< Absolute path of the file to read from

private:
};

#endif  // WREADER_H
