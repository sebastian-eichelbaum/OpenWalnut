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

#ifndef WMSUPERQUADRICGLYPHS_H
#define WMSUPERQUADRICGLYPHS_H

#include <string>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Uniform>
#include <osg/MatrixTransform>

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

#include "../../dataHandler/WDataSetSingle.h"

#include "../../graphicsEngine/WGEGroupNode.h"
#include "../../graphicsEngine/WShader.h"

/**
 * Rendering of GPU bases Superquadric Glyphs. These glyphs are completely raytraced on the GPU.
 *
 * \ingroup modules
 */
class WMSuperquadricGlyphs: public WModule
{
public:

    /**
     * Constructor.
     */
    WMSuperquadricGlyphs();

    /**
     * Destructor.
     */
    virtual ~WMSuperquadricGlyphs();

    /**
     * Gives back the name of this module.
     *
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     *
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

    /**
     * Callback for m_active. Overwrite this in your modules to handle m_active changes separately.
     */
    virtual void activate();

private:

    /**
     * Initializes the needed geodes, transformations and vertex arrays. This needs to be done once for each new dataset.
     */
    void initOSG();

    /**
     * The Geode containing all the glyphs. In fact it only contains a quad per glyph on which the raytracing is done.
     */
    osg::ref_ptr< WGEGroupNode > m_output;

    /**
     * The transformation node moving the X slice through the dataset space if the sliders are used
     */
    osg::ref_ptr< osg::MatrixTransform > m_xSlice;

    /**
     * The transformation node moving the Y slice through the dataset space if the sliders are used
     */
    osg::ref_ptr< osg::MatrixTransform > m_ySlice;

    /**
     * The transformation node moving the Z slice through the dataset space if the sliders are used
     */
    osg::ref_ptr< osg::MatrixTransform > m_zSlice;

    /**
     * The input dataset. It contains the second order tensor data needed here.
     */
    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_input;

    /**
     * The current tensor dataset.
     */
    boost::shared_ptr< const WDataSetSingle > m_dataSet;

    /**
     * The current tensor dataset's grid.
     */
    boost::shared_ptr< WGridRegular3D > m_dataSetGrid;

    /**
     * The current tensor dataset's valueset.
     */
    boost::shared_ptr< WValueSetBase > m_dataSetValueSet;

    /**
     * Number of cells in X direction. Stored as a member to avoid permanent gird look ups.
     */
    size_t m_maxX;

    /**
     * Number of cells in Y direction. Stored as a member to avoid permanent gird look ups.
     */
    size_t m_maxY;

    /**
     * Number of cells in Z direction. Stored as a member to avoid permanent gird look ups.
     */
    size_t m_maxZ;

    /**
     * Number of glyphs on X Plane.
     */
    size_t m_nbGlyphsX;

    /**
     * Number of glyphs on Y Plane.
     */
    size_t m_nbGlyphsY;

    /**
     * Number of glyphs on Z Plane.
     */
    size_t m_nbGlyphsZ;

    /**
     * the shader actually doing the glyph raytracing
     */
    osg::ref_ptr< WShader > m_shader;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    WPropInt      m_xPos; //!< x position of the slice

    WPropInt      m_yPos; //!< y position of the slice

    WPropInt      m_zPos; //!< z position of the slice

    WPropBool     m_showonX; //!< indicates whether the vector should be shown on slice X

    WPropBool     m_showonY; //!< indicates whether the vector should be shown on slice Y

    WPropBool     m_showonZ; //!< indicates whether the vector should be shown on slice Z

    /**
     * The eigenvalue threshold to filter glyphs
     */
    WPropDouble   m_evThreshold;

    /**
     * The FA threshold to filter glyphs
     */
    WPropDouble   m_faThreshold;

    /**
     * Sharpness of the glyphs
     */
    WPropDouble   m_gamma;

    /**
     * Scaling of the glyphs
     */
    WPropDouble   m_scaling;

    /**
     * True if the EV should be normalized
     */
    WPropBool     m_unifyEV;

    /**
     * Adds a cube to the vertex array.
     *
     * \param position the position in world
     * \param vertices the vertex array
     * \param orientation the tex coord array storing the orientation
     */
    inline void addGlyph( osg::Vec3 position, osg::ref_ptr< osg::Vec3Array > vertices, osg::ref_ptr< osg::Vec3Array > orientation );

    /**
     * Adds a tensor to two arrays. 6*4 times per glyph.
     *
     * \param idx the idx of the tensor in the valueset
     * \param diag the diagonal array
     * \param offdiag the off-diagonal array
     */
    inline void addTensor( size_t idx, osg::Vec3Array* diag, osg::Vec3Array* offdiag );

    /**
     * Class handling uniform update during render traversal
     */
    class SafeUniformCallback: public osg::Uniform::Callback
    {
    public:

        /**
         * Constructor.
         *
         * \param module just set the creating module as pointer for later reference.
         */
        explicit SafeUniformCallback( WMSuperquadricGlyphs* module ): m_module( module )
        {
        };

        /**
         * The callback. Called every render traversal for the uniform.
         *
         * \param uniform the uniform for which this callback is.
         * \param nv the visitor.
         */
        virtual void operator() ( osg::Uniform* uniform, osg::NodeVisitor* nv );

        /**
         * Pointer used to access members of the module to modify the node.
         */
        WMSuperquadricGlyphs* m_module;
    };


    /**
     * Node callback to handle updates in the glyph tensor data
     */
    class GlyphGeometryNodeCallback : public osg::Drawable::UpdateCallback
    {
    public: // NOLINT

        /**
         * Constructor.
         *
         * \param geo the geometry object to handle
         */
        explicit GlyphGeometryNodeCallback( osg::Geometry* geo ):
            osg::Drawable::UpdateCallback(),
            m_geometry( geo )
        {
        }

        /**
         * This operator gets called by OSG every update cycle.
         *
         * \param d the osg drawable
         * \param nv the node visitor
         */
        virtual void update( osg::NodeVisitor* nv, osg::Drawable* d );

        /**
         * Updates the tensor data in the glyph slice.
         *
         * \param diag the diagonal elements as texture coords
         * \param offdiag the off diagonal elements as texture coords
         */
        void setNewTensorData( osg::ref_ptr< osg::Vec3Array > diag, osg::ref_ptr< osg::Vec3Array > offdiag );

        /**
         * The geometry node to handle here
         */
        osg::Geometry* m_geometry;

        /**
         * Dirty flag. If true, the m_tensorDiag and m_tensorOffDiag get set
         */
        bool m_dirty;

        /**
         * Diagonal tensor elements in texture coordinate array.
         */
        osg::ref_ptr< osg::Vec3Array > m_tensorDiag;

        /**
         * Off-diagonal tensor elements in texture coordinate array.
         */
        osg::ref_ptr< osg::Vec3Array > m_tensorOffDiag;
    };

    /**
     * The update callback of m_xSlice glphs.
     */
    osg::ref_ptr< GlyphGeometryNodeCallback > m_xSliceGlyphCallback;

    /**
     * The update callback of m_ySlice glphs.
     */
    osg::ref_ptr< GlyphGeometryNodeCallback > m_ySliceGlyphCallback;

    /**
     * The update callback of m_zSlice glphs.
     */
    osg::ref_ptr< GlyphGeometryNodeCallback > m_zSliceGlyphCallback;

    /**
     * Node callback to handle updates in the slice position properly
     */
    class SliceNodeCallback : public osg::NodeCallback
    {
    public: // NOLINT

        /**
         * Constructor.
         *
         * \param mt the transform node.
         * \param axe the axe to translate along
         * \param property the property containing the value
         */
        SliceNodeCallback( osg::MatrixTransform* mt, osg::Vec3 axe, WPropInt property ):
            osg::NodeCallback(),
            m_axe( axe ),
            m_pos( property ),
            m_oldPos( -1 ),
            m_slice( mt )
        {
        }

        /**
         * This operator gets called by OSG every update cycle.
         *
         * \param node the osg node
         * \param nv the node visitor
         */
        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

        /**
         * The axis to transform along.
         */
        osg::Vec3 m_axe;

        /**
         * The position
         */
        WPropInt m_pos;

        /**
         * Cache the old position for proper update
         */
        int m_oldPos;

        /**
         * The transform node to handle here
         */
        osg::MatrixTransform* m_slice;
    };

    /**
     * The update callback of m_xSlice.
     */
    osg::ref_ptr< SliceNodeCallback > m_xSliceCallback;

    /**
     * The update callback of m_ySlice.
     */
    osg::ref_ptr< SliceNodeCallback > m_ySliceCallback;

    /**
     * The update callback of m_zSlice.
     */
    osg::ref_ptr< SliceNodeCallback > m_zSliceCallback;
};

#endif  // WMSUPERQUADRICGLYPHS_H
