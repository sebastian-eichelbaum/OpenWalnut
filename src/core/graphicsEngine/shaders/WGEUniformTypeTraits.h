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

#ifndef WGEUNIFORMTYPETRAITS_H
#define WGEUNIFORMTYPETRAITS_H

#include <osg/Vec3>
#include <osg/Vec2>

#include "../../common/math/linearAlgebra/WLinearAlgebra.h"
#include "../../common/math/WInterval.h"

class WItemSelector;

namespace wge
{
    /**
     * Class helping to adapt types specified as template parameter into the best matching osg::Uniform (GLSL) type. This is useful especially for
     * property-types to uniform type conversion.
     * \note: bool map to bool, int to int, unsigned int to unsigned int. Unallowed types like std::string will then cause compilation errors as
     * osg::Uniform does not offer proper constructors/setters for these types.
     */
    template< typename T >
    class UniformType
    {
    public:
        /**
         * The best matching GLSL uniform type for the specified template parameter.
         */
        typedef T Type;
    };

    /**
     * Maps doubles to floats as only floats are allowed in uniforms.
     */
    template<>
    class UniformType< double >
    {
    public:
        /**
         * The best matching GLSL uniform type for the specified template parameter.
         */
        typedef float Type;
    };

    /**
     * Maps WVector3d/WPosition to osg::Vec3.
     */
    template<>
    class UniformType< WVector3d >
    {
    public:
        /**
         * The best matching GLSL uniform type for the specified template parameter.
         */
        typedef osg::Vec3 Type;
    };

    /**
     * Maps Selection Properties to ints.
     */
    template<>
    class UniformType< WItemSelector >
    {
    public:
        /**
         * The best matching GLSL uniform type for the specified template parameter.
         */
        typedef int Type;
    };

    /**
     * Maps Selection Properties to ints.
     */
    template<>
    class UniformType< WIntervalDouble >
    {
    public:
        /**
         * The best matching GLSL uniform type for the specified template parameter.
         */
        typedef osg::Vec2 Type;
    };

    /**
     * Create an instance of the uniform target type given some input type.
     *
     * \tparam InType the input type
     * \param in the input value
     *
     * \return the resulting uniform type
     */
    template< typename InType >
    typename UniformType< InType >::Type toUniformType( const InType& in )
    {
        return static_cast< typename UniformType< InType >::Type >( in );
    }

    /**
     * Creates an OSG vec2 when an interval is inserted.
     *
     * \param in the interval to convert
     *
     * \return the vector
     */
    osg::Vec2 toUniformType( const WIntervalDouble& in );
}

#endif  // WGEUNIFORMTYPETRAITS_H
