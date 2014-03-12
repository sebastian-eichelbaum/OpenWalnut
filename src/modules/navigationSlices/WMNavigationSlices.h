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

#ifndef WMNAVIGATIONSLICES_H
#define WMNAVIGATIONSLICES_H

#include <string>
#include <vector>

#include <osg/Camera>

#include <boost/signals2.hpp>

#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/WGEGroupNode.h"
#include "core/common/math/linearAlgebra/WVectorFixed.h"
#include "core/common/WPropertyTypes.h"
#include "core/graphicsEngine/WPickInfo.h"
#include "core/graphicsEngine/WGECamera.h"

#include "core/kernel/WModule.h"

/**
 * This module provides simple slices that can be color-mapped. It is the default navigation tool for three-dimensional datasets.
 *
 * \ingroup modules
 */
class WMNavigationSlices: public WModule
{
public:
    /**
     * Default constructor.
     */
    WMNavigationSlices();

    /**
     * Destructor.
     */
    virtual ~WMNavigationSlices();

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
     * Initializes the needed geodes, transformations and vertex arrays.
     */
    void initOSG();

    /**
     * Handles picking events.
     */
    class PickCallback
    {
    public:
        /**
         * For the lazy guys.
         */
        typedef boost::shared_ptr< PickCallback > SPtr;

        /**
         * The callback, controlling
         *
         * \param property the property controlling the position of the slice
         * \param node the node getting controlled by this instance.
         * \param negateDirection if true, the translation direction is switched.
         */
        PickCallback( osg::ref_ptr< osg::Node > node, WPropDouble property, bool negateDirection = false );

    private:
        /**
         * Pick event handler. Uses for dragging the slices
         *
         * \param pickInfo the pick information.
         */
        void pick( WPickInfo pickInfo );

        /**
         * The pick connection for the pick() event handler.
         */
        boost::signals2::scoped_connection m_pickConnection;

        /**
         * The slice node. Used for checking if picked.
         */
        osg::ref_ptr< osg::Node > m_node;

        /**
         * The property controlling the position of the slice
         */
        WPropDouble m_property;

        /**
         * The camera under which the node is placed. Needed for proper back-projection.
         */
        osg::ref_ptr< WGECamera > m_camera;

        /**
         * If true, the item is still picked.
         */
        bool m_isPicked;

        /**
         * This uniform is used to inform the shader about current pick-state
         */
        osg::ref_ptr< osg::Uniform > m_pickUniform;

        /**
         * The stored pixel position of a previous drag event.
         */
        WVector2d m_oldPixelPosition;

        /**
         * The transformation direction. This is needed since the OSG coord system has a mirrored coronal direction.
         */
        float m_dir;
    };

    /**
     * X-Slice pick callback.
     */
    PickCallback::SPtr m_xSlicePicker;

    /**
     * Y-Slice pick callback.
     */
    PickCallback::SPtr m_ySlicePicker;

    /**
     * Z-Slice pick callback.
     */
    PickCallback::SPtr m_zSlicePicker;

    /**
     * The Geode containing all the slices and the mesh
     */
    osg::ref_ptr< WGEManagedGroupNode > m_output;

    /**
     * The geode with the slice only for the nav widgets
     */
    osg::ref_ptr< WGEGroupNode > m_axialOutput;

    /**
     * The geode with the slice only for the nav widgets
     */
    osg::ref_ptr< WGEGroupNode > m_coronalOutput;

    /**
     * The geode with the slice only for the nav widgets
     */
    osg::ref_ptr< WGEGroupNode > m_sagittalOutput;

    WPropBool    m_noTransparency;  //!< if true, the whole slices are shown.

    WPropGroup    m_sliceGroup; //!< the group contains several slice properties

    WPropDouble    m_xPos; //!< x position of the slice

    WPropDouble    m_yPos; //!< y position of the slice

    WPropDouble    m_zPos; //!< z position of the slice

    WPropBool     m_showonX; //!< indicates whether the vector should be shown on slice X

    WPropBool     m_showonY; //!< indicates whether the vector should be shown on slice Y

    WPropBool     m_showonZ; //!< indicates whether the vector should be shown on slice Z

    /**
     * If true, the nav slices get positioned in the middle of the first dataset.
     */
    bool m_first;
};

#endif  // WMNAVIGATIONSLICES_H

