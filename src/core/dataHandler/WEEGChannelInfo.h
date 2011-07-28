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

#ifndef WEEGCHANNELINFO_H
#define WEEGCHANNELINFO_H

#include <cstddef>

#include <string>

#include <boost/shared_ptr.hpp>

#include "../common/math/linearAlgebra/WLinearAlgebra.h"
#include "WEEGPositionsLibrary.h"
#include "exceptions/WDHException.h"
#include "io/WPagerEEG.h"
#include "WExportDataHandler.h"


/**
 * Class which contains information about one channel of an EEG recording, read
 * from a WPagerEEG.
 * \ingroup dataHandler
 */
class OWDATAHANDLER_EXPORT WEEGChannelInfo // NOLINT
{
public:
    /**
     * Constructor
     *
     * \param channelID        number of this channel
     * \param pager            pager class which contains the data, read from a
     *                         file on demand
     * \param positionsLibrary class which contains the positions of the
     *                         electrodes
     */
    WEEGChannelInfo( std::size_t channelID,
                     boost::shared_ptr< WPagerEEG > pager,
                     boost::shared_ptr< WEEGPositionsLibrary > positionsLibrary );

    /**
     * Get the unit used by the recording of the channel.
     *
     * \return unit as string
     */
    std::string getUnit() const;

    /**
     * Get the label of the channel.
     *
     * \return label as string
     */
    std::string getLabel() const;

    /**
     * Get the position of the electrode.
     *
     * \return position
     */
    WPosition getPosition() const throw( WDHException );

protected:
private:
    std::string m_unit; //!< unit used by the recording of the channel
    std::string m_label; //!< label of the channel
    bool m_hasPosition; //!< whether there is a valid position for this electrode
    WPosition m_position; //!< position of the electrode
};

#endif  // WEEGCHANNELINFO_H
