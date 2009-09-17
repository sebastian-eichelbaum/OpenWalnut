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

#ifndef WEEG_H
#define WEEG_H

#include <map>
#include <string>
#include <vector>
#include "WRecording.h"



///======================================
// TODO(wiebel): use this szuff or remove it
#include "../math/WPosition.h"
typedef double dummyType;
class WEEGElectrodeObject
{
public:
protected:
private:
    wmath::WPosition m_position;
    //  WColor color;
};
class WEEGSegementInfo
{
public:
protected:
private:
    dummyType m_startSample;
    dummyType m_endSample;
    dummyType m_segmentStartDate;
};
//================================================

typedef std::vector< double > WEEGElectrode;
typedef std::vector< WEEGElectrode > WEEGSegment;
typedef std::vector< WEEGSegment > WEEGSegmentArray;

typedef std::vector< WEEGElectrodeObject > WEEGElectrodeLibrary;
typedef std::vector< std::pair< std::string, std::string > > WEEGChannelLabels;
/**
 * Contains EEG recording data.
 * \ingroup dataHandler
 */
class WEEG : public WRecording
{
public:


    /**
     * TODO(wiebel): Document this!
     */
    explicit WEEG( const WEEGSegmentArray& data,
                   const WEEGElectrodeLibrary& electrodeLib,
                   const WEEGChannelLabels& channelLabels );

    /**
     * Access operator for single samples.
     */
    const double& operator()( size_t segment, size_t signal, size_t sample ) const
    {
        return m_segments[segment][signal][sample];
    }

    /**
     * Returns number of samples of a given segment.
     */
    size_t getNumberOfSamples( size_t segmentId ) const
    {
        return m_segments[segmentId][0].size();
    }

    /**
     * Return the number of channels this EEG has.
     */
    size_t getNumberOfChannels() const
    {
        return m_segments[0].size();
    }

    /**
     * Return the number of segments this EEG consists of.
     */
    size_t getNumberOfSegments() const
    {
        return m_segments.size();
    }

    /**
     * Return the label of a certain channel.
     */
    std::string getChannelLabel( size_t channelId ) const
    {
        return  m_channelLabels[channelId].first;
    }

protected:
private:
    /**
     * We have only on sampling rate for all channels.
     */
    double m_samplingRate;
    /**
     * Information about segments. An array of WEEGSegmentInfo objects.
     */
    std::vector< WEEGSegementInfo > m_segmentInfo;
    /**
     * Description of electrodes
     */
    std::map< std::string, size_t > m_electrodeDescriptions;

    /**
     * Information about the electrodes.
     */
    WEEGElectrodeLibrary m_electrodeLibrary;

    /**
     * Contains the EEG data as an arry of segements
     * of data which consist of an array of electrodes
     * which again consist of an array of samples over time.
     */
    WEEGSegmentArray m_segments;

    /**
     * Label for each channel.
     */
    WEEGChannelLabels m_channelLabels;

    /**
     * Is the channel enabled?
     */
    std::vector< bool > m_channelEnabled;
};


#endif  // WEEG_H
