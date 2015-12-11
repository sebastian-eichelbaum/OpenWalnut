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

#ifndef WPAGEREEGLIBEEP_H
#define WPAGEREEGLIBEEP_H

#ifdef WEEP_ENABLED

#include <cstddef>

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "core/dataHandler/WEEGValueMatrix.h"
#include "core/dataHandler/io/WPagerEEG.h"

typedef struct eeg_dummy_t eeg_t;

/**
 * Class to load an EEG file and keep it open to support paging.
 * Uses the libeep library to read the CNT format.
 * \ingroup dataHandler
 */
class WPagerEEGLibeep : public WPagerEEG // NOLINT
{
public:
    /**
     * Constructor
     *
     * \param fileName path and filename to a CNT file
     */
    explicit WPagerEEGLibeep( std::string fileName );

    /**
     * Virtual destructor
     */
    virtual ~WPagerEEGLibeep();

    virtual std::size_t getNumberOfSegments() const;

    virtual std::size_t getNumberOfChannels() const;

    virtual std::size_t getNumberOfSamples( std::size_t segmentID ) const;

    virtual boost::shared_ptr< WEEGValueMatrix > getValues( std::size_t segmentID, std::size_t start, std::size_t length ) const;

    virtual double getSamplingRate() const;

    virtual std::string getChannelUnit( std::size_t channelID ) const;

    virtual std::string getChannelLabel( std::size_t channelID ) const;

protected:
private:
    eeg_t* m_eeg; //!< handler for the cnt file opened by libeep

    std::size_t m_nbChannels; //!< number of channels
    std::size_t m_nbSamples; //!< number of samples

    std::vector< double > m_scales; //!< scale factors of the channels
};

#endif  // WEEP_ENABLED

#endif  // WPAGEREEGLIBEEP_H
