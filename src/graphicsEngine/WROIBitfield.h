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

#ifndef WROIBITFIELD_H
#define WROIBITFIELD_H

#include <vector>

#include "WExportWGE.h"

#include "WROI.h"

/**
 * defines a roi by a simple bitfield that never changes
 * while a normal roi computes it's bitfield from it's location in space and the fibers passing through,
 * this roi simply stores a bitfield of previously selected fibers
 */
class WGE_EXPORT WROIBitfield : public WROI
{
public:
    /**
     * constructor
     * \param bitfield pointer to a bitfield
     */
    explicit WROIBitfield( boost::shared_ptr< std::vector<bool> > bitfield );

    /**
     * destructor
     */
    virtual ~WROIBitfield();

    /**
     * getter
     * \return the bitfield of selected fibers
     */
    boost::shared_ptr< std::vector<bool> >getBitfield();

protected:
private:
    /**
     * does nothing but had to be implemented beccause it's pure virtual in WROI
     */
    void updateGFX();

    boost::shared_ptr< std::vector<bool> >m_bitfield; //!< stores pointer to the bitfield
};

inline boost::shared_ptr< std::vector<bool> >WROIBitfield::getBitfield()
{
    return m_bitfield;
}

#endif  // WROIBITFIELD_H
