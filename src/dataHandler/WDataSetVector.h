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

/**
 * This data set type contains vectors as values.
 * \ingroup dataHandler
 */
class WDataSetVector : public WDataSetSingle
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
};

#endif  // WDATASETVECTOR_H
