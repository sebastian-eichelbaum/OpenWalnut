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

#ifndef WGETEXTURE_H
#define WGETEXTURE_H

#include <string>
#include <limits>

#include <boost/shared_ptr.hpp>

#include <osg/Node>
#include <osg/StateSet>
#include <osg/Texture>
#include <osg/Texture1D>
#include <osg/Texture2D>
#include <osg/Texture3D>

#include "callbacks/WGEFunctorCallback.h"
#include "../common/WBoundingBox.h"
#include "../common/WProperties.h"
#include "../common/WPropertyTypes.h"
#include "../common/WPropertyHelper.h"
#include "../common/math/linearAlgebra/WLinearAlgebra.h"

#include "WGETextureUtils.h"

/**
 * This calls serves a simple purpose: have a texture and its scaling information together which allows very easy binding of textures to nodes
 * with associated shaders. When this texture gets bind using the bindTo methods, uniforms get added containing needed scaling information.
 */
template < typename TextureType = osg::Texture >
class WGETexture: public TextureType
{
public:
    //! We support only 8 textures because some known hardware does not support more texture coordinates.
    static std::size_t const MAX_NUMBER_OF_TEXTURES = 8;

    //! The maximum texture dimension.
    static std::size_t const MAX_TEXTURE_DIMENSION = 2048;

    /**
     * Default constructor. Creates an empty instance of the texture.
     *
     * \param scale the scaling factor needed for de-scaling the texture values
     * \param min the minimum value allowing negative values too.
     */
    WGETexture( double scale = 1.0, double min = 0.0 );

    /**
     * Creates texture from given image. Scaling is set to identity.
     *
     * \param image the image to use as texture
     * \param scale the scaling factor needed for de-scaling the texture values
     * \param min the minimum value allowing negative values too.
     */
    WGETexture( osg::Image* image, double scale = 1.0, double min = 0.0 );

    /**
     * Copy the texture.
     *
     * \param texture the texture to copy
     * \param copyop
     */
    WGETexture( const WGETexture< TextureType >& texture, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY );

    /**
     * Destructor.
     */
    virtual ~WGETexture();

    /**
     * Returns the name property of the texture. You should set it if you create a texture.
     *
     * \return texture name property
     */
    WPropString name() const;

    /**
     * The sorting index in the colormapper's texture list.
     *
     * \return the property.
     */
    WPropInt sortIndex() const;

    /**
     * Get the index used to refer to an unset sort index.
     *
     * \return the value.
     */
    static WPVBaseTypes::PV_INT getUnsetSortIndex();

    /**
     * Get the minimum in the de-scaled value space. The property can be changed. A change affects all colormaps using this texture. But be
     * careful as the texture creating depends on these values.
     *
     * \return the minimum
     */
    WPropDouble minimum() const;

    /**
     * Get the scaling factor for de-scaling the texture. The property can be changed. A change affects all colormaps using this texture. But be
     * careful as the texture creating depends on these values.
     *
     * \return the scale
     */
    WPropDouble scale() const;

    /**
     * Returns the alpha property. The property can be changed. A change affects all colormaps using this texture.
     *
     * \return alpha property
     */
    WPropDouble alpha() const;

    /**
     * Clip the values assumed to be zero.
     *
     * \return true to clip.
     */
    WPropBool clipZero() const;

    /**
     * Returns the threshold property. The property can be changed. A change affects all colormaps using this texture.
     *
     * \return threshold property
     */
    WPropDouble thresholdLower() const;

    /**
     * Returns the threshold property. The property can be changed. A change affects all colormaps using this texture.
     *
     * \return threshold property
     */
    WPropDouble thresholdUpper() const;

    /**
     * Returns the property responsible for enabling threshold based clipping. If this is false, the threshold is ignored.
     *
     * \return threshold-enable property.
     */
    WPropBool thresholdEnabled() const;

    /**
     * Returns the interpolation property. The property can be changed. A change affects all colormaps using this texture.
     *
     * \return interpolation property
     */
    WPropBool interpolation() const;

    /**
     * Returns the colormap property. The property can be changed. A change affects all colormaps using this texture.
     *
     * \return colormap property
     */
    WPropSelection colormap() const;

    /**
     * Returns the active property. The property can be changed. A change affects all colormaps using this texture.
     *
     * \return active property
     */
    WPropBool active() const;

    /**
     * Returns the window level definition for the colormap. The property can be changed. A change affects all colormaps using this texture.
     *
     * \return window colormap
     */
    WPropInterval window() const;

    /**
     * Returns the property responsible for enabling window based interval scaling. If this is false, the window setting is ignored.
     *
     * \return windowing-enable property.
     */
    WPropBool windowEnabled() const;

    /**
     * Returns the texture transformation matrix. The property can be changed. A change affects all colormaps using this texture. This matrix
     * converts an world-space coordinate to an texture coordinate! This can be seen as a scaled inverse matrix of the grid's transformation.
     *
     * \return the matrix
     */
    WPropMatrix4X4 transformation() const;

    /**
     * Binds the texture to the specified node and texture unit. It also adds two uniforms: u_textureXMin and u_textureXScale, where X
     * is the unit number. This can be used in shaders to unscale it.
     *
     * \param node the node where to bind the texture to
     * \param unit the unit, by default 0
     */
    void bind( osg::ref_ptr< osg::Node > node, size_t unit = 0 );

    /**
     * Return a pointer to the properties object of the dataset. Add all the modifiable settings here. This allows the user to modify several
     * properties of a dataset.
     *
     * \return the properties.
     */
    boost::shared_ptr< WProperties > getProperties() const;

    /**
     * Return a pointer to the information properties object of the dataset. The dataset intends these properties to not be modified.
     *
     * \return the properties.
     */
    boost::shared_ptr< WProperties > getInformationProperties() const;

    /**
     * Applies some custom uniforms to the specified state-set which directly relate to this texture
     *
     * \param prefix the prefix used for the uniforms
     * \param states the state where to add the uniforms
     */
    virtual void applyUniforms( std::string prefix, osg::StateSet* states ) const;

    /**
     * For all the lazy guys to set the filter MIN and MAG at once.
     *
     * \param mode the new mode for MIN_FILTER and MAG_FILTER.
     */
    void setFilterMinMag( osg::Texture::FilterMode mode );

    /**
     * For all the lazy guys to set the wrapping for s,t and r directions at once.
     *
     * \param mode the new mode for WRAP_S, WRAP_T and WRAP_R.
     */
    void setWrapSTR( osg::Texture::WrapMode mode );

    /**
     * Returns the texture's bounding box. This is const. Although there exists the transformation() property, it is an information property and
     * can't be changed.
     *
     * \return the bounding box.
     */
    virtual WBoundingBox getBoundingBox() const;

protected:
    /**
     * Handles all property updates. Called by m_propCondition.
     */
    virtual void handleUpdate();

    /**
     * Creates the texture data. Overwrite this method if you want to provide a custom texture creation procedure.
     */
    virtual void create();

    /**
     * This method implements an update callback which updates the texture image if needed and several other properties like texture matrix.
     *
     * \param state the state to update
     */
    virtual void updateCallback( osg::StateAttribute* state );

    /**
     * Initialize the size of the texture properly according to real texture type (1D,2D,3D).
     * \note This is needed because osg::Texture::setImage is not virtual and does not set the size from the image.
     *
     * \param texture the texture where to set the size
     * \param width the new width
     * \param height the new height
     * \param depth the new depth
     */
    static void initTextureSize( osg::Texture1D* texture, int width, int height, int depth );

    /**
     * Initialize the size of the texture properly according to real texture type (1D,2D,3D).
     * \note This is needed because osg::Texture::setImage is not virtual and does not set the size from the image.
     *
     * \param texture the texture where to set the size
     * \param width the new width
     * \param height the new height
     * \param depth the new depth
     */
    static void initTextureSize( osg::Texture2D* texture, int width, int height, int depth );

    /**
     * Initialize the size of the texture properly according to real texture type (1D,2D,3D).
     * \note This is needed because osg::Texture::setImage is not virtual and does not set the size from the image.
     *
     * \param texture the texture where to set the size
     * \param width the new width
     * \param height the new height
     * \param depth the new depth
     */
    static void initTextureSize( osg::Texture3D* texture, int width, int height, int depth );

private:
    /**
     * Creates and assigns all properties.
     *
     * \param min the min value of the texture
     * \param scale the scale value of the texture
     */
    void setupProperties( double scale, double min );

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * The property object for the dataset.
     */
    boost::shared_ptr< WProperties > m_properties;

    /**
     * The property object for the dataset containing only props whose purpose is "PV_PURPOSE_INFORMNATION". It is useful to define some property
     * to only be of informational nature. The GUI does not modify them. As it is a WProperties instance, you can use it the same way as
     * m_properties.
     */
    boost::shared_ptr< WProperties > m_infoProperties;

    /**
     * If true, the texture gets created. This is used to create texture on demand.
     */
    bool m_needCreate;

    /**
     * The texture name. This might be useful to identify textures.
     */
    WPropString m_name;

    /**
     * The sort index of the texture. This is important for restoring and saving the colormapper's sorting.
     */
    WPropInt m_sortIdx;

    /**
     * The minimum of each value in the texture in unscaled space.
     */
    WPropDouble m_min;

    /**
     * The scaling factor to de-scale a [0-1] texture to original space.
     */
    WPropDouble m_scale;

    /**
     * A list of color map selection types
     */
    boost::shared_ptr< WItemSelection > m_colorMapSelectionsList;

    /**
     * Selection property for color map
     */
    WPropSelection m_colorMap;

    /**
     * Alpha blending value.
     */
    WPropDouble m_alpha;

    /**
     * If set to true, zero values are clipped by making them transparent
     */
    WPropBool m_clipZero;

    /**
     * Threshold for clipping areas.
     */
    WPropDouble m_thresholdLower;

    /**
     * Threshold for clipping areas.
     */
    WPropDouble m_thresholdUpper;

    /**
     * Threshold-enable flag.
     */
    WPropBool m_thresholdEnabled;

    /**
     * True if interpolation should be used.
     */
    WPropBool m_interpolation;

    /**
     * True if the texture is active.
     */
    WPropBool m_active;

    /**
     * The texture transformation matrix.
     */
    WPropMatrix4X4 m_texMatrix;

    /**
     * Window level setting for the current colormap
     */
    WPropInterval m_window;

    /**
     * Window-Level-Setting-enable flag.
     */
    WPropBool m_windowEnabled;
};

// Some convenience typedefs

/**
 * OSG's Texture1D with scaling features
 */
typedef WGETexture< osg::Texture1D > WGETexture1D;

/**
 * OSG's Texture2D with scaling features
 */
typedef WGETexture< osg::Texture2D > WGETexture2D;

/**
 * OSG's Texture3D with scaling features
 */
typedef WGETexture< osg::Texture3D > WGETexture3D;


template < typename TextureType >
WGETexture< TextureType >::WGETexture( double scale, double min ):
    TextureType(),
    m_propCondition( boost::shared_ptr< WCondition >( new WCondition() ) ),
    m_properties( boost::shared_ptr< WProperties >( new WProperties( "Texture Properties", "Properties of a texture." ) ) ),
    m_infoProperties( boost::shared_ptr< WProperties >( new WProperties( "Texture Info Properties", "Texture's information properties." ) ) ),
    m_needCreate( true )
{
    setupProperties( scale, min );
}

template < typename TextureType >
WGETexture< TextureType >::WGETexture( osg::Image* image, double scale, double min ):
    TextureType( image ),
    m_propCondition( boost::shared_ptr< WCondition >( new WCondition() ) ),
    m_properties( boost::shared_ptr< WProperties >( new WProperties( "Texture Properties", "Properties of a texture." ) ) ),
    m_infoProperties( boost::shared_ptr< WProperties >( new WProperties( "Texture Info Properties", "Texture's information properties." ) ) ),
    m_needCreate( true )
{
    setupProperties( scale, min );
    WGETexture< TextureType >::initTextureSize( this, image->s(), image->t(), image->r() );
}

template < typename TextureType >
WGETexture< TextureType >::WGETexture( const WGETexture< TextureType >& texture, const osg::CopyOp& copyop ):
    TextureType( texture, copyop ),
    m_min( texture.m_min ),
    m_scale( texture.m_scale )
{
    // initialize members
}

template < typename TextureType >
void WGETexture< TextureType >::setupProperties( double scale, double min )
{
    m_propCondition->subscribeSignal( boost::bind( &WGETexture< TextureType >::handleUpdate, this ) );

    m_name = m_properties->addProperty( "Name", "The name of the texture.", std::string( "Unnamed" ) );

    m_sortIdx = m_properties->addProperty( "Sort Index",
                                           "The index specifies the index in the colormapper, used to restore colormapper sorting on load.",
                                           getUnsetSortIndex() );
    m_sortIdx->setHidden( true );

    // initialize members
    m_min = m_properties->addProperty( "Minimum", "The minimum value in the original space.", min, true );
    m_min->removeConstraint( m_min->getMin() );
    m_min->removeConstraint( m_min->getMax() );

    m_scale = m_properties->addProperty( "Scale", "The scaling factor to un-scale the texture values to the original space.", scale, true );
    m_scale->removeConstraint( m_scale->getMin() );
    m_scale->removeConstraint( m_scale->getMax() );

    m_alpha = m_properties->addProperty( "Alpha", "The alpha blending value.", 1.0 );
    m_alpha->setMin( 0.0 );
    m_alpha->setMax( 1.0 );

    m_clipZero = m_properties->addProperty( "Enable Zero Clip", "If enabled, zero values are clipped.", true );

    m_thresholdEnabled = m_properties->addProperty( "Enable Threshold",
                                                    "If enabled, threshold based clipping is used. If not, threshold is ignored.", false );

    m_thresholdLower = m_properties->addProperty( "Lower Threshold", "The threshold used to clip areas below the specified value.", 0.0 );
    m_thresholdLower->setMin( min );
    m_thresholdLower->setMax( min + scale );

    m_thresholdUpper = m_properties->addProperty( "Upper Threshold", "The threshold used to clip areas above the specified value.", 1.0 );
    m_thresholdUpper->setMin( min );
    m_thresholdUpper->setMax( min + scale );

    m_windowEnabled = m_properties->addProperty( "Enable Windowing", "If enabled, window level settings are applied.", false );
    m_window = m_properties->addProperty( "Window Level", "Define the interval in the data which is mapped to the colormap.",
                                          make_interval( 0.0, 1.0 ) );

    m_interpolation = m_properties->addProperty( "Interpolate", "Interpolation of the volume data.", true, m_propCondition );

    m_colorMapSelectionsList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_colorMapSelectionsList->addItem( "Grayscale", "" );
    m_colorMapSelectionsList->addItem( "Rainbow", "" );
    m_colorMapSelectionsList->addItem( "Hot iron", "" );
    m_colorMapSelectionsList->addItem( "Negative to positive", "" );
    m_colorMapSelectionsList->addItem( "Atlas", "" );
    m_colorMapSelectionsList->addItem( "Blue-Green-Purple", "" );
    m_colorMapSelectionsList->addItem( "Vector", "" );

    m_colorMap = m_properties->addProperty( "Colormap", "The colormap of this texture.", m_colorMapSelectionsList->getSelectorFirst() );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_colorMap );

    m_active = m_properties->addProperty( "Active", "Can dis-enable a texture.", true );

    WMatrix4d m = WMatrix4d::identity();
    m_texMatrix = m_properties->addProperty( "Texture Transformation", "Usable to transform the texture.", m );
    m_texMatrix->setHidden();
    m_texMatrix->setPurpose( PV_PURPOSE_INFORMATION );

    TextureType::setResizeNonPowerOfTwoHint( false );
    TextureType::setUpdateCallback( new WGEFunctorCallback< osg::StateAttribute >(
        boost::bind( &WGETexture< TextureType >::updateCallback, this, _1 ) )
    );

    // init filters
    TextureType::setFilter( osg::Texture::MIN_FILTER, m_interpolation->get( true ) ? osg::Texture::LINEAR : osg::Texture::NEAREST );
    TextureType::setFilter( osg::Texture::MAG_FILTER, m_interpolation->get( true ) ? osg::Texture::LINEAR : osg::Texture::NEAREST );
}

template < typename TextureType >
WGETexture< TextureType >::~WGETexture()
{
    // cleanup.
}

template < typename TextureType >
boost::shared_ptr< WProperties > WGETexture< TextureType >::getProperties() const
{
    return m_properties;
}

template < typename TextureType >
boost::shared_ptr< WProperties > WGETexture< TextureType >::getInformationProperties() const
{
    return m_infoProperties;
}

template < typename TextureType >
inline WPropString WGETexture< TextureType >::name() const
{
    return m_name;
}

template < typename TextureType >
inline WPropInt WGETexture< TextureType >::sortIndex() const
{
    return m_sortIdx;
}

template < typename TextureType >
inline WPropDouble WGETexture< TextureType >::minimum() const
{
    return m_min;
}

template < typename TextureType >
inline WPropDouble WGETexture< TextureType >::scale() const
{
    return m_scale;
}

template < typename TextureType >
inline WPropDouble WGETexture< TextureType >::alpha() const
{
    return m_alpha;
}

template < typename TextureType >
inline WPropBool WGETexture< TextureType >::clipZero() const
{
    return m_clipZero;
}

template < typename TextureType >
inline WPropDouble WGETexture< TextureType >::thresholdLower() const
{
    return m_thresholdLower;
}

template < typename TextureType >
inline WPropDouble WGETexture< TextureType >::thresholdUpper() const
{
    return m_thresholdUpper;
}

template < typename TextureType >
inline WPropBool WGETexture< TextureType >::thresholdEnabled() const
{
    return m_thresholdEnabled;
}

template < typename TextureType >
inline WPropBool WGETexture< TextureType >::interpolation() const
{
    return m_interpolation;
}

template < typename TextureType >
inline WPropSelection WGETexture< TextureType >::colormap() const
{
    return m_colorMap;
}

template < typename TextureType >
inline WPropBool WGETexture< TextureType >::active() const
{
    return m_active;
}

template < typename TextureType >
inline WPropBool WGETexture< TextureType >::windowEnabled() const
{
    return m_windowEnabled;
}

template < typename TextureType >
inline WPropInterval WGETexture< TextureType >::window() const
{
    return m_window;
}

template < typename TextureType >
inline WPropMatrix4X4 WGETexture< TextureType >::transformation() const
{
    return m_texMatrix;
}

template < typename TextureType >
void  WGETexture< TextureType >::handleUpdate()
{
    if( m_interpolation->changed() )
    {
        TextureType::setFilter( osg::Texture::MIN_FILTER, m_interpolation->get( true ) ? osg::Texture::LINEAR : osg::Texture::NEAREST );
        TextureType::setFilter( osg::Texture::MAG_FILTER, m_interpolation->get( true ) ? osg::Texture::LINEAR : osg::Texture::NEAREST );
    }
}

template < typename TextureType >
void  WGETexture< TextureType >::applyUniforms( std::string prefix, osg::StateSet* states ) const
{
    states->addUniform( new WGEPropertyUniform< WPropDouble >( prefix + "Min", minimum() ) );
    states->addUniform( new WGEPropertyUniform< WPropDouble >( prefix + "Scale", scale() ) );
    states->addUniform( new WGEPropertyUniform< WPropDouble >( prefix + "Alpha", alpha() ) );
    states->addUniform( new WGEPropertyUniform< WPropBool >( prefix + "ClipZeroEnabled", clipZero() ) );
    states->addUniform( new WGEPropertyUniform< WPropBool >( prefix + "ThresholdEnabled", thresholdEnabled() ) );
    states->addUniform( new WGEPropertyUniform< WPropDouble >( prefix + "ThresholdLower", thresholdLower() ) );
    states->addUniform( new WGEPropertyUniform< WPropDouble >( prefix + "ThresholdUpper", thresholdUpper() ) );
    states->addUniform( new WGEPropertyUniform< WPropSelection >( prefix + "Colormap", colormap() ) );
    states->addUniform( new WGEPropertyUniform< WPropBool >( prefix + "Active", active() ) );
    states->addUniform( new WGEPropertyUniform< WPropBool >( prefix + "WindowEnabled", windowEnabled() ) );
    states->addUniform( new WGEPropertyUniform< WPropInterval >( prefix + "Window", window() ) );
}

template < typename TextureType >
void WGETexture< TextureType >::bind( osg::ref_ptr< osg::Node > node, size_t unit )
{
    // let our utilities do the work
    wge::bindTexture( node, osg::ref_ptr< WGETexture< TextureType > >( this ), unit ); // to avoid recursive stuff -> explicitly specify the type
}

template < typename TextureType >
void WGETexture< TextureType >::create()
{
    // do nothing. Derived classes may implement this.
}

template < typename TextureType >
void WGETexture< TextureType >::updateCallback( osg::StateAttribute* /*state*/ )
{
    // create if not done yet
    if( m_needCreate )
    {
        m_needCreate = false;
        create();
        TextureType::dirtyTextureObject();
    }
}

template < typename TextureType >
void WGETexture< TextureType >::setFilterMinMag( osg::Texture::FilterMode mode )
{
    this->setFilter( osg::Texture2D::MIN_FILTER, mode );
    this->setFilter( osg::Texture2D::MAG_FILTER, mode );
}

template < typename TextureType >
void WGETexture< TextureType >::setWrapSTR( osg::Texture::WrapMode mode )
{
    this->setWrap( osg::Texture2D::WRAP_S, mode );
    this->setWrap( osg::Texture2D::WRAP_T, mode );
    this->setWrap( osg::Texture2D::WRAP_R, mode );
}

template < typename TextureType >
void WGETexture< TextureType >::initTextureSize( osg::Texture1D* texture, int width, int /*height*/, int /*depth*/ )
{
    texture->setTextureWidth( width );
}

template < typename TextureType >
void WGETexture< TextureType >::initTextureSize( osg::Texture2D* texture, int width, int height, int /*depth*/ )
{
    texture->setTextureSize( width, height );
}

template < typename TextureType >
void WGETexture< TextureType >::initTextureSize( osg::Texture3D* texture, int width, int height, int depth )
{
    texture->setTextureSize( width, height, depth );
}

template < typename TextureType >
WBoundingBox WGETexture< TextureType >::getBoundingBox() const
{
    return WBoundingBox( 0.0, 0.0, 0.0, 1.0, 1.0, 1.0 );
}

template < typename TextureType >
WPVBaseTypes::PV_INT WGETexture< TextureType >::getUnsetSortIndex()
{
    return std::numeric_limits< WPVBaseTypes::PV_INT >::max();
}

#endif  // WGETEXTURE_H

