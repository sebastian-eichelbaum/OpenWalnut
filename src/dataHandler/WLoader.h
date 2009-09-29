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

#ifndef WLOADER_H
#define WLOADER_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "exceptions/WDHIOFailure.h"
#include "WDataHandler.h"

/**
 * Base class to all WLoaders which imports data from a given file and generate
 * a WDataSet out of it. The WLoader subclasses will also have to determine
 * the subject a new dataSet belongs to.
 * \ingroup dataHandler
 */
class WLoader
{
public:
    /**
     * Constructs basic Loader with access to the DataHandler (for inserting
     * new stuff) and a file name.
     */
    explicit WLoader( std::string fileName, boost::shared_ptr< WDataHandler > dataHanlder ) throw( WDHIOFailure );

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
    /**
     * Name of file to load.
     */
    std::string m_fileName;

    /**
     * Reference to DataHandler
     */
    boost::shared_ptr< WDataHandler > m_dataHandler;
private:
};

#endif  // WLOADER_H
