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

#ifndef WMTEEMGLYPHS_H
#define WMTEEMGLYPHS_H

#include <string>

#include <osg/Geode>

#include "core/common/WItemSelection.h"
#include "core/common/WItemSelector.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WDataSetSphericalHarmonics.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

/**
 * Spherical harmonics glyphs using teem (http://teem.sourceforge.net/).
 * According to http://www.ci.uchicago.edu/~schultz/sphinx/home-glyph.htm
 * Thanks to Thomas Schultz.
 *
 * \warning Not yet in useful state.
 * \ingroup modules
 */
class WMTeemGlyphs: public WModule
{
public:

    /**
     * Nothing special with this constructor.
     */
    WMTeemGlyphs();

    /**
     * Nothing special with this constructor.
     */
    virtual ~WMTeemGlyphs();

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

    /**
     * Handle an exception that was thrown by the threaded function in any worker thread.
     *
     * \param e The exception that was thrown.
     */
    void handleException( WException const& e );

private:
    /**
     * An input connector that accepts spherical harmonics datasets.
     */
    boost::shared_ptr< WModuleInputData< WDataSetSphericalHarmonics > > m_input;

    /**
     * This condition denotes whether we need to recompute the surface
     */
    boost::shared_ptr< WCondition > m_recompute;

    /**
     * Just a preliminary function to avoid polluting moduleMain()
     */
    void execute();

    /**
     * Gets signaled from the properties object when something was changed. Now, only m_active is used. This method therefore simply
     * activates/deactivates the glyphs.
     */
    void activate();

    /**
     * Renders all glyphs for the given slice
     * \param sliceId The number of the slice to be rendered.
     */
    void renderSlice( size_t sliceId );

    boost::shared_ptr< WDataSetSphericalHarmonics > m_dataSet; //!< Pointer to the treated data set.

    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_inputGFA; //!< The input for the GFA.
    osg::ref_ptr< WGEShader > m_shader; //!< The shader used for the glyph surfaces
    boost::shared_ptr< WItemSelection > m_sliceOrientations; //!< A list of the selectable slice orientations, i.e  x, y and z.
    WPropSelection m_sliceOrientationSelectionProp; //!< To choose whether to x, y or z slice.
    WPropBool m_usePolarPlotProp; //!< Property indicating whether to use polar plot instead of HOME glyph
    WPropBool m_useNormalizationProp; //!< Indicates whether to use min max normalization.
    WPropBool m_useRadiusNormalizationProp; //!< Indicates whether to use radius normalization.
    WPropBool m_hideNegativeLobesProp; //!< Indicates whether to hide negativ radius lobes of glyphs
    WPropDouble m_GFAThresholdProp; //!< Property holding the threshold of GFA above which glyphs should be drawn.
    WPropDouble m_glyphSizeProp; //!< Property holding the size of the displayed glyphs
    WPropInt m_sliceIdProp; //!< Property holding the slice ID
    boost::shared_ptr< WItemSelection > m_orders; //!< A list of the selectable orders
    WPropSelection m_orderProp; //!< Property holding the order of the SH to show.

    WPropInt m_moduloProp; //!< Property holding information on how many glyphs will be omited between two glyphs (modulo-1).
    WPropInt m_subdivisionLevelProp; //!< Property holding information on the subdivision level of the spheres (resolution).

    osg::ref_ptr< osg::Geode > m_glyphsGeode; //!< Pointer to geode containing the glyphs.
    osg::ref_ptr< WGEGroupNode > m_moduleNode; //!< Pointer to the modules group node.

    //! The last exception thrown by any worker thread.
    boost::shared_ptr< WException > m_lastException;
    boost::mutex m_moduleNodeLock; //!< Lock to prevent concurrent threads trying access m_moduleNode

    //! condition indicating if any exception was thrown.
    boost::shared_ptr< WCondition > m_exceptionCondition;

    /**
     * This class actually generated the glyph geometry. This class has () operator that the work.
     * The () operator enables it to be parallelized by WThreadedFunction.
     */
    class GlyphGeneration
    {
    public:
        /**
         * Constructor setting the data pointers and the properties from the module.
         *
         * \param dataSet Pointer to the treated data set.
         * \param dataGFA GFA data for dataSet.
         * \param thresholdGFA Threshold of GFA below which we will not draw the glyphs
         * \param sliceId Rendered slice
         * \param order Order of the rendered spherical harmonics.
         * \param subdivisionLevel Subidivision level of spheres that are basis for glyphs (resolution)
         * \param modulo Show only every modulo-th glyph in each direction.
         * \param sliceType Slice direction (sagittal, coronal, axial )
         * \param usePolar Use polar glyphs (HOME otherwise)
         * \param scale Resize the glyphs.
         * \param useNormalization Scale minimum and maximum radius to [0,1].
         * \param useRadiusNormalization Scale glyphs to make them all the same size.
         * \param hideNegativeLobes Vertices with negative radius will be set to zero.
         */
        GlyphGeneration(  boost::shared_ptr< WDataSetSphericalHarmonics > dataSet,
                          boost::shared_ptr< WDataSetScalar > dataGFA,
                          double thresholdGFA,
                          const size_t& sliceId,
                          const size_t& order,
                          const size_t& subdivisionLevel,
                          const size_t& modulo,
                          const size_t& sliceType,
                          const bool& usePolar,
                          const float& scale,
                          const bool& useNormalization,
                          const bool& useRadiusNormalization,
                          const bool& hideNegativeLobes );

        /**
         * Destructor freeing the data.
         */
        ~GlyphGeneration();

        /**
         * Computes the glyphs.
         *
         * \param id Thread id.
         * \param numThreads The number of threads accessing this function simultaneously
         * \param b A flag allowing to check whether a an abort has been requested.
         */
        void operator()( size_t id, size_t numThreads, WBoolFlag& b ); //NOLINT

        /**
         * Get the geode of the computed glyphs.
         *
         * \return geode
         */
        osg::ref_ptr< osg::Geode > getGraphics();
    private:

        /**
         * Makes the radii of the glyph be distributed between [0,1]
         * \param glyph The glyph that will be normalized given as teem's limnPolyData.
         * \param nbVertCoords Number of coordinates per vertex for the glyph poly-data
         */
        void minMaxNormalization( limnPolyData *glyph, const size_t& nbVertCoords );

        size_t m_nA; //!< Internal variable holding the number of glyphs in the first  direction of the slice.
        size_t m_nB; //!< Internal variable holding the number of glyphs in the second direction of the slice.
        size_t m_nX; //!< Number of voxels in x direction.
        size_t m_nY; //!< Number of voxels in y direction.
        size_t m_nZ; //!< Number of voxels in z direction.
        boost::shared_ptr< WDataSetSphericalHarmonics > m_dataSet; //!< Pointer to the treated data set.
        boost::shared_ptr< WDataSetScalar > m_dataGFA; //!< Pointer to possible GFA data set.
        boost::shared_ptr< WGridRegular3D > m_grid; //!< Pointer to the grid of the treated data set.
        osg::ref_ptr< osg::Vec3Array > m_vertArray; //!< Vertices of the triangles of the glyphs.
        osg::ref_ptr< osg::Vec3Array > m_normals; //!< Normals of the vertices of the glyphs.
        osg::ref_ptr< osg::Vec4Array > m_colors; //!< Colors of the vertices of the glyphs.
        osg::ref_ptr< osg::DrawElementsUInt > m_glyphElements; //!< Indices of the vertices of the triangles of the glyphs.

        double m_thresholdGFA; //!< Stores the GFA threshold from the property.
        size_t m_sliceId; //!< Stores option from property.
        size_t m_order; //!< Stores option from property.
        size_t m_sliceType; //!< Stores option from property.
        size_t m_subdivisionLevel; //!< Store option from property
        size_t m_modulo; //!< Store option from property
        bool m_usePolar; //!< Stores option from property.
        float m_scale; //!< Stores option from property.
        bool m_useNormalization; //!< Stores option from property.
        bool m_useRadiusNormalization; //!< Stores option from property.
        bool m_hideNegativeLobes; //!< Stores option from property.

        limnPolyData *m_sphere; //!< The geometry of the subdivided icosahedron
    };
};

#endif  // WMTEEMGLYPHS_H
