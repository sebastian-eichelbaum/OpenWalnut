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

#ifndef WMFIBERCULLING_H
#define WMFIBERCULLING_H

#include <string>

#include <boost/shared_ptr.hpp>

#include <osg/Geode>

#include "../../dataHandler/WDataSetFibers.h"
#include "../../kernel/WModule.h"
#include "../../math/WFiber.h"

/**
 * Test module for culling fibers
 */
class WMFiberCulling : public WModule
{
friend class WMFiberCullingTest;
public:
    /**
     * Constructs new FiberTestModule
     */
    WMFiberCulling();

    /**
     * Destructs this FiberTestModule
     */
    virtual ~WMFiberCulling();

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

protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void threadMain();

    /**
     * Detect and removes fibers that have a short distance in terms of the
     * dSt metric and are below the threshold given via the member
     * m_dSt_culling_t.
     *
     * \param fibers Fiber dataset, which should be processed.
     */
    virtual void cullOutFibers( boost::shared_ptr< WDataSetFibers > fibers );

    /**
     * Proximity threshold, which defines the minimum distance which should be
     * considered in the calculation of the mean-minimum-distance of two fibers.
     */
    double m_proximity_t;

    double m_dSt_culling_t; //!< Threshold to cull out short fibers along long fibers.

    bool m_saveCulledCurves; //!< If true, remaining fibers are saved to a file.

    std::string m_savePath; //!< Path where remaining fibers should be stored

private:
};

inline const std::string WMFiberCulling::getName() const
{
    return std::string( "FiberCulling" );
}

inline const std::string WMFiberCulling::getDescription() const
{
    return std::string( "Removes or culls out fibers from a WDataSetFibers" );
}

#endif  // WMFIBERCULLING_H
