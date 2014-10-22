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

#ifndef WMREADLAS_H
#define WMREADLAS_H

#include <string>
#include <vector>

#include "core/dataHandler/WDataSetPoints.h"

#include "core/kernel/WDataModule.h"
#include "core/kernel/WModuleOutputData.h"

/**
 * This module loads LAS files (point data).
 *
 * \ingroup modules
 */
class WMReadLAS: public WDataModule
{
public:
    /**
     * Default constructor.
     */
    WMReadLAS();

    /**
     * Destructor.
     */
    virtual ~WMReadLAS();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const;

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Define a list of file filters we support.
     *
     * \return the list of filters
     */
    virtual std::vector< WDataModuleInputFilter::ConstSPtr > getInputFilter() const;
protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();

    /**
     * Load data.
     */
    virtual void load();

    /**
     * Handle a newly set input. Implement this method to load the newly set input. You can get the input using the \ref getInput and \ref getInputAs
     * methods. Please remember that it is possible to get a NULL pointer here.
     * This happens when the user explicitly sets no input. In this case, you should clean up and reset your output connectors.
     *
     * \note it is very important to NOT load the data inside of this method. It is usually called in the GUI thread. This would block the whole GUI.
     * Instead, use this method for firing a condition, which then wakes your module thread.
     */
    virtual void handleInputChange();
private:
    /**
     * The output connector for the filtered data.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetPoints > > m_output;

    /**
     * True if the load function needs to be called. Usually set by handleInputChange or the reload trigger
     */
    bool m_reload;
};

#endif  // WMREADLAS_H
