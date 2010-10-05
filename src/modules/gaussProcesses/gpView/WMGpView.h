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

#ifndef WMGPVIEW_H
#define WMGPVIEW_H

#include <string>

#include <osg/Geode>

#include "../../../kernel/WModule.h"
#include "../../../kernel/WModuleInputData.h"
#include "../../../graphicsEngine/WGEManagedGroupNode.h"

// TODO(math): be sure to change this include when this dataSet lives in dataHandler again.
#include "../detTract2GPConvert/WDataSetGP.h"

/**
 * This module is intended for Gaussian Process display. A Slice is generated which is used to cut
 * the volume and depict the mean function color coded onto its surface.
 * \ingroup modules
 */
class WMGpView: public WModule
{
public:

    /**
     * Constructs a new GP viewer.
     */
    WMGpView();

    /**
     * Destructs this viewer.
     */
    virtual ~WMGpView();

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

    // virtual osg::ref_ptr< osg::Geode > generateSlice() const;

private:
//    /**
//     * Input connector for the gaussian proccesses.
//     */
//    boost::shared_ptr< WModuleInputData< WDataSetGP > > m_gpIC;

    /**
     * The root node used for this modules graphics. For OSG nodes, always use osg::ref_ptr to ensure proper resource management.
     */
    osg::ref_ptr< WGEManagedGroupNode > m_rootNode;

    /**
     * The base point of the plane.
     */
    WPropPosition m_pos;

    /**
     * The normal of the plane
     */
    WPropPosition m_normal;
};

#endif  // WMGPVIEW_H
