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

#ifndef WDATASETMULTIPLE_H
#define WDATASETMULTIPLE_H

#include "../common/WPrototyped.h"
#include "WDataSet.h"

/**
 * TODO(math): Document this!
 * \ingroup dataHandler
 */
class WDataSetMultiple : public WDataSet
{
public:
    /**
     * Constructs a new empty dataset. The constructed instance is not usable.
     */
    WDataSetMultiple();

    /**
     * Gets the name of this prototype.
     *
     * \return the name.
     */
    virtual std::string getName() const;

    /**
     * Gets the description for this prototype.
     *
     * \return the description
     */
    virtual std::string getDescription() const;

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

#endif  // WDATASETMULTIPLE_H
