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
#include "boost/signals2/signal.hpp"
#include "boost/signals2/connection.hpp"

#include "WColor.h"

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
     *
     * \param name String with name of property
     * \param value value of the property
     * \param hidden true if property shouldn't appear in automatically created control widgets
     * \param shortDesc short description of the property
     * \param longDesc long description of the property
     */
    WProperty( std::string name, std::string value, bool hidden = false, std::string shortDesc = "", std::string longDesc = "" );
    /**
     * constructors
     *
     * \param name String with name of property
     * \param value value of the property
     * \param hidden true if property shouldn't appear in automatically created control widgets
     * \param shortDesc short description of the property
     * \param longDesc long description of the property
     */
    WProperty( std::string name, bool value, bool hidden = false, std::string shortDesc = "", std::string longDesc = "" );
    /**
     * constructors
     *
     * \param name String with name of property
     * \param value value of the property
     * \param hidden true if property shouldn't appear in automatically created control widgets
     * \param shortDesc short description of the property
     * \param longDesc long description of the property
     */
    WProperty( std::string name, char value, bool hidden = false, std::string shortDesc = "", std::string longDesc = "" );
    /**
     * constructors
     *
     * \param name String with name of property
     * \param value value of the property
     * \param hidden true if property shouldn't appear in automatically created control widgets
     * \param shortDesc short description of the property
     * \param longDesc long description of the property
     */
    WProperty( std::string name, int value, bool hidden = false, std::string shortDesc = "", std::string longDesc = "" );
    /**
     * constructors
     *
     * \param name String with name of property
     * \param value value of the property
     * \param hidden true if property shouldn't appear in automatically created control widgets
     * \param shortDesc short description of the property
     * \param longDesc long description of the property
     */
    WProperty( std::string name, float value, bool hidden = false, std::string shortDesc = "", std::string longDesc = "" );
    /**
     * constructors
     *
     * \param name String with name of property
     * \param value value of the property
     * \param hidden true if property shouldn't appear in automatically created control widgets
     * \param shortDesc short description of the property
     * \param longDesc long description of the property
     */
    WProperty( std::string name, double value, bool hidden = false, std::string shortDesc = "", std::string longDesc = "" );
    /**
     * constructors
     *
     * \param name String with name of property
     * \param value value of the property
     * \param hidden true if property shouldn't appear in automatically created control widgets
     * \param shortDesc short description of the property
     * \param longDesc long description of the property
     */
    WProperty( std::string name, WColor value, bool hidden = false, std::string shortDesc = "", std::string longDesc = "" );

    /**
     * destructor
     */
    virtual ~WProperty();

    /**
     * getter for type
     *
     * \return property type
     */
    PropertyType getType();

    /**
     * getter for name
     *
     * \return name of property
     */
    std::string getName();

    /**
     * setter for short description
     *
     * \param desc description string
     */
    void setShortDesc( const std::string desc );

    /**
     * setter for long description
     *
     * \param desc description string
     */
    void setLongDesc( const std::string desc );

    /**
     * hides property
     */
    void hide();

    /**
     * unhides property
     */
    void unhide();

    /**
     * true if property is hidden
     *
     * \return bool
     */
    bool isHidden();

    /**
     * getter for short description
     *
     * \return string with description
     */
    std::string getShortDesc();

    /**
     * getter for long description
     *
     * \return string with long description
     */
    std::string getLongDesc();

    /**
     * getter for the boost signal object that indicates a changed property
     *
     * \return signal object
     */
    boost::signals2::signal1< void, std::string >* getSignalValueChanged();

    /**
     * setter for value
     *
     * \param arg the value
     */
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

    /**
     * setter for minimum boundary
     *
     * \param arg the minimum value specified for the property
     */
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

    /**
     * setter for maximum boundary
     *
     * \param arg the maximum value specified for the property
     */
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

    /**
     * getter for value
     *
     * \return the value
     */
    template < typename T >  T  getValue()
    {
        return boost::lexical_cast< T >( m_value );
    }

    /**
     * getter for minimum value
     *
     * \return the minimum value
     */
    template < typename T >  T  getMin()
    {
        return boost::lexical_cast< T >( m_min );
    }

    /**
     * getter for maximum value
     *
     * \return the maximum value
     */
    template < typename T >  T  getMax()
    {
        return boost::lexical_cast< T >( m_max );
    }

    /**
     * getter for value as string, all value are stored internally as std::strings
     *
     * \return string with value
     */
    std::string getValueString();

private:
    /**
     * type of property
     */
    PropertyType m_type;

    /**
     * name of property
     */
    std::string m_name;

    /**
     * value of property
     */
    std::string m_value;

    /**
     * minimum value of property
     */
    std::string m_min;

    /**
     * maximum value of property
     */
    std::string m_max;

    /**
     * short description
     */
    std::string m_shortDesc;

    /**
     * long description
     */
    std::string m_longDesc;

    /**
     * flag if property should be hidden from automatic widget creation
     */
    bool m_isHidden;

    /**
     * boost signal object to indicate property changes
     */
    boost::signals2::signal1< void, std::string >m_signalValueChanged;
};

#endif  // WPROPERTY_H
