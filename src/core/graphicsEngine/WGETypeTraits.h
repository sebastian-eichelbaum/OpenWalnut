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

#ifndef WGETYPETRAITS_H
#define WGETYPETRAITS_H

#include <stdint.h>

#include <osg/Image>

#include "WExportWGE.h"

namespace wge
{
    /**
     * Class helping to adapt types specified as template parameter into the best matching OpenGL type.
     */
    template< typename T >
    class WGE_EXPORT GLType
    {
    public:
        /**
         * The best matching OpenGL type for the specified template parameter.
         */
        typedef T Type;

        /**
         * The enum type of OpenGL
         */
        static const GLenum TypeEnum = GL_BYTE;
    };

    /**
     * Class helping to adapt types specified as template parameter into the best matching OpenGL type.
     */
    template <>
    class WGE_EXPORT GLType< double >
    {
    public:
        /**
         * The best matching OpenGL type for the specified template parameter.
         */
        typedef float Type;

        /**
         * The enum type of OpenGL
         */
        static const GLenum TypeEnum = GL_FLOAT;

        /**
         * The value for full intensity.
         *
         * \return the full intensity value
         */
        static Type FullIntensity(){ return 1.0; }  // NOLINT
    };

    /**
     * Class helping to adapt types specified as template parameter into the best matching OpenGL type.
     */
    template <>
    class WGE_EXPORT GLType< float >
    {
    public:
        /**
         * The best matching OpenGL type for the specified template parameter.
         */
        typedef float Type;

        /**
         * The enum type of OpenGL
         */
        static const GLenum TypeEnum = GL_FLOAT;

        /**
         * The value for full intensity.
         *
         * \return the full intensity value
         */
        static Type FullIntensity(){ return 1.0; }  // NOLINT
    };

    /**
     * Class helping to adapt types specified as template parameter into the best matching OpenGL type.
     */
    template <>
    class WGE_EXPORT GLType< int8_t >
    {
    public:
        /**
         * The best matching OpenGL type for the specified template parameter.
         */
        typedef int8_t Type;

        /**
         * The enum type of OpenGL
         */
        static const GLenum TypeEnum = GL_BYTE;

        /**
         * The value for full intensity.
         *
         * \return the full intensity value
         */
        static Type FullIntensity(){ return 127; }  // NOLINT
    };

    /**
     * Class helping to adapt types specified as template parameter into the best matching OpenGL type.
     */
    template <>
    class WGE_EXPORT GLType< uint8_t >
    {
    public:
        /**
         * The best matching OpenGL type for the specified template parameter.
         */
        typedef uint8_t Type;

        /**
         * The enum type of OpenGL
         */
        static const GLenum TypeEnum = GL_UNSIGNED_BYTE;

        /**
         * The value for full intensity.
         *
         * \return the full intensity value
         */
        static Type FullIntensity(){ return 255; }  // NOLINT
    };

    /**
     * Class helping to adapt types specified as template parameter into the best matching OpenGL type.
     *
     * \note integral types get downscaled to float. This is done for easiness but might be better if the real type is used instead.
     */
    template <>
    class WGE_EXPORT GLType< int16_t >
    {
    public:
        /**
         * The best matching OpenGL type for the specified template parameter.
         */
        typedef float Type;

        /**
         * The enum type of OpenGL
         */
        static const GLenum TypeEnum = GL_FLOAT;

        /**
         * The value for full intensity.
         *
         * \return the full intensity value
         */
        static Type FullIntensity(){ return 1.0; }  // NOLINT
    };

    /**
     * Class helping to adapt types specified as template parameter into the best matching OpenGL type.
     *
     * \note integral types get downscaled to float. This is done for easiness but might be better if the real type is used instead.
     */
    template <>
    class WGE_EXPORT GLType< uint16_t >
    {
    public:
        /**
         * The best matching OpenGL type for the specified template parameter.
         */
        typedef float Type;

        /**
         * The enum type of OpenGL
         */
        static const GLenum TypeEnum = GL_FLOAT;

        /**
         * The value for full intensity.
         *
         * \return the full intensity value
         */
        static Type FullIntensity(){ return 1.0; }  // NOLINT
    };

    /**
     * Class helping to adapt types specified as template parameter into the best matching OpenGL type.
     *
     * \note integral types get downscaled to float. This is done for easiness but might be better if the real type is used instead.
     */
    template <>
    class WGE_EXPORT GLType< int32_t >
    {
    public:
        /**
         * The best matching OpenGL type for the specified template parameter.
         */
        typedef float Type;

        /**
         * The enum type of OpenGL
         */
        static const GLenum TypeEnum = GL_FLOAT;

        /**
         * The value for full intensity.
         *
         * \return the full intensity value
         */
        static Type FullIntensity(){ return 1.0; }  // NOLINT
    };

    /**
     * Class helping to adapt types specified as template parameter into the best matching OpenGL type.
     *
     * \note integral types get downscaled to float. This is done for easiness but might be better if the real type is used instead.
     */
    template <>
    class WGE_EXPORT GLType< uint32_t >
    {
    public:
        /**
         * The best matching OpenGL type for the specified template parameter.
         */
        typedef float Type;

        /**
         * The enum type of OpenGL
         */
        static const GLenum TypeEnum = GL_FLOAT;

        /**
         * The value for full intensity.
         *
         * \return the full intensity value
         */
        static Type FullIntensity(){ return 1.0; }  // NOLINT
    };
}

#endif  // WGETYPETRAITS_H

