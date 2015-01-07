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

#ifndef WFIBERCLUSTER_H
#define WFIBERCLUSTER_H

#include <list>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "../../common/WColor.h"
#include "../../common/WTransferable.h"
#include "../WDataSetFiberVector.h"


/**
 * Represents a cluster of indices of a WDataSetFiberVector.
 */
class WFiberCluster: public WTransferable // NOLINT
{
friend class WFiberClusterTest;
public:
    /**
     * Shared pointer abbreviation.
     */
    typedef boost::shared_ptr< WFiberCluster > SPtr;

    /**
     * Const shared pointer abbreviation.
     */
    typedef boost::shared_ptr< const WFiberCluster > ConstSPtr;

    /**
     * This is the list of indices of fibers.
     */
    typedef std::list< size_t > IndexList;

    /**
     * Const iterator on the index list.
     */
    typedef IndexList::const_iterator IndexListConstIterator;

    /**
     * Constructs an cluster with one fiber and a reference to the fiber dataset
     * to compute the intercluster distance.
     *
     * \param index The index of the first fiber belonging to this cluster
     */
    explicit WFiberCluster( size_t index );

    /**
     * Constructs a cluster with the specified set of indices and the given color.
     *
     * \param indices the indices initially used for this clustering
     * \param color the color of this cluster
     */
    WFiberCluster( const IndexList& indices, const WColor& color = WColor(), std::string const& annotation = std::string() );

    /**
     * Constructs a clustering with the given set of indices. The indexlist is generated using the given iterators. It copies the elements in
     * [indicesBegin,indicesEnd).
     *
     * \param indicesBegin begin iterator in the predefined index set
     * \param indicesEnd end iterator in the predefined index set
     * \param color the color of this cluster
     */
    WFiberCluster( IndexListConstIterator indicesBegin, IndexListConstIterator indicesEnd,
                   const WColor& color = WColor(), std::string const& annotation = std::string() );

    /**
     * Copies the specified \ref WFiberCluster Instance. The copy does not contain a valid centerline or longest line.
     *
     * \param other the other instance to clone.
     */
    WFiberCluster( const WFiberCluster& other );

    /**
     * Constructs an empty cluster.
     */
    WFiberCluster();

    /**
     * Destructs. Frees used locks/mutex.
     */
    virtual ~WFiberCluster();

    /**
     * Returns true if there are no fibers in that cluster, false otherwise.
     *
     * \return true if empty
     */
    bool empty() const;

    /**
     * Merge the fibers of the other cluster with the fibers of this cluster.
     * Afterwards the other cluster is empty.
     *
     * \param other The other WFiberCluster which should merged into this one
     */
    void merge( WFiberCluster &other ); // NOLINT

    /**
     * Copy the elements denoted by the two iterators to this cluster. In contrast to the other merge() methods, this will not clean the source
     * list.
     *
     * \param indicesBegin begin iterator in the predefined index set
     * \param indicesEnd end iterator in the predefined index set
     */
    void merge( IndexListConstIterator indicesBegin, IndexListConstIterator indicesEnd );

    /**
     * Returns a const reference of all indices inside this cluster
     *
     * \return the index list
     */
    const IndexList& getIndices() const;

    /**
     * Reset the indices belonging to that cluster
     *
     * \param indices list of indices
     */
    void setIndices( const IndexList& indices );

    /**
     * Sort the indices of fibers associated with this cluster in ascending
     * order.
     */
    void sort();

    /**
     * \return Number of fibers associated with this cluster.
     */
    size_t size() const;

    /**
     * Make this cluster empty. Note: The real fibers from fiber dataset are
     * not deleted.
     */
    void clear();

    /**
     * Sets the color of which all fibers of this clusters should be painted
     * with.
     *
     * \param color The color for all fibers of this cluster.
     */
    void setColor( WColor color );

    /**
     * Gets the color of which all fibers of this clusters should be painted
     * with.
     *
     * \return cluster color.
     */
    WColor getColor() const;

    /**
     * Set the annotation for this fiber cluster.
     *
     * \param anno The new annotation.
     */
    void setAnnotation( std::string const& anno );

    /**
     * Get the annotation string.
     *
     * \return The annotation string.
     */
    std::string const& getAnnotation() const;

    /**
     * Sets the main direction of the cluster
     *
     * \param mainDirection the cluster's main direction
     */
    void setMainDirection( osg::Vec3 mainDirection );

    /**
     * Gets the main direction of the cluster ( if set )
     *
     * \return the cluster's main direction
     */
    osg::Vec3 getMainDirection() const;

    /**
     * The name of this transferable. This is useful information for the users.
     *
     * \return the name.
     */
    virtual const std::string getName() const;

    /**
     *
     * The description of this transferable. This is useful information for the users.
     *
     * \return A description
     */
    virtual const std::string getDescription() const;

    /**
     * \param other The other fiber which should be compared
     * \return true If both clusters having same fibers IN SAME ORDER!
     */
    bool operator==( const WFiberCluster& other ) const;

    /**
     * The opposite of the operator==
     *
     * \param other The other fiber which should be compared
     * \return false If both clusters having same fibers IN SAME ORDER!
     */
    bool operator!=( const WFiberCluster& other ) const;

    /**
     * Copy assignment operator which does NOT copy the mutex's!!!
     *
     * \param other The instance to copy.
     *
     * \return itself
     */
    WFiberCluster& operator=( const WFiberCluster& other )
    {
        WTransferable::operator=( other );
        m_memberIndices = other.m_memberIndices;
        m_fibs = other.m_fibs;
        m_color = other.m_color;
        m_centerLineCreationLock = new boost::shared_mutex();
        m_longestLineCreationLock = new boost::shared_mutex();
        // copy them only if they exist
        if( other.m_centerLine )
        {
            m_centerLine = boost::shared_ptr< WFiber >( new WFiber( *other.m_centerLine.get() ) );
        }
        if( other.m_longestLine )
        {
            m_longestLine = boost::shared_ptr< WFiber >( new WFiber( *other.m_longestLine.get() ) );
        }
        return *this;
    }

    // TODO(math): The only reason why we store here a Reference to the fiber
    // dataset is, we need it in the WMVoxelizer module as well as the clustering
    // information. Since we don't have the possibility of multiple
    // InputConnectors we must agglomerate those into one object. Please remove this.
    // \cond Suppress_Doxygen
    void setDataSetReference( boost::shared_ptr< const WDataSetFiberVector > fibs );
    boost::shared_ptr< const WDataSetFiberVector > getDataSetReference() const;
    // \endcond

    /**
     * Returns a prototype instantiated with the true type of the deriving class.
     *
     * \return the prototype.
     */
    static boost::shared_ptr< WPrototyped > getPrototype();

    /**
     * Returns the center line of this cluster. The centerline gets calculated during the first call of this method.
     *
     * \return Reference to the center line
     */
    boost::shared_ptr< WFiber > getCenterLine() const;

    /**
     * Returns the center line of this cluster. The longest line gets calculated during the first call if this method.
     *
     * \return Reference to the longest line
     */
    boost::shared_ptr< WFiber > getLongestLine() const;

    /**
     * Makes the hard work to compute the center line.
     */
    void generateCenterLine() const;

    /**
     * Makes the hard work to find the longest line.
     */
    void generateLongestLine() const;

    /**
     * Recomputes on every call the axis aligned bounding box incorporating all tracts in this cluster.
     *
     * \return AABB as WBoundingBox.
     */
    WBoundingBox getBoundingBox() const;

protected:
    /**
     * Prototype for this dataset
     */
    static boost::shared_ptr< WPrototyped > m_prototype;

    /**
     * Alings all fibers within the given dataset to be in one main direction. But Alignment only may swap the ordering of the fibers
     * but not the positions or something similar. We need this only for the centerline generation.
     *
     * \param fibs The dataset
     */
    void unifyDirection( boost::shared_ptr< WDataSetFiberVector > fibs ) const;

private:
    /**
     * The centerline may be shortened due to the averaging of outliers. To
     * nevertheless color almost the whole bundle surface we need a surface
     * parameterization (given via the centerline) upto the endings of the
     * bundle. Therefore the centerline is stepwise elongated with the last
     * known direction, until no perpendicular plane intersects any of the
     * tracts inside of the bundle.
     */
    void elongateCenterLine() const;

    /**
     * All indices in this set are members of this cluster
     */
    IndexList m_memberIndices;

    // TODO(math): The only reason why we store here a Reference to the fiber
    // dataset is, we need it in the WMVoxelizer module as well as the clustering
    // information. Since we don't have the possibility of multiple
    // InputConnectors we must agglomerate those into one object. Please remove this.
    /**
     * Reference to the real fibers of the brain this cluster belongs to.
     */
    boost::shared_ptr< const WDataSetFiberVector > m_fibs;

    /**
     * Color which is used to paint the members of this cluster.
     */
    WColor m_color;

    /**
     * An annotation for the bundle, usually a name.
     */
    std::string m_annotation;

    /**
     * The cluster's main direction
     */
    osg::Vec3 m_mainDirection;

    /**
     * Lock the modification in the m_centerLine mutable. The lock is stored as pointer to avoid copy construction problems.
     */
    boost::shared_mutex* m_centerLineCreationLock;

    /**
     * Lock the modification in the m_longestLine mutable. The lock is stored as pointer to avoid copy construction problems.
     */
    boost::shared_mutex* m_longestLineCreationLock;

    /**
     * Average fiber for this cluster representing the main direction and curvature of this cluster.
     *
     * \note This member is mutable as it needs to be modified during a const getter.
     */
    mutable boost::shared_ptr< WFiber > m_centerLine;

    /**
     * The longest fiber in the dataset.
     *
     * \note This member is mutable as it needs to be modified during a const getter.
     */
    mutable boost::shared_ptr< WFiber > m_longestLine;
};

inline bool WFiberCluster::empty() const
{
    return m_memberIndices.empty();
}

inline void WFiberCluster::sort()
{
    m_memberIndices.sort();
}

inline size_t WFiberCluster::size() const
{
    return m_memberIndices.size();
}

inline void WFiberCluster::clear()
{
    m_memberIndices.clear();
}

inline void WFiberCluster::setColor( WColor color )
{
    m_color = color;
}

inline WColor WFiberCluster::getColor() const
{
    return m_color;
}

inline void WFiberCluster::setAnnotation( std::string const& anno )
{
    m_annotation = anno;
}

inline std::string const& WFiberCluster::getAnnotation() const
{
    return m_annotation;
}

inline void WFiberCluster::setMainDirection( osg::Vec3 mainDirection )
{
    m_mainDirection = mainDirection;
}

inline osg::Vec3 WFiberCluster::getMainDirection() const
{
    return m_mainDirection;
}

inline const std::string WFiberCluster::getName() const
{
    return "FiberCluster";
}

inline const std::string WFiberCluster::getDescription() const
{
    return "A collection of indices for fibers representing a fiber cluster";
}

inline bool WFiberCluster::operator==( const WFiberCluster& other ) const
{
    return m_memberIndices == other.m_memberIndices;
}

inline bool WFiberCluster::operator!=( const WFiberCluster& other ) const
{
    return m_memberIndices != other.m_memberIndices;
}

inline const WFiberCluster::IndexList& WFiberCluster::getIndices() const
{
    return m_memberIndices;
}

inline void WFiberCluster::setIndices( const WFiberCluster::IndexList& indices )
{
    m_memberIndices = indices;
}

inline std::ostream& operator<<( std::ostream& os, const WFiberCluster& c )
{
    using string_utils::operator<<;
    return os << c.getIndices();
}

#endif  // WFIBERCLUSTER_H
