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

#ifndef WPROPERTY_H
#define WPROPERTY_H

#include <string>

#include <boost/lexical_cast.hpp>
#include "../gui/qt4/signalslib.hpp"

#include "WColor.hpp"

typedef enum
{
    P_UNDEFINED,
    P_BOOL,
    P_CHAR,
    P_UNSIGNED_CHAR,
    P_INT,
    P_UNSIGNED_INT,
    P_FLOAT,
    P_DOUBLE,
    P_STRING
}
PropertyType;

/**
 * class to encapsulate one property
 */
class WProperty
{
public:
    /**
     * constructors
     */
    WProperty( std::string name, std::string value, std::string shortDesc = "", std::string longDesc = "" );
    WProperty( std::string name, bool value, std::string shortDesc = "", std::string longDesc = "" );
    WProperty( std::string name, char value, std::string shortDesc = "", std::string longDesc = "" );
    WProperty( std::string name, int value, std::string shortDesc = "", std::string longDesc = "" );
    WProperty( std::string name, float value, std::string shortDesc = "", std::string longDesc = "" );
    WProperty( std::string name, double value, std::string shortDesc = "", std::string longDesc = "" );
    WProperty( std::string name, WColor value, std::string shortDesc = "", std::string longDesc = "" );

    /**
     * destructor
     */
    virtual ~WProperty();

    PropertyType getType();
    std::string getName();

    void setShortDesc( const std::string desc );
    void setLongDesc( const std::string desc );

    void hide();
    void unhide();
    bool isHidden();

    std::string getShortDesc();
    std::string getLongDesc();

    boost::signal1< void, std::string >* getSignalValueChanged();


    template < typename T >  void  setValue( const T& arg )
    {
        try
        {
            m_value = boost::lexical_cast< std::string >( arg );
        }
        catch( const boost::bad_lexical_cast& e )
        {
            m_value = "";
        }
        m_signalValueChanged( m_name );
    }

    template < typename T >  void  setMin( const T& arg )
    {
        try
        {
            m_min = boost::lexical_cast< std::string >( arg );
        }
        catch( const boost::bad_lexical_cast& e )
        {
            m_min = "";
        }
    }

    template < typename T >  void  setMax( const T& arg )
    {
        try
        {
            m_max = boost::lexical_cast< std::string >( arg );
        }
        catch( const boost::bad_lexical_cast& e )
        {
            m_max = "";
        }
    }



    template < typename T >  T  getValue()
    {
        return boost::lexical_cast< T >( m_value );
    }

    template < typename T >  T  getMin()
    {
        return boost::lexical_cast< T >( m_min );
    }

    template < typename T >  T  getMax()
    {
        return boost::lexical_cast< T >( m_max );
    }

    std::string getValueString();

private:
    PropertyType m_type;
    std::string m_name;
    std::string m_value;
    std::string m_min;
    std::string m_max;
    std::string m_shortDesc;
    std::string m_longDesc;

    bool m_isHidden;

    boost::signal1< void, std::string >m_signalValueChanged;
};

#endif  // WPROPERTY_H
