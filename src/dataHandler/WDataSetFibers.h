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

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "../common/math/WPosition.h"

#include "WDataSet.h"

/**
 * converts an integer into a byte array and back
 */
union converterByteINT32
{
        unsigned char b[4]; //!< the bytes
        int i; //!< the int
};

/**
 * converts a float into a byte array and back
 */
union converterByteFloat
{
        unsigned char b[4]; //!< the bytes
        float f; //!< the float
};

/**
 * Represents a simple set of WFibers.
 */
class WDataSetFibers : public WDataSet
{
public:
    /**
     * Constructs a new set of fibers, usage of WFiber here is for backward compatibility and should be removed
     *
     * \param vertices the vertices of the fibers, stored in x1,y1,z1,x2,y2,z2, ..., xn,yn,zn scheme
     * \param lineStartIndexes the index in which the fiber start (index of the 3D-vertex, not the index of the float in the vertices vector)
     * \param lineLengths how many vertices belong to a fiber
     * \param verticesReverse stores for each vertex the index of the corresponding fiber
     */
    WDataSetFibers( boost::shared_ptr< std::vector< float > >vertices,
                    boost::shared_ptr< std::vector< size_t > > lineStartIndexes,
                    boost::shared_ptr< std::vector< size_t > > lineLengths,
                    boost::shared_ptr< std::vector< size_t > > verticesReverse );

    /**
     * Constructs a new set of WFibers. The constructed instance is not usable.
     */
    WDataSetFibers();

    /**
     * Get number of fibers in this data set.
     */
    size_t size() const;

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
    boost::shared_ptr< std::vector< size_t > > getLineStartIndexes() const;

    /**
     * Getter
     */
    boost::shared_ptr< std::vector< size_t > > getLineLengths() const;

    /**
     * Getter
     */
    boost::shared_ptr< std::vector< size_t > > getVerticesReverse() const;

    /**
     * Getter
     */
    boost::shared_ptr< std::vector< float > > getTangents() const;

    /**
     * Reference to the vector storing the global colors.
     *
     * \return Pointer to the float array.
     */
    boost::shared_ptr< std::vector< float > > getGlobalColors() const;

    /**
     * Reference to the vector storing the local colors.
     *
     * \return Pointer to the float array.
     */
    boost::shared_ptr< std::vector< float > > getLocalColors() const;

    /**
     * returns the position in space for a vertex of a given fiber
     *
     * \param fiber
     * \param vertex
     */
    wmath::WPosition getPosition( size_t fiber, size_t vertex ) const;

    /**
     * calculates the tangent for a point on the fiber
     *
     * \param fiber
     * \param vertex
     */
    wmath::WPosition getTangent( size_t fiber, size_t vertex ) const;

    /**
     * saves the selected fiber bundles to a file
     *
     *\param filename
     * \param active bitfield of the fiber selection
     */
    void saveSelected( std::string filename, boost::shared_ptr< std::vector< bool > > active ) const;
protected:

    /**
     * The prototype as singleton.
     */
    static boost::shared_ptr< WPrototyped > m_prototype;

private:
    /**
     * Point vector for all fibers
     */
    boost::shared_ptr< std::vector< float > > m_vertices;

    /**
     * Point vector for tangents at each vertex, used for fake tubes
     */
    boost::shared_ptr< std::vector< float > > m_tangents;

    /**
     * Storing the global color value of the fibers for each point.
     */
    boost::shared_ptr< std::vector< float > > m_globalColors;

    /**
     * Storing the local color value of the fibers for each point.
     * \note it is mutable to allow getLocalColors creating it on demand.
     */
    mutable boost::shared_ptr< std::vector< float > > m_localColors;

    /**
     * Line vector that contains the start index of its first point for each line.
     * \warning The index returned cannot be used in the vertices array until
     * the number of components for each point is multiplied.
     */
    boost::shared_ptr< std::vector< size_t > > m_lineStartIndexes;

    /**
     * Line vector that contains the number of vertices for each line
     */
    boost::shared_ptr< std::vector< size_t > > m_lineLengths;

    /**
     * Reverse lookup table for which point belongs to which fiber
     */
    boost::shared_ptr< std::vector< size_t > > m_verticesReverse;
};

#endif  // WDATASETFIBERS_H
