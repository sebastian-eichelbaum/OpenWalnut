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

#ifndef WDATAMODULE_H
#define WDATAMODULE_H

#include <boost/shared_ptr.hpp>

#include "WModule.h"

/**
 * Base for all data loader modules. This currently is only a prototype to move WMData out of the core. Later, it will provide a whole interface
 * to handle arbitrary data/multi-file data and other complex things.
 */
class WDataModule: public WModule
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WDataModule >.
     */
    typedef boost::shared_ptr< WDataModule > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WDataModule >.
     */
    typedef boost::shared_ptr< const WDataModule > ConstSPtr;

    /**
     * Default constructor.
     */
    WDataModule();

    /**
     * Destructor.
     */
    virtual ~WDataModule();

    /**
     * Gets the type of the module. This is useful for FAST differentiation between several modules like standard modules and data
     * modules which play a special role in OpenWalnut/Kernel.
     *
     * \return the Type. If you do not overwrite this method, it will return MODULE_ARBITRARY.
     */
    virtual MODULE_TYPE getType() const;

    /**
     * Getter for the dataset.
     *
     * \return the dataset encapsulated by this module.
     */
    virtual boost::shared_ptr< WDataSet > getDataSet() = 0;

    /**
     * Sets the filename of the file to load. If this method is called multiple times it has no effect. It has to be called right after
     * construction BEFORE running the data module.
     *
     * \note The reason for using this method to set the filename instead of a property is, that a property gets set AFTER ready(), but this (and
     * only this module) needs it before ready got called.
     *
     * \param fname the name of the file
     */
    virtual void setFilename( boost::filesystem::path fname ) = 0;

    /**
     * Gets the path of the file that has been loaded. It always is the value which has been set during the FIRST call of setFilename.
     *
     * \return the path of the file that has been loaded.
     */
    virtual boost::filesystem::path getFilename() const = 0;

protected:
private:
};

#endif  // WDATAMODULE_H

