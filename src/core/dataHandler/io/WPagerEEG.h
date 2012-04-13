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

#ifndef WPAGEREEG_H
#define WPAGEREEG_H

#include <cstddef>

#include <string>

#include <boost/shared_ptr.hpp>

#include "../WEEGValueMatrix.h"
#include "../../common/WDefines.h"

/**
 * Abstract class to load an EEG file and keep it open to support paging.
 * \ingroup dataHandler
 */
class WPagerEEG // NOLINT
{
public:
    /**
     * Virtual destructor
     */
    virtual ~WPagerEEG();

    /**
     * Get the name of the loaded file.
     *
     * \return name of file
     */
    std::string getFilename() const;

    /**
     * Get the name of the loaded file.
     *
     * \return name of file
     * \deprecated use getFilename instead
     */
    OW_API_DEPRECATED std::string getFileName() const;

    /**
     * Get the number of segments this EEG consists of.
     *
     * \return number of segments
     */
    virtual std::size_t getNumberOfSegments() const = 0;

    /**
     * Get the number of channels this EEG has.
     *
     * \return number of channels
     */
    virtual std::size_t getNumberOfChannels() const = 0;

    /**
     * Get the number of samples of a given segment.
     *
     * \param segmentID segment number being inspected
     * \return number of samples
     */
    virtual std::size_t getNumberOfSamples( std::size_t segmentID ) const = 0;

    /**
     * Get the values of all channels for a given sample range.
     *
     * \param segmentID segment number to read the values from
     * \param start start sample of the sample range
     * \param length length of the sample range
     * \return matrix of values
     */
    virtual boost::shared_ptr< WEEGValueMatrix > getValues( std::size_t segmentID, std::size_t start, std::size_t length ) const = 0;

    /**
     * Get the sampling rate used by the recording.
     *
     * \return sampling rate
     */
    virtual double getSamplingRate() const = 0;

    /**
     * Get the unit used by the recording of a given channel.
     *
     * \param channelID channel number
     * \return unit as string
     */
    virtual std::string getChannelUnit( std::size_t channelID ) const = 0;

    /**
     * Get the label of a given channel.
     *
     * \param channelID channel number
     * \return label as string
     */
    virtual std::string getChannelLabel( std::size_t channelID ) const = 0;

protected:
    /**
     * Constructor
     *
     * \param filename path and filename to the file to load
     */
    explicit WPagerEEG( std::string filename );
private:
    std::string m_filename; //!< name of the loaded file
};

#endif  // WPAGEREEG_H
