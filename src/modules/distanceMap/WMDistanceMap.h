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

#ifndef WMDISTANCEMAP_H
#define WMDISTANCEMAP_H

#include <string>

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

#include "core/dataHandler/WDataSetScalar.h"

/**
 * Computes a distance map from an anatomy dataset.
 * \ingroup modules
 */
class WMDistanceMap : public WModule
{
/**
 * Only UnitTests may be friends.
 */
friend class WMDistanceMapTest;

public:
    /**
     * Standard constructor.
     */
    WMDistanceMap();

    /**
     * Destructor.
     */
    ~WMDistanceMap();

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

    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_input;  //!< Input connector required by this module.

    /**
     * Source dataset.
     */
    boost::shared_ptr< WDataSetScalar > m_dataSet;

    /**
     * Connector to provide the distance map to other modules.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetScalar > > m_output;

    /**
     * Target dataset.
     */
    boost::shared_ptr< WDataSetScalar > m_distanceMapDataSet;

    /**
     * Function to create a distance map from Anatomy data set.
     * Take from FiberNavigator.
     * \param dataSet the data set that is used to compute the distance field.
     * The distance is computed to the boundary between foreground an background
     *
     * \return the distance map values
     */
    boost::shared_ptr< WValueSet< float > > createOffset( boost::shared_ptr< const WDataSetScalar > dataSet );

    /**
     * Gauss function.
     *
     * \param x position of evaluation
     * \param sigma standard deviation
     *
     * \return Gauss value for given parameters
     */
    double xxgauss( double x, double sigma );
};

#endif  // WMDISTANCEMAP_H
