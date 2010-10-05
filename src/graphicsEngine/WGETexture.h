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

#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#include <osg/Node>
#include <osg/StateSet>
#include <osg/Texture>
#include <osg/Texture1D>
#include <osg/Texture2D>
#include <osg/Texture3D>

#include "callbacks/WGEFunctorCallback.h"
#include "../common/WProperties.h"
#include "../common/WPropertyHelper.h"

#include "WGEUtils.h"

/**
 * This calls serves a simple purpose: have a texture and its scaling information together which allows very easy binding of textures to nodes
 * with associated shaders. When this texture gets bind using the bindTo methods, uniforms get added containing needed scaling information.
 */
template < typename TextureType = osg::Texture >
class WGETexture: public TextureType
{
public:
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
     * Returns the threshold property. The property can be changed. A change affects all colormaps using this texture.
     *
     * \return threshold property
     */
    WPropDouble threshold() const;

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
     * Returns the matrix used for transforming the texture coordinates to match the texture.
     *
     * \return the matrix allowing direct application to osg::TexMat.
     */
    virtual osg::Matrix getTexMatrix() const;

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

private:

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
     * Threshold for clipping areas.
     */
    WPropDouble m_threshold;

    /**
     * True if interpolation should be used.
     */
    WPropBool m_interpolation;

    /**
     * True if the texture is active.
     */
    WPropBool m_active;
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
    m_propCondition->subscribeSignal( boost::bind( &WGETexture< TextureType >::handleUpdate, this ) );

    // initialize members
    m_min = m_properties->addProperty( "Minimum", "The minimum value in the original space.", min );
    m_scale = m_properties->addProperty( "Scale", "The scaling factor to un-scale the texture values to the original space.", scale );

    m_alpha = m_properties->addProperty( "Alpha", "The alpha blending value.", 1.0, m_propCondition );
    m_alpha->setMin( 0.0 );
    m_alpha->setMax( 1.0 );

    m_threshold = m_properties->addProperty( "Threshold", "The threshold used to clip areas.", 0.0, m_propCondition );
    m_threshold->setMin( 0.0 );
    m_threshold->setMin( 1.0 );

    m_interpolation = m_properties->addProperty( "Interpolate", "Interpolation of the volume data.", true, m_propCondition );

    m_colorMapSelectionsList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_colorMapSelectionsList->addItem( "Grayscale", "" );
    m_colorMapSelectionsList->addItem( "Rainbow", "" );
    m_colorMapSelectionsList->addItem( "Hot iron", "" );
    m_colorMapSelectionsList->addItem( "Red-Yellow", "" );
    m_colorMapSelectionsList->addItem( "Atlas", "" );
    m_colorMapSelectionsList->addItem( "Blue-Green-Purple", "" );

    m_colorMap = m_properties->addProperty( "Colormap", "The colormap of this texture.", m_colorMapSelectionsList->getSelectorFirst(),
        m_propCondition
    );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_colorMap );

    m_active = m_properties->addProperty( "Active", "Can dis-enable a texture.", true, m_propCondition );

    TextureType::setResizeNonPowerOfTwoHint( false );
    TextureType::setUpdateCallback( new WGEFunctorCallback< osg::StateAttribute >(
        boost::bind( &WGETexture< TextureType >::updateCallback, this, _1 ) )
    );
}

template < typename TextureType >
WGETexture< TextureType >::WGETexture( osg::Image* image, double scale, double min ):
    TextureType( image ),
    m_propCondition( boost::shared_ptr< WCondition >( new WCondition() ) ),
    m_properties( boost::shared_ptr< WProperties >( new WProperties( "Texture Properties", "Properties of a texture." ) ) ),
    m_infoProperties( boost::shared_ptr< WProperties >( new WProperties( "Texture Info Properties", "Texture's information properties." ) ) ),
    m_needCreate( true )
{
    m_propCondition->subscribeSignal( boost::bind( &WGETexture< TextureType >::handleUpdate, this ) );

    // initialize members
    m_min = m_properties->addProperty( "Minimum", "The minimum value in the original space.", min );
    m_scale = m_properties->addProperty( "Scale", "The scaling factor to un-scale the texture values to the original space.", scale );

    m_alpha = m_properties->addProperty( "Alpha", "The alpha blending value.", 1.0, m_propCondition );
    m_alpha->setMin( 0.0 );
    m_alpha->setMax( 1.0 );

    m_threshold = m_properties->addProperty( "Threshold", "The threshold used to clip areas.", 0.0, m_propCondition );
    m_threshold->setMin( 0.0 );
    m_threshold->setMin( 1.0 );

    m_interpolation = m_properties->addProperty( "Interpolate", "Interpolation of the volume data.", true, m_propCondition );

    m_colorMapSelectionsList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_colorMapSelectionsList->addItem( "Grayscale", "" );
    m_colorMapSelectionsList->addItem( "Rainbow", "" );
    m_colorMapSelectionsList->addItem( "Hot iron", "" );
    m_colorMapSelectionsList->addItem( "Red-Yellow", "" );
    m_colorMapSelectionsList->addItem( "Atlas", "" );
    m_colorMapSelectionsList->addItem( "Blue-Green-Purple", "" );

    m_colorMap = m_properties->addProperty( "Colormap", "The colormap of this texture.", m_colorMapSelectionsList->getSelectorFirst(),
        m_propCondition
    );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_colorMap );

    m_active = m_properties->addProperty( "Active", "Can dis-enable a texture.", true, m_propCondition );

    TextureType::setResizeNonPowerOfTwoHint( false );
    TextureType::setUpdateCallback( new WGEFunctorCallback< osg::StateAttribute >(
        boost::bind( &WGETexture< TextureType >::updateCallback, this, _1 ) )
    );
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
inline WPropDouble WGETexture< TextureType >::threshold() const
{
    return m_threshold;
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
void  WGETexture< TextureType >::handleUpdate()
{
    if ( m_interpolation->changed() )
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
    states->addUniform( new WGEPropertyUniform< WPropDouble >( prefix + "Threshold", threshold() ) );
    states->addUniform( new WGEPropertyUniform< WPropSelection >( prefix + "Colormap", colormap() ) );
    states->addUniform( new WGEPropertyUniform< WPropBool >( prefix + "Active", active() ) );
}

template < typename TextureType >
void WGETexture< TextureType >::bind( osg::ref_ptr< osg::Node > node, size_t unit )
{
    // let our utilities do the work
    wge::bindTexture( node, osg::ref_ptr< WGETexture< TextureType > >( this ), unit ); // to avoid recursive stuff -> explicitly specify the type
}

template < typename TextureType >
osg::Matrix WGETexture< TextureType >::getTexMatrix() const
{
    return osg::Matrix::identity();
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
    if ( m_needCreate )
    {
        m_needCreate = false;
        create();
    }
}

#endif  // WGETEXTURE_H

