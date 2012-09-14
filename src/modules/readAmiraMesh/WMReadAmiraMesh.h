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

#ifndef WMREADAMIRAMESH_H
#define WMREADAMIRAMESH_H

#include <string>

#include <osg/Geode>

#include "core/dataHandler/WDataSetFibers.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

/**
 * Reader for amiraMesh (.am) files.
 *
 * \ingroup modules
 */
class WMReadAmiraMesh: public WModule
{
/**
 * Only UnitTests may be friends.
 */
friend class WMReadAmiraMeshTest;

public:

    /**
     * Standard constructor
     */
    WMReadAmiraMesh();

    /**
     * Destructor for cleaning up resources.
     */
    virtual ~WMReadAmiraMesh();

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
     * Very prelimiary and specialized way of reading a spatial graph from an amira mesh
     */
    bool readAmiraMesh( std::string fileName );

    void prepareResult();

    WPropFilename m_dataFile; //!< The data will be read from this file.
    boost::shared_ptr< WCondition > m_propCondition;  //!< A condition used to notify about changes in several properties.

    std::vector< WPosition > m_vertices; //!< vertices of the spatial graph
    std::vector< std::pair< size_t, size_t > > m_edges; //!< Edge connectivity of the spatial graph.
    std::vector< size_t > m_numEdgePoints; //!< Number "points" per edge.
    std::vector< WPosition > m_edgePoints; //!< The positions of the points building the edges between the vertices.

    /**
     * The only output of this data module.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetFibers > > m_output;
    boost::shared_ptr< WDataSetFibers > m_graph;
};

#endif  // WMREADAMIRAMESH_H
