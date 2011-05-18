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

#ifndef WWRITERDENDROGRAM_H
#define WWRITERDENDROGRAM_H

#include <string>

// Use filesystem version 2 for compatibility with newer boost versions.
#ifndef BOOST_FILESYSTEM_VERSION
    #define BOOST_FILESYSTEM_VERSION 2
#endif
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

#include "../WExportDataHandler.h"
#include "WWriter.h"

class WDendrogram;
/**
 * Writes a Dendrogram file.
 */
class OWDATAHANDLER_EXPORT WWriterDendrogram : public WWriter // NOLINT
{
public:
    /**
     * Creates a writer object for Dendrogram file writing.
     *
     * \param path to the target file where stuff will be written to
     * \param overwrite If true existing files will be overwritten
     */
    WWriterDendrogram( const boost::filesystem::path& path, bool overwrite = false );

    /**
     * Writes finaly the given dendrogram to the previously specified file.
     *
     * \param dendrogram The dendrogram to save
     */
    void write( boost::shared_ptr< const WDendrogram > dendrogram ) const;

protected:
private:
};

#endif  // WWRITERDENDROGRAM_H
