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

#ifndef WPROJECTFILE_H
#define WPROJECTFILE_H

#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

#include "../common/WProjectFileIO.h"
#include "WExportKernel.h"

/**
 * Class loading project files. This class opens an file and reads it line by line. It delegates the actual parsing to each of the known
 * WProjectFileIO instances which then do their job.
 */
class OWKERNEL_EXPORT WProjectFile: public WThreadedRunner,
                                      public boost::enable_shared_from_this< WProjectFile >
{
public:
    /**
     * Default constructor. It does NOT parse the file. Parsing is done by apply().
     *
     * \param project the project file to load.
     */
    explicit WProjectFile( boost::filesystem::path project );

    /**
     * Destructor.
     */
    virtual ~WProjectFile();

    /**
     * Parses the project file and applies it. It applies the project file asynchronously!
     */
    virtual void load();

    /**
     * Saves the current state to the file specified in the constructor.
     */
    virtual void save();

    /**
     * Saves the current state to the file specified in the constructor. This also supports a custom list of writers. This is useful to only
     * write some parts of the state.
     *
     * \param writer the list of writers to use.
     */
    virtual void save( const std::vector< boost::shared_ptr< WProjectFileIO > >& writer );

    /**
     * Returns an instance of the Camera writer.
     *
     * \return the writer able to output the camera configuration to a stream.
     */
    static boost::shared_ptr< WProjectFileIO > getCameraWriter();

    /**
     * Returns an instance of the module writer.
     *
     * \return the writer able to output the module configuration to a stream.
     */
    static boost::shared_ptr< WProjectFileIO > getModuleWriter();

    /**
     * Returns an instance of the ROI writer.
     *
     * \return the writer able to output the ROI configuration to a stream.
     */
    static boost::shared_ptr< WProjectFileIO > getROIWriter();

protected:
    /**
     * Function that has to be overwritten for execution. It gets executed in a separate thread after run()
     * has been called.
     */
    virtual void threadMain();

    /**
     * The project file to parse.
     */
    boost::filesystem::path m_project;

    /**
     * The parser instances. They are used to parse the file.
     */
    std::vector< boost::shared_ptr< WProjectFileIO > > m_parsers;

    /**
     * Do custom exception handling.
     *
     * \param e the exception
     */
    virtual void onThreadException( const WException& e );

private:
};

#endif  // WPROJECTFILE_H

