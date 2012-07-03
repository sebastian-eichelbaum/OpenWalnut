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
     * \param firstTimeStep First time where the magnitude is not 0
     * \param lastTimeStep Last time where the magnitude is not 0
     */
    explicit WDataSetDipoles( WPosition dipPos, std::vector<float> mags, std::vector<float> times,
                              size_t firstTimeStep, size_t lastTimeStep );

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
     * \param firstTimeStep First time where the magnitude is not 0
     * \param lastTimeStep Last time where the magnitude is not 0
     *
     * \return Id of the added dipole.
     */
    size_t addDipole( WPosition dipPos, std::vector<float> mags, std::vector<float> times,
                      size_t firstTimeStep, size_t lastTimeStep );

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
     * Return first time where the magnitude is not 0.
     *
     * \param dipoleId Id number of dipole
     * \return First time where the magnitude is not 0.
     */
    float getStartTime( size_t dipoleId = 0u ) const;

    /**
     * Return last time where the magnitude is not 0.
     *
     * \param dipoleId Id number of dipole
     * \return Last time where the magnitude is not 0.
     */
    float getEndTime( size_t dipoleId = 0u ) const;

    /**
     * Return the times where the magnitude is not 0.
     *
     * \param dipoleId Id number of dipole
     * \return Times where the magnitude is not 0.
     */
    std::vector<float> getTimes( size_t dipoleId = 0u ) const;

    /**
     * Return the magnitudes where the magnitude is not 0.
     *
     * \param dipoleId Id number of dipole
     * \return Magnitudes where the magnitude is not 0.
     */
    std::vector<float> getMagnitudes( size_t dipoleId = 0u ) const;

    /**
     * Return number of dipoles in this dataset
     *
     * \return number of dipoles in this dataset.
     */
    size_t getNumberOfDipoles();

    /**
     * Return the biggest magnitude of all dipoles.
     *
     * \return Biggest magnitude of all dipoles.
     */
    float getMaxMagnitude() const;

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
        size_t m_firstTimeStep; //!< First time where the magnitude is not 0
        size_t m_lastTimeStep; //!< Last time where the magnitude is not 0
    };

    std::vector< Dipole > m_dipoles; //!< List of dipoles representeing this dipoles dataset
    float m_maxMagnitude; //!< Biggest magnitude of all dipoles
};

#endif  // WDATASETDIPOLES_H
