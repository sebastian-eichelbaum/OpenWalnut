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
#include "../common/math/WVector3D.h"
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
                      boost::shared_ptr< std::vector< WVector3D > > newGradients,
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
     * Creates a copy (clone) of this instance but allows to change the valueset. Unlike copy construction, this is a very useful function if you
     * want to keep the dynamic type of your dataset even if you just have a WDataSetSingle.
     *
     * \param newValueSet the new valueset.
     *
     * \return the clone
     */
    virtual WDataSetSingle::SPtr clone( boost::shared_ptr< WValueSetBase > newValueSet ) const;

    /**
     * Creates a copy (clone) of this instance but allows to change the grid. Unlike copy construction, this is a very useful function if you
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
    const WVector3D& getGradient( size_t index ) const;

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
    std::vector< WVector3D > const& getOrientations() const;

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
    boost::shared_ptr< std::vector< WVector3D > > m_gradients; //!< Gradients of measurements
    /**
     * Strength (b-value) of the so-called magnetic diffusion gradient.
     */
    double m_diffusionBValue;
};

#endif  // WDATASETRAWHARDI_H
