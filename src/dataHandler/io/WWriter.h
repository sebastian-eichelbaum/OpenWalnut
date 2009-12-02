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

#ifndef WWRITER_H
#define WWRITER_H

#include <string>

/**
 * Writes some data to a file. This class is only for convenience access to
 * rudimentary file management things such as: file exists, if the given
 * file should be overwritten, etc. Subclasses may use those mechanisms in
 * their own public memberfunctions where the signature is different every
 * time. e.g. writeFibs( WDataSetFibers ds );
 */
class WWriter
{
public:
    /**
     * Creates a writer object for writing something to a file. Use the flags
     * to specify the behaviour.
     *
     * \param fname path to the target file where stuff will be written to
     * \param overwrite If true existing files will be overwritten
     */
    WWriter( std::string fname, bool overwrite = false );

    /**
     * Reset the destination (file) where the writing should take place.
     */
    void setFileName( std::string fname );

protected:
    std::string m_fname; //!< Absolute path of the file to write to
    bool m_overwrite; //!< flag indicating if the file may be overwritten (true) or not (false)

private:
};

#endif  // WWRITER_H
