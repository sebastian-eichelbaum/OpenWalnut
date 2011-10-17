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
#include <utility>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>

#include "../common/math/linearAlgebra/WLinearAlgebra.h"
#include "../common/WBoundingBox.h"
#include "../common/WProperties.h"
#include "WDataSet.h"
#include "WExportDataHandler.h"

// forward declarations
class WFiber;

/**
 * Represents a simple set of WFibers.
 */
class OWDATAHANDLER_EXPORT WDataSetFibers : public WDataSet // NOLINT
{
public:

    // some type alias for the used arrays.
    /**
     * Pointer to dataset.
     */
    typedef boost::shared_ptr< WDataSetFibers > SPtr;

    /**
     * Pointer to const dataset.
     */
    typedef boost::shared_ptr< const WDataSetFibers > ConstSPtr;

    /**
     * List of vertex coordinates in term of components of vertices.
     */
    typedef boost::shared_ptr< std::vector< float > > VertexArray;

    /**
     * Index list indexing fibers in VertexArray in terms of vertex numbers.
     */
    typedef boost::shared_ptr< std::vector< size_t > > IndexArray;

    /**
     * Lengths of fibers in terms of verties.
     */
    typedef boost::shared_ptr< std::vector< size_t > > LengthArray;

    /**
     * Tangents at each vertex in VertexArray.
     */
    typedef boost::shared_ptr< std::vector< float > > TangentArray;

    /**
     * Colors for each vertex in VertexArray.
     */
    typedef boost::shared_ptr< std::vector< float > > ColorArray;

    /**
     * Item used in the selection below also containing color info.
     */
    class ColorScheme: public WItemSelectionItem
    {
    friend class WDataSetFibers;
    public:

        /**
         * different kinds of color arrays can be used in this class. This enum defines their possible types.
         */
        typedef enum
        {
            GRAY = 1,   //!< gray value per vertex
            RGB  = 3,   //!< rgb per vertex
            RGBA = 4    //!< rgba per vertex
        }
        ColorMode;

        /**
         * Constructor. Creates new item.
         *
         * \param name name, name of item.
         * \param description description of item. Can be empty.
         * \param icon icon, can be NULL
         * \param color the color array of this item.
         * \param mode the mode of the color array. This defines whether the colors are luminance, RGB or RGBA
         */
        ColorScheme( std::string name, std::string description, const char** icon, ColorArray color, ColorMode mode = RGB ):
            WItemSelectionItem( name, description, icon ),
            m_color( color ),
            m_mode( mode )
        {
        };

        /**
         * Get the color.
         *
         * \return the color array.
         */
        ColorArray getColor() const
        {
            return m_color;
        };

        /**
         * Returns the mode of the color scheme.
         *
         * \return the mode.
         */
        ColorMode getMode() const
        {
            return m_mode;
        };

    protected:

        /**
         * Sets the color array for this item.
         *
         * \param color the color to set.
         * \param mode the mode of the color array. This defines whether the colors are luminance, RGB or RGBA
         */
        void setColor( ColorArray color, ColorMode mode = RGB )
        {
            m_color = color;
            m_mode = mode;
        };

    private:
        /**
         * The color array associated with the item.
         */
        ColorArray m_color;

        /**
         * Coloring mode.
         */
        ColorMode m_mode;
    };

    /**
     * Constructs a new set of fibers.
     *
     * \param vertices the vertices of the fibers, stored in x1,y1,z1,x2,y2,z2, ..., xn,yn,zn scheme
     * \param lineStartIndexes the index in which the fiber start (index of the 3D-vertex, not the index of the float in the vertices vector)
     * \param lineLengths how many vertices belong to a fiber
     * \param verticesReverse stores for each vertex the index of the corresponding fiber
     * \param boundingBox The bounding box of the fibers (first minimum, second maximum).
     */
    WDataSetFibers( boost::shared_ptr< std::vector< float > >vertices,
                    boost::shared_ptr< std::vector< size_t > > lineStartIndexes,
                    boost::shared_ptr< std::vector< size_t > > lineLengths,
                    boost::shared_ptr< std::vector< size_t > > verticesReverse,
                    WBoundingBox boundingBox );

    /**
     * Constructs a new set of fibers. This constructor determines the bounding box by using the coordinates of the vertices.
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
     * Constructs a new set of tracts. The constructed instance is not usable but needed for prototype mechanism.
     */
    WDataSetFibers();

    /**
     * Get number of tracts in this data set.
     * \return number of fibers
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
     * Getter for the lines' vertices
     * \return The vertices of the lines
     */
    VertexArray getVertices() const;

    /**
     * Return the indices that indicate at which vertex ID each line begins in the vertex array.
     * \return The start indices of the lines
     */
    IndexArray getLineStartIndexes() const;

    /**
     * Return the number of vertices for all lines.
     * \return The numbers of all lines' vertices
     */
    LengthArray getLineLengths() const;

    /**
     * Returns a reverse lookup table that allow do find out which vertex belongs to which line.
     * \return Lookup table from vertices to lines.
     */
    IndexArray getVerticesReverse() const;

    /**
     * Returns an array containing the tangents of the fibers at the vertices.
     * \return The tangents of the fibers.
     */
    TangentArray getTangents() const;

    /**
     * Reference to the vector storing the global colors.
     *
     * \return Pointer to the float array. This always is RGB.
     */
    ColorArray getGlobalColors() const;

    /**
     * Reference to the vector storing the local colors.
     *
     * \return Pointer to the float array. This always is RGB.
     */
    ColorArray getLocalColors() const;

    /**
     * This method adds a new color scheme to the list of available colors. The color scheme needs to have a name and description to allow the
     * user to identify which color has which meaning. If the specified color array already exists, only an update is triggered and the name and
     * description is ignored. It detects the type of colors by its size.
     *
     * \param colors the color array. Needs to have exactly getVertices()->size() items.
     * \param name name of the color scheme. Should be a telling name.
     * \param description description. How calculated and so on.
     */
    void addColorScheme( WDataSetFibers::ColorArray colors, std::string name, std::string description );

    /**
     * This method removes the specified color scheme from the list and triggers an update.
     *
     * \param colors the color array.
     */
    void removeColorScheme( WDataSetFibers::ColorArray colors );

    /**
     * Replaces the specified old color scheme by the new color scheme. If the old color scheme did not exist, nothing happens.
     *
     * \param oldColors old colors to remove
     * \param newColors new colors to set
     */
    void replaceColorScheme( WDataSetFibers::ColorArray oldColors, WDataSetFibers::ColorArray newColors );

    /**
     * Get the color scheme with the specified name. If it is not found, an exception gets thrown.
     *
     * \param name the name of the color scheme
     *
     * \return the color scheme
     * \throw WDHNoSuchDataSet if the name could not be found.
     */
    const boost::shared_ptr< ColorScheme > getColorScheme( std::string name ) const;

    /**
     * Get the color scheme with the specified ID. If the index is invalid, an exception gets thrown.
     *
     * \param idx the index
     *
     * \return the color scheme
     */
    const boost::shared_ptr< ColorScheme > getColorScheme( size_t idx ) const;

    /**
     * Convenience method returning the currently selected scheme. This is a comfortable alternative to using the color scheme selection
     * property.
     *
     * \return the current active color scheme
     */
    const boost::shared_ptr< ColorScheme > getColorScheme() const;

    /**
     * Returns the property controlling the color scheme selection.
     *
     * \return the property.
     */
    const WPropSelection getColorSchemeProperty() const;

    /**
     * returns the position in space for a vertex of a given fiber
     *
     * \param fiber Index of fiber
     * \param vertex Index of vertex in fiber.
     *
     * \return Position of the given vertex of the also given fiber
     */
    WPosition getPosition( size_t fiber, size_t vertex ) const;

    /**
     * calculates the tangent for a point on the fiber
     *
     * \param fiber Index of fiber
     * \param vertex Index of vertex in fiber
     *
     * \return Tangent of the given vertex of the also given fiber
     */
    WPosition getTangent( size_t fiber, size_t vertex ) const;

    /**
     * Get the bounding box.
     * \return The bounding box of all lines.
     */
    WBoundingBox getBoundingBox() const;

    /**
     * Constructs a WFiber out of the given tract number.
     *
     * \param numTract Number of the tract to generate a WFiber object for
     *
     * \return The WFiber object. Attention: copy by value!
     */
    WFiber operator[]( size_t numTract ) const;

protected:

    /**
     * The prototype as singleton.
     */
    static boost::shared_ptr< WPrototyped > m_prototype;

private:
    /**
     * This does the common initialisation of the constructors.
     */
    void init();

    /**
     * Point vector for all fibers
     */
    VertexArray m_vertices;

    /**
     * Point vector for tangents at each vertex, used for fake tubes
     */
    TangentArray m_tangents;

    // the following typedefs are for convenience.

    /**
     * An array of color arrays. The first two elements are: 0: global color, 1: local color
     */
    boost::shared_ptr< WItemSelection > m_colors;

    /**
     * Property keeping track of the active color in m_colors.
     */
    WPropSelection m_colorProp;

    /**
     * Line vector that contains the start index of its first point for each line.
     * \warning The index returned cannot be used in the vertices array until
     * the number of components for each point is multiplied.
     */
    IndexArray m_lineStartIndexes;

    /**
     * Line vector that contains the number of vertices for each line
     */
    LengthArray m_lineLengths;

    /**
     * Reverse lookup table for which point belongs to which fiber
     */
    IndexArray m_verticesReverse;

    /**
     * Axis aligned bounding box for all tract-vertices of this dataset.
     */
    WBoundingBox m_bb;
};

#endif  // WDATASETFIBERS_H
