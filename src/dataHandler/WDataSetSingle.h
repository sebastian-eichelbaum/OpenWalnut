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

#include <boost/shared_ptr.hpp>

#include "WGrid.h"
#include "WGridRegular3D.h"
#include "WValueSet.hpp"

#include "WDataSet.h"

class WDataTexture3D;

/**
 * A data set consisting of a set of values based on a grid.
 * \ingroup dataHandler
 */
class WDataSetSingle : public WDataSet
{
public:
    /**
     * Constructs an instance out of a value set and a grid.
     */
    WDataSetSingle( boost::shared_ptr< WValueSetBase > newValueSet,
                    boost::shared_ptr< WGrid > newGrid );

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
     * Get the value stored at a certain grid position of the data set
     * \param x index in x direction
     * \param y index in y direction
     * \param z index in z direction
     */
    template < typename T > T getValueAt( int x, int y, int z )
    {
        boost::shared_ptr< WValueSet< T > > vs = boost::shared_dynamic_cast< WValueSet< T > >( m_valueSet );
        boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_grid );

        size_t id = x + y * grid->getNbCoordsX() + z * grid->getNbCoordsX() * grid->getNbCoordsY();

        T v = vs->getScalar( id );
        return v;
    }

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

private:
    /**
     * Stores the reference of the WGrid of this DataSetSingle instance.
     */
    boost::shared_ptr< WGrid > m_grid;

    /**
     * Stores the reference of the WValueSet of this DataSetSingle instance.
     */
    boost::shared_ptr< WValueSetBase > m_valueSet;

    /**
     * The 3D texture representing this dataset.
     */
    boost::shared_ptr< WDataTexture3D > m_texture3D;
};

#endif  // WDATASETSINGLE_H
