//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#ifndef WLOADERBIOSIG_H
#define WLOADERBIOSIG_H

#include <string>
#include <boost/shared_ptr.hpp>

#include "../WLoader.h"

class WDataHandler;

/**
 * Loader for several formats for biological signal.
 * Uses BiosigC++ 4.
 */
class WLoaderBiosig : public WLoader
{
public:
    /**
     * Constructs a loader to be executed in its own thread and ets the data needed
     * for the loader when executed in its own thread.
     */
    WLoaderBiosig( std::string fileName, boost::shared_ptr< WDataHandler > dataHandler );

    /**
     * This  function is automatically called when creating a new thread for the
     * loader with boost::thread.
     */
    void operator()();
protected:
private:
    void biosigLoader();
    std::string m_fileName;
    boost::shared_ptr< WDataHandler > m_dataHandler;
};

#endif  // WLOADERBIOSIG_H
