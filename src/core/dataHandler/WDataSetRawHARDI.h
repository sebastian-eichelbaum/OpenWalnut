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

#ifndef WDATASETRAWHARDI_H
#define WDATASETRAWHARDI_H

#include <string>
#include <vector>

#include "WDataSetSingle.h"
#include "../common/math/linearAlgebra/WLinearAlgebra.h"
#include "WExportDataHandler.h"

/**
 * This data set type contains raw HARDI and its gradients.
 * \ingroup dataHandler
 */
class OWDATAHANDLER_EXPORT WDataSetRawHARDI : public WDataSetSingle // NOLINT
{
public:
    /**
     * Constructs an instance out of:
     *  - an appropriate value set with a vector of measure values for each voxel,
     *  - a grid and
     *  - the gradients used during the measurement of the different values.
     *
     * \param newValueSet the vector value set to use
     * \param newGrid the grid which maps world space to the value set
     * \param newGradients the Gradients of the
     * \param diffusionBValue Strength of the gradient
     */
    WDataSetRawHARDI( boost::shared_ptr< WValueSetBase > newValueSet,
                      boost::shared_ptr< WGrid > newGrid,
                      boost::shared_ptr< std::vector< WVector3d > > newGradients,
                      double diffusionBValue = 1.0 );

    /**
     * Construct an empty and unusable instance. This is needed for the prototype mechanism.
     */
    WDataSetRawHARDI();

    /**
     * Destroys this DataSet instance
     */
    virtual ~WDataSetRawHARDI();

    /**
     * Creates a copy (clone) of this instance but allows one to change the valueset. Unlike copy construction, this is a very useful function if you
     * want to keep the dynamic type of your dataset even if you just have a WDataSetSingle.
     *
     * \param newValueSet the new valueset.
     *
     * \return the clone
     */
    virtual WDataSetSingle::SPtr clone( boost::shared_ptr< WValueSetBase > newValueSet ) const;

    /**
     * Creates a copy (clone) of this instance but allows one to change the grid. Unlike copy construction, this is a very useful function if you
     * want to keep the dynamic type of your dataset even if you just have a WDataSetSingle.
     *
     * \param newGrid the new grid.
     *
     * \return the clone
     */
    virtual WDataSetSingle::SPtr clone( boost::shared_ptr< WGrid > newGrid ) const;

    /**
     * Creates a copy (clone) of this instance. Unlike copy construction, this is a very useful function if you
     * want to keep the dynamic type of your dataset even if you just have a WDataSetSingle.
     *
     * \return the clone
     */
    virtual WDataSetSingle::SPtr clone() const;

    /**
     * Returns a prototype instantiated with the true type of the deriving class.
     *
     * \return the prototype.
     */
    static boost::shared_ptr< WPrototyped > getPrototype();

    /**
     * Returns the gradient for the index.
     *
     * \return gradient of measurement
     *
     * \param index
     */
    const WVector3d& getGradient( size_t index ) const;

    /**
     * Returns the count of measurements per voxel, which is equal to the count of the used gradients.
     *
     * \return measurements per voxel
     */
    std::size_t getNumberOfMeasurements() const;

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
     * Get the orientations.
     *
     * \return A vector of orientations.
     */
    std::vector< WVector3d > const& getOrientations() const;

    /**
     * Get the indexes of zero gradients.
     *
     * \return Returns the indexes for the which gradient is zero.
     */
    std::vector< size_t > const& getZeroGradientIndexes() const;

    /**
     * Get the indexes of non-zero gradients.
     *
     * \return Returns the indexes for the which gradient is non-zero.
     */
    std::vector< size_t > const& getNonZeroGradientIndexes() const;

    /**
     * Returns only the measurements for which the gradient was non-zero.
     *
     * \param index the index of the voxel.
     *
     * \return non-zero gradient signals
     */
    template< typename T > WValue< T > getNonZeroGradientSignals( size_t index ) const;

    /**
     * Returns the \e b-value of the diffusion.
     *
     * \return b-value as double
     */
    double getDiffusionBValue() const;

protected:
    /**
     * The prototype as singleton.
     */
    static boost::shared_ptr< WPrototyped > m_prototype;

private:
    /**
     * Build indexes for the zero and non-zero gradients.
     */
    void buildGradientIndexes();

    boost::shared_ptr< std::vector< WVector3d > > m_gradients; //!< Gradients of measurements
    /**
     * Strength (b-value) of the so-called magnetic diffusion gradient.
     */
    double m_diffusionBValue;

    /**
     * The indexes for the which gradient is zero.
     */
    std::vector< size_t > m_zeroGradientIndexes;

    /**
     * The indexes for the which gradient is non-zero.
     */
    std::vector< size_t > m_nonZeroGradientIndexes;
};

inline std::vector< size_t > const& WDataSetRawHARDI::getZeroGradientIndexes() const
{
    return m_zeroGradientIndexes;
}

inline std::vector< size_t > const& WDataSetRawHARDI::getNonZeroGradientIndexes() const
{
    return m_nonZeroGradientIndexes;
}

template< typename T > WValue< T > WDataSetRawHARDI::getNonZeroGradientSignals( size_t index ) const
{
    WValue< T > result( m_nonZeroGradientIndexes.size() );
    size_t idx = 0;
    boost::shared_ptr< WValueSet< T > > vs = boost::shared_dynamic_cast< WValueSet< T > >( m_valueSet );
    WValue< T > signal( vs->getWValue( index ) );
    for( std::vector< size_t >::const_iterator cit = m_nonZeroGradientIndexes.begin(); cit != m_nonZeroGradientIndexes.end(); ++cit )
    {
        result[ idx ] = signal[ *cit ];
        ++idx;
    }
    return result;
}


#endif  // WDATASETRAWHARDI_H
