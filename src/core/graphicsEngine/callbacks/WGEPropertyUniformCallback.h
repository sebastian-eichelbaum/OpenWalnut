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

#ifndef WGEPROPERTYUNIFORMCALLBACK_H
#define WGEPROPERTYUNIFORMCALLBACK_H

#include <osg/Uniform>

#include "../../common/WProperties.h"

#include "../shaders/WGEUniformTypeTraits.h"

/**
 * This class is an OSG Callback which allows uniforms to be controlled by properties. This is useful to simply forward properties to a shader.
 * Please note that you still need to add this callback to the desired uniforms. A convenience class is WGEPropertyUniform which uses this
 * callback. On each traversal of the OSG, the callback sets the value of the property to the uniform but does NOT reset the change flag.
 *
 * \tparam T the type used as control mechanism. The type specified must support access via T->get(). Specialize the class if
 *           you do not specify a pointer.
 */
template< typename T >
class WGEPropertyUniformCallback: public osg::Uniform::Callback
{
public:
    /**
     * Constructor. Creates the callback. You still need to add it to the desired uniform.
     *
     * \param property the property containing the value
     */
    explicit WGEPropertyUniformCallback( T property );

    /**
     * Destructor.
     */
    virtual ~WGEPropertyUniformCallback();

    /**
     * The callback. Called every render traversal for the uniform. It sets the value of the property to the uniform.
     *
     * \param uniform the uniform for which this callback is.
     * \param nv the visitor.
     */
    virtual void operator() ( osg::Uniform* uniform, osg::NodeVisitor* nv );

    /**
     * The type which is used for this uniform.
     */
    typedef typename wge::UniformType< typename T::element_type::ValueType >::Type UniformType;

protected:
    /**
     * The property
     */
    T m_property;

private:
};

template< typename T >
WGEPropertyUniformCallback< T >::WGEPropertyUniformCallback( T property ):
    osg::Uniform::Callback(),
    m_property( property )
{
    // initialize members
}

template< typename T >
WGEPropertyUniformCallback< T >::~WGEPropertyUniformCallback()
{
    // cleanup
}

template< typename T >
void WGEPropertyUniformCallback< T >::operator()( osg::Uniform* uniform, osg::NodeVisitor* /*nv*/ )
{
    uniform->set( static_cast< typename WGEPropertyUniformCallback< T >::UniformType >( m_property->get() ) );
}

#endif  // WGEPROPERTYUNIFORMCALLBACK_H


