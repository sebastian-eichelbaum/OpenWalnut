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

#ifndef WTRAITSBASE_H
#define WTRAITSBASE_H

#include <string>

/**
 * Base class to all custom Value Traits for CxxTest. This is mainly not to
 * rewrite the asString method again and again until my fingers are bleeding
 * and hurt! ;)
 */
class WTraitsBase
{
public:
    /**
     * \return A WFiber as char array aka string
     */
    const char *asString() const;

protected:
    std::string m_s; //!< storing the string representation for output
};

inline const char* WTraitsBase::asString() const
{
    return m_s.c_str();
}
#endif  // WTRAITSBASE_H
