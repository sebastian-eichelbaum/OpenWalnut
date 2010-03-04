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

#ifndef WPICKINFO_H
#define WPICKINFO_H

#include <string>
#include <utility>

#include "../common/math/WPosition.h"
#include "../common/math/WVector3D.h"

/**
 * Encapsulates info for pick action.
 */
class WPickInfo
{
public:
    /**
     * Different types of modifier keys.
     */
    enum modifierKey
    {
        NONE,
        SHIFT,
        STRG,
        ALT,
        WIN
    };

    /**
     * Creates an object with the needed information.
     * \param name name of picked object
     * \param pickPosition position where object was hit
     * \param pixelCoords pixel coordinates fo the mouse
     * \param modKey relevant modifier key pressed during the pick
     * \param pickNormal normal at position where object was hit. (0,0,0) means not set.
     */
    inline WPickInfo( std::string name,
                      wmath::WPosition pickPosition,
                      std::pair< float, float > pixelCoords,
                      modifierKey modKey,
                      wmath::WVector3D pickNormal = wmath::WVector3D() );

    /**
     * Creates an object with the empty name, zero position and no modkey.
     */
    inline WPickInfo();

    /**
     * Get the modifier key associated with the pick
     */
    inline modifierKey getModifierKey() const;

    /**
     * Set the modifier key associated with the pick
     * \param modKey new modifier key
     */
    inline void setModifierKey( const modifierKey& modKey );

    /**
     * Get name of picked object.
     */
    inline std::string getName() const;

    /**
     * Get position where object was hit.
     */
    inline wmath::WPosition getPickPosition() const;

    /**
     * Get normal at position where object was hit.
     */
    inline wmath::WVector3D getPickNormal() const;

    /**
     * Get pixel coordinates where object was hit.
     */
    inline std::pair< float, float > getPickPixelPosition() const;

    /**
     * Tests two pick infos for equality
     * \param rhs right hand side of comparison
     */
    inline bool operator==( WPickInfo rhs ) const;

    /**
     * Tests two pick infos for inequality
     * \param rhs right hand side of comparison
     */
    inline bool operator!=( WPickInfo rhs ) const;

protected:
private:

    std::string m_name; //!< name of picked object.
    wmath::WPosition m_pickPosition; //!< position where object was hit.
    std::pair< float, float > m_pixelCoords; //!< Pixel coordinates of the mouse.
    modifierKey m_modKey; //!< modifier key associated with the pick
    wmath::WVector3D m_pickNormal; //!< normal at position where object was hit.
};

WPickInfo::WPickInfo( std::string name,
                      wmath::WPosition pickPosition,
                      std::pair< float, float > pixelCoords,
                      modifierKey modKey,
                      wmath::WVector3D pickNormal ) :
    m_name( name ),
    m_pickPosition( pickPosition ),
    m_pixelCoords( pixelCoords ),
    m_modKey( modKey ),
    m_pickNormal( pickNormal )
{
}

WPickInfo::WPickInfo() :
    m_name( "" ),
    m_pickPosition( wmath::WPosition() ),
    m_pixelCoords( std::make_pair( 0, 0 ) ),
    m_modKey( WPickInfo::NONE )
{
}

WPickInfo::modifierKey WPickInfo::getModifierKey() const
{
    return m_modKey;
}

void WPickInfo::setModifierKey( const modifierKey& modKey )
{
    m_modKey = modKey;
}


std::string WPickInfo::getName() const
{
    return m_name;
}

wmath::WPosition WPickInfo::getPickPosition() const
{
    return m_pickPosition;
}

wmath::WVector3D WPickInfo::getPickNormal() const
{
    return m_pickNormal;
}

std::pair< float, float > WPickInfo::getPickPixelPosition() const
{
    return m_pixelCoords;
}

inline bool WPickInfo::operator==( WPickInfo rhs ) const
{
    return ( this->m_name == rhs.m_name
             && this->m_pickPosition == rhs.m_pickPosition
             && this->m_modKey == rhs.m_modKey );
}

inline bool WPickInfo::operator!=( WPickInfo rhs ) const
{
    return !( *this == rhs );
}

#endif  // WPICKINFO_H
