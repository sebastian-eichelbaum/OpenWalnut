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

#ifndef WMFIBERDISPLAYSIMPLE_H
#define WMFIBERDISPLAYSIMPLE_H

#include <string>

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

// forward declarations
class WDataSetFibers;
class WDataSetFiberClustering;
class WGEShader;

/**
 * This modules takes a dataset and equalizes its histogram.
 *
 * \ingroup modules
 */
class WMFiberDisplaySimple: public WModule
{
public:
    /**
     * Default constructor.
     */
    WMFiberDisplaySimple();

    /**
     * Destructor.
     */
    virtual ~WMFiberDisplaySimple();

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
     * Geometry update callback. Handles fiber cluster filtering
     *
     * \param geometry the geometry instance to handle
     */
    void geometryUpdate( osg::Drawable* geometry );

    /**
     * If true, the geometryUpdate() callback will upload a new filter attribute array.
     */
    bool m_fiberClusteringUpdate;

    /**
     * The fiber dataset which is going to be filtered.
     */
    boost::shared_ptr< WModuleInputData< WDataSetFibers > > m_fiberInput;

    /**
     * An optional fiber clustering can be specified to filter in m_fiberInput.
     */
    boost::shared_ptr< WModuleInputData< WDataSetFiberClustering > > m_fiberClusteringInput;

    /**
     * The current fiber data
     */
    WDataSetFibers::SPtr m_fibers;

    /**
     * The current fiber clustering
     */
    WDataSetFiberClustering::SPtr m_fiberClustering;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * The properties of the fiber dataset.
     */
    WProperties::SPtr m_fibProps;

    /**
     * The shader used for actually drawing the fake tubes or lines.
     */
    osg::ref_ptr< WGEShader > m_shader;

    /**
     * The shader used for drawing end cap sprites if in tube mode.
     */
    osg::ref_ptr< WGEShader > m_endCapShader;

    /**
     * Illumination.
     */
    WPropBool m_illuminationEnable;

    /**
     * The ratio between colormap and fiber color.
     */
    WPropDouble m_colormapRatio;

    /**
     * True if colormapping should be used.
     */
    WPropBool m_colormapEnabled;

    /**
     * A property group for all the clipping related props.
     */
    WPropGroup m_clipPlaneGroup;

    /**
     * Property for en-/disable clipping.
     */
    WPropBool m_clipPlaneEnabled;

    /**
     * Property for en-/disabling of the clip plane plane.
     */
    WPropBool m_clipPlaneShowPlane;

    /**
     * Point on the plane. Defines the plane.
     */
    WPropPosition m_clipPlanePoint;

    /**
     * Vector of the plane. Defines the plane.
     */
    WPropPosition m_clipPlaneVector;

    /**
     * Distance from plane. Used as threshold.
     */
    WPropDouble m_clipPlaneDistance;

    /**
     * Prop denoting whether to use tubes or line strips
     */
    WPropBool m_tubeEnable;

    /**
     * Property denoting whether to use end-caps on tubes
     */
    WPropBool m_tubeEndCapsEnable;

    /**
     * Prop denoting whether tubes can be zoomed or not.
     */
    WPropBool m_tubeZoomable;

    /**
     * Creates a ribbon-like appearance.
     */
    WPropBool m_tubeRibbon;

    /**
     * The size. The meaning somehow relates to tubeZoomable. If a tube is zoomable, the size is the smallest size in pixels on screen of totally
     * zoomed out.
     */
    WPropDouble m_tubeSize;

    /**
     * Group containing tube specific properties
     */
    WPropGroup m_tubeGroup;

    /**
     * Group containing line specific properties
     */
    WPropGroup m_lineGroup;

    /**
     * Line width.
     */
    WPropDouble m_lineWidth;

    /**
     * Line smoothing.
     */
    WPropBool m_lineSmooth;

    /**
     * Update the transform node to provide an cue were the plane actually is.
     *
     * \param node the transform node
     */
    void clipPlaneCallback( osg::Node* node ) const;

    /**
     * Creates the clip plane with corresponding callbacks.
     *
     * \return the clip plane node
     */
    osg::ref_ptr< osg::Node > createClipPlane() const;

    /**
     * Creates a geode containing the fiber geometry
     *
     * \param fibers the fiber data
     * \param fibGeode the geode with the fibers as tube strip or lines
     * \param endCapGeode the end cap sprites. Not used if not in tube mode.
     */
    void createFiberGeode( boost::shared_ptr< WDataSetFibers > fibers, osg::ref_ptr< osg::Geode > fibGeode,
                                                                                         osg::ref_ptr< osg::Geode > endCapGeode );

    /**
     * The plane node.
     */
    osg::ref_ptr< osg::Node > m_plane;

    /**
     * Callback for the line geode to allow interactive modification of line smooth and width states.
     *
     * \param state the state
     */
    void lineGeodeStateCallback( osg::StateSet* state );
};

#endif  // WMFIBERDISPLAYSIMPLE_H

