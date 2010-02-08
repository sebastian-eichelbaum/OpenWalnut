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

#ifndef WMCONNECTOMEVIEW_H
#define WMCONNECTOMEVIEW_H

#include <map>
#include <string>
#include <vector>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Uniform>

#include "../../dataHandler/WDataSetFibers.h"

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleContainer.h"
#include "../../kernel/WModuleInputForwardData.h"
#include "../../kernel/WModuleOutputForwardData.h"

/**
 * This module is able to visualize connectome data in the context of MRI data. It uses the module container class to allow the
 * module to be composed from other modules.
 * \ingroup modules
 */
class WMConnectomeView : public WModuleContainer
{
public:
    /**
     * Standard constructor.
     */
    WMConnectomeView();

    /**
     * Destructor.
     */
    ~WMConnectomeView();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description of module.
     */
    virtual const std::string getDescription() const;

    /**
     * Determine what to do if a property was changed.
     */
    //void slotPropertyChanged( std::string propertyName );
    void slotPropertyChanged();

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

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

private:

    /**
     * Test property
     */
    WPropInt m_test;

    /**
     * The T1 image used as context
     */
    boost::shared_ptr< WModuleInputForwardData< WDataSetSingle > > m_mrtInput;

    /**
     * The fiber dataset used.
     */
    boost::shared_ptr< WModuleInputForwardData< WDataSetFibers > > m_fiberInput;

    /**
     * the current dataset
     */
    boost::shared_ptr< const WDataSetSingle > m_dataSet; //!< pointer to dataSet to be able to access it throughout the whole module.
};

#endif  // WMCONNECTOMEVIEW_H
