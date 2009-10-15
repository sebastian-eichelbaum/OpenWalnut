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

#ifndef WPROPERTIES_H
#define WPROPERTIES_H

#include <map>
#include <string>
#include <vector>

#include "WProperty.h"

/**
 * class to manage properties of an object and to provide convinience methods for easy access and manipulation
 */
class WProperties
{
public:
    /**
     * standard constructor
     */
    explicit WProperties();

    /**
     * destructor
     */
    virtual ~WProperties();

    void addBool( std::string name, bool value = false, std::string shortDesc = "", std::string longDesc = "" );
    void addChar( std::string name, char value = 0, std::string shortDesc = "", std::string longDesc = "" );
    void addInt( std::string name, int value = 0, std::string shortDesc = "", std::string longDesc = "" );
    void addFloat( std::string name, float value = 0.0, std::string shortDesc = "", std::string longDesc = "" );
    void addDouble( std::string name, double value = 0.0, std::string shortDesc = "", std::string longDesc = "" );
    void addString( std::string name, std::string value = "", std::string shortDesc = "", std::string longDesc = "" );

    std::string getValueString( std::string prop );

    template < typename T >  void setValue( std::string prop, const T& arg )
    {
        if ( findProp( prop ) )
        {
            findProp( prop )->setValue( arg );
        }
    }

    template < typename T >  T  getValue( std::string prop )
    {
        if ( findProp( prop ) )
        {
            return findProp( prop )->getValue< T >();
        }
        return 0;
    }
private:
    WProperty* findProp( std::string name );

    std::map < std::string, WProperty* >m_propertyList;
};

#endif  // WPROPERTIES_H
