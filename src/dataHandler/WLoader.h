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

#ifndef WLOADER_H
#define WLOADER_H

#include <string>

/**
 * Base class to all WLoaders which imports data from a given file and generate
 * a WDataSet out of it.
 * \ingroup dataHandler
 */
class WLoader
{
public:
    explicit WLoader( std::string fileName );

    virtual ~WLoader()
    {
    };

    /**
     * This function is automatically called when creating a new thread for the
     * loader with boost::thread. The method is pure virtual here to ensure
     * that subclasses implement it.
     */
    virtual void operator()() = 0;

protected:
    std::string m_fileName;
private:
};

#endif  // WLOADER_H
