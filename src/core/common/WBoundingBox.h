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

#include <ostream>
#include <iomanip> // for setprecision
#include <cmath>   // std::sqrt

#include <osg/BoundingBox>

#include "exceptions/WInvalidBoundingBox.h"
//#include "math/linearAlgebra/WLinearAlgebra.h"

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
     * Copy construct using a given bounding box
     *
     * \param bb the source bb
     */
    WBoundingBoxImpl( const WBoundingBoxImpl& bb );

    /**
     * Create BoundinmgBox using a given sphere.
     *
     * \param bs the sphere
     */
    WBoundingBoxImpl( const osg::BoundingSphereImpl< VT >& bs );

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
     *
     * \return squared bbox radius
     */
    value_type radiusSquare() const;

    using osg::BoundingBoxImpl< VT >::corner;

    /**
     * Explicit type conversion function to use a WBoundingBox as osg::BoundingBox.
     *
     * \return A copy of this bounding box as osg::BoundingBox.
     */
    osg::BoundingBox toOSGBB() const;

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

    /**
     * Gives the front lower left aka minimum corner.
     *
     * \return Minimum corner.
     */
    const vec_type& getMin() const;

    /**
     * Gives the back upper right aka maximum corner.
     *
     * \return Maximum corner.
     */
    const vec_type& getMax() const;

protected:
private:
    /**
     * Checks if the two given intervals intersect and computes the distance between them.
     *
     * \param a0 lower bound of the first interval
     * \param a1 upper bound of the first interval
     * \param b0 lower bound of the second interval
     * \param b1 upper bound if the second interval
     *
     * \return The distance between those intervals if they don't overlap, zero otherwise
     */
    double intervalDistance( double a0, double a1, double b0, double b1 ) const;
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
inline WBoundingBoxImpl< VT >::WBoundingBoxImpl( const osg::BoundingSphereImpl< VT >& bs )
    : osg::BoundingBoxImpl< VT >( bs.center() - VT( bs.radius(), bs.radius(), bs.radius() ) ,
                                  bs.center() + VT( bs.radius(), bs.radius(), bs.radius() ) )
{
}

template< class VT >
inline WBoundingBoxImpl< VT >::WBoundingBoxImpl( const WBoundingBoxImpl< VT >& bb )
    : osg::BoundingBoxImpl< VT >( bb )
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
inline osg::BoundingBox WBoundingBoxImpl< VT >::toOSGBB() const
{
    return osg::BoundingBox( osg::BoundingBoxImpl< VT >::_min, osg::BoundingBoxImpl< VT >::_max );
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
inline double WBoundingBoxImpl< VT >::intervalDistance( double a0, double a1, double b0, double b1 ) const
{
    if( a1 < b0 )
    {
        return b0 - a1;
    }
    else if( b1 < a0 )
    {
        return a0 - b1;
    }
    return 0.0;
}

template< class VT >
inline typename WBoundingBoxImpl< VT >::value_type WBoundingBoxImpl< VT >::minDistance( const WBoundingBoxImpl< VT > &bb ) const
{
    // test if they are valid
    if( !valid() || !bb.valid() )
    {
        throw WInvalidBoundingBox( "One of the both bounding boxes inside minDistance computation is not valid." );
    }

    double dx = intervalDistance( xMin(), xMax(), bb.xMin(), bb.xMax() );
    double dy = intervalDistance( yMin(), yMax(), bb.yMin(), bb.yMax() );
    double dz = intervalDistance( zMin(), zMax(), bb.zMin(), bb.zMax() );
    if( dx == 0.0 && dy == 0.0 && dz == 0.0 )
    {
        return 0.0;
    }
    return std::sqrt( dx * dx + dy * dy + dz * dz );
}

/**
 * Output operator for the WBoundingBoxImpl class.
 *
 * \param out Output stream operator
 * \param bb The box which should be streamed out
 *
 * \return reference to the output stream
 */
template< class VT >
inline std::ostream& operator<<( std::ostream& out, const WBoundingBoxImpl< VT >& bb )
{
    out << std::scientific << std::setprecision( 16 );
    out << "AABB( min: " << bb.xMin() << ", " << bb.yMin() << ", " << bb.zMin();
    out << " max: " << bb.xMax() << ", " << bb.yMax() << ", " << bb.zMax() << " )";
    return out;
}

template< class VT >
inline const typename WBoundingBoxImpl< VT >::vec_type& WBoundingBoxImpl< VT >::getMin() const
{
    return  osg::BoundingBoxImpl< VT >::_min;
}

template< class VT >
inline const typename WBoundingBoxImpl< VT >::vec_type& WBoundingBoxImpl< VT >::getMax() const
{
    return  osg::BoundingBoxImpl< VT >::_max;
}

typedef WBoundingBoxImpl< osg::Vec3 > WBoundingBox;

#endif  // WBOUNDINGBOX_H
