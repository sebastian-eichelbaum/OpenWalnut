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

#ifndef WMFIBERSTIPPLES_H
#define WMFIBERSTIPPLES_H

#include <string>

#include "core/kernel/WModule.h"

// forward declarations to reduce compile dependencies
template< class T > class WModuleInputData;
class WDataSetScalar;
class WDataSetVector;
class WGEManagedGroupNode;

/**
 * Draws Fiber Stipples on slice in order to visualize probabilistic tractograms.
 *
 * \note For further information see http://dx.doi.org/10.1109/BioVis.2011.6094044.
 *
 * \ingroup modules
 */
class WMFiberStipples: public WModule
{
public:
    /**
     * Default constructor.
     */
    WMFiberStipples();

    /**
     * Destructor.
     */
    virtual ~WMFiberStipples();

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

private:
    /**
     * Initialize OSG root node for this module. All other nodes from this module should be attached to this root node.
     *
     * \return Pointer to the root node.
     */
    osg::ref_ptr< WGEManagedGroupNode > initOSG();

    /**
     * The probabilistic tractogram input connector.
     */
    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_probIC;

    /**
     * Input connector for the largest eigen vector dataset.
     */
    boost::shared_ptr< WModuleInputData< WDataSetVector > > m_vectorIC;

    /**
     * The OSG root node for this module. All other geodes or OSG nodes will be attached on this single node.
     */
    osg::ref_ptr< WGEManagedGroupNode > m_output;
};

#endif  // WMFIBERSTIPPLES_H
