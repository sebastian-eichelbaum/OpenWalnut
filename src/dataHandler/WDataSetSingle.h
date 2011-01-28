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

#ifndef WDATASETSINGLE_H
#define WDATASETSINGLE_H

#include <string>

#include <osg/ref_ptr>

#include <boost/shared_ptr.hpp>

#include "WDataSet.h"
#include "WGrid.h"
#include "WGridRegular3D.h"
#include "WValueSet.h"

#include "WExportDataHandler.h"

class WDataTexture3D;
class WDataTexture3D_2;

/**
 * A data set consisting of a set of values based on a grid.
 * \ingroup dataHandler
 */
class OWDATAHANDLER_EXPORT WDataSetSingle : public WDataSet // NOLINT
{
public:
    /**
     * Constructs an instance out of a value set and a grid.
     *
     * \param newValueSet the value set to use
     * \param newGrid the grid which maps world space to the value set
     */
    WDataSetSingle( boost::shared_ptr< WValueSetBase > newValueSet,
                    boost::shared_ptr< WGrid > newGrid );

    /**
     * Construct an empty and unusable instance. This is useful for prototypes.
     */
    WDataSetSingle();

    /**
     * Destroys this DataSet instance
     */
    virtual ~WDataSetSingle();

    /**
     * \return Reference to its WValueSet
     */
    boost::shared_ptr< WValueSetBase > getValueSet() const;

    /**
     * \return Reference to its WGrid
     */
    boost::shared_ptr< WGrid > getGrid() const;

    /**
     * Get the value stored at position of the value set. This is the grid position only for scalar data sets.
     *
     * \param id The id'th value in the data set
     *
     * \return Scalar value for that given position
     */
    template< typename T > T getValueAt( size_t id );

    /**
     *  Get the value stored at position of the value set. This is the grid position only for scalar data sets.
     *
     * \param id The id'th value in the data set
     *
     * \return Scalar value for that given position
     */
    double getValueAt( size_t id ) const;

    /**
     * Determines whether this dataset can be used as a texture.
     *
     * \return true if usable as texture.
     */
    virtual bool isTexture() const;

    /**
     * Returns the texture- representation of the dataset. May throw an exception if no texture is available.
     *
     * \return The texture.
     */
    virtual boost::shared_ptr< WDataTexture3D > getTexture();

    /**
     * Returns the texture representation of the dataset. May throw an exception if no texture is available.
     *
     * \return the texture.
     */
    virtual osg::ref_ptr< WDataTexture3D_2 > getTexture2() const;

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

    /**
     * Stores the reference of the WGrid of this DataSetSingle instance.
     */
    boost::shared_ptr< WGrid > m_grid;

    /**
     * Stores the reference of the WValueSet of this DataSetSingle instance.
     */
    boost::shared_ptr< WValueSetBase > m_valueSet;

private:
    /**
     * The 3D texture representing this dataset.
     */
    boost::shared_ptr< WDataTexture3D > m_texture3D;

    /**
     * The 3D texture representing this dataset.
     */
    osg::ref_ptr< WDataTexture3D_2 > m_texture;
};

template< typename T > T WDataSetSingle::getValueAt( size_t id )
{
    boost::shared_ptr< WValueSet< T > > vs = boost::shared_dynamic_cast< WValueSet< T > >( m_valueSet );
    return vs->getScalar( id );
}
#endif  // WDATASETSINGLE_H
