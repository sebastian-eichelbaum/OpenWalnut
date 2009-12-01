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

#ifndef WWRITERVTK_H
#define WWRITERVTK_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "../WDataSetFibers.h"

/**
 * Writes a VTK file.
 */
class WWriterVTK
{
public:
    /**
     * Creates a writer object for VTK file writing.
     *
     * \param fname absolute file name
     */
    explicit WWriterVTK( std::string fname, bool overwrite = false );

    /**
     * Reset the destination (file) where the writing should take place.
     */
    void setFileName( std::string fname );

    /**
     * Writes a WDataSetFibers down to the previousely given file
     */
    void writeFibs( boost::shared_ptr< const WDataSetFibers > fiberDS ) const;

protected:

private:
    std::string m_fname; //!< Absolute path of the file to write to
    bool m_overwrite; //!< flag indicating if the file may be overwritten (true) or not (false)
};

#endif  // WWRITERVTK_H
