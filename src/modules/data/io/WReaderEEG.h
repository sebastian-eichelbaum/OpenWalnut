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

#ifndef WREADEREEG_H
#define WREADEREEG_H

#include <string>

#include "core/dataHandler/WEEG.h"
#include "core/dataHandler/exceptions/WDHIOFailure.h"
#include "core/dataHandler/io/WReader.h"

/**
 * Abstract base class for all Readers who handle with EEG data
 * \ingroup dataHandler
 */
class WReaderEEG : public WReader // NOLINT
{
public:
protected:
    /**
     * Constructs basic eeg Reader with a file name.
     *
     * \param fileName Path to be loaded
     * \throw WDHIOFailure in case of an error
     */
    explicit WReaderEEG( std::string fileName ) throw( WDHIOFailure );

    /**
     * Load electrode positions from ELC file with same name
     *
     * \return electrode library containig the loaded positions
     */
    WEEGElectrodeLibrary extractElectrodePositions();
private:
};

#endif  // WREADEREEG_H
