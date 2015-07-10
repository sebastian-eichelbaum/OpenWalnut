#ifndef WMEDUPICKING_H
#define WMEDUPICKING_H

#include <string>

#include "core/kernel/WModule.h"

//Debug Switch
//#define WMEDU_PICKING_DEBUG

//Module Defines
#define WMEDU_PICKING_MAX_INT			"Picking - Maximum Intensity"
#define WMEDU_PICKING_FIRST_HIT			"Picking - First Hit"
#define WMEDU_PICKING_THRESHOLD			"Picking - Threshold"
#define WMEDU_PICKING_MOST_CONTRIBUTING	"Picking - Most Contributing"
#define WMEDU_PICKING_WYSIWYP			"Picking - WYSIWYP"


// forward declarations to reduce compile dependencies
template< class T > class WModuleInputData;
class WDataSetScalar;
class WGEManagedGroupNode;


/**
 * Computes contour lines (aka isolines) for the given data and render them on a 2D plane.
 * \ingroup modules
 */
class WMEDUPicking: public WModule
{
public:
    /**
     * Creates the module for drawing contour lines.
     */
	WMEDUPicking();

    /**
     * Destroys this module.
     */
    virtual ~WMEDUPicking();

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

    /**
     * Initialize requirements for this module.
     */
    virtual void requirements();

private:
    /**
     * Handles picking and calculates ray start/end-position
     */
    void pickHandler( WPickInfo pickInfo );

    /**
     * Input connector for scalar data.
     */
    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_scalarIC;

    /**
     * The transfer function as an input data set
     */
    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_transferFunction;

    /**
     * Output connector for Triangle Grid
     */
    boost::shared_ptr< WModuleOutputData< WTriangleMesh > > m_gridOutput;

    /**
     * The OSG root node for this module. All other geodes or OSG nodes will be attached on this single node.
     */
    osg::ref_ptr< WGEManagedGroupNode > m_rootNode;

    /**
     * The geometry rendered by this module.
     */
    osg::ref_ptr< osg::Geode > m_geode;

    /**
     * Needed for recreating the geometry, incase when resolution changes.
     */
    boost::shared_ptr< WCondition > m_propCondition;

	/**
     * We want some nice shading effects, so we need a nice shader.
     */
    WGEShader::RefPtr m_shader;

    /**
     * Trigger Property, will be triggered by the pickhandler
     */
    WPropBool m_triggerCalculation;

    /**
     * Color Property:
     */
    WPropColor m_color;

    /**
     * Int Property: sampel Rate
     */
    WPropInt m_sampleRate;

    /**
     * Double Property: Alpha Value and alpha Percentage
     */
    WPropDouble m_alphaThreshold;
	
	/**
     * Double Property: Picking Crosshair Thinkness
     */
    WPropDouble m_crossThickness;
	
	/**
     * Double Property: Picking Crosshair Size
     */
    WPropDouble m_crossSize;
	
	/**
     * Current Method
     */
    WPropSelection m_pickingCritereaCur;

    /**
     * Possible Criterion
     */
	boost::shared_ptr< WItemSelection > m_pickingCritereaList;

	/**
	 * Intersection Positions
	 */
	osg::Vec3f m_posStart;
	osg::Vec3f m_posEnd;

	/**
	 * Boolean to check if the intersected positions are valid
	 */
	bool m_bIntersected;
};

#endif  // WMEDUPICKING_H
