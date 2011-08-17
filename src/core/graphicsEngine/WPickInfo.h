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

#include "../common/math/linearAlgebra/WLinearAlgebra.h"
#include "../common/WDefines.h"
#include "WExportWGE.h"


/**
 * Encapsulates info for pick action.
 */
class WGE_EXPORT WPickInfo
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
     * Different types of mouse buttons.
     */
    typedef enum
    {
        NOMOUSE,
        MOUSE_LEFT,
        MOUSE_RIGHT,
        MOUSE_MIDDLE,
        MOUSE4,
        MOUSE5
    }
    WMouseButton;

    /**
     * Creates an object with the needed information.
     * \param name name of picked object
     * \param viewerName name of the viewer
     * \param pickPosition position where object was hit
     * \param pixelCoords pixel coordinates of the mouse
     * \param modKey relevant modifier key pressed during the pick
     * \param mButton mouse button that initiated the pick
     * \param pickNormal normal at position where object was hit. (0,0,0) means not set.
     */
    inline WPickInfo( std::string name,
                      std::string viewerName,
                      WPosition pickPosition,
                      std::pair< float, float > pixelCoords,
                      modifierKey modKey,
                      WMouseButton mButton = WPickInfo::MOUSE_LEFT,
                      WVector3d pickNormal = WVector3d() );

    /**
     * Creates an object with the empty name, zero position and no modkey.
     */
    inline WPickInfo();

    /**
     * Get the modifier key associated with the pick
     *
     * \return the mod key
     */
    inline modifierKey getModifierKey() const;

    /**
     * Get the mouse button associated with the pick
     *
     * \return the mouse button
     */
    inline WMouseButton getMouseButton() const;

    /**
     * Set the modifier key associated with the pick
     * \param modKey new modifier key
     */
    inline void setModifierKey( const modifierKey& modKey );

    /**
     * Set the modifier key associated with the pick
     * \param mButton new mouse button
     */
    inline void setMouseButton( const WMouseButton& mButton );


    /**
     * Get name of picked object.
     *
     * \return object name
     */
    inline std::string getName() const;

    /**
     * Get name of the viewer.
     *
     * \return viewer name
     */
    inline std::string getViewerName() const;

    /**
     * Get position where object was hit.
     *
     * \return the pick position
     */
    inline WPosition getPickPosition() const;

    /**
     * Get normal at position where object was hit.
     *
     * \return pick normal
     */
    inline WVector3d getPickNormal() const;

    /**
     * Returns the picked pixel coordinates in screen-space.
     *
     * \return the coordinates
     */
    inline WVector2d getPickPixel() const;

    /**
     * Tests two pick infos for equality
     * \param rhs right hand side of comparison
     *
     * \return true if equal
     */
    inline bool operator==( WPickInfo rhs ) const;

    /**
     * Tests two pick infos for inequality
     *
     * \param rhs right hand side of comparison
     *
     * \return true if not equal
     */
    inline bool operator!=( WPickInfo rhs ) const;

protected:
private:

    std::string m_name; //!< name of picked object.
    std::string m_viewerName; //!< name of the viewer
    WPosition m_pickPosition; //!< position where object was hit.
    std::pair< float, float > m_pixelCoords; //!< Pixel coordinates of the mouse.
    modifierKey m_modKey; //!< modifier key associated with the pick
    WMouseButton m_mouseButton; //!< which mouse button was used for the pick
    WVector3d m_pickNormal; //!< normal at position where object was hit.
};

WPickInfo::WPickInfo( std::string name,
                      std::string viewerName,
                      WPosition pickPosition,
                      std::pair< float, float > pixelCoords,
                      modifierKey modKey,
                      WMouseButton mButton,
                      WVector3d pickNormal ) :
    m_name( name ),
    m_viewerName( viewerName ),
    m_pickPosition( pickPosition ),
    m_pixelCoords( pixelCoords ),
    m_modKey( modKey ),
    m_mouseButton( mButton ),
    m_pickNormal( pickNormal )
{
}

WPickInfo::WPickInfo() :
    m_name( "" ),
    m_viewerName( "" ),
    m_pickPosition( WPosition() ),
    m_pixelCoords( std::make_pair( 0.0, 0.0 ) ),
    m_modKey( WPickInfo::NONE ),
    m_mouseButton( WPickInfo::MOUSE_LEFT )
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

WPickInfo::WMouseButton WPickInfo::getMouseButton() const
{
    return m_mouseButton;
}

void WPickInfo::setMouseButton( const WMouseButton& mButton )
{
    m_mouseButton = mButton;
}

std::string WPickInfo::getName() const
{
    return m_name;
}

std::string WPickInfo::getViewerName() const
{
    return m_viewerName;
}

WPosition WPickInfo::getPickPosition() const
{
    return m_pickPosition;
}

WVector3d WPickInfo::getPickNormal() const
{
    return m_pickNormal;
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

inline WVector2d WPickInfo::getPickPixel() const
{
    WVector2d v;
    v[0] = m_pixelCoords.first;
    v[1] = m_pixelCoords.second;
    return v;
}

#endif  // WPICKINFO_H
