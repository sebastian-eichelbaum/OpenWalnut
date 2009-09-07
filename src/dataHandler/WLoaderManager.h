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

#ifndef WLOADERMANAGER_H
#define WLOADERMANAGER_H

#include <string>
#include <boost/shared_ptr.hpp>

class WDataSet;
class WDataHandler;

/**
 * Decouples file loading from the rest of OpenWalnut into a single thread.
 * \ingroup dataHandler
 */
class WLoaderManager
{
public:
    /**
     * Selects correct loader for fileName and creates loader thread.
     */
    void load( std::string fileName, boost::shared_ptr< WDataHandler > dataHandler );
protected:
private:
};

#endif  // WLOADERMANAGER_H
