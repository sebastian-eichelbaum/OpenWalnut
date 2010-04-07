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

#ifndef WPROPERTIESOLD_H
#define WPROPERTIESOLD_H

#include <map>
#include <string>
#include <vector>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread.hpp>

#include "WPropertyOld.h"

/**
 * class to manage properties of an object and to provide convinience methods for easy access and manipulation
 */
class WPropertiesOld
{
public:
    /**
     * standard constructor
     */
    WPropertiesOld();

    /**
     * destructor
     */
    virtual ~WPropertiesOld();

    /**
     * sets a flag hidden, which can be used by the datasetbrowser for instance
     *
     * \param name of the property to hide
     */
    void hideProperty( std::string name );

    /**
     * sets a flag hidden, which can be used by the datasetbrowser for instance
     *
     * \param name name of the property to unhide
     */
    void unhideProperty( std::string name );

    /**
     * returns a vector to the properties sorted by their insertion order
     */
    std::vector< WPropertyOld* >& getPropertyVector();

    /**
     * adds a boolean property to the list of properties
     *
     * \param name of the property
     * \param value of the property
     * \param hidden true if hidden from automatic widget generation
     * \param shortDesc short description
     * \param longDesc long description
     *
     * \return pointer to boost signal object that is fired whern a property changes
     */
    boost::signals2::signal1< void, std::string >*
        addBool( std::string name, bool value = false, bool hidden = false, std::string shortDesc = "", std::string longDesc = "" );

    /**
     * adds a char property to the list of properties
     *
     * \param name of the property
     * \param value of the property
     * \param hidden true if hidden from automatic widget generation
     * \param shortDesc short description
     * \param longDesc long description
     *
     * \return pointer to boost signal object that is fired whern a property changes
     */
    boost::signals2::signal1< void, std::string >*
        addChar( std::string name, char value = 0, bool hidden = false, std::string shortDesc = "", std::string longDesc = "" );

    /**
     * adds an integer property to the list of properties
     *
     * \param name of the property
     * \param value of the property
     * \param hidden true if hidden from automatic widget generation
     * \param shortDesc short description
     * \param longDesc long description
     *
     * \return pointer to boost signal object that is fired whern a property changes
     */
    boost::signals2::signal1< void, std::string >*
        addInt( std::string name, int value = 0, bool hidden = false, std::string shortDesc = "", std::string longDesc = "" );

    /**
     * adds a float property to the list of properties
     *
     * \param name of the property
     * \param value of the property
     * \param hidden true if hidden from automatic widget generation
     * \param shortDesc short description
     * \param longDesc long description
     *
     * \return pointer to boost signal object that is fired whern a property changes
     */
    boost::signals2::signal1< void, std::string >*
        addFloat( std::string name, float value = 0.0, bool hidden = false, std::string shortDesc = "", std::string longDesc = "" );

    /**
     * adds a double property to the list of properties
     *
     * \param name of the property
     * \param value of the property
     * \param hidden true if hidden from automatic widget generation
     * \param shortDesc short description
     * \param longDesc long description
     *
     * \return pointer to boost signal object that is fired whern a property changes
     */
    boost::signals2::signal1< void, std::string >*
        addDouble( std::string name, double value = 0.0, bool hidden = false, std::string shortDesc = "", std::string longDesc = "" );

    /**
     * adds a string property to the list of properties
     *
     * \param name of the property
     * \param value of the property
     * \param hidden true if hidden from automatic widget generation
     * \param shortDesc short description
     * \param longDesc long description
     *
     * \return pointer to boost signal object that is fired whern a property changes
     */
    boost::signals2::signal1< void, std::string >*
        addString( std::string name, std::string value = "", bool hidden = false, std::string shortDesc = "", std::string longDesc = "" );

    /**
     * adds a color property to the list of properties
     *
     * \param name of the property
     * \param value of the property
     * \param hidden true if hidden from automatic widget generation
     * \param shortDesc short description
     * \param longDesc long description
     *
     * \return pointer to boost signal object that is fired whern a property changes
     */
    boost::signals2::signal1< void, std::string >*
        addColor( std::string name, WColor value, bool hidden = false, std::string shortDesc = "", std::string longDesc = "" );

    /**
     * adds a color property to the list of properties
     *
     * \param name of the property
     * \param value of the property
     * \param hidden true if hidden from automatic widget generation
     * \param shortDesc short description
     * \param longDesc long description
     *
     * \return pointer to boost signal object that is fired whern a property changes
     */
    template< typename T> boost::signals2::signal1< void, std::string >*
    addProperty( std::string name, T value, bool hidden = false, std::string shortDesc = "", std::string longDesc = "" );

    /**
     * Simply insert the specified property to the list.
     *
     * \param prop the property to add
     */
    void addProperty( WPropertyOld* prop );

    /**
     * getter for the value of a property as std string
     *
     * \param prop the name of the property
     * \return string of property
     */
    std::string getValueString( const std::string prop );

    /**
     * sets the value of an existing property
     *
     * \param prop string with name of property
     * \param arg value
     */
    template < typename T >  void setValue( std::string prop, const T& arg )
    {
        boost::shared_lock< boost::shared_mutex > slock;
        slock = boost::shared_lock< boost::shared_mutex >( m_updateLock );

        if( findProp( prop ) )
        {
            findProp( prop )->setValue( arg );
        }

        slock.unlock();
    }

    /**
     * sets the minimum value of an existing property
     *
     * \param prop string with name of property
     * \param arg value
     */
    template < typename T >  void setMin( std::string prop, const T& arg )
    {
        if( findProp( prop ) )
        {
            findProp( prop )->setMin( arg );
        }
    }

    /**
     * sets the maximum value of an existing property
     *
     * \param prop string with name of property
     * \param arg value
     */
    template < typename T >  void setMax( std::string prop, const T& arg )
    {
        if( findProp( prop ) )
        {
            findProp( prop )->setMax( arg );
        }
    }

    /**
     * returns the value of an existing property
     *
     * \param prop name of the property
     * \return the value
     */
    template < typename T >  T  getValue( std::string prop )
    {
        if( findProp( prop ) )
        {
            return findProp( prop )->getValue< T >();
        }

        // TODO(schurade): do not use NULL as return value in this case. Throw exception instead.
        return 0;
    }

    /**
     * returns the minimum value of an existing property
     *
     * \param prop name of the property
     * \return the minimum value
     */
    template < typename T >  T  getMin( std::string prop )
    {
        if( findProp( prop ) )
        {
            return findProp( prop )->getMin< T >();
        }

        // TODO(schurade): do not use NULL as return value in this case. Throw exception instead.
        return 0;
    }

    /**
     * returns the maximum value of an existing property
     *
     * \param prop name of the property
     * \return the maximum value
     */
    template < typename T >  T  getMax( std::string prop )
    {
        if( findProp( prop ) )
        {
            return findProp( prop )->getMax< T >();
        }

        // TODO(schurade): do not use NULL as return value in this case. Throw exception instead.
        return 0;
    }

    /**
     * helper function that finds a property by its name
     *
     * \param name name of searched property.
     */
    bool existsProp( std::string name );

    /**
     * If there are WPropertyOld objects which are marked as dirty, then their
     * signals are reemitted.
     */
    void reemitChangedValueSignals();

    /**
     * Indicates if at least one WPropertyOld is dirty or all are clean.
     *
     * \return True if at least one property is dirty, false otherwise
     */
    bool isDirty() const;

    /**
     * helper function that finds a property by its name
     *
     * \param name the name of the property
     * \return pointer to a WPropertyOld object
     */
    WPropertyOld* findProp( std::string name );

private:
    /**
     * map of properties for easy access with name string
     */
    std::map < std::string, WPropertyOld* > m_propertyList;

    /**
     * vector of properties to retain the order of creation so that widgets created from this
     * properties object always look the same
     */
    std::vector< WPropertyOld* > m_propertyVector;

    /**
     * boost mutex object for thread safety of updating of properties
     */
    boost::shared_mutex m_updateLock;
};

template< typename T > boost::signals2::signal1< void, std::string >* WPropertiesOld::addProperty( std::string name, T value, bool hidden,
        std::string shortDesc, std::string longDesc )
{
    WPropertyOld* prop = new WPropertyOld( name, value, hidden, shortDesc, longDesc );
    m_propertyList[name] = prop;
    m_propertyVector.push_back( prop );
    return prop->getSignalValueChanged();
}

#endif  // WPROPERTIESOLD_H
