//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#ifndef WMREADDIPOLES_H
#define WMREADDIPOLES_H

#include <string>
#include <vector>

#include <osg/Geode>

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

class WDataSetDipoles;

/**
 * Someone should add some documentation here.
 * Probably the best person would be the module's
 * creator, i.e. "wiebel".
 *
 * This is only an empty template for a new module. For
 * an example module containing many interesting concepts
 * and extensive documentation have a look at "src/modules/template"
 *
 * \ingroup modules
 */
class WMReadDipoles: public WModule
{
public:
    /**
     * Simple constructor
     */
    WMReadDipoles();

    /**
     * Destructs the reader.
     */
    virtual ~WMReadDipoles();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const;

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     *
     * \return The icon.
     */
    virtual const char** getXPMIcon() const;

protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();

    /**
     * Initialize requirements for this module.
     */
    virtual void requirements();

private:
    /**
     * Function that composes the data read by readFile() from the different files to one WDataSetDipoles
     *
     * \param filenames Names and locations of files to read.
     * \return A pointer to the loaded dataset
     */
    boost::shared_ptr< WDataSetDipoles > readFiles( std::vector< std::string > filenames );

    /**
     * Function doing the actual reading from one file
     *
     * \param filename Name and locations of file to read.
     * \param pos position of the dipole
     * \param times time values of the time steps
     * \param magnitudes magnitudes of the dipole at the different time steps.
     * \param firstTimeStep First time where the magnitude is not 0
     * \param lastTimeStep Last time where the magnitude is not 0
     */
    void readFile( std::string filename,
                   WPosition* pos,
                   std::vector< float >* times,
                   std::vector< float >* magnitudes,
                   size_t* firstTimeStep,
                   size_t* lastTimeStep );

    /**
     * Function reading meta file with filenames of dipole files
     *
     * \param filename Name and location of meta file to read.
     * \return A pointer to the loaded dataset
     */
    boost::shared_ptr< WDataSetDipoles > readMetaData( std::string filename );

    /**
     * Output connector for dipoles of EEG data
     */
    boost::shared_ptr< WModuleOutputData< WDataSetDipoles > > m_dipoles;

    /**
     * Pointer to the loaded dataset
     */
    boost::shared_ptr< WDataSetDipoles > m_dataSet;

    boost::shared_ptr< WCondition > m_propCondition;  //!< A condition used to notify about changes in several properties.
    WPropFilename m_dataFile; //!< The data will be read from this file.
    WPropBool m_metaFile; //!< Use meta file containing fileNames.
};

#endif  // WMREADDIPOLES_H
