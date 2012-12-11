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

#ifndef WGEPROPERTYUNIFORM_H
#define WGEPROPERTYUNIFORM_H

#include <string>

#include <osg/Uniform>

#include "../callbacks/WGEPropertyUniformCallback.h"
#include "WGEUniformTypeTraits.h"


/**
 * Class implementing a uniform which can be controlled by a property instance. This is mainly a convenience class for
 * WGEPropertyUniformCallback (which is used here).
 *
 * \tparam the class used as controlling mechanism. The class needs to be a boost::shared_ptr to a type supporting get() method: T->get()
 * returns the value (bool, int, double, WPosition supported). For other types specialize the template.
 */
template< typename T >
class WGEPropertyUniform: public osg::Uniform
{
public:
    /**
     * Convenience typedef for an osg::ref_ptr
     */
    typedef osg::ref_ptr< WGEPropertyUniform > RefPtr;

    /**
     * Convenience typedef for an osg::ref_ptr; const
     */
    typedef osg::ref_ptr< const WGEPropertyUniform > ConstRefPtr;

    /**
     * Creates a new uniform with controlled by the specified property.
     *
     * \param name  the name of the uniform; consider our style guide for uniform names.
     * \param property the property controlling it
     */
    WGEPropertyUniform( std::string name, T property );

    /**
     * Destructor.
     */
    virtual ~WGEPropertyUniform();

    /**
     * The type which is used for this uniform.
     */
    typedef typename WGEPropertyUniformCallback< T >::UniformType UniformType;

protected:
    /**
     * The property controlling the uniform.
     */
    T m_property;

    /**
     * The name of the uniform.
     */
    std::string m_name;
private:
};

template < typename T >
WGEPropertyUniform< T >::WGEPropertyUniform( std::string name, T property ):
    osg::Uniform( name.c_str(), typename WGEPropertyUniform< T >::UniformType() ),
    m_property( property ),
    m_name( name )
{
    // simply create a new callback and add it
    setUpdateCallback( new WGEPropertyUniformCallback< T >( property ) );
}

template < typename T >
WGEPropertyUniform< T >::~WGEPropertyUniform()
{
    // clean up
}

#endif  // WGEPROPERTYUNIFORM_H

