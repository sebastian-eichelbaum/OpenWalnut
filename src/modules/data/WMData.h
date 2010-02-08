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

#ifndef WMDATA_H
#define WMDATA_H

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "../../kernel/WKernel.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleConnector.h"
#include "../../kernel/WModuleOutputData.h"

#include "../../dataHandler/WDataSet.h"
#include "../../dataHandler/WDataSetSingle.h"

/**
 * Module for encapsulating WDataSets. It can encapsulate almost everything, but is intended to be used with WDataSets and its
 * inherited classes. This class builds a "source" in OpenWalnut's DataFlow Network.
 * \ingroup modules
 */
class WMData: public WModule
{
public:

    /**
     * Default constructor.
     */
    WMData();

    /**
     * Destructor.
     */
    virtual ~WMData();

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
     * Getter for the dataset.
     *
     * \return the dataset encapsulated by this module.
     */
    virtual boost::shared_ptr< WDataSet > getDataSet();

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

protected:
    /**
     * \par Description
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Initialize connectors in this function. This function must not be called multiple times for one module instance.
     * The module container manages calling those functions -> so just implement it.
     */
    virtual void connectors();

    /**
     * Initialize properties in this function. This function must not be called multiple times for one module instance.
     * The module container manages calling those functions -> so just implement it. Once initialized the number and type
     * of all properties should be set.
     */
    virtual void properties();

    /**
     * Gets called whenever a connector gets connected to the specified input.
     *
     * \param here the connector of THIS module that got connected to "there"
     * \param there the connector that has been connected with the connector "here" of this module.
     */
    virtual void notifyConnectionEstablished( boost::shared_ptr< WModuleConnector > here,
                                              boost::shared_ptr< WModuleConnector > there );
    /**
     * Gets called whenever a connection between a remote and local connector gets closed.
     *
     * \param here the connector of THIS module getting disconnected.
     * \param there the connector of the other module getting disconnected.
     */
    virtual void notifyConnectionClosed( boost::shared_ptr< WModuleConnector > here, boost::shared_ptr< WModuleConnector > there );

    /**
     * Gets called when the module should quit. This is from WThreadedRunner.
     */
    virtual void notifyStop();

    /**
     * The filename to load.
     */
    WPropFilename m_filename;

    /**
     * The name of the dataset. Usually the filename.
     */
    WPropString m_dataName;

    /**
     * Interpolation?
     */
    WPropBool m_interpolation;

    /**
     * Threshold value for this data.
     */
    WPropInt m_threshold;

    /**
     * Opacity value for this data.
     */
    WPropInt m_opacity;

    /**
     * Called whenever a property changes.
     *
     * \param property the property that has been changed
     */
    void propertyChanged( boost::shared_ptr< WPropertyBase > property );

private:

    /**
     * The associated dataset.
     */
    boost::shared_ptr< WDataSet > m_dataSet;

    /**
     * The only output of this data module.
     */
    boost::shared_ptr< WModuleOutputData< WDataSet > > m_output;
};

#endif  // WMDATA_H

