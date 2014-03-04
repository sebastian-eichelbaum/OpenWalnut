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
#include <list>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/signals2/signal.hpp>

#include "../common/WSharedSequenceContainer.h"
#include "../common/WThreadedRunner.h"

#include "../common/WProjectFileIO.h"

class WModuleProjectFileCombiner;
class WModule;

/**
 * Class loading project files. This class opens an file and reads it line by line. It delegates the actual parsing to each of the known
 * WProjectFileIO instances which then do their job.
 */
class WProjectFile: public WThreadedRunner,
                    public boost::enable_shared_from_this< WProjectFile >
{
public:
    /**
     * Abbreviation for a shared pointer.
     */
    typedef boost::shared_ptr< WProjectFile > SPtr;

    /**
     * Abbreviation for const shared pointer.
     */
    typedef boost::shared_ptr< const WProjectFile > ConstSPtr;

    /**
     * A callback function type reporting bach a finished load job. The given string vector contains a list of errors if any.
     */
    typedef boost::function< void( boost::filesystem::path, std::vector< std::string >, std::vector< std::string > ) > ProjectLoadCallback;

    /**
     * A callback function signal type reporting bach a finished load job. The given string vector contains a list of errors if any.
     */
    typedef boost::signals2::signal< void( boost::filesystem::path, std::vector< std::string >, std::vector< std::string > ) >
            ProjectLoadCallbackSignal;

    /**
     * Default constructor. It does NOT parse the file. Parsing is done by using load.
     *
     * \param project the project file to load or save.
     */
    explicit WProjectFile( boost::filesystem::path project );

    /**
     * Default constructor. It does NOT parse the file. Parsing is done by using load.
     *
     * \param project the project file to load.
     * \param doneCallback gets called whenever the load thread finishes.
     */
    WProjectFile( boost::filesystem::path project, ProjectLoadCallback doneCallback );

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
    virtual void save( const std::list< boost::shared_ptr< WProjectFileIO > >& writer );

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

    /**
     * Register a custom project file parser. Use this to add and re-read information from project files. The change takes effect when creating a
     * new instance of WProjectFile. The custom parser needs to implement \ref WProjectFileIO::clone as this is used for instancing the parser
     * for each project file.
     *
     * \note: See \ref WQtNetworkEditorProjectFileIO for a working example.
     *
     * \param parser the parser. Can be added multiple times.
     */
    static void registerParser( WProjectFileIO::SPtr parser );

    /**
     * Remove parser from registry.  The change takes effect when creating a new instance of WProjectFile.
     *
     * \param parser the parser to remove. If not existing, nothing happens.
     */
    static void deregisterParser( WProjectFileIO::SPtr parser );

    /**
     * Map a given project file ID to a module. This method must not be used by WModuleProjectFileCombiner, as it builds this mapping. All other
     * \ref WProjectFileIO implementations are allowed to use it in their save and apply methods (NOT in parse()).
     *
     * \param id the id
     *
     * \return the module, or NULL if ID is not known.
     */
    boost::shared_ptr< WModule > mapToModule( unsigned int id ) const;

    /**
     * Map a given module to project file ID. This method must not be used by WModuleProjectFileCombiner, as it builds this mapping. All other
     * \ref WProjectFileIO implementations are allowed to use it in their save and apply methods (NOT in parse()).
     *
     * \param id the id
     *
     * \return the ID, or numeric_limits< unisigned int >::max() if module not known.*
     */
    unsigned int mapFromModule(  boost::shared_ptr< WModule > module ) const;

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
    std::list< boost::shared_ptr< WProjectFileIO > > m_parsers;

    /**
     * The writer instances. They are used to write the file.
     */
    std::list< boost::shared_ptr< WProjectFileIO > > m_writers;

    /**
     * Do custom exception handling.
     *
     * \param e the exception
     */
    virtual void onThreadException( const WException& e );

private:
    /**
     * Type used for returning lists of parser prototypes.
     */
    typedef WSharedSequenceContainer< std::vector< WProjectFileIO::SPtr > > ParserList;

    /**
     * Signal used to callback someone that the loader was finished.
     */
    ProjectLoadCallbackSignal m_signalLoadDone;

    /**
     * Connection managing the signal m_signalLoadDone. This is the one and only connection allowed and will be disconnected upon thread has
     * finished.
     */
    boost::signals2::connection m_signalLoadDoneConnection;

    /**
     * List of all additional parser prototypes. Handled as singleton.
     */
    static ParserList m_additionalParsers;

    /**
     * This is the only WProjectFileIO instance which is needed. It is used to map ID to module.
     */
    boost::shared_ptr< WModuleProjectFileCombiner > m_moduleIO;
};

#endif  // WPROJECTFILE_H

