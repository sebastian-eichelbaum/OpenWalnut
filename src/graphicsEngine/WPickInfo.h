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

#include "../math/WPosition.h"

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
        SHIFT,
        STRG,
        ALT,
        WIN
    };

    /**
     * Creates an object with the needed information.
     * \param name name of picked object
     * \param pickPosition position where object was hit
     * \param modKey relevant modifier key pressed during the pick
     */
    WPickInfo( std::string name, wmath::WPosition pickPosition, WPickInfo::modifierKey modKey );

    /**
     * Get the modifier key associated with the pick
     */
    inline modifierKey getModifierKey();

    /**
     * Get name of picked object.
     */
    inline std::string getName();

    /**
     * Get position where object was hit.
     */
    inline wmath::WPosition getPickPosition();

protected:
private:

    std::string m_name; //!< name of picked object.
    wmath::WPosition m_pickPosition; //!< position where object was hit.
    modifierKey m_modKey; //!< modifier key associated with the pick
};

WPickInfo::WPickInfo( std::string name, wmath::WPosition pickPosition, WPickInfo::modifierKey modKey ) :
    m_name( name ),
    m_pickPosition( pickPosition ),
    m_modKey( modKey )
{
}

WPickInfo::modifierKey WPickInfo::getModifierKey()
{
    return m_modKey;
}

std::string WPickInfo::getName()
{
    return m_name;
}

wmath::WPosition WPickInfo::getPickPosition()
{
    return m_pickPosition;
}

#endif  // WPICKINFO_H
