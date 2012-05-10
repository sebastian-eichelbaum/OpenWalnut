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

#ifndef WDATASETDIPOLES_H
#define WDATASETDIPOLES_H

#include <vector>

#include <boost/shared_ptr.hpp>

#include "WDataSet.h"

/**
 * Represents a dipole dataset.
 */
class WDataSetDipoles : public WDataSet
{
public:
    /**
     * Creates a new dipole dataset.
     */
    WDataSetDipoles();

    /**
     * Creates a new dipole data set containing one dipole
     * with the given information and checks consistency of the information.
     *
     * \param dipPos Spatial location of the dipole
     * \param mags Magnitudes of dipole over time
     * \param times Times for the dipole activity
     */
    explicit WDataSetDipoles( WPosition dipPos, std::vector<float> mags, std::vector<float> times );

    /**
     * Destructs this dataset.
     */
    ~WDataSetDipoles();

    /**
     * Returns a prototype instantiated with the true type of the deriving class.
     *
     * \return the prototype.
     */
    static boost::shared_ptr< WPrototyped > getPrototype();

    /**
     * Adds a new dipole with given information and checks consistency of the information.
     *
     * \param dipPos Spatial location of the dipole
     * \param mags Magnitudes of dipole over time
     * \param times Times for the dipole activity
     *
     * \return Id of the added dipole.
     */
    size_t addDipole( WPosition dipPos, std::vector<float> mags, std::vector<float> times );

    /**
     * Return position of dipole.
     *
     * \param dipoleId Id number of dipole
     * \return Position of the dipole.
     */
    WPosition getPosition( size_t dipoleId = 0 );

    /**
     * Return magnitude of dipole for a given time.
     *
     * \param time The selected time.
     * \param dipoleId Id number of dipole
     * \return Magnitude of the dipole.
     */
    float getMagnitude( float time, size_t dipoleId = 0 );

    /**
     * Return number of dipoles in this dataset
     *
     * \return number of dipoles in this dataset.
     */
    size_t getNumberOfDipoles();

protected:
    /**
     * The prototype as singleton.
     */
    static boost::shared_ptr< WPrototyped > m_prototype;

private:
    /**
     * Internal class representing one dipole
     */
    class Dipole
    {
    public:
        WPosition m_dipolePosition; //!< The location of the dipole
        std::vector<float> m_magnitudes; //!< The magnitude of the dipole
        std::vector<float> m_times; //!< Times for the different magnitudes
    };

    std::vector< Dipole > m_dipoles; //!< List of dipoles representeing this dipoles dataset
};

#endif  // WDATASETDIPOLES_H
