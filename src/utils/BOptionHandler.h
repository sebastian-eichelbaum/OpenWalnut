//---------------------------------------------------------------------------
//
// Project: BrainCognize
//
// Copyright 2009 SomeCopyrightowner
//
// This file is part of BrainCognize.
//
// BrainCognize is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// BrainCognize is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with BrainCognize. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

#ifndef BOPTIONHANDLER_H
#define BOPTIONHANDLER_H

#include <boost/program_options.hpp>

#include "../gui/qt4/BMainApplication.h"

// since the namespace is far to big we use a shortcut here:
namespace po = boost::program_options;

/**
 * Parses commandline options, print usage and issue other appropriate actions.
 */
class BOptionHandler
{
public:
    /**
     * Initiate the handling of the commandline parameter given via argc and
     * argv with the ProgramOptions library from boost.
     */
    BOptionHandler( int argc, char* argv[] );

    /**
     * Evaluates the given options and initiate the appropriate actions.
     *
     * \return Exitcode for main()
     */
    int takeActions();

protected:
private:
    int m_argc;
    char** m_argv;
    bool m_errorOccured;

    /**
     * Stores the option definition such as type, name and description.
     */
    po::options_description m_desc;

    /**
     * Stores the values for each option and provides access to them.
     */
    po::variables_map m_map;

    /**
     * This is a good place to define more Options.
     */
    void createOptions();

    /**
     * Triggers the process of option parsing done by boost program_options.
     */
    void parseOptions();
};

#endif  // BOPTIONHANDLER_H

