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

#ifndef WMDATASETMANIPULATOR_H
#define WMDATASETMANIPULATOR_H

#include <string>

#include <osg/Geode>

#include "core/graphicsEngine/WROISphere.h"

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

/**
 * Module allowing manipulation of the dataset properties via manipulators in the 3D scene.
 *
 * \ingroup modules
 */
class WMDatasetManipulator: public WModule
{
public:

    /**
     *
     */
    WMDatasetManipulator();

    /**
     *
     */
    virtual ~WMDatasetManipulator();

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
     * \return the icon.
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
     * initializes the manipulator and other settings
     */
    void init();

    /**
     * Removes the knobs and cleans up.
     */
    void shutdown();

    /**
     * reacts to movements of the manipulator knobs
     */
    void manipulatorMoved();

    /**
     * reacts to movements of the manipulator knobs
     */
    void manipulatorRotMoved();

    /**
     * function move manipulators to new positions when they are affected by other operations, like translate
     * or rotate
     */
    void adjustManipulatorPositions();

    /**
     * locks the manipulators depending on resize or rotation mode
     */
    void setManipulatorMode();

    /**
     * sets the manipulator positions unsing the bounding box
     */
    void setManipulatorsFromBoundingBox();

    /**
     * Notify the module of an update of the transform.
     */
    void notifyChanged();

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * True if the manipulator spheres should be shown
     */
    WPropBool m_showManipulators;

    /**
     * Toggles between resize and rotation mode
     */
    WPropBool m_rotationMode;

    boost::shared_ptr< boost::function< void() > > m_changeRotRoiSignal; //!< Signal that can be used to update the rotation manipulator
    boost::shared_ptr< boost::function< void() > > m_changeRoiSignal; //!< Signal that can be used to update the translation manipulator

    osg::ref_ptr<WROISphere> m_knobCenter; //!< stores pointer to the center manipulator
    osg::ref_ptr<WROISphere> m_knobx1; //!< stores pointer to manipulator 1
    osg::ref_ptr<WROISphere> m_knobx2; //!< stores pointer to manipulator 2
    osg::ref_ptr<WROISphere> m_knoby1; //!< stores pointer to manipulator 1
    osg::ref_ptr<WROISphere> m_knoby2; //!< stores pointer to manipulator 2
    osg::ref_ptr<WROISphere> m_knobz1; //!< stores pointer to manipulator 1
    osg::ref_ptr<WROISphere> m_knobz2; //!< stores pointer to manipulator 2

    WPosition m_posCenter; //!< stores the old position of the manipulator
    WPosition m_posx1; //!< stores the old position of the manipulator
    WPosition m_posx2; //!< stores the old position of the manipulator
    WPosition m_posy1; //!< stores the old position of the manipulator
    WPosition m_posy2; //!< stores the old position of the manipulator
    WPosition m_posz1; //!< stores the old position of the manipulator
    WPosition m_posz2; //!< stores the old position of the manipulator

    WPosition m_posCenterOrig; //!< stores the old position of the manipulator
    WPosition m_posx1Orig; //!< stores the old position of the manipulator
    WPosition m_posx2Orig; //!< stores the old position of the manipulator
    WPosition m_posy1Orig; //!< stores the old position of the manipulator
    WPosition m_posy2Orig; //!< stores the old position of the manipulator
    WPosition m_posz1Orig; //!< stores the old position of the manipulator
    WPosition m_posz2Orig; //!< stores the old position of the manipulator

    osg::ref_ptr<WROISphere> m_knobRotCenter; //!< stores pointer to the center manipulator
    osg::ref_ptr<WROISphere> m_knobRot; //!< stores pointer to manipulator 1

    WPosition m_posRotCenter; //!< stores the old position of the manipulator
    WPosition m_posRot; //!< stores the old position of the manipulator

    WPosition m_posRotCenterOrig; //!< stores the old position of the manipulator
    WPosition m_posRotOrig; //!< stores the old position of the manipulator

    /**
     * An input connector that accepts order 1 datasets.
     */
    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_input;

    /**
     * An output connector for the output scalar dsataset.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetSingle > > m_output;

    /**
     * This is a pointer to the dataset the module is currently working on.
     */
    boost::shared_ptr< WDataSetSingle > m_dataSet;

    /**
     * stores a pointer to the grid we use;
     */
    boost::shared_ptr< WGridRegular3D > m_grid;

    /**
     * grouping the texture manipulation properties
     */
    WPropGroup    m_groupTexManip;

    /**
     * translation of the texture
     */
    WPropInt m_translationX;

    /**
     * translation of the texture
     */
    WPropInt m_translationY;

    /**
     * translation of the texture
     */
    WPropInt m_translationZ;

    /**
     * voxel size in x direction
     */
    WPropDouble m_stretchX;

    /**
     * voxel size in y direction
     */
    WPropDouble m_stretchY;

    /**
     * voxel size in z direction
     */
    WPropDouble m_stretchZ;

    /**
     * rotation around the x axis
     */
    WPropInt m_rotationX;

    /**
     * rotation around the y axis
     */
    WPropInt m_rotationY;

    /**
     * rotation around the z axis
     */
    WPropInt m_rotationZ;

    //! The grid's transformation.
    boost::shared_ptr< WGridTransformOrtho > m_transform;

    //! Whether the output connector should be updated.
    bool m_updated;

    //! A condition that gets notified when any changes were made to the transformation.
    boost::shared_ptr< WCondition > m_transformChangedCondition;

    //! A mutex for changes to members of this module.
    boost::mutex m_updateMutex;
};

#endif  // WMDATASETMANIPULATOR_H
