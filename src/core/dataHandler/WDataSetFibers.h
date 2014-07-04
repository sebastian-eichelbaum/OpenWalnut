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

#include "../common/math/linearAlgebra/WPosition.h"
#include "../common/WBoundingBox.h"
#include "../common/WProperties.h"
#include "WDataSet.h"


// forward declarations
class WFiber;

class WFiberIterator;

class WFiberPointsIterator;

class WFiberSegmentsIterator;

/**
 * Represents a simple set of WFibers.
 */
class WDataSetFibers : public WDataSet // NOLINT
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
     * Lengths of fibers in terms of vertices.
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
     * Parameter storage for each vertex.
     */
    typedef boost::shared_ptr< std::vector< double > > VertexParemeterArray;

    /**
     * Iterator to go through the fibers.
     */
    typedef WFiberIterator iterator;

    /**
     * Const iterator to go through fibers. As the WFiberIterators does not allow any modifications per-se, the const iterator and the standard
     * iterator are the same.
     */
    typedef WFiberIterator const_iterator;

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
     * Constructs a new set of fibers.
     *
     * \param vertices the vertices of the fibers, stored in x1,y1,z1,x2,y2,z2, ..., xn,yn,zn scheme
     * \param lineStartIndexes the index in which the fiber start (index of the 3D-vertex, not the index of the float in the vertices vector)
     * \param lineLengths how many vertices belong to a fiber
     * \param verticesReverse stores for each vertex the index of the corresponding fiber
     * \param boundingBox The bounding box of the fibers (first minimum, second maximum).
     * \param vertexParameters optional per-vertex scalar.
     */
    WDataSetFibers( boost::shared_ptr< std::vector< float > >vertices,
                    boost::shared_ptr< std::vector< size_t > > lineStartIndexes,
                    boost::shared_ptr< std::vector< size_t > > lineLengths,
                    boost::shared_ptr< std::vector< size_t > > verticesReverse,
                    WBoundingBox boundingBox,
                    VertexParemeterArray vertexParameters );

    /**
     * Constructs a new set of fibers. This constructor determines the bounding box by using the coordinates of the vertices.
     *
     * \param vertices the vertices of the fibers, stored in x1,y1,z1,x2,y2,z2, ..., xn,yn,zn scheme
     * \param lineStartIndexes the index in which the fiber start (index of the 3D-vertex, not the index of the float in the vertices vector)
     * \param lineLengths how many vertices belong to a fiber
     * \param verticesReverse stores for each vertex the index of the corresponding fiber
     * \param vertexParameters optional per-vertex scalar.
     */
    WDataSetFibers( boost::shared_ptr< std::vector< float > >vertices,
                    boost::shared_ptr< std::vector< size_t > > lineStartIndexes,
                    boost::shared_ptr< std::vector< size_t > > lineLengths,
                    boost::shared_ptr< std::vector< size_t > > verticesReverse,
                    VertexParemeterArray vertexParameters );

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
     * Get the parameter values for each vertex. Same indexing as vertices. Used to store additional scalar values for each vertex.
     *
     * \return the array. Can be NULL.
     */
    VertexParemeterArray getVertexParameters() const;

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

    /**
     * Returns an iterator to the first fiber of the dataset. The iterator does not allow any modification of the data.
     *
     * \return An iterator to the first fiber.
     */
    const_iterator begin() const;

    /**
     * Returns an iterator pointing beyond the last fiber. The iterator does not allow any modification of the data.
     *
     * \return An iterator pointing beyond the last fiber.
     */
    const_iterator end() const;

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

    /**
     * Parameter array. Used to store additional scalar values for each vertex.
     */
    VertexParemeterArray m_vertexParameters;
};

/**
 * \class WFiberIterator
 *
 * \brief An iterator for fibers of a fiber dataset.
 *
 * This class iterates fibers of a fiber dataset.
 */
class WFiberIterator
{
public:
    /**
     * Constructor. Creates an invalid iterator pointing nowhere.
     */
    WFiberIterator();

    /**
     * Constructor. Creates an iterator for a specific fiber dataset.
     *
     * \param fibers A pointer to the fiber data.
     * \param idx The index of the fiber to point to.
     */
    WFiberIterator( WDataSetFibers const* fibers, std::size_t idx );

    /**
     * Copy constructor.
     *
     * \param iter The iterator to copy from.
     */
    WFiberIterator( WFiberIterator const& iter ); // NOLINT explicit

    /**
     * Destructor.
     */
    ~WFiberIterator();

    /**
     * Copy operator.
     *
     * \param iter The iterator to copy from.
     *
     * \return *this
     */
    WFiberIterator& operator= ( WFiberIterator const& iter );

    /**
     * Increment operator. Makes the iterator point to the next fiber.
     *
     * \return The incremented iterator.
     */
    WFiberIterator& operator++ ();

    /**
     * Decrement operator. Makes the iterator point to the previous fiber.
     *
     * \return The decremented iterator.
     */
    WFiberIterator& operator-- ();

    /**
     * Increment operator. Makes the iterator point to the next fiber.
     *
     * \return The iterator before incrementing.
     */
    WFiberIterator operator++ ( int );

    /**
     * Decrement operator. Makes the iterator point to the previous fiber.
     *
     * \return The iterator before decrementing.
     */
    WFiberIterator operator-- ( int );

    /**
     * Compare to another fiber iterator.
     *
     * \param rhs The second fiber iterator.
     *
     * \return true, iff the two iterators point to the same fiber of the same fiber dataset.
     */
    bool operator == ( WFiberIterator const& rhs ) const;

    /**
     * Compare to another fiber iterator.
     *
     * \param rhs The second fiber iterator.
     *
     * \return false, iff the two iterators point to the same fiber of the same fiber dataset.
     */
    bool operator != ( WFiberIterator const& rhs ) const;

    /**
     * Creates a point iterator for forward iteration, pointing to the first point of the fiber.
     *
     * \return A point iterator pointing to the first point.
     */
    WFiberPointsIterator begin();

    /**
     * Creates a point iterator for forward iteration, pointing beyond the last point of the fiber.
     *
     * \return A point iterator pointing beyond the last point.
     */
    WFiberPointsIterator end();

    /**
     * Creates a point iterator for backward iteration, pointing to the last point of the fiber.
     *
     * \return A point iterator pointing to the last point.
     */
    WFiberPointsIterator rbegin();

    /**
     * Creates a point iterator for backward iteration, pointing beyond the first point of the fiber.
     *
     * \return A point iterator pointing beyond the first point.
     */
    WFiberPointsIterator rend();

    /**
     * Returns a forward iterator to the first segment.
     *
     * \return A forward iterator to the first segment.
     */
    WFiberSegmentsIterator sbegin();

    /**
     * Returns a forward iterator to the invalid segment after the last (i.e. an end iterator).
     *
     * \return A forward end iterator.
     */
    WFiberSegmentsIterator send();

    /**
     * Returns a backward iterator to the last segment.
     *
     * \return A backward iterator to the last segment.
     */
    WFiberSegmentsIterator srbegin();

    /**
     * Returns a backward iterator to the invalid segment before the first (i.e. an end iterator).
     *
     * \return A backward end iterator.
     */
    WFiberSegmentsIterator srend();

    /**
     * Returns the number of points of the current fiber.
     *
     * \return The number of points.
     */
    std::size_t numPoints() const;

    /**
     * Get the index in the point array where the points data starts for this line. You can use \ref numPoints to know how much data to read
     * from the vertex array.
     *
     * \note You should avoid using these indices as can use the iterators to query the data. But it might get handy in some situations,
     * where raw data processing is needed.
     *
     * \return the start index.
     */
    std::size_t getLineStartIndex() const;

    /**
     * Get the index of the line.
     *
     * \note You should avoid using these indices as can use the iterators to query the data. But it might get handy in some situations,
     * where raw data processing is needed.
     *
     * \return the index.
     */
    std::size_t getIndex() const;

private:
    //! The pointer to the fibers.
    WDataSetFibers const* m_fibers;

    //! The current index in the fiber data.
    std::size_t m_index;
};

/**
 * \class WFiberPointsIterator
 *
 * \brief An iterator for iterating the points of a fiber.
 *
 * Allows for both forward and backward iteration of points.
 */
class WFiberPointsIterator
{
public:
    /**
     * Default contructor. Creates an invalid iterator.
     */
    WFiberPointsIterator();

    /**
     * Constructor. Creates an iterator pointing to a certain point of a fiber.
     *
     * \param fibers The pointer to the fiber data.
     * \param fbIdx The index of the fiber in the fiber dataset.
     * \param idx The index of the point of the current fiber.
     * \param reverse Whether to iterate backwards.
     */
    WFiberPointsIterator( WDataSetFibers const* fibers, std::size_t fbIdx, std::size_t idx, bool reverse = false );

    /**
     * Copy constructor.
     *
     * \param iter The iterator to copy from.
     */
    WFiberPointsIterator( WFiberPointsIterator const& iter ); //NOLINT explicit

    /**
     * Destructor.
     */
    ~WFiberPointsIterator();

    /**
     * Copy operator.
     *
     * \param iter The iterator to copy from.
     *
     * \return *this
     */
    WFiberPointsIterator& operator= ( WFiberPointsIterator const& iter );

    /**
     * Increment operator. Makes the iterator point to the next point.
     *
     * \return The incremented iterator.
     */
    WFiberPointsIterator& operator++ ();

    /**
     * Decrement operator. Makes the iterator point to the previous point.
     *3
     * \return The decremented iterator.
     */
    WFiberPointsIterator& operator-- ();

    /**
     * Increment operator. Makes the iterator point to the next point.
     *
     * \return The iterator before incrementing.
     */
    WFiberPointsIterator operator++ ( int );

    /**
     * Decrement operator. Makes the iterator point to the previous point.
     *
     * \return The iterator before decrementing.
     */
    WFiberPointsIterator operator-- ( int );

    /**
     * Compare to another point iterator.
     *
     * \param rhs The second point iterator.
     *
     * \return true, iff the two iterators point to the same point of the same fiber.
     */
    bool operator== ( WFiberPointsIterator const& rhs ) const;

    /**
     * Compare to another point iterator.
     *
     * \param rhs The second point iterator.
     *
     * \return false, iff the two iterators point to the same point of the same fiber.
     */
    bool operator!= ( WFiberPointsIterator const& rhs ) const;

    /**
     * Returns the coordinates of the point currently pointed to.
     *
     * \return The current coordinates.
     */
    WPosition operator* ();

    /**
     * Returns the parameter specified in the vertex parameter array of the dataset. If no such array was set, the specified default will be
     * returned.
     *
     * \param def the default value which will be returned if no vertex parameter array was defined.
     *
     * \return the value or the specified default
     */
    double getParameter( double def = 0.0 ) const;

    /**
     * The tangent of the point.
     *
     * \return the tangent
     */
    WPosition getTangent() const;

    /**
     * Return the color of the point.
     *
     * \return the color.
     */
    WColor getColor() const;

    /**
     * Return the color of the point.
     *
     * \param idx the index of the colorscheme to use.
     *
     * \throw WDHNoSuchDataSet if the colorscheme does not exist.
     *
     * \return the color.
     */
    WColor getColor( std::size_t idx ) const;

    /**
     * Return the color of the point.
     *
     * \param name the name of the colorscheme to use.
     *
     * \throw WDHNoSuchDataSet if the colorscheme does not exist.
     *
     * \return the color.
     */
    WColor getColor( std::string name ) const;

    /**
     * Return the color of the point.
     *
     * \param scheme the colorscheme to use.
     *
     * \throw WDHNoSuchDataSet if the colorscheme does not exist.
     *
     * \return the color.
     */
    WColor getColor( const boost::shared_ptr< WDataSetFibers::ColorScheme > scheme ) const;

protected:
    /**
     * Calculates the index of this point in the dataset arrays. But be aware that this index works vertex-wise. This mens, getting the y
     * coordinate of the vertex in the dataset vertex array, use  3 * getBaseIndex() + 1. This depends on the type of array you like to query.
     *
     * \note this function properly handles the case when walking in reverse direction.
     *
     * \return the base index, vertex-wise.
     */
    std::size_t getBaseIndex() const;

private:
    //! The pointer to the fibers.
    WDataSetFibers const* m_fibers;

    //! The index of the fiber.
    std::size_t m_fiberIndex;

    //! The index of the current point.
    std::size_t m_index;

    //! Whether to iterate backwards.
    bool m_reverse;
};

/**
 * \class WFiberSegmentsIterator
 *
 * \brief An iterator for iterating the segments of a fiber.
 *
 * Allows for both forward and backward iteration of segments.
 */
class WFiberSegmentsIterator
{
public:
    /**
     * Default contructor. Creates an invalid iterator.
     */
    WFiberSegmentsIterator();

    /**
     * Constructor. Creates an iterator pointing to a certain segment of a fiber.
     *
     * \param fibers The pointer to the fiber data.
     * \param fbIdx The index of the fiber in the fiber dataset.
     * \param idx The index of the starting point of the segment of the current fiber.
     * \param reverse Whether to iterate backwards.
     */
    WFiberSegmentsIterator( WDataSetFibers const* fibers, std::size_t fbIdx, std::size_t idx, bool reverse = false );

    /**
     * Copy constructor.
     *
     * \param iter The iterator to copy from.
     */
    WFiberSegmentsIterator( WFiberSegmentsIterator const& iter ); //NOLINT explicit

    /**
     * Destructor.
     */
    ~WFiberSegmentsIterator();

    /**
     * Copy operator.
     *
     * \param iter The iterator to copy from.
     *
     * \return *this
     */
    WFiberSegmentsIterator& operator= ( WFiberSegmentsIterator const& iter );

    /**
     * Increment operator. Makes the iterator point to the next segment.
     *
     * \return The incremented iterator.
     */
    WFiberSegmentsIterator& operator++ ();

    /**
     * Decrement operator. Makes the iterator point to the previous segment.
     *3
     * \return The decremented iterator.
     */
    WFiberSegmentsIterator& operator-- ();

    /**
     * Increment operator. Makes the iterator point to the next segment.
     *
     * \return The iterator before incrementing.
     */
    WFiberSegmentsIterator operator++ ( int );

    /**
     * Decrement operator. Makes the iterator point to the previous segment.
     *
     * \return The iterator before decrementing.
     */
    WFiberSegmentsIterator operator-- ( int );

    /**
     * Compare to another segment iterator.
     *
     * \param rhs The second segment iterator.
     *
     * \return true, iff the two iterators point to the same segment of the same fiber and have the same directions.
     */
    bool operator== ( WFiberSegmentsIterator const& rhs ) const;

    /**
     * Compare to another segment iterator.
     *
     * \param rhs The second segment iterator.
     *
     * \return false, iff the two iterators point to the same segment of the same fiber and have the same directions.
     */
    bool operator!= ( WFiberSegmentsIterator const& rhs ) const;

    /**
     * Returns an iterator to the starting point of the segment.
     *
     * \return An iterator to the starting point of the segment.
     */
    WFiberPointsIterator start() const;

    /**
     * Returns an iterator to the end point of the segment.
     *
     * \return An iterator to the end point of the segment.
     */
    WFiberPointsIterator end() const;

    /**
     * Returns the vector from the starting point position to the end point position.
     *
     * \return *end() - *start()
     */
    osg::Vec3 direction() const;

private:
    //! The pointer to the fibers.
    WDataSetFibers const* m_fibers;

    //! The index of the fiber.
    std::size_t m_fiberIndex;

    //! The index of the current point.
    std::size_t m_index;

    //! Whether to iterate backwards.
    bool m_reverse;
};

#endif  // WDATASETFIBERS_H

