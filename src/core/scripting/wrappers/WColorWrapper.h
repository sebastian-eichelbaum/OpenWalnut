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

#ifndef WCOLORWRAPPER_H_
#define WCOLORWRAPPER_H_

#include "../../common/WColor.h"

/**
 * Wraps the WColor class for scripts.
 */
class WColorWrapper
{
public:
    /**
     * Contructs a new wrapper.
     *
     * \param r The red component in [0,1].
     * \param g The green component in [0,1].
     * \param b The blue component in [0,1].
     * \param a The alpha component in [0,1].
     */
    WColorWrapper( float r, float g, float b, float a )
        : m_color( r, g, b, a )
    {
    }

    /**
     * Destructor.
     */
    ~WColorWrapper()
    {
    }

    /**
     * Get the red component.
     *
     * \return The red component.
     */
    float getR() const
    {
        return m_color[ 0 ];
    }

    /**
     * Get the green component.
     *
     * \return The green component.
     */
    float getG() const
    {
        return m_color[ 1 ];
    }

    /**
     * Get the blue component.
     *
     * \return The blue component.
     */
    float getB() const
    {
        return m_color[ 2 ];
    }

    /**
     * Get the alpha component.
     *
     * \return The alpha component.
     */
    float getA() const
    {
        return m_color[ 3 ];
    }

    /**
     * Set the red component.
     *
     * \param r The red component.
     */
    void setR( float r )
    {
        m_color[ 0 ] = r;
    }

    /**
     * Set the green component.
     *
     * \param g The green component.
     */
    void setG( float g )
    {
        m_color[ 1 ] = g;
    }

    /**
     * Set the blue component.
     *
     * \param b The blue component.
     */
    void setB( float b )
    {
        m_color[ 2 ] = b;
    }

    /**
     * Set the alpha component.
     *
     * \param a The alpha component.
     */
    void setA( float a )
    {
        m_color[ 3 ] = a;
    }

    /**
     * Get the wrapped color.
     *
     * \return The color.
     */
    WColor const& get() const
    {
        return m_color;
    }

private:
    //! The color.
    WColor m_color;
};

#endif  // WCOLORWRAPPER_H_

