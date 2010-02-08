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

#ifndef WDATASETFIBERVECTOR_H
#define WDATASETFIBERVECTOR_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "WDataSet.h"
#include "../math/WFiber.h"

/**
 * Represents a simple set of WFibers.
 */
class WDataSetFiberVector : public WDataSet
{
public:
    /**
     * Constructs a new set of WFibers
     *
     * \param fibs Fiber vector to store in this data set
     */
    explicit WDataSetFiberVector( boost::shared_ptr< std::vector< wmath::WFiber > > fibs );

    /**
     * Constructs a new set of WFibers. The constructed instance is not usable.
     */
    WDataSetFiberVector();

    /**
     * Get number of fibers in this data set.
     */
    size_t size() const;

    /**
     * \param index The index number of the fiber which should be returned
     * \return The i'th fiber.
     */
    const wmath::WFiber& operator[]( const size_t index ) const;

    /**
     * Sort fibers descending on their length.
     */
    void sortDescLength();

    /**
     * Deletes all those fibers which are marked true in the given
     * unused vector.
     *
     * \param unused Vector having the those inidices set to true which should
     * be deleted.
     */
    void erase( const std::vector< bool > &unused );

    /**
     * Determines whether this dataset can be used as a texture.
     *
     * \return true if usable as texture.
     */
    virtual bool isTexture() const;

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

protected:

    /**
     * The prototype as singleton.
     */
    static boost::shared_ptr< WPrototyped > m_prototype;

private:
    boost::shared_ptr< std::vector< wmath::WFiber > > m_fibers; //!< stores all the fibers
};

#endif  // WDATASETFIBERVECTOR_H
