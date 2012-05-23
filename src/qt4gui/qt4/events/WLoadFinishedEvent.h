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

#ifndef WLOADFINISHEDEVENT_H
#define WLOADFINISHEDEVENT_H

#include <vector>
#include <string>

#include <boost/filesystem/path.hpp>

#include <QtCore/QEvent>

/**
 * Event signalling a finished asynchronous load job.
 */
class WLoadFinishedEvent: public QEvent
{
public:
    /**
     * Creates a new event instance denoting that a asynchronous load was finished.
     *
     * \param filename the filename of the file that was loaded
     * \param errors the list of errors. Can be empty.
     */
    WLoadFinishedEvent( boost::filesystem::path filename, std::vector< std::string > errors );

    /**
     * Destructor.
     */
    virtual ~WLoadFinishedEvent();

    /**
     * Returns the list of errors
     *
     * \return the errors.
     */
    const std::vector< std::string >& getErrors() const;

    /**
     * The filename of the file loaded.
     *
     * \return the filename
     */
    std::string getFilename() const;

protected:
private:
    /**
     * The filename of the file loaded
     */
    boost::filesystem::path m_filename;

    /**
     * The error list.
     */
    std::vector< std::string > m_errors;
};

#endif  // WLOADFINISHEDEVENT_H


