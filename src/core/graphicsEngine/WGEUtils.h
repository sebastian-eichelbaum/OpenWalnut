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

#ifndef WGEUTILS_H
#define WGEUTILS_H

#include <string>
#include <vector>

#include <osg/Array>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Camera>
#include <osg/Uniform>

#include "../common/WColor.h"
#include "../common/WBoundingBox.h"
#include "../common/WAssert.h"
#include "../common/WPropertyVariable.h"
#include "../graphicsEngine/shaders/WGEPropertyUniform.h"

namespace wge
{
    /**
     * Enable transparency for the given node. This enabled blending and sets the node to the transparency bin.
     *
     * \param node the node
     */
    void enableTransparency( osg::ref_ptr< osg::Node > node );

    /**
     * Transforms a direction given via two points into a RGB color.
     *
     * \param pos1 First point
     * \param pos2 Second point
     *
     * \return converts a vector to a color
     */
    WColor getRGBAColorFromDirection( const WPosition &pos1, const WPosition &pos2 );

    /**
     * Converts a whole vector of WPositions into an osg::Vec3Array.
     *
     * \param posArray The given positions vector
     *
     * \return Refernce to the same vector but as osg::Vec3Array.
     */
    osg::ref_ptr< osg::Vec3Array > osgVec3Array( const std::vector< WPosition >& posArray );

    /**
     * Converts screen coordinates into Camera coordinates.
     *
     * \param screen the screen coordinates
     * \param camera The matrices of this camera will used for unprojecting.
     *
     * \return un-projects a screen coordinate back to world space
     */
    osg::Vec3 unprojectFromScreen( const osg::Vec3 screen, osg::ref_ptr< osg::Camera > camera  );

    /**
     * Converts screen coordinates into Camera coordinates.
     * \note this method can be useful to work with vectors (w component 0)
     *
     * \param screen the screen coordinates
     * \param camera The matrices of this camera will used for unprojecting.
     *
     * \return un-projects a screen coordinate back to world space
     */
    osg::Vec4 unprojectFromScreen( const osg::Vec4 screen, osg::ref_ptr< osg::Camera > camera  );

    /**
     * creates the same color as the atlas colormap shader from the index
     *
     * \param index unsigned char that indexes the color
     * \return the color
     */
    WColor createColorFromIndex( int index );

    /**
     * creates a rgb WColor from a HSV value
     * \param h hue
     * \param s saturation
     * \param v value
     * \return the color
     */
    WColor createColorFromHSV( int h, float s = 1.0, float v = 1.0 );

    /**
     * creates the nth color of a partition of the hsv color circle
     *
     * \param n number of the color
     * \return the color
     */
    WColor getNthHSVColor( int n );

    /**
     * Creates a osg::Uniform with given type and name and applies it to the given node.
     *
     * \tparam T This is the data used for the uniform. It may be a PropertyType, an integral or an osg::Uniform.
     *
     * \param node Node where the uniform should be bound to.
     * \param prop The type of the uniform.
     * \param name The name of the uniform.
     */
    template< typename T >
    void bindAsUniform( osg::Node* node, T prop, std::string name );

    /**
     * Generate a proxy cube, which ensures OSG does proper near-far plane calculation and culling. This is especially useful if you create some
     * geometry and modify it on the GPU by shaders. In these cases, OSG will not properly cull and near-far clip. This cull proxy is basically a
     * cube, which gets shrinked to zero size on the GPU. This ensures you cannot see it, but it makes OSG see you proper bounding volume.
     *
     * \param bbox the bounding box to cover
     *
     * \return the proxy. Add it to your scene root.
     */
    osg::ref_ptr< osg::Node > generateCullProxy( const WBoundingBox& bbox );

    /**
     * Generate a proxy cube, which ensures OSG does proper near-far plane calculation and culling. This is especially useful if you create some
     * geometry and modify it on the GPU by shaders. In these cases, OSG will not properly cull and near-far clip. This cull proxy is basically a
     * cube, which gets shrinked to zero size on the GPU. This ensures you cannot see it, but it makes OSG see you proper bounding volume.
     *
     * \note dynamic cull proxys use the getBound method of the specified node before culling. So if you implement objects that change their size
     * dynamically, add a  osg::Node::ComputeBoundingSphereCallback to them
     *
     * \note This method uses update callbacks. Remember to add your own callbacks via addUpdateCallback, instead of setUpdateCallback.
     *
     * \param node the node
     *
     * \return the proxy. Add it to your scene root.
     */
    osg::ref_ptr< osg::Node > generateDynamicCullProxy( osg::ref_ptr< osg::Node > node );
}

inline WColor wge::getRGBAColorFromDirection( const WPosition &pos1, const WPosition &pos2 )
{
    WPosition direction( normalize( pos2 - pos1 ) );
    return WColor( std::abs( direction[0] ), std::abs( direction[1] ), std::abs( direction[2] ), 1.0f );
}

namespace wge
{
    template< typename T >
    inline void bindAsUniform( osg::Node* node, T prop, std::string name )
    {
        osg::ref_ptr< osg::Uniform > uniform = new osg::Uniform( name.c_str(), prop );
        osg::StateSet *states = node->getOrCreateStateSet();
        states->addUniform( uniform );
    }

    /**
     * Template specialization for double values.
     *
     * \param node Node where the uniform should be bound to.
     * \param prop The type of the uniform.
     * \param name The name of the uniform.
     */
    template<>
    inline void bindAsUniform< double >( osg::Node* node, double prop, std::string name )
    {
        osg::ref_ptr< osg::Uniform > uniform( new osg::Uniform( name.c_str(), static_cast< float >( prop ) ) );
        osg::StateSet *states = node->getOrCreateStateSet();
        states->addUniform( uniform );
    }

    /**
     * Template specialization for size_t values.
     *
     * \param node Node where the uniform should be bound to.
     * \param prop The type of the uniform.
     * \param name The name of the uniform.
     */
    template<>
    inline void bindAsUniform< size_t >( osg::Node* node, size_t prop, std::string name )
    {
        osg::ref_ptr< osg::Uniform > uniform( new osg::Uniform( name.c_str(), static_cast< int >( prop ) ) );
        osg::StateSet *states = node->getOrCreateStateSet();
        states->addUniform( uniform );
    }

    /**
     * Template specialization for WPropDouble values.
     *
     * \param node Node where the uniform should be bound to.
     * \param prop The type of the uniform.
     * \param name The name of the uniform.
     */
    template<>
    inline void bindAsUniform< WPropDouble >( osg::Node* node, WPropDouble prop, std::string name )
    {
        osg::ref_ptr< osg::Uniform > uniform( new WGEPropertyUniform< WPropDouble >( name, prop ) );
        osg::StateSet *states = node->getOrCreateStateSet();
        states->addUniform( uniform );
    }

    /**
     * Template specialization for WPropColor values.
     *
     * \param node Node where the uniform should be bound to.
     * \param prop The type of the uniform.
     * \param name The name of the uniform.
     */
    template<>
    inline void bindAsUniform< WPropColor >( osg::Node* node, WPropColor prop, std::string name )
    {
        osg::ref_ptr< osg::Uniform > uniform( new WGEPropertyUniform< WPropColor >( name, prop ) );
        osg::StateSet *states = node->getOrCreateStateSet();
        states->addUniform( uniform );
    }

    /**
     * Template specialization for osg::Uniform values.
     *
     * \param node Node where the uniform should be bound to.
     * \param uniform The type of the uniform.
     */
    template<>
    inline void bindAsUniform< osg::ref_ptr< osg::Uniform > >( osg::Node* node, osg::ref_ptr< osg::Uniform > uniform, std::string /* name */ )
    {
        osg::StateSet *states = node->getOrCreateStateSet();
        states->addUniform( uniform );
    }
}
#endif  // WGEUTILS_H

