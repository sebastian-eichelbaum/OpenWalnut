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

#ifndef WDATASETTIMESERIES_H
#define WDATASETTIMESERIES_H

#include <limits>
#include <string>
#include <utility>
#include <vector>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include "../common/WLimits.h"
#include "../common/WProperties.h"
#include "../common/WTransferable.h"
#include "WDataSetScalar.h"


//! forward declaration
class WDataSetTimeSeriesTest;

/**
 * A dataset that stores a time series.
 *
 * \note Only works for scalar datasets at the moment!
 * \note this is only a temporary solution
 */
class WDataSetTimeSeries : public WDataSet
{
    //! the test is a friend
    friend class WDataSetTimeSeriesTest;

    //! a conveniance typedef
    typedef WDataSetTimeSeries This;

    //! a time slice
    typedef std::pair< boost::shared_ptr< WDataSetScalar const >, float > TimeSlice;

public:
    /**
     * Returns a name.
     *
     * \return A name.
     */
    std::string const getName() const;

    /**
     * Returns a description.
     *
     * \return A description.
     */
    std::string const getDescription() const;

    /**
     * Returns a prototype instantiated with the true type of the deriving class.
     *
     * \return the prototype.
     */
    static boost::shared_ptr< WPrototyped > getPrototype();

    /**
     * Construct time series from multiple 3D datasets. They do not have to be sorted by time.
     *
     * \param datasets A list of datasets to add.
     * \param times A list of times for the datasets.
     */
    WDataSetTimeSeries( std::vector< boost::shared_ptr< WDataSetScalar const > > datasets, std::vector< float > times );

    /**
     * Destructor.
     */
    virtual ~WDataSetTimeSeries();

    /**
     * Get the first point of time in the time series.
     *
     * \return The first point of time.
     */
    inline float getMinTime() const;

    /**
     * Get the last point of time in the time series.
     *
     * \return The last point of time.
     */
    inline float getMaxTime() const;

    /**
     * Check if there exists a predefined dataset at the given point in time, i.e.
     * no interpolation has to be done to access data at that point in time.
     *
     * If the time is not in the represented interval [getMinTime(),getMaxTime()],
     * it will return false.
     *
     * \param time The point in time to test.
     * \return true, iff the time is one of those that were given in the constructor.
     */
    bool isTimeSlice( float time ) const;

    /**
     * Find the nearest time slice for a given time. If there are two
     * nearest time slices, the smaller one will be returned.
     *
     * \param time The time.
     * \return The nearest time slice.
     */
    float findNearestTimeSlice( float time ) const;

    /**
     * Get a pointer to the dataset at a given time or a NULL-pointer, if there
     * was no dataset given for that point in time.
     *
     * \note You need to provide the exact time.
     *
     * \param time The time.
     * \return A pointer to the appropriate dataset or a NULL-pointer.
     */
    boost::shared_ptr< WDataSetScalar const > getDataSetPtrAtTimeSlice( float time ) const;

    /**
     * Calculates a new dataset with values interpolated between the two nearest
     * time slices. if the time is not in the interval [getMinTime(),getMaxTime()],
     * a NULL-pointer will be returned.
     *
     * \param time The time.
     * \param name The name of the new dataset.
     * \return A new interpolated dataset.
     */
    boost::shared_ptr< WDataSetScalar const > calcDataSetAtTime( float time, std::string const& name ) const;

    /**
     * Interpolate a value for a single point in space and time.
     *
     * \param pos The spatial location.
     * \param time The temporal location.
     * \param[out] success A flag indicating if the position was in the dataset.
     * \return The value at the given location.
     */
    template< typename Data_T >
    Data_T interpolate( WVector3d const& pos, float time, bool* success ) const;

    /**
     * Get the smallest value in all datasets.
     *
     * \return The smallest value.
     */
    double getMinValue();

    /**
     * Get the largest value in all datasets.
     *
     * \return The largest value.
     */
    double getMaxValue();

private:
    /**
     * Find the largest time slice position that is smaller than or equal to time,
     * or return -inf, if there is no such time slice.
     *
     * \param time The time.
     * \return The largest time slice that is smaller than or equal to time or -inf.
     */
    float getLBTimeSlice( float time ) const;

    /**
     * Find the smallest time slice position that is larger than time, or return
     * inf, if there is no such time slice.
     *
     * \param time The time.
     * \return The largest time slice that is smaller than time or inf.
     */
    float getUBTimeSlice( float time ) const;

    /**
     * Interpolate a valueset from two neighboring slices.
     *
     * \param lb Time of one slice.
     * \param ub Time of the other slice.
     * \param time The actual time of the interpolated slice.
     * \return A valueset with linearly interpolated values.
     */
    template< typename Data_T >
    boost::shared_ptr< WValueSetBase > calcInterpolatedValueSet( float lb, float ub, float time ) const;

    /**
     * Standard constructor.
     */
    WDataSetTimeSeries();

    /**
     * A compare functor for time slices.
     */
    class TimeSliceCompare
    {
    public:
        /**
         * Compares the time of two time slices.
         *
         * \param t0 The first time slice.
         * \param t1 The other time slice.
         * \return true, iff the time of the first slice is smaller than that of the other one.
         */
        bool operator() ( TimeSlice const& t0, TimeSlice const& t1 );

        /**
         * Compares the time of two time slices.
         *
         * \param t0 The first time slice.
         * \param t1 The other time slice.
         * \return true, iff the time of the first slice is smaller than that of the other one.
         */
        bool operator() ( float const& t0, TimeSlice const& t1 );

        /**
         * Compares the time of two time slices.
         *
         * \param t0 The first time slice.
         * \param t1 The other time slice.
         * \return true, iff the time of the first slice is smaller than that of the other one.
         */
        bool operator() ( TimeSlice const& t0, float const& t1 );
    };

    //! the datasets that compose the time series
    std::vector< TimeSlice > m_dataSets;

    //! The prototype as singleton.
    static boost::shared_ptr< WPrototyped > m_prototype;

    //! the smallest value
    double m_minValue;

    //! the largest value
    double m_maxValue;
};

template< typename Data_T >
Data_T WDataSetTimeSeries::interpolate( WVector3d const& pos, float time, bool* success ) const
{
    static const float inf = std::numeric_limits< float >::infinity();
    WAssert( success, "" );
    WAssert( !wlimits::isNaN( length( pos ) ), "" );
    WAssert( !wlimits::isNaN( time ), "" );
    if( time < getMinTime() || time > getMaxTime() )
    {
        *success = false;
        throw WException( std::string( "The provided time is not in the interval of this time series." ) );
    }
    float lb = getLBTimeSlice( time );
    float ub = getUBTimeSlice( time );
    if( lb == time || ub == time )
    {
        boost::shared_ptr< WDataSetScalar const > ds = getDataSetPtrAtTimeSlice( time );
        return static_cast< Data_T >( const_cast< WDataSetScalar& >( *ds ).interpolate( pos, success ) );
    }
    WAssert( lb != -inf && ub != inf, "" );
    boost::shared_ptr< WDataSetScalar const > f = getDataSetPtrAtTimeSlice( lb );
    boost::shared_ptr< WDataSetScalar const > g = getDataSetPtrAtTimeSlice( ub );
    WAssert( f && g, "" );
    float ml = ( ub - time ) / ( ub - lb );
    float mu = ( time - lb ) / ( ub - lb );
    return static_cast< Data_T >( ml * const_cast< WDataSetScalar& >( *f ).interpolate( pos, success )
                                + mu * const_cast< WDataSetScalar& >( *g ).interpolate( pos, success ) );
}

template< typename Data_T >
boost::shared_ptr< WValueSetBase > WDataSetTimeSeries::calcInterpolatedValueSet( float lb, float ub, float time ) const
{
    static const float inf = std::numeric_limits< float >::infinity();
    WAssert( lb != -inf && ub != inf, "" );
    boost::shared_ptr< WDataSetScalar const > f = getDataSetPtrAtTimeSlice( lb );
    boost::shared_ptr< WDataSetScalar const > g = getDataSetPtrAtTimeSlice( ub );
    WAssert( f && g, "" );
    boost::shared_ptr< WValueSet< Data_T > > vf = boost::dynamic_pointer_cast< WValueSet< Data_T > >( f->getValueSet() );
    boost::shared_ptr< WValueSet< Data_T > > vg = boost::dynamic_pointer_cast< WValueSet< Data_T > >( g->getValueSet() );
    WAssert( vf && vg, "" );
    boost::shared_ptr< std::vector< Data_T > > values = boost::shared_ptr< std::vector< Data_T > >( new std::vector< Data_T >( vf->size() ) );
    float ml = ( ub - time ) / ( ub - lb );
    float mu = ( time - lb ) / ( ub - lb );
    for( std::size_t k = 0; k < values->size(); ++k )
    {
        ( *values )[ k ] = ml * vf->getScalar( k ) + mu * vg->getScalar( k );
    }
    return boost::shared_ptr< WValueSetBase >( new WValueSet< Data_T >( 0, 1, values, DataType< Data_T >::type ) );
}

float WDataSetTimeSeries::getMinTime() const
{
    return m_dataSets.front().second;
}

float WDataSetTimeSeries::getMaxTime() const
{
    return m_dataSets.back().second;
}

#endif  // WDATASETTIMESERIES_H
