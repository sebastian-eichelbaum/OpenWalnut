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
     */
    WDataSetRawHARDI( boost::shared_ptr< WValueSetBase > newValueSet,
                      boost::shared_ptr< WGrid > newGrid,
                      boost::shared_ptr< std::vector< wmath::WVector3D > > newGradients );

    /**
     * Construct an empty and unusable instance. This is needed for the prototype mechanism.
     */
    WDataSetRawHARDI();

    /**
     * Destroys this DataSet instance
     */
    virtual ~WDataSetRawHARDI();

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
    const wmath::WVector3D& getGradient( size_t index ) const;

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

protected:

    /**
     * The prototype as singleton.
     */
    static boost::shared_ptr< WPrototyped > m_prototype;

private:
    boost::shared_ptr< std::vector< wmath::WVector3D > > m_gradients; //!< Gradients of measurements
};

#endif  // WDATASETRAWHARDI_H
