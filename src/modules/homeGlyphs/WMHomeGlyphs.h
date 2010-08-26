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

#ifndef WMHOMEGLYPHS_H
#define WMHOMEGLYPHS_H

#include <string>

#include <osg/Geode>

#include "../../dataHandler/WDataSetSphericalHarmonics.h"
#include "../../graphicsEngine/WShader.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

/**
 * Spherical harmonics glyphs usingf teem (http://teem.sourceforge.net/).
 * According to http://www.ci.uchicago.edu/~schultz/sphinx/home-glyph.htm
 * Thanks to Thomas Schultz.
 *
 * \warning Not yet in useful state.
 * \ingroup modules
 */
class WMHomeGlyphs: public WModule
{
public:

    /**
     * Nothing special with this constructor.
     */
    WMHomeGlyphs();

    /**
     * Nothing special with this constructor.
     */
    virtual ~WMHomeGlyphs();

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


private:
    /**
     * An input connector that accepts sperical harmonics datasets.
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

    osg::ref_ptr< WShader > m_shader; //!< The shader used for the glyph surfaces
    boost::shared_ptr< WItemSelection > m_sliceOrientations; //!< A list of the selectable slice orientations, i.e  x, y and z.
    WPropSelection m_sliceOrientationSelection; //!< To choose whether to x, y or z slice.
    WPropBool m_usePolarPlotProp; //!< Property indicating whether to use polar plot instead of HOME glyph
    WPropBool m_useNormalizationProp; //!< Indicates whether to us radius normalization.
    WPropDouble m_glyphSizeProp; //!< Property holding the size of the displayed glyphs
    WPropInt m_sliceIdProp; //!< Property holding the slice ID

    osg::ref_ptr< osg::Geode > m_glyphsGeode; //!< Pointer to geode containing the glyphs.
    osg::ref_ptr< WGEGroupNode > m_moduleNode; //!< Pointer to the modules group node.
    static const size_t m_nbVertCoords; //!< The teem limn data structure has 4 values for a coordinate: x, y, z, w.

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
         * \param sliceId Rendered slice
         * \param sliceType Slice direction (sagittal, coronal, axial )
         * \param usePolar Use polar glyphs (HOME otherwise)
         * \param scale Resize the glyphs.
         * \param useNormalization Scale minimum and maximum radius to [0,1].
         */
        GlyphGeneration(  boost::shared_ptr< WDataSetSphericalHarmonics > dataSet,
                          const size_t& sliceId,
                          const size_t& sliceType,
                          const bool& usePolar,
                          const float& scale,
                          const bool& useNormalization );

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
         */
        osg::ref_ptr< osg::Geode > getGraphics();
    private:

        /**
         * Makes the radii of the glyph be distributed between [0,1]
         * \param glyph The glyph thta will be normalized given as teem's limnPolyData.
         */
        void minMaxNormalization( limnPolyData *glyph );

        size_t m_nA; //!< Internal variable holding the number of glyphs in the first  direction of the slice.
        size_t m_nB; //!< Internal variable holding the number of glyphs in the second direction of the slice.
        size_t m_nX; //!< Number of voxels in x direction.
        size_t m_nY; //!< Number of voxels in y direction.
        size_t m_nZ; //!< Number of voxels in z direction.
        boost::shared_ptr< WDataSetSphericalHarmonics > m_dataSet; //!< Pointer to the treated data set.
        boost::shared_ptr< WGridRegular3D > m_grid; //!< Pointer to the grid of the treated data set.
        osg::Geometry* m_glyphGeometry; //!< All glyphs.
        osg::ref_ptr< osg::Geode > m_glyphsGeode; //!< The geode containing the glyphs.
        osg::ref_ptr< osg::Vec3Array > m_vertArray; //!< Vertices of the triangles of the glyphs.
        osg::ref_ptr< osg::Vec3Array > m_normals; //!< Normals of the vertices of the glyphs.
        osg::ref_ptr< osg::Vec4Array > m_colors; //!< Colors of the vertices of the glyphs.
        osg::ref_ptr< osg::DrawElementsUInt > m_glyphElements; //!< Indices of the vertices of the triangles of the glyphs.
        osg::ref_ptr< WGEGroupNode > m_generatorNode; //!< Pointer to the generators group node.

        size_t m_sliceId; //!< Stores option from property.
        size_t m_sliceType; //!< Stores option from property.
        bool m_usePolar; //!< Stores option from property.
        float m_scale; //!< Stores option from property.
        bool m_useNormalization; //!< Stores option from property.

        limnPolyData *m_sphere; //!< The geometry of the subdivided icosahedron
    };
};

#endif  // WMHOMEGLYPHS_H
