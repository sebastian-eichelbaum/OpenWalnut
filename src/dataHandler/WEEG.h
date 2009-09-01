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

#ifndef WEEG_H
#define WEEG_H

#include <map>
#include <string>
#include <vector>
#include "WRecording.h"


///======================================
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

/**
 * Contains EEG recording data.
 * \ingroup dataHandler
 */
class WEEG : public WRecording
{
public:
    typedef std::vector< double > WEEGElectrode;
    typedef std::vector< WEEGElectrode > WEEGSegment;
    typedef std::vector< WEEGSegment > WEEGSegmentArray;

    /**
     * TODO(wiebel): Document this!
     */
    explicit WEEG( boost::shared_ptr< WMetaInfo > metaInfo, const WEEGSegmentArray& data )
        : WRecording( metaInfo ), m_segments( data )
    {
    }

    const double& operator()( size_t segment, size_t signal, size_t sample ) const
    {
        return m_segments[segment][signal][sample];
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
    std::vector< WEEGElectrodeObject > m_electrodeLibrary;

    /**
     * Contains the EEG data as an arry of segements
     * of data which consist of an array of electrodes 
     * which again consist of an array of samples over time.
     */
    WEEGSegmentArray m_segments;
};


#endif  // WEEG_H
