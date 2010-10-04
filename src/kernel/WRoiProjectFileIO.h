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

#ifndef WROIPROJECTFILEIO_H
#define WROIPROJECTFILEIO_H

#include <string>

#include "../common/WProjectFileIO.h"

#include "WExportKernel.h"

/**
 * IO Class for writing the ROI structure to a project file.
 */
class OWKERNEL_EXPORT WRoiProjectFileIO: public WProjectFileIO
{
public:

    /**
     * Default constructor.
     */
    WRoiProjectFileIO();

    /**
     * Destructor.
     */
    virtual ~WRoiProjectFileIO();

    /**
     * This method parses the specified line and interprets it. It gets called line by line by WProjectFile.
     *
     * \param line the current line as string
     * \param lineNumber the current line number. Useful for error/warning/debugging output.
     *
     * \return true if the line could be parsed.
     */
    virtual bool parse( std::string line, unsigned int lineNumber );

    /**
     * Called whenever the end of the project file has been reached. This is useful if your specific parser class wants to do some post
     * processing after parsing line by line.
     */
    virtual void done();

    /**
     * Saves the state to the specified stream.
     *
     * \param output the stream to print the state to.
     */
    virtual void save( std::ostream& output );   // NOLINT

protected:

private:
};

#endif  // WROIPROJECTFILEIO_H

