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

#ifndef WDATASETFIBERS_H
#define WDATASETFIBERS_H

#include <vector>

#include <boost/shared_ptr.hpp>

#include "WDataSet.h"
#include "../math/WFiber.h"

/**
 * Represents a simple set of WFibers.
 */
class WDataSetFibers : public WDataSet
{
public:
    /**
     * Constructs a new set of WFibers
     */
    explicit WDataSetFibers( boost::shared_ptr< std::vector< wmath::WFiber > > fibs ) : m_fibers( fibs )
    {
    }

    /**
     * Get number of fibers in this data set.
     */
    inline size_t size() const
    {
        return m_fibers->size();
    }

    /**
     * \return The i'th fiber.
     */
    inline const wmath::WFiber& operator[]( const size_t index ) const
    {
        assert( index < m_fibers->size() );
        return (*m_fibers)[index];
    }

    /**
     * Sort fibers descending on their length.
     */
    void sortDescLength();

    /**
     * Deletes all those fibers which are marked true in the given
     * unused vector.
     */
    void erase( const std::vector< bool > &unused );

protected:

private:
    boost::shared_ptr< std::vector< wmath::WFiber > > m_fibers;
};

#endif  // WDATASETFIBERS_H
