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

#ifndef WREADEREEGPOTENTIALS_H
#define WREADEREEGPOTENTIALS_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "../WDataSetScalar.h"
#include "../exceptions/WDHIOFailure.h"
#include "../exceptions/WDHNoSuchFile.h"
#include "../exceptions/WDHParseError.h"
#include "WReader.h"

/**
 * Class loading EEG reconstructed potential fields. The format is quite similar to all these common ASCII VTK files.
 */
class WReaderEEGPotentials: public WReader
{
public:

    /**
     * Constructs and makes a new reader.
     *
     * \param fname File name where to read data from
     *
     * \throws WDHNoSuchFile if the file could not be opened
     */
    WReaderEEGPotentials( std::string fname );

    /**
     * Destructor.
     */
    virtual ~WReaderEEGPotentials();

    /**
     * Reads the potentials file and creates a dataset out of it.
     *
     * \return Reference to the dataset.
     */
    virtual boost::shared_ptr< WDataSetScalar > read();

protected:

private:
};

#endif  // WREADEREEGPOTENTIALS_H

