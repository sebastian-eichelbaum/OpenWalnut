//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2015-2017 A. Betz, D. Gerlicher, OpenWalnut Community
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

#ifndef WMPICKINGDVR_H
#define WMPICKINGDVR_H

#include <string>
#include <utility>
#include <vector>

#include "core/kernel/WModule.h"

#include "WVisiTrace.h"

// forward declarations to reduce compile dependencies
template< class T > class WModuleInputData;
class WDataSetScalar;
class WGEManagedGroupNode;

/**
 * Allows to select a 3D position in a DVR by selecting a 2D location on the screen.
 * \ingroup modules
 */
class WMPickingDVR: public WModule
{
public:
    /**
     * Creates the module for drawing contour lines.
     */
    WMPickingDVR();

    /**
     * Destroys this module.
     */
    virtual ~WMPickingDVR();

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
     * Handles picking and calculates ray start/end-position
     *
     * \param pickInfo class summarizing all important facts about the pick
     */
    void pickHandler( WPickInfo pickInfo );

    /**
     * Shows and hide relevant properties according to chosen picking mode.
     *
     * \param pickingMode The type of picking criterion used
     */
    void updateModuleGUI( std::string pickingMode );



    /**
     * Calculates which interval is the most visible according
     * to WYSIWYP (http://dx.doi.org/10.1109/TVCG.2012.292)
     *
     * \param vecAlphaAcc The accumulated alpha values at the samples along the viewing ray
     *
     * \result lower and upper bounds of the determined interval
     */
    std::pair<int, int> getWYSIWYPBounds( const std::vector<double>& vecAlphaAcc );

    /**
     * Calculates the object intervals according
     * to WYSIWYP (http://dx.doi.org/10.1109/TVCG.2012.292)
     *
     * \param vecAlphaAcc The accumulated alpha values at the samples along the viewing ray
     * \param vecIndicesLowerBounds Returns the lower bounds of all intervals.
     *
     * \result lower and upper bounds of the most visible interval
     */
    std::pair<int, int> calculateIntervalsWYSIWYP( const std::vector<double>& vecAlphaAcc,
                                                   std::vector<int>& vecIndicesLowerBounds );

    /**
     * Set the new position of the graphical representation
     * of the  picked position.
     *
     * \param position The position where the indicator should appear.
     */
    void updatePositionIndicator( osg::Vec3f position );


    /**
     * Samples the scalar data set along the viewing ray.
     *
     * \return vector of scalar values at sample positions
     */
    std::vector< std::pair< double, WPosition > > sampleIntensityAlongRay();

    /**
     * Evaluate transfer function.
     *
     * \param scalar Evaluate transfer function for this value.
     *
     * \returnOpacity obtained from transfer function.
     */
    double getTFAlpha( const double scalar ) const;

    /**
     * Perform front to back compositing.
     *
     * \param accAlpha Opacity accumulated until this step.
     * \param currentAlpha Opacity obtained from data value at this step.
     *
     * \return New accumulated opacity.
     */
    double compositingStep( const double accAlpha, double currentAlpha );

    /**
     * Compute the position picked in the DVR
     *
     * \param pickingMode Which mode for picking is chosen.
     * \param pickingSuccess Variable will be set to whether a position in the DVR has been determined or not.
     *
     * \return The picked position.
     */
    WPosition getPickedDVRPosition( std::string pickingMode, bool* pickingSuccess );

    /**
     * Create the line rendering of the VisiTrace line.
     */
    void updateCurveRendering();

    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_scalarIC; //!< Input connector for scalar data.

    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_transferFunction; //!< The transfer function as an input data set

    osg::ref_ptr< WGEManagedGroupNode > m_rootNode; //!< All other geodes or OSG nodes of this module will be attached on this node.

    osg::ref_ptr< osg::Geode > m_geode; //!< The geometry rendered by this module.

    boost::shared_ptr< WCondition > m_propCondition; //!< Needed for recreating the geometry, incase when resolution changes.

    WPropBool m_triggerCalculation; //!< Trigger Property, will be triggered by the pickhandle

    WPropBool m_continuousDrawing; //!< Should line be shown during drawing acrion?

    WPropSelection m_selectionType; //!< Property determining what type of structure will be selected

    boost::shared_ptr< WItemSelection > m_selectionTypesList; //!< List of selection types for \ref m_selectionType

    WPropColor m_lineColor; //!< Color of line/s indicating selected position/s

    WPropInt m_sampleSteps; //!< Number of samples along the ray

    /**
     * If active, the opacity of the classified fragment gets scaled according to sample count to ensure relative opacities even if sampling
     * number changes (\ref m_sampleSteps)
     */
    WPropBool m_opacityCorrectionEnabled;

    WPropDouble m_alphaThreshold; //!< Alpha value and alpha percentage

    WPropDouble m_lineThickness; //!< Thinkness of line/s indicating selected position/s

    WPropDouble m_crossSize; //!< Picking crosshair length of lines

    WPropSelection m_pickingCriteriaCur; //!< Current picking method

    boost::shared_ptr< WItemSelection > m_pickingCriteriaList; //!< Possible picking criteria for \ref m_pickingCriteriaCur

    WPropSelection m_wysiwypPositionType; //!< Selecting the type of position obtained by WYSIWYP

    boost::shared_ptr< WItemSelection > m_wysiwypPositionTypesList; //!< Possible position types for WYSIWYP

    osg::Vec3f m_posStart; //!< Intersection position start

    osg::Vec3f m_posEnd; //!< Intersection position end

    bool m_intersected; //!< Indicates if intersected positions are valid

    bool m_pickInProgress; //!< Indicates whether continouos picking is still in progress.

    std::vector<WPosition> m_curve3D; //!< Positions representing the from VisiTrace selection mode.

    WVisiTrace m_visiTrace; //!< Class providing VisiTrace optimization.
};

#endif  // WMPICKINGDVR_H
