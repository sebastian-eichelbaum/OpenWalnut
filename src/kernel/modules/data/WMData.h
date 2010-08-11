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

#include "../../WKernel.h"
#include "../../WModule.h"
#include "../../WModuleConnector.h"
#include "../../WModuleOutputData.h"

#include "../../../dataHandler/WDataSet.h"
#include "../../../dataHandler/WDataSetSingle.h"

#include "../../WExportKernel.h"

/**
 * Module for encapsulating WDataSets. It can encapsulate almost everything, but is intended to be used with WDataSets and its
 * inherited classes. This class builds a "source" in OpenWalnut's DataFlow Network.
 * \ingroup modules
 */
class OWKERNEL_EXPORT WMData: public WModule
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

    /**
     * Get the icon for this module in XPM format.
     */
    virtual const char** getXPMIcon() const;

    /**
     * Gets the type of the module. This is useful for FAST differentiation between several modules like standard modules and data
     * modules which play a special role in OpenWalnut/Kernel.
     *
     * \return the Type. This will return MODULE_DATA.
     */
    virtual MODULE_TYPE getType() const;

    /**
     * Sets the filename of the file to load. If this method is called multiple times it has no effect. It has to be called right after
     * construction BEFORE running the data module.
     *
     * \note The reason for using this method to set the filename instead of a property is, that a property gets set AFTER ready(), but this (and
     * only this module) needs it before ready got called.
     *
     * \param fname the name of the file
     */
    virtual void setFilename( boost::filesystem::path fname );

    /**
     * Gets the path of the file that has been loaded. It always is the value which has been set during the FIRST call of setFilename.
     *
     * \return the path of the file that has been loaded.
     */
    virtual boost::filesystem::path getFilename() const;

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
     * The filename of the dataset to load.
     */
    boost::filesystem::path m_fileName;

    /**
     * true if the m_fileName has been set earlier.
     */
    bool m_fileNameSet;

    /**
     * The name of the dataset. Usually the filename.
     */
    WPropString m_dataName;

    /**
     * grouping the texture display properties
     */
    WPropGroup    m_groupTex;

    /**
     * grouping the texture manipulation properties
     */
    WPropGroup    m_groupTexManip;

    /**
     * Interpolation?
     */
    WPropBool m_interpolation;

    /**
     * A list of color map selection types
     */
    boost::shared_ptr< WItemSelection > m_colorMapSelectionsList;

    /**
     * Selection property for color map
     */
    WPropSelection m_colorMapSelection;

    /**
     * Threshold value for this data.
     */
    WPropDouble m_threshold;

    /**
     * Opacity value for this data.
     */
    WPropInt m_opacity;

    /**
     * translation of the texture
     */
    WPropInt m_translationX;

    /**
     * translation of the texture
     */
    WPropInt m_translationY;

    /**
     * translation of the texture
     */
    WPropInt m_translationZ;

    /**
     * voxel size in x direction
     */
    WPropDouble m_stretchX;

    /**
     * voxel size in y direction
     */
    WPropDouble m_stretchY;

    /**
     * voxel size in z direction
     */
    WPropDouble m_stretchZ;

    /**
     * rotation around the x axis
     */
    WPropInt m_rotationX;

    /**
     * rotation around the y axis
     */
    WPropInt m_rotationY;

    /**
     * rotation around the z axis
     */
    WPropInt m_rotationZ;

    /**
     * A list of color map selection types
     */
    boost::shared_ptr< WItemSelection > m_matrixSelectionsList;

    /**
     * Selection property for color map
     */
    WPropSelection m_matrixSelection;


    bool m_isTexture; //!< Indicates whether the loaded dataSet will be available as texture.

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

