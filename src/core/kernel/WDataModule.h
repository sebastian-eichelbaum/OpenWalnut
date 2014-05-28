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

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "WDataModuleInput.h"
#include "WDataModuleInputFilter.h"

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
     * Allows suppression of colormap registration in data modules. This can be handy if you use data modules in a container to construct more
     * complex data sets from multiple input files.
     *
     * \note call this before adding and running the module.
     *
     * \param suppress true if suppress
     */
    virtual void setSuppressColormaps( bool suppress = true );

    /**
     * Checks whether suppression of colormaps is active.
     *
     * \return true if colormaps are suppressed.
     */
    bool getSuppressColormaps() const;

    /**
     * Return a list of input filters. This defines what your module can load.
     * You can return an empty vector. This causes the data module to be "un-matched" for all kinds of data.
     *
     * Implement this function and ensure it works with module prototypes (do not rely on any properties, connectors and similar here).
     *
     * \return the list of filters
     */
    virtual std::vector< WDataModuleInputFilter::ConstSPtr > getInputFilter() const = 0;

    /**
     * Set the input of this data module. This is called after construction but before running the module.
     *
     * \param input the input to use for loading.
     */
    virtual void setInput( WDataModuleInput::SPtr input );

    /**
     * Get the currently set input.
     *
     * \return the input
     */
    virtual WDataModuleInput::SPtr getInput() const;

    /**
     * Get the currently set input.
     *
     * \return the input. Null if not set or type mismatch.
     *
     * \tparam InputType get the input in this type
     */
    template< typename InputType >
    boost::shared_ptr< InputType > getInputAs() const;

    /**
     * Sets the filename of the file to load. If this method is called multiple times it has no effect. It has to be called right after
     * construction BEFORE running the data module.
     *
     * \note The reason for using this method to set the filename instead of a property is, that a property gets set AFTER ready(), but this (and
     * only this module) needs it before ready got called.
     *
     * \param fname the name of the file
     *
     * \deprecated do not use this anymore. Use setInput and getInputFilter to define possible data inputs.
     */
    OW_API_DEPRECATED virtual void setFilename( boost::filesystem::path fname );

    /**
     * Gets the path of the file that has been loaded. It always is the value which has been set during the FIRST call of setFilename.
     *
     * \return the path of the file that has been loaded.
     *
     * \deprecated do not use this anymore. Use setInput and getInputFilter to define possible data inputs.
     */
    OW_API_DEPRECATED virtual boost::filesystem::path getFilename() const;

protected:
private:
    /**
     * If true, data modules are instructed to suppress colormap registration.
     */
    bool m_suppressColormaps;

    /**
     * The input this data module should use
     */
    WDataModuleInput::SPtr m_dataModuleInput;
};

template< typename InputType >
boost::shared_ptr< InputType > WDataModule::getInputAs() const
{
    if( getInput() )
    {
        return boost::dynamic_pointer_cast< InputType >( getInput() );
    }
    return boost::shared_ptr< InputType >();
}

#endif  // WDATAMODULE_H

