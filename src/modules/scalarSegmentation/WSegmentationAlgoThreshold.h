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

#ifndef WSEGMENTATIONALGOTHRESHOLD_H
#define WSEGMENTATIONALGOTHRESHOLD_H

#include <string>
#include <vector>

#include "WSegmentationAlgo.h"

/**
 * A very simple threshold segmentation working in two modi: If in LOWER_THRESHOLD mode than voxels
 * that have a value smaller than the threshold are set to 0, while the rest of the voxels are set
 * to 1 where as in UPPER_THRESHOLD mode the opposite applies.
 *
 * \class WSegmentationAlgoThreshold
 */
class WSegmentationAlgoThreshold : public WSegmentationAlgo
{
public:
    /**
     * Standard constructor.
     */
    WSegmentationAlgoThreshold();

    /**
     * Destructor.
     */
    virtual ~WSegmentationAlgoThreshold();

    /**
     * Return the name of this algorithm.
     * \return The name.
     */
    virtual std::string getName();

    /**
     * Return a description of this algorithm.
     * \return A description.
     */
    virtual std::string getDescription();

    /**
     * Checks if any properties were changed.
     * \return True, iff any properties were changed.
     */
    virtual bool propChanged();

    /**
     * Implements the operation.
     *
     * \tparam The type of the values in the dataset's valueset.
     * \param valueset The dataset's valueset.
     * \return The resulting dataset.
     */
    template< typename T >
    DataSetPtr operator() ( WValueSet< T > const* valueset ) const;

private:
    /**
     * Initializes the algorithm's properties.
     */
    virtual void properties();

    /**
     * A virtual function that calls the correct segmentation operation.
     * \return The resulting dataset.
     */
    virtual DataSetPtr applyOperation();

    //! The lower threshold in %.
    WPropDouble m_low_threshold;

    //! The upper threshold in %.
    WPropDouble m_upp_threshold;

    /**
     * Whether the values inside range of thresholds should be keept, or
     * resulting images should contain only 0 or 1 values (binarized).
     */
    WPropBool m_binarize;
};

/**
 * This function call operator can be called by a visitor. It performs
 * the actual segmentation as mentioned in the description of this class
 * \param valueset operator will work on this set of values
 * \return The resulting values after applying the segmentation criterion.
 */
template< typename T >
WSegmentationAlgo::DataSetPtr WSegmentationAlgoThreshold::operator() ( WValueSet< T > const* valueset ) const
{
    boost::shared_ptr< std::vector< T > > values( new std::vector< T >( valueset->size() ) );

    double low_threshold = valueset->getMinimumValue() + m_low_threshold->get( true ) * ( valueset->getMaximumValue() - valueset->getMinimumValue() );
    double upp_threshold = valueset->getMinimumValue() + m_upp_threshold->get( true ) * ( valueset->getMaximumValue() - valueset->getMinimumValue() );

    double val = 0.0;
    if( m_binarize->get( true ) )
    {
        for( std::size_t k = 0; k < valueset->size(); ++k )
        {
            val =  static_cast< double >( valueset->getScalar( k ) );
            ( *values )[k] = static_cast< T >( val < low_threshold || val > upp_threshold ? 0 : 1 );
        }
    }
    else
    {
        for( std::size_t k = 0; k < valueset->size(); ++k )
        {
            val =  static_cast< double >( valueset->getScalar( k ) );
            ( *values )[k] = static_cast< T >( val < low_threshold || val > upp_threshold ? 0 : val );
        }
    }
    boost::shared_ptr< WValueSet< T > > vs( new WValueSet< T >( 0, 1, values, DataType< T >::type ) );
    return DataSetPtr( new WDataSetScalar( vs, m_dataSet->getGrid() ) );
}

#endif  // WSEGMENTATIONALGOTHRESHOLD_H
