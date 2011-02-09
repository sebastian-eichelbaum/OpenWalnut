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

#ifndef WMNAVSLICES_H
#define WMNAVSLICES_H

#include <list>
#include <string>
#include <vector>
#include <utility>

#include <osg/Geometry>
#include <osg/Node>

#include "../../WModule.h"

#include "../../../graphicsEngine/WGEViewer.h"
#include "../../WExportKernel.h"

// forward declarations
class WGEGroupNode;
class WGEShader;
class WModuleConnector;
class WPickInfo;
template< class T > class WModuleInputData;

/**
 * Navigation slice module
 * \ingroup modules
 */
class OWKERNEL_EXPORT WMNavSlices: public WModule
{
public:

    /**
     * \par Description
     * Default constructor.
     */
    WMNavSlices();

    /**
     * \par Description
     * Destructor.
     */
    virtual ~WMNavSlices();

    /**
     * \par Description
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * \par Description
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

    /**
     * Checks whether the nav slices are already running.
     *
     * \return true if running.
     */
    static bool isRunning();

protected:

    /**
     * \par Description
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
     * Receive DATA_CHANGE notifications.
     *
     * \param input the input connector that got the change signal. Typically it is one of the input connectors from this module.
     * \param output the output connector that sent the signal. Not part of this module instance.
     */
    virtual void notifyDataChange( boost::shared_ptr<WModuleConnector> input,
                                   boost::shared_ptr<WModuleConnector> output );


private:
    /**
     * updates the positions of the navigation slices
     */
    void updateGeometry();

    /**
    * updates the matrix for each slice View, so the slices are drawn centered
    */
    void updateViewportMatrix();

    /**
     *  updates textures and shader parameters
     */
    void updateTextures();

    /**
     * Used as callback which simply sets m_textureChanged to true. Called by WSubject whenever the datasets change.
     */
    void notifyTextureChange();

    /**
     * Computes the bounding box from the loaded textures an sets min/max of the slice positions
     */
    void setMaxMinFromBoundingBox();

    WBoundingBox m_bb; //!< bounding box of textures.

    /**
     * True when textures have changed.
     */
    bool m_textureChanged;

    /**
     * Axial slice position.
     */
    WPropInt m_axialPos;

    /**
     * Coronal slice position.
     */
    WPropInt m_coronalPos;

    /**
     * Sagittal slice position.
     */
    WPropInt m_sagittalPos;

    /**
     * Maximum position of axial slice.
     */
    WPropInt m_maxAxial;

    /**
     * Maximum position of coronal slice.
     */
    WPropInt m_maxCoronal;

    /**
     * Maximum position of sagittal slice.
     */
    WPropInt m_maxSagittal;

    /**
     * True if axial slice should be visible.
     */
    WPropBool m_showAxial;

    /**
     * True if coronal slice should be visible.
     */
    WPropBool m_showCoronal;

    /**
     * True if sagittal slice should be visible.
     */
    WPropBool m_showSagittal;

    /**
     * True if the shader shouldn't discard a fragment when the value is zero
     */
    WPropBool m_showComplete;

    /**
     * The current position as information property.
     */
    WPropPosition m_currentPosition;

    /**
     * initial create method
     */
    void create();

    /**
     * Sets slice positions from interaction in the main GL widget
     * \param pickInfo The information provided by the pick handler
     */
    void setSlicePosFromPick( WPickInfo pickInfo );

    /**
     * Initial creation function for the slice geometry
     * \param slice ID of the slice to be drawn. 0=y, 1=x, 2=z
     */
    osg::ref_ptr<osg::Geometry> createGeometry( int slice );

    /**
    * creates the geometry for the cross
    * \param slice ID of the slice to be drawn. 0=y, 1=x, 2=z
    */
    osg::ref_ptr<osg::Geometry> createCrossGeometry( int slice );

    /**
     * creates and initializes the uniform parameters for the shader
     * \param rootState The uniforms will be applied to this state.
     */
    void initUniforms( osg::StateSet* rootState );

    std::pair< float, float > m_oldPixelPosition; //!< Caches the old picked position to a allow for comparison
    bool m_isPicked; //!< Indicates whether a slice is currently picked or not.
    bool m_isPickedSagittal; //!< Indicates whether a sagittal slice is currently picked or not.
    bool m_isPickedCoronal; //!< Indicates whether coronal slice is currently picked or not.
    bool m_isPickedAxial; //!< Indicates whether axial slice is currently picked or not.
    boost::shared_ptr< WGEViewer > m_viewer; //!< Stores reference to the main viewer

    /**
     * the root node for this module
     */
    osg::ref_ptr< WGEGroupNode > m_rootNode;

    /**
     * the root node for the slices
     */
    osg::ref_ptr< WGEGroupNode > m_slicesNode;

    /**
     * Allows to activate and deactivate slices very easy.
     */
    osg::ref_ptr< osg::Switch > m_slicesSwitchNode;

    /**
     * nodes for each slice, to be reused in other widgets
     */
    osg::ref_ptr<osg::Geode> m_xSliceNode;

    /**
     * nodes for each slice, to be reused in other widgets
     */
    osg::ref_ptr<osg::Geode> m_ySliceNode;

    /**
     * nodes for each slice, to be reused in other widgets
     */
    osg::ref_ptr<osg::Geode> m_zSliceNode;

    /**
    * nodes for each cross, to be reused in other widgets
    */
    osg::ref_ptr<osg::Geode> m_xCrossNode;

    /**
    * nodes for each cross, to be reused in other widgets
    */
    osg::ref_ptr<osg::Geode> m_yCrossNode;

    /**
    * nodes for each cross, to be reused in other widgets
    */
    osg::ref_ptr<osg::Geode> m_zCrossNode;

    /**
     * the shader object for this module
     */
    osg::ref_ptr< WGEShader > m_shader;

    /**
     * lock to prevent concurrent threads trying to update the osg node
     */
    boost::shared_mutex m_updateLock;

    /**
     * vector of uniforms for type of texture
     */
    std::vector< osg::ref_ptr<osg::Uniform> > m_typeUniforms;

    /**
     * vector of alpha values per texture
     */
    std::vector< osg::ref_ptr<osg::Uniform> > m_alphaUniforms;

    /**
     * vector of thresholds per texture
     */
    std::vector< osg::ref_ptr<osg::Uniform> > m_thresholdUniforms;

    /**
     * vector of color maps per texture
     */
    std::vector< osg::ref_ptr<osg::Uniform> > m_cmapUniforms;

    /**
     * vector of samplers
     */
    std::vector< osg::ref_ptr<osg::Uniform> > m_samplerUniforms;

    osg::ref_ptr<osg::Uniform> m_highlightUniformSagittal; //!< Determines whether the slice is highlighted
    osg::ref_ptr<osg::Uniform> m_highlightUniformCoronal; //!< Determines whether the slice is highlighted
    osg::ref_ptr<osg::Uniform> m_highlightUniformAxial; //!< Determines whether the slice is highlighted

    osg::ref_ptr<osg::Uniform> m_showCompleteUniform; //!< Determines whether the slices should be drawn completely

    /**
     * To avoid multiple instances of the nav slices.
     */
    static bool m_navsliceRunning;

    /**
    * Current width of the axial widget
    */
    int m_axialWidgetWidth;

    /**
    * Current height of the axial widget
    */
    int m_axialWidgetHeight;

    /**
    * Current width of the sagittal widget
    */
    int m_sagittalWidgetWidth;

    /**
    * Current height of the sagittal widget
    */
    int m_sagittalWidgetHeight;

    /**
    * Current width of the coronal widget
    */
    int m_coronalWidgetWidth;

    /**
    * Current height of the coronal widget
    */
    int m_coronalWidgetHeight;

    /**
    * Wrapper class for userData to prevent cyclic destructor calls
    */
    class userData: public osg::Referenced
    {
        friend class WMNavSlices;
    public:
        /**
        * userData Constructor with shared pointer to module
        * \param _parent pointer to the module
        */
        explicit userData( boost::shared_ptr< WMNavSlices > _parent )
        {
            parent = _parent;
        }

        /**
        * updateGeometry wrapper Function
        */
        void updateGeometry();

        /**
        * updateTextures wrapper Function
        */
        void updateTextures();
    private:
        /**
        * shared pointer to the module
        */
        boost::shared_ptr< WMNavSlices > parent;
    };

    /**
     * Node callback to handle updates properly
     */
    class sliceNodeCallback : public osg::NodeCallback
    {
    public: // NOLINT
        /**
         * operator ()
         *
         * \param node the osg node
         * \param nv the node visitor
         */
        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
        {
            osg::ref_ptr< userData > module = static_cast< userData* > ( node->getUserData() );

            if ( module )
            {
                module->updateGeometry(); // Keep this order. The update routines depend on it.
                module->updateTextures(); // Keep this order. The update routines depend on it.
            }
            traverse( node, nv );
        }
    };
};



#endif  // WMNAVSLICES_H

