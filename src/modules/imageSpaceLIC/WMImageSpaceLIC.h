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

#ifndef WMIMAGESPACELIC_H
#define WMIMAGESPACELIC_H

#include <string>
#include <vector>

#include "../../dataHandler/WDataSetVector.h"
#include "../../dataHandler/WDataSetScalar.h"
#include "../../dataHandler/WSubject.h"
#include "../../graphicsEngine/WTriangleMesh.h"
#include "../../graphicsEngine/WGEManagedGroupNode.h"

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"

/**
 * This module takes an vector dataset and used it to apply an image space based (fast) LIC to an arbitrary surface. The surface can be specified
 * as tri mesh or, if not specified, slices.
 *
 * \ingroup modules
 */
class WMImageSpaceLIC: public WModule
{
public:

    /**
     * Default constructor.
     */
    WMImageSpaceLIC();

    /**
     * Destructor.
     */
    virtual ~WMImageSpaceLIC();

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
     * Initializes the needed geodes, transformations and vertex arrays. This needs to be done once for each new dataset.
     *
     * \param grid the grid to places the slices in
     * \param mesh the mesh to use if not NULL and m_useSlices is false
     */
    void initOSG( boost::shared_ptr< WGridRegular3D > grid, boost::shared_ptr< WTriangleMesh > mesh );

    /**
     * The input connector containing the vector field.
     */
    boost::shared_ptr< WModuleInputData< WDataSetVector > > m_vectorsIn;

    /**
     * The input connector containing the scalar field whose derived field is used for LIC.
     */
    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_scalarIn;

    /**
     * The input containing the surface on which the LIC should be applied on
     */
    boost::shared_ptr< WModuleInputData< WTriangleMesh > > m_meshIn;

    /**
     * A property allowing the user to select whether the slices or the mesh should be used
     */
    WPropSelection m_geometrySelection;

    /**
     * A list of items that can be selected using m_geometrySelection.
     */
    boost::shared_ptr< WItemSelection > m_geometrySelections;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * The Geode containing all the slices and the mesh
     */
    osg::ref_ptr< WGEManagedGroupNode > m_output;

    /**
     * The transformation node moving the X slice through the dataset space if the sliders are used
     */
    osg::ref_ptr< WGEManagedGroupNode > m_xSlice;

    /**
     * The transformation node moving the Y slice through the dataset space if the sliders are used
     */
    osg::ref_ptr< WGEManagedGroupNode > m_ySlice;

    /**
     * The transformation node moving the Z slice through the dataset space if the sliders are used
     */
    osg::ref_ptr< WGEManagedGroupNode > m_zSlice;

    WPropGroup    m_sliceGroup; //!< the group contains several slice properties

    WPropGroup    m_geometryGroup; //!< the group contains several input geometry parameters

    WPropGroup    m_licGroup; //!< the group contains several LIC properties

    WPropBool     m_useSlices; //!< indicates whether the vector should be shown on slices or input geometry

    WPropInt      m_xPos; //!< x position of the slice

    WPropInt      m_yPos; //!< y position of the slice

    WPropInt      m_zPos; //!< z position of the slice

    WPropBool     m_showonX; //!< indicates whether the vector should be shown on slice X

    WPropBool     m_showonY; //!< indicates whether the vector should be shown on slice Y

    WPropBool     m_showonZ; //!< indicates whether the vector should be shown on slice Z

    WPropBool     m_showHUD; //!< indicates whether to show the texture HUD

    WPropBool     m_useEdges; //!< indicates whether to show the edges

    WPropBool     m_useLight; //!< indicates whether to use Phong

    WPropBool     m_useDepthCueing; //!< indicates whether to use depth cueing in the shader

    WPropBool     m_useHighContrast; //!< use the high contrast version?

    WPropInt      m_numIters; //!< the number of iterations done per frame

    WPropDouble   m_cmapRatio; //!< the ratio between colormap and LIC

    // TODO(ebaum): this belongs to some central place
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

    osg::ref_ptr<osg::Uniform> m_showCompleteUniform; //!< Determines whether the slices should be drawn completely

    static const int m_maxNumberOfTextures = 8; //!< We support only 8 textures because some known hardware does not support more texture coordinates.

    /**
     * creates and initializes the uniform parameters for the shader
     * \param rootState The uniforms will be applied to this state.
     */
    void initUniforms( osg::StateSet* rootState )
    {
        m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type0", 0 ) ) );
        m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type1", 0 ) ) );
        m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type2", 0 ) ) );
        m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type3", 0 ) ) );
        m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type4", 0 ) ) );
        m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type5", 0 ) ) );
        m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type6", 0 ) ) );
        m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type7", 0 ) ) );
        m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type8", 0 ) ) );
        m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type9", 0 ) ) );

        m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha0", 1.0f ) ) );
        m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha1", 1.0f ) ) );
        m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha2", 1.0f ) ) );
        m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha3", 1.0f ) ) );
        m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha4", 1.0f ) ) );
        m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha5", 1.0f ) ) );
        m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha6", 1.0f ) ) );
        m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha7", 1.0f ) ) );
        m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha8", 1.0f ) ) );
        m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha9", 1.0f ) ) );

        m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold0", 0.0f ) ) );
        m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold1", 0.0f ) ) );
        m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold2", 0.0f ) ) );
        m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold3", 0.0f ) ) );
        m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold4", 0.0f ) ) );
        m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold5", 0.0f ) ) );
        m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold6", 0.0f ) ) );
        m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold7", 0.0f ) ) );
        m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold8", 0.0f ) ) );
        m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold9", 0.0f ) ) );

        m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex0", 2 ) ) );
        m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex1", 3 ) ) );
        m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex2", 4 ) ) );
        m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex3", 5 ) ) );
        m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex4", 6 ) ) );
        m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex5", 7 ) ) );
        m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex6", 8 ) ) );
        m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex7", 9 ) ) );
        m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex8", 10 ) ) );
        m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex9", 11 ) ) );

        m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap0", 0 ) ) );
        m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap1", 0 ) ) );
        m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap2", 0 ) ) );
        m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap3", 0 ) ) );
        m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap4", 0 ) ) );
        m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap5", 0 ) ) );
        m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap6", 0 ) ) );
        m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap7", 0 ) ) );
        m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap8", 0 ) ) );
        m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap9", 0 ) ) );

        for ( int i = 0; i < m_maxNumberOfTextures; ++i )
        {
            rootState->addUniform( m_typeUniforms[i] );
            rootState->addUniform( m_thresholdUniforms[i] );
            rootState->addUniform( m_alphaUniforms[i] );
            rootState->addUniform( m_samplerUniforms[i] );
            rootState->addUniform( m_cmapUniforms[i] );
        }

        rootState->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "showComplete", 0 ) ) );
    }

    /**
     * Node callback to handle updates properly
     */
    class sliceNodeCallback : public osg::NodeCallback
    {
    public: // NOLINT

        /**
         * Constructor.
         *
         * \param parent the module creating this instance.
         */
        explicit sliceNodeCallback( boost::shared_ptr< WMImageSpaceLIC > parent ):
            m_module( parent )
        {
        }

        /**
         * operator ()
         *
         * \param node the osg node
         * \param nv the node visitor
         */
        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
        {
            m_module->updateTextures(); // Keep this order. The update routines depend on it.
            traverse( node, nv );
        }

        /**
         * The module that created the instance
         */
        boost::shared_ptr< WMImageSpaceLIC > m_module;
    };

    /**
     * True when textures have changed.
     */
    bool m_textureChanged;

    /**
     * Called on texture change.
     */
    void notifyTextureChange()
    {
        m_textureChanged = true;
    }

    /**
     * Setups uniforms and several callbacks.
     */
    void setupTexturing()
    {
        initUniforms( m_output->getOrCreateStateSet() );
        m_textureChanged = true;

        // now, to watch changing/new textures use WSubject's change condition
        boost::signals2::connection con = WDataHandler::getDefaultSubject()->getChangeCondition()->subscribeSignal(
                boost::bind( &WMImageSpaceLIC::notifyTextureChange, this )
        );

        m_output->addUpdateCallback( new sliceNodeCallback( boost::shared_dynamic_cast< WMImageSpaceLIC >( shared_from_this() ) ) );
    }

    /**
     * Updates texture bindings and property->value pairs
     */
    void updateTextures()
    {
        osg::StateSet* rootState = m_output->getOrCreateStateSet();
        if ( m_textureChanged )
        {
            m_textureChanged = false;

            // grab a list of data textures
            std::vector< boost::shared_ptr< WDataTexture3D > > tex = WDataHandler::getDefaultSubject()->getDataTextures( true );

            if ( tex.size() > 0 )
            {
                // reset all uniforms
                for ( int i = 0; i < m_maxNumberOfTextures; ++i )
                {
                    m_typeUniforms[i]->set( 0 );
                }

                // for each texture -> apply
                int c = 0;

                //////////////////////////////////////////////////////////////////////////////////////////////////
                if ( WKernel::getRunningKernel()->getSelectionManager()->getUseTexture() )
                {
                    osg::ref_ptr<osg::Texture3D> texture3D = WKernel::getRunningKernel()->getSelectionManager()->getTexture();


                    m_typeUniforms[c]->set( W_DT_UNSIGNED_CHAR  );
                    m_thresholdUniforms[c]->set( 0.0f );
                    m_alphaUniforms[c]->set( WKernel::getRunningKernel()->getSelectionManager()->getTextureOpacity() );
                    m_cmapUniforms[c]->set( 4 );

                    texture3D->setFilter( osg::Texture::MIN_FILTER, osg::Texture::NEAREST );
                    texture3D->setFilter( osg::Texture::MAG_FILTER, osg::Texture::NEAREST );

                    rootState->setTextureAttributeAndModes( c + 2, texture3D, osg::StateAttribute::ON );
                    ++c;
                }
                //////////////////////////////////////////////////////////////////////////////////////////////////

                for ( std::vector< boost::shared_ptr< WDataTexture3D > >::const_iterator iter = tex.begin(); iter != tex.end(); ++iter )
                {
                    osg::ref_ptr<osg::Texture3D> texture3D = ( *iter )->getTexture();

                    if ( ( *iter )->isInterpolated() )
                    {
                        texture3D->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
                        texture3D->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
                    }
                    else
                    {
                        texture3D->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
                        texture3D->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
                    }
                    rootState->setTextureAttributeAndModes( c + 2, texture3D, osg::StateAttribute::ON );

                    // set threshold/opacity as uniforms
                    float minValue = ( *iter )->getMinValue();
                    float maxValue = ( *iter )->getMaxValue();
                    float t = ( ( *iter )->getThreshold() - minValue ) / ( maxValue - minValue ); // rescale to [0,1]
                    float a = ( *iter )->getAlpha();
                    int cmap = ( *iter )->getSelectedColormap();

                    m_typeUniforms[c]->set( ( *iter )->getDataType() );
                    m_thresholdUniforms[c]->set( t );
                    m_alphaUniforms[c]->set( a );
                    m_cmapUniforms[c]->set( cmap );

                    ++c;
                    if( c == m_maxNumberOfTextures )
                    {
                        break;
                    }
                }
            }
        }
    }
};

#endif  // WMIMAGESPACELIC_H

