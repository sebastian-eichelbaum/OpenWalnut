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

#ifndef WMFIBERCREATOR_H
#define WMFIBERCREATOR_H

#include <string>

#include "../../dataHandler/WDataSetFibers.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleOutputData.h"

/**
 * This module creates fiber datasets using some scheme, like spirals. This is very useful tool for papers where artificial data is needed
 * sometimes to create images showing some certain strength or weakness.
 *
 * \ingroup modules
 */
class WMFiberCreator: public WModule
{
public:

    /**
     * Default constructor.
     */
    WMFiberCreator();

    /**
     * Destructor.
     */
    virtual ~WMFiberCreator();

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
     * \return icon in XPM format
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

private:

    /**
     * The output connector used to provide the calculated data to other modules.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetFibers > > m_fiberOutput;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * Number of fibers.
     */
    WPropInt m_numFibers;

    /**
     * Number of vertices per fiber.
     */
    WPropInt m_numVertsPerFiber;

    /**
     * Fiber color.
     */
    WPropColor m_fibColor;

    /**
     * Creates a crossing fiber bundle.
     *
     * \param numFibers the number of fibers to create
     * \param numVertsPerFiber the number of vertices per fiber
     * \param vertices vertices of the line strips
     * \param fibIdx fibIdx the indices to the vertices
     * \param lengths lengths of each fiber
     * \param fibIdxVertexMap index of fiber for each vertex
     * \param colors colors for each vertex. RGB only.
     */
    void crossing( size_t numFibers, size_t numVertsPerFiber,
            WDataSetFibers::VertexArray vertices,
            WDataSetFibers::IndexArray fibIdx,
            WDataSetFibers::LengthArray lengths,
            WDataSetFibers::IndexArray fibIdxVertexMap,
            WDataSetFibers::ColorArray colors );

    /**
     * Creates a spiral fiber bundle.
     *
     * \param numFibers the number of fibers to create
     * \param numVertsPerFiber the number of vertices per fiber
     * \param vertices vertices of the line strips
     * \param fibIdx fibIdx the indices to the vertices
     * \param lengths lengths of each fiber
     * \param fibIdxVertexMap index of fiber for each vertex
     * \param colors colors for each vertex. RGB only.
     */
    void spiral( size_t numFibers, size_t numVertsPerFiber,
            WDataSetFibers::VertexArray vertices,
            WDataSetFibers::IndexArray fibIdx,
            WDataSetFibers::LengthArray lengths,
            WDataSetFibers::IndexArray fibIdxVertexMap,
            WDataSetFibers::ColorArray colors );
};

#endif  // WMFIBERCREATOR_H

