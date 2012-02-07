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

#ifndef WRECORDING_H
#define WRECORDING_H

#include <string>

#include "../common/WPrototyped.h"

#include "WDataSet.h"


/**
 * Base class for all recorded data and results with events
 * and sensor positions.
 * \ingroup dataHandler
 */
class WRecording : public WDataSet // NOLINT
{
public:
    /**
     * Empty standard constructor for recordings
     */
    explicit WRecording();

    /**
     * Gets the name of this prototype.
     *
     * \return the name.
     */
    virtual const std::string getName() const;

    /**
     * Gets the description for this prototype.
     *
     * \return the description
     */
    virtual const std::string getDescription() const;

    /**
     * Returns a prototype instantiated with the true type of the deriving class.
     *
     * \return the prototype.
     */
    static boost::shared_ptr< WPrototyped > getPrototype();

    /**
     * Maximum number of channels for a certain modality.
     */
    static const unsigned int MAX_RECORDING_CHANNELS = 1024;

    /**
     * Maximum number of samples of a recording.
     * (2^32)-1 this is often equal to UINT_MAX
     */
    static const unsigned int MAX_RECORDING_SAMPLES = 4294967295U;

    /**
     * Maximum number of segments of a recording.
     */
    static const unsigned int MAX_RECORDING_SEGMENTS = 128;

    /**
     * Maximum samplimg frequency of a recording.
     */
    static const unsigned int MAX_RECORDING_SAMPLING_FREQUENCY = 20000;

protected:
    /**
     * The prototype as singleton.
     */
    static boost::shared_ptr< WPrototyped > m_prototype;

private:
};

#endif  // WRECORDING_H
