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

#include "core/kernel/WDataModule.h"

// forward declarations
class WDataSet;
class WDataSetSingle;
class WModuleConnector;
template< class T > class WModuleOutputData;

/**
 * Module for encapsulating WDataSets. It can encapsulate almost everything, but is intended to be used with WDataSets and its
 * inherited classes. This class builds a "source" in OpenWalnut's DataFlow Network.
 * \ingroup modules
 */
class WMData: public WDataModule
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
     * \return The icon.
     */
    virtual const char** getXPMIcon() const;

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
     * The basic type of data used in this data set (e.g. float, double, ...)
     */
    WPropString m_dataType;

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

    // in case of a nifti file, there may be several transforms specified in the file
    //! a standard transform (should be an identity transform)
    WMatrix< double > m_transformNoMatrix;

    //! a standard transform (should be an identity transform)
    WMatrix< double > m_transformSForm;

    //! a standard transform (should be an identity transform)
    WMatrix< double > m_transformQForm;

private:
    //! returns info property group with the three availabe transformation matrixes
    boost::shared_ptr< WProperties > getTransformationProperties() const;

    //! a condition for the matrix selection
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * Get a string for the datatype of the given dataset.
     *
     * \param dss the data set whose name should be returned.
     *
     * \return the type name of the specified dataset
     */
    std::string getDataTypeString( boost::shared_ptr< WDataSetSingle > dss );

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

