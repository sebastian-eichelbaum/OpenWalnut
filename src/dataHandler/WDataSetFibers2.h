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

#ifndef WDATASETFIBERS2_H
#define WDATASETFIBERS2_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "../math/WPosition.h"

#include "WDataSet.h"


/**
 * Represents a simple set of WFibers.
 */
class WDataSetFibers2 : public WDataSet
{
public:
    /**
     * Constructs a new set of fibers, usage of WFiber here is for backward compatibility and should be removed
     *
     * \param fibs Fiber vector to store in this data set
     * \param vertices
     * \param lineStartIndexes
     * \param lineLengths
     * \param verticesReverse
     */
    WDataSetFibers2( boost::shared_ptr< std::vector< float > >vertices,
                    boost::shared_ptr< std::vector< unsigned int > > lineStartIndexes,
                    boost::shared_ptr< std::vector< unsigned int > > lineLengths,
                    boost::shared_ptr< std::vector< unsigned int > > verticesReverse );

    /**
     * Constructs a new set of WFibers. The constructed instance is not usable.
     */
    WDataSetFibers2();

    /**
     * Get number of fibers in this data set.
     */
    size_t size() const;

    /**
     * \param index The index number of the fiber which should be returned
     * \return The i'th fiber.
     */
    //const wmath::WFiber& operator[]( const size_t index ) const;

    /**
     * Sort fibers descending on their length.
     */
    void sortDescLength();

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

    /**
     * Getter for m_vertices
     */
    boost::shared_ptr< std::vector< float > > getVertices() const;

    /**
     * Getter
     */
    boost::shared_ptr< std::vector< unsigned int > > getLineStartIndexes() const;

    /**
     * Getter
     */
    boost::shared_ptr< std::vector< unsigned int > > getLineLengths() const;

    /**
     * Getter
     */
    boost::shared_ptr< std::vector< unsigned int > > getVerticesReverse() const;

    /**
     *
     */
    wmath::WPosition getPosition( unsigned int fiber, unsigned int vertex ) const;

protected:

    /**
     * The prototype as singleton.
     */
    static boost::shared_ptr< WPrototyped > m_prototype;

private:
    /**
     * Point vector for all fibers that is actually usable for what we want to do
     */
    boost::shared_ptr< std::vector< float > > m_vertices;

    /**
     * Line vector that contains the start index for each line
     */
    boost::shared_ptr< std::vector< unsigned int > > m_lineStartIndexes;

    /**
     * Line vector that contains the number of vertices for each line
     */
    boost::shared_ptr< std::vector< unsigned int > > m_lineLengths;

    /**
     * Reverse lookup table for which point belongs to which fiber
     */
    boost::shared_ptr< std::vector< unsigned int > > m_verticesReverse;
};

#endif  // WDATASETFIBERS2_H
