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

#ifndef WDATASETSCALAR_H
#define WDATASETSCALAR_H

#include "WDataSetSingle.h"

/**
 * This data set type contains scalars as values.
 * \ingroup dataHandler
 */
class WDataSetScalar : public WDataSetSingle
{
public:

    /**
     * Constructs an instance out of an appropriate value set and a grid.
     * Computes the maximum an minimum values stored as member variables.
     *
     * \param newValueSet the scalar value set to use
     * \param newGrid the grid which maps world space to the value set
     */
    WDataSetScalar( boost::shared_ptr< WValueSetBase > newValueSet,
                    boost::shared_ptr< WGrid > newGrid );

    /**
     * Constructs an instance out of an appropriate value set and a grid.
     * Computes the maximum an minimum values stored as member variables.
     *
     * \param newValueSet the scalar value set to use
     * \param newGrid the grid which maps world space to the value set
     * \param min a priori known smallest scalar value in newValueSet
     * \param max a priori known largest scalar value in newValueSet
     */
    WDataSetScalar( boost::shared_ptr< WValueSetBase > newValueSet,
                    boost::shared_ptr< WGrid > newGrid,
                    double min,
                    double max );

    /**
     * Construct an empty and unusable instance. This is needed for the prototype mechanism.
     */
    WDataSetScalar();

    /**
     * Destroys this DataSet instance
     */
    virtual ~WDataSetScalar();

    /**
     * Returns the largest of the scalars stored in the data set
     */
    double getMax() const;

    /**
     * Returns the smallest of the scalars stored in the data set
     */
    double getMin() const;

    /**
     * Interpolate the value fo the valueset at the given position
     *
     * \param pos The position for wich we would like to get a value.
     *
     * \return Scalar value for that given position
     */
    double interpolate( wmath::WPosition pos );


    /**
     * Returns a prototype instantiated with the true type of the deriving class.
     *
     * \return the prototype.
     */
    static boost::shared_ptr< WPrototyped > getPrototype();

protected:

    /**
     * The prototype as singleton.
     */
    static boost::shared_ptr< WPrototyped > m_prototype;

private:
    double m_maximum; //!< Largest scalar of data set.
    double m_minimum; //!< Smallest scalar of data set.
};

#endif  // WDATASETSCALAR_H
