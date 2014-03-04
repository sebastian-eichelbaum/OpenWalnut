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

#ifndef WQTNETWORKEDITORPROJECTFILEIO_H
#define WQTNETWORKEDITORPROJECTFILEIO_H

#include <map>
#include <string>

#include <QtCore/QPoint>

#include "core/common/WProjectFileIO.h"

class WQtNetworkEditor;
class WProjectFile;

/**
 * IO class for writing the network editor meta data for the modules.
 */
class WQtNetworkEditorProjectFileIO: public WProjectFileIO
{
public:
    /**
     * Default constructor.
     *
     * \param ne the network editor to use to query meta info.
     */
    explicit WQtNetworkEditorProjectFileIO( WQtNetworkEditor* ne );

    /**
     * Destructor.
     */
    virtual ~WQtNetworkEditorProjectFileIO();

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

    /**
     * Create a clone of the IO. This is especially useful for custom parsers registered at \ref WProjectFile::registerParser. Implement this
     * function.
     *
     * \param project the project file using this parser instance.
     *
     * \return Cloned instance.
     */
    virtual SPtr clone( WProjectFile* project ) const;

protected:
private:
    /**
     * The network editor to use.
     */
    WQtNetworkEditor* m_networkEditor;

    /**
     * Map between module ID and network coord
     */
    typedef std::map< unsigned int, QPoint > ModuleNetworkCoordinates;

    /**
     * The mapping of network coordinates for each module.
     */
    ModuleNetworkCoordinates m_networkCoords;

    /**
     * The bounding coordinates of the loaded grid: top-left
     */
    QPoint m_bbTL;

    /**
     * The bounding coordinates of the loaded grid: top-left
     */
    QPoint m_bbBR;
};

#endif  // WQTNETWORKEDITORPROJECTFILEIO_H

