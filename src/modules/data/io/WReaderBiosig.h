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

#ifndef WREADERBIOSIG_H
#define WREADERBIOSIG_H

#ifdef WBIOSIG_ENABLED

#include <biosig.h>

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "WReaderEEG.h"

/**
 * Reader for several formats for biological signal.
 * Uses BiosigC++ 4.
 * \ingroup dataHandler
 */
class WReaderBiosig : public WReaderEEG
{
public:
    /**
     * Constructs a loader to be executed in its own thread and sets the data needed
     * for the loader when executed in its own thread.
     * \param fileName this file will be loaded
     */
    explicit WReaderBiosig( std::string fileName );

    /**
     * Loads the dataset.
     *
     * \return the dataset loaded.
     */
    virtual boost::shared_ptr< WDataSet > load();

protected:
private:
    /**
     * Fill the data into the segment doing the needed conversions, assuming
     * column based channels
     * \param segment the segment to be filled
     * \param data the data to be filled into the segment
     */
    void fillSegmentColumnBased( std::vector<std::vector<double> >* segment, biosig_data_type* data );

    /**
     * Fill the data into the segment doing the needed conversions, assuming
     * row based channels
     * \param segment the segment to be filled
     * \param data the data to be filled into the segment
     */
    void fillSegmentRowBased( std::vector<std::vector<double> >* segment, biosig_data_type* data );

    HDRTYPE* hd; //!< Header of file
    size_t m_columns; //!< columns of the storage in the file
    size_t m_rows; //!< rows of the storage in the file
};

#endif  // WBIOSIG_ENABLED

#endif  // WREADERBIOSIG_H
