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

#ifndef WMODULEMETAINFORMATION_H
#define WMODULEMETAINFORMATION_H

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/path.hpp>

#include "../common/WStructuredTextParser.h"

#include "WModule.h"

/**
 * A class abstracting module meta information. It encapsulates module name, description, icon, author lists, help resources, online resources
 * and much more. It is guaranteed to at least provide a module name. Everything else is optional. It also encapsulates the
 * WModuleMetaInformationParser class for loading the data.
 */
class WModuleMetaInformation
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WModuleMetaInformation >.
     */
    typedef boost::shared_ptr< WModuleMetaInformation > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WModuleMetaInformation >.
     */
    typedef boost::shared_ptr< const WModuleMetaInformation > ConstSPtr;

    /**
     * Constructor. The help object will be empty, meaning there is no further meta info available. The name is the only required value. Of
     * course, this is of limited use in most cases.
     *
     * \param name the name of the module
     */
    explicit WModuleMetaInformation( std::string name );

    /**
     * Construct a meta info object that loads all information from the specified file.
     *
     * \param name the module name
     * \param metafile the path to the module's meta file. Usually, this is somewhere in WModule::m_localPath.
     */
    WModuleMetaInformation( std::string name, boost::filesystem::path metafile );

    /**
     * Destructor. Cleans internal list.
     */
    virtual ~WModuleMetaInformation();
protected:
private:
    /**
     * The name of the module providing this meta information.
     */
    std::string m_name;

    /**
     * The tree representing the data
     */
    //WStructuredTextParser::SyntaxTree m_metaData;
};

#endif  // WMODULEMETAINFORMATION_H

