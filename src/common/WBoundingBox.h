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

#ifndef WBOUNDINGBOX_H
#define WBOUNDINGBOX_H

#include <osg/BoundingBox>

#include "math/WVector3D.h"

/**
 * Represents a \e axis \e parallel bounding box and provides some useful operations with them.
 *
 * \note Reason for subclassing: We don't want \c _min and \c _max member variables to be public.
 * \note Reason for not having a \e private osg::BoundingBox member is, we don't have to wrap many
 * member functions and can make use of the using directive. A downside on this is, we cannot
 * automatical cast to osg::BoundingBox even if we provide a cast operator! Hence when we need this
 * we will provide a toOsgBB() member function.
 */
template< class VT >
class WBoundingBoxImpl : private osg::BoundingBoxImpl< VT >
{
public:
    /**
     * Vertex type for min and max positions of this box.
     */
    typedef typename osg::BoundingBoxImpl< VT >::vec_type vec_type;

    /**
     * Value type of the vertex type for example double, float, etc.
     */
    typedef typename osg::BoundingBoxImpl< VT >::value_type value_type;

    /**
     * Default constructor.
     */
    WBoundingBoxImpl();

    /**
     * Wrapps the component wise bounding box constructor from osg::BoundingBox.
     *
     * \param xmin Minimal x coordinate
     * \param ymin Minimal y coordinate
     * \param zmin Minimal z coordinate
     * \param xmax Maximal x coordinate
     * \param ymax Maximal y coordinate
     * \param zmax Maximal z coordinate
     */
    WBoundingBoxImpl( value_type xmin, value_type ymin, value_type zmin, value_type xmax, value_type ymax, value_type zmax );

    /**
     * Constructs a bounding box by min and max positions.
     *
     * \param min Position containing minx miny and minz coordinates.
     * \param max Position containing maxx maxy and maxz coordinates.
     */
    WBoundingBoxImpl( const vec_type& min, const vec_type& max );

    /**
     * Destructs this instance.
     */
    virtual ~WBoundingBoxImpl();

    /**
     * Resets this box to an initial state where max is FLT_MIN and min FLT_MAX.
     *
     * \note This is a wrapper call to osg::BoundingBoxImpl< VT >::init()
     */
    void reset();

    using osg::BoundingBoxImpl< VT >::valid;
    using osg::BoundingBoxImpl< VT >::set;
    using osg::BoundingBoxImpl< VT >::xMin;
    using osg::BoundingBoxImpl< VT >::yMin;
    using osg::BoundingBoxImpl< VT >::zMin;
    using osg::BoundingBoxImpl< VT >::xMax;
    using osg::BoundingBoxImpl< VT >::yMax;
    using osg::BoundingBoxImpl< VT >::zMax;
    using osg::BoundingBoxImpl< VT >::center;
    using osg::BoundingBoxImpl< VT >::radius;

    /**
     * Calculates and returns the squared length of the bounding box radius.
     *
     * \note This is a wrapper call to osg::BoundingBoxImpl< VT >::radius2()
     */
    value_type radiusSquare() const;

    using osg::BoundingBoxImpl< VT >::corner;

    /**
     * Converts this bounding box to the osg::BoundingBoxImpl< VT >.
     *
     * \return Copy of this casted to the osg::BoundingBoxImpl< VT > type
     */
    osg::BoundingBoxImpl< VT > toOSGBB() const;

    using osg::BoundingBoxImpl< VT >::expandBy;

    /**
     * Expands this bounding box to include the given bounding box.
     *
     * \param bb The other bounding box.
     */
    void expandBy( const WBoundingBoxImpl< VT > &bb );

    /**
     * Checks for intersection of this bounding box with the specified bounding box.
     *
     * \param bb The other bouding box to tetst with.
     *
     * \return True if they intersect, false otherwise.
     */
    bool intersects( const WBoundingBoxImpl< VT > &bb ) const;

    /**
     * Computes the minimal distance of tow axis parallel bounding boxes.
     *
     * \param bb The other bounding box.
     *
     * \return Zero if they intersect, otherwise their minimal distance.
     */
    value_type minDistance( const WBoundingBoxImpl< VT > &bb ) const;

    using osg::BoundingBoxImpl< VT >::contains;

protected:
private:
};

template< class VT >
inline WBoundingBoxImpl< VT >::WBoundingBoxImpl()
    : osg::BoundingBoxImpl< VT >()
{
}

template< class VT >
inline WBoundingBoxImpl< VT >::WBoundingBoxImpl( value_type xmin, value_type ymin, value_type zmin, value_type xmax, value_type ymax, value_type zmax ) // NOLINT line length
    : osg::BoundingBoxImpl< VT >( xmin, ymin, zmin, xmax, ymax, zmax )
{
}

template< class VT >
inline WBoundingBoxImpl< VT >::WBoundingBoxImpl( const vec_type& min, const vec_type& max )
    : osg::BoundingBoxImpl< VT >( min, max )
{
}

template< class VT >
inline WBoundingBoxImpl< VT >::~WBoundingBoxImpl()
{
}

template< class VT >
inline void WBoundingBoxImpl< VT >::reset()
{
    this->init();
}

template< class VT >
inline typename WBoundingBoxImpl< VT >::value_type WBoundingBoxImpl< VT >::radiusSquare() const
{
    return this->raidus2();
}

template< class VT >
inline osg::BoundingBoxImpl< VT > WBoundingBoxImpl< VT >::toOSGBB() const
{
    return *this;
}

template< class VT >
inline void WBoundingBoxImpl< VT >::expandBy( const WBoundingBoxImpl< VT > &bb )
{
    osg::BoundingBoxImpl< VT >::expandBy( bb );
}

template< class VT >
inline bool WBoundingBoxImpl< VT >::intersects( const WBoundingBoxImpl< VT > &bb ) const
{
    return osg::BoundingBoxImpl< VT >::intersects( bb );
}

template< class VT >
inline typename WBoundingBoxImpl< VT >::value_type WBoundingBoxImpl< VT >::minDistance( const WBoundingBoxImpl< VT > &bb ) const
{
// TODO(math): implement the exception used below!
//
//    // test if they are valid
//    if( !valid() || !bb.valid() )
//    {
//        throw WInvalidBoundingBoxes( "One of the both bounding boxes inside minDistance computation is not valid." );
//    }

    //TODO(math): test if they are axis parallel

    return 0.0;
}

typedef WBoundingBoxImpl< wmath::WVector3D > WBoundingBox;

#endif  // WBOUNDINGBOX_H
