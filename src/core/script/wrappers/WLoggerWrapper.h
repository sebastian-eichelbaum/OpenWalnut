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

#ifndef WLOGGERWRAPPER_H
#define WLOGGERWRAPPER_H

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>


#include "../../common/WLogger.h"
#include "../../common/WLogStream.h"

/**
 * \class WLoggerWrapper
 *
 * A wrapper for WLogger. This is used to expose a part of the WLogger functionality
 * to script interpreters.
 */
class WLoggerWrapper
{
    struct fileStreamEntry
    {
        std::string m_filename;
        boost::shared_ptr< std::ofstream > m_fileStream;
        WLogStream::SharedPtr m_WLogStream;
    };
public:
    /**
     * Constructor. Creates an empty wrapper.
     */
    WLoggerWrapper();

    /**
     * Constructor.
     *
     * \param module The actual module.
     */
    explicit WLoggerWrapper( WLogger* logger );
    /**
     * Destructor.
     */
    ~WLoggerWrapper();

    /**
     * Add a file to which the logger output will be written.
     */
    bool addFileStream( std::string filename );


    /**
     * Remove a file to which the logger writes.
     */
    bool removeFileStream( std::string filename );

    /**
     * Remove all files to which the logger writes (and which were added by this wrapper).
     */
    void removeAllFileStreams();


private:
    //! Helper function that removes the file stream with the given index
    void removeFileStreamNumber( size_t i );

    //! A pointer to the logger.
    WLogger* m_logger;

    //! list of file streams
    std::vector< fileStreamEntry > m_fileStreamList;
};

#endif  // WMODULEWRAPPER_H
