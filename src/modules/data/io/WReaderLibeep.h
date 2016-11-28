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

#ifndef WREADERLIBEEP_H
#define WREADERLIBEEP_H

#ifdef WEEP_ENABLED

#include <string>

#include "WReaderEEG.h"

/**
 * Reader for the CNT format supported by the libeep library.
 * \ingroup dataHandler
 */
class WReaderLibeep : public WReaderEEG // NOLINT
{
public:
    /**
     * Constructs a loader to be executed in its own thread and sets the data
     * needed for the loader when executed in its own thread.
     *
     * \param fileName this file will be loaded
     */
    explicit WReaderLibeep( std::string fileName );

    /**
     * Loads the dataset.
     *
     * \return the dataset loaded.
     */
    virtual boost::shared_ptr< WDataSet > load();
protected:
private:
};

#endif  // WEEP_ENABLED

#endif  // WREADERLIBEEP_H
