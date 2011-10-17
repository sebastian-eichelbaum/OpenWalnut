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

#ifndef WDATASETSPHERICALHARMONICS_H
#define WDATASETSPHERICALHARMONICS_H

#include <string>
#include <vector>

#include "../common/math/WSymmetricSphericalHarmonic.h"

#include "WValueSet.h"

#include "WDataSetSingle.h"
#include "WExportDataHandler.h"

/**
 * This data set type contains spherical harmonic coefficients as values. The index scheme is like in the Descoteaux paper "Regularized, Fast, and Robust Analytical Q-Ball Imaging".
 * \ingroup dataHandler
 */
class OWDATAHANDLER_EXPORT WDataSetSphericalHarmonics : public WDataSetSingle // NOLINT
{
public:

    /**
     * Constructs an instance out of an appropriate value set and a grid.
     *
     * \param newValueSet the value set with the spherical harmonics coefficients to use
     * \param newGrid the grid which maps world space to the value set
     */
    WDataSetSphericalHarmonics( boost::shared_ptr< WValueSetBase > newValueSet,
                                boost::shared_ptr< WGrid > newGrid );

    /**
     * Construct an empty and unusable instance. This is needed for the prototype mechanism.
     */
    WDataSetSphericalHarmonics();

    /**
     * Destroys this DataSet instance
     */
    virtual ~WDataSetSphericalHarmonics();

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
     * Interpolates the field of spherical harmonics at the given position
     *
     * \param pos position to interpolate
     * \param success if the position was inside the grid
     *
     * \return Interpolated spherical harmonic.
     */
    WSymmetricSphericalHarmonic interpolate( const WPosition &pos, bool *success ) const;

    /**
     * Get the spherical harmonic on the given position in value set.
     *
     * \param index the position where to get the spherical harmonic from
     *
     * \return the spherical harmonic
     */
    WSymmetricSphericalHarmonic getSphericalHarmonicAt( size_t index ) const;

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
     * Determines whether this dataset can be used as a texture.
     *
     * \return true if usable as texture.
     */
    virtual bool isTexture() const;

protected:

    /**
     * The prototype as singleton.
     */
    static boost::shared_ptr< WPrototyped > m_prototype;

private:
    /**
     * The regular 3d grid of the data set.
     */
    boost::shared_ptr< WGridRegular3D > m_gridRegular3D;

    /**
     * The valueset of the data set
     */
    boost::shared_ptr< WValueSetBase > m_valueSet;
};

#endif  // WDATASETSPHERICALHARMONICS_H
