//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#ifndef WMREADRAWDATA_H
#define WMREADRAWDATA_H

#include <string>
#include <vector>

#include <osg/Geode>

#include "core/dataHandler/WDataSetScalar.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

/**
 * A module to read scalar data stored as array of raw data.
 *
 * \ingroup modules
 */
class WMReadRawData: public WModule
{
public:

    /**
     * Constructor of module.
     */
    WMReadRawData();

    /**
     * Destructor of module.
     */
    virtual ~WMReadRawData();

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
     * Get the icon for this module in XPM format.
     *
     * \return The icon.
     */
    virtual const char** getXPMIcon() const;

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
     * Initialize requirements for this module.
     */
    virtual void requirements();


private:
    /**
     * Perform the reading from file an interpretation
     *
     * \param fileName Location of the data file.
     *
     * \return The read data as scalar data set.
     */
    boost::shared_ptr< WDataSetScalar > readData( std::string fileName );

    /**
     * Helper function to read data of the given data type.
     *
     * \param fileName Location of the data file.
     *
     * \return A vector containing the scalar data values.
     */
    template< class T >  boost::shared_ptr< std::vector< T > > readDataTyped( std::string fileName );

    boost::shared_ptr< WCondition > m_propCondition;  //!< A condition used to notify about changes in several properties.
    WPropFilename m_dataFile; //!< The data will be read from this file.
    WPropInt m_X; //!< Samples in X direction
    WPropInt m_Y; //!< Samples in Y direction
    WPropInt m_Z; //!< Samples in Z direction

    boost::shared_ptr< WItemSelection > m_dataTypeSelectionsList; //!< A list of file type selection types
    WPropSelection m_dataTypeSelection; //!< Selection property for file types

    boost::shared_ptr< WDataSetScalar > m_dataSet; //!< This data set is provided as output through the connector.
    boost::shared_ptr< WModuleOutputData< WDataSetScalar > > m_output;  //!< Output connector provided by this module.
};



template< class T >
boost::shared_ptr< std::vector< T > > WMReadRawData::readDataTyped( std::string fileName )
{
    size_t numX = m_X->get();
    size_t numY = m_Y->get();
    size_t numZ = m_Z->get();
    size_t numVoxels = numX * numY * numZ;
    boost::shared_ptr< std::vector< T > > values;
    values = boost::shared_ptr< std::vector< T > >( new std::vector< T >( numVoxels ) );


    std::ifstream ifs;
    ifs.open( fileName.c_str(), std::ifstream::in|std::ios::binary );
    if( !ifs || ifs.bad() )
    {
        WLogger::getLogger()->addLogMessage( "Try load broken file '" + fileName + "'", "Read Raw Data", LL_ERROR );
        throw std::runtime_error( "Problem during reading file. Probably file not found." );
    }

    T *pointData = new T[ numVoxels ];
    ifs.read( reinterpret_cast< char* >( pointData ), sizeof( T ) * numVoxels );

    //T *pointDataAsT = reinterpret_cast< T* >( pointData );

    for( size_t voxelId = 0; voxelId < numVoxels; ++voxelId )
    {
        (*values)[voxelId] = pointData[voxelId];
    }
    ifs.close();

    delete pointData;

    return values;
}

#endif  // WMREADRAWDATA_H
