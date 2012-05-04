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

#ifndef WDATASETDIPOLE_H
#define WDATASETDIPOLE_H

#include <boost/shared_ptr.hpp>

#include "WDataSet.h"


/**
 * Represents a dipole dataset.
 */
class WDataSetDipole : public WDataSet
{
public:
    /**
     * Creates a new dipole dataset.
     */
    WDataSetDipole();

    /**
     * Creates a new dipole with given information and checks consistency of the information.
     *
     * \param dipPos Spatial location of the dipole
     * \param mags Magnitudes of dipole over time
     * \param times Times for the dipole activity
     */
    explicit WDataSetDipole( WPosition dipPos, std::vector<float> mags, std::vector<float> times );

    /**
     * Destructs this dataset.
     */
    ~WDataSetDipole();

    /**
     * Returns a prototype instantiated with the true type of the deriving class.
     *
     * \return the prototype.
     */
    static boost::shared_ptr< WPrototyped > getPrototype();

    /**
     * Return position of dipole.
     *
     * \return Position of the dipole.
     */
    WPosition getPosition();

    /**
     * Return magnitude of dipole for a given time.
     *
     * \param time The selected time.
     * \return Magnitude of the dipole.
     */
    float getMagnitude( float time );

protected:
    /**
     * The prototype as singleton.
     */
    static boost::shared_ptr< WPrototyped > m_prototype;

private:
    WPosition m_dipolePosition; //!< The location of the dipole
    std::vector<float> m_magnitudes; //!< The magnitude of the dipole
    std::vector<float> m_times; //!< Times for the different magnitudes
};

#endif  // WDATASETDIPOLE_H