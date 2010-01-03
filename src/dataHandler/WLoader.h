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
#include "WSubject.h"

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
     *
     *\param fileName Path to be loaded
     *\param dataHanlder A pointer where the loaded data set are stored
     *\throw WDHIOFailure in case of an error
     */
    explicit WLoader( std::string fileName, boost::shared_ptr< WDataHandler > dataHanlder ) throw( WDHIOFailure );

    /**
     * Destructor is virtual since there are virtual methods.
     */
    virtual ~WLoader()
    {
    };

    /**
     * Loads the dataset.
     *
     * \return the dataset loaded.
     */
    virtual boost::shared_ptr< WDataSet > load() = 0;

protected:
    std::string m_fileName; //!< Name of file to load.

    boost::shared_ptr< WDataHandler > m_dataHandler; //!< Reference to DataHandler
    /**
     * Commit new data into the DataHandler.
     * \param data This dataset will be added to the WDataHandler
     */
    void commitDataSet( boost::shared_ptr< WDataSet > data );

private:
};

#endif  // WLOADER_H
