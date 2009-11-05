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

#ifndef WFIBERCULLING_H
#define WFIBERCULLING_H

#include <string>

#include <boost/shared_ptr.hpp>

#include <osg/Geode>

#include "../../dataHandler/WDataSetFibers.h"
#include "../../kernel/WModule.h"
#include "../../math/WFiber.h"

/**
 * Test module for culling fibers
 */
class WFiberCulling : public WModule
{
friend class WFiberCullingTest;
public:
    /**
     * Constructs new FiberTestModule
     */
    WFiberCulling();

    /**
     * Destructs this FiberTestModule
     */
    virtual ~WFiberCulling();

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

protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void threadMain();

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

#endif  // WFIBERCULLING_H
