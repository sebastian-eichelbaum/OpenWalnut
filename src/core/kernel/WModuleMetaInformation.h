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

class WModule;

/**
 * A class abstracting module meta information. It encapsulates module name, description, icon, author lists, help resources, online resources
 * and much more. It is guaranteed to, at least, provide a module name. Everything else is optional. It also encapsulates the
 * WStructuredTextParser class for loading the data.
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
     * Construct a meta info object that loads all information from the specified file. If the file exist and could not be parsed, only
     * an error log is printed. No exception is thrown.
     *
     * \param module The module to load the meta file for.
     */
    explicit WModuleMetaInformation( boost::shared_ptr< WModule > module );

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
    WStructuredTextParser::StructuredValueTree m_metaData;

    /**
     * If true, m_metaData should be queried in all getters. If false, you can query m_meta but it will only tell you that the desired value
     * could not be found.
     */
    bool m_loaded;
};

#endif  // WMODULEMETAINFORMATION_H

