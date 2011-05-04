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

#ifndef WDATASETVECTOR_H
#define WDATASETVECTOR_H

#include "WDataSetSingle.h"
#include "WExportDataHandler.h"

/**
 * This data set type contains vectors as values.
 * \ingroup dataHandler
 */
class OWDATAHANDLER_EXPORT WDataSetVector : public WDataSetSingle // NOLINT
{
public:

    /**
     * Constructs an instance out of an appropriate value set and a grid.
     *
     * \param newValueSet the vector value set to use
     * \param newGrid the grid which maps world space to the value set
     */
    WDataSetVector( boost::shared_ptr< WValueSetBase > newValueSet,
                    boost::shared_ptr< WGrid > newGrid );

    /**
     * Construct an empty and unusable instance. This is needed for the prototype mechanism.
     */
    WDataSetVector();

    /**
     * Destroys this DataSet instance
     */
    virtual ~WDataSetVector();

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
     * Interpolates the vector field at the given position
     *
     * \param pos position to interpolate
     * \param success if the position was inside the grid
     *
     * \return Vector beeing the interpolate.
     */
    WVector3d interpolate( const WPosition &pos, bool *success ) const;

    /**
     * Interpolates the very same way as \ref interpolate but it assures that all vecs are aligned to point into the same
     * half-space.  This is useful for eigenvector fields, where -v, and v both are eigenvectors.
     *
     * \param pos Position to interpolate a vector for
     * \param success return parameter which is true if pos was inside of the grid, false otherwise.
     *
     * \return The resulting interpolated vector.
     */
    WVector3d eigenVectorInterpolate( const WPosition &pos, bool *success ) const;

    /**
     * Get the vector on the given position in value set.
     * \note currently only implmented for WVector3d
     *
     * \param index the position where to get the vector from
     *
     * \return the vector
     */
    WVector3d getVectorAt( size_t index ) const;

    /**
     * Determines whether this dataset can be used as a texture.
     *
     * \return true if usable as texture.
     */
    virtual bool isTexture() const;

    /**
     * Overwrites the isVectorDataSet check.
     *
     * \return Non empty reference to the dataset if it is a vector dataset, empty if not.
     */
    boost::shared_ptr< WDataSetVector > isVectorDataSet();

protected:

    /**
     * The prototype as singleton.
     */
    static boost::shared_ptr< WPrototyped > m_prototype;

private:
};

inline boost::shared_ptr< WDataSetVector > WDataSetVector::isVectorDataSet()
{
    return boost::shared_static_cast< WDataSetVector >( shared_from_this() );
}

#endif  // WDATASETVECTOR_H
