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

#ifndef WPROPERTYBASE_H
#define WPROPERTYBASE_H

#include <string>

#include <boost/function.hpp>
#include <boost/signals2/signal.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "WPropertyTypes.h"
#include "WCondition.h"
#include "WConditionSet.h"

/**
 * Abstract base class for all properties. Simply provides name and type information.
 */
class WPropertyBase: public boost::enable_shared_from_this< WPropertyBase >
{
public:

    /**
     * Create an empty named property.
     *
     * \param name  the name of the property
     * \param description the description of the property
     */
    WPropertyBase( std::string name, std::string description );

    /**
     * Destructor.
     */
    virtual ~WPropertyBase();

    /**
     * Gets the name of the class.
     *
     * \return the name.
     */
    std::string getName() const;

    /**
     * Gets the description of the property.
     *
     * \return the description
     */
    std::string getDescription() const;

    /**
     * Determines whether the property is hidden or not.
     *
     * \return true if hidden
     */
    bool isHidden() const;

    /**
     * Sets the property hidden. This flag is especially used by the GUI.
     *
     * \param hidden true if it should be hidden.
     */
    void setHidden( bool hidden = true );

    /**
     * Gets the real WPropertyVariable type of this instance.
     *
     * \return the real type.
     */
    virtual PROPERTY_TYPE getType() const;

    /**
     * This methods allows properties to be set by a string value. This is especially useful when a property is only available as string and the
     * real type of the property is unknown. This is a shortcut for casting the property and then setting the lexically casted value.
     *
     * \param value the new value to set.
     *
     * \return true if value could be set.
     */
    virtual bool setAsString( std::string value ) = 0;

    /**
     * Returns the current value as a string. This is useful for debugging or project files. It is not implemented as << operator, since the <<
     * should also print min/max constraints and so on. This simply is the value.
     *
     * \return the value as a string.
     */
    virtual std::string getAsString() = 0;

    /**
     * This method returns a condition which gets fired whenever the property changes somehow. It is fired when:
     * \li \ref setHidden is called and the hidden state changes
     * \li \ref setAsString is called and the value changes
     * \li WPropertyVariable::set is called and the value changes (regardless of suppression during set)
     * \li WPropertyVariable::setMin/setMax is called and the value changes
     * \li WPropertyVariable::addConstraint is called
     * \li WPropertyVariable::removeConstraints is called
     * \li WProperties::addProperty is called
     * \li WProperties::addPropertyGroup is called
     * This is especially useful if you simply want to know that something has happened.
     *
     * \return a condition notified whenever something changes.
     */
    virtual boost::shared_ptr< WCondition > getUpdateCondition() const;

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Helpers for easy conversion to the possible types
    /////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Helper converts this instance to its native type.
     *
     * \return the property as integer property
     */
    WPropInt toPropInt();

    /**
     * Helper converts this instance to its native type.
     *
     * \return the property as double property
     */
    WPropDouble toPropDouble();

    /**
     * Helper converts this instance to its native type.
     *
     * \return the property as bool property
     */
    WPropBool toPropBool();

    /**
     * Helper converts this instance to its native type.
     *
     * \return the property as string property
     */
    WPropString toPropString();

    /**
     * Helper converts this instance to its native type.
     *
     * \return the property as path property
     */
    WPropFilename toPropFilename();

    /**
     * Helper converts this instance to its native type.
     *
     * \return the property as selection property
     */
    WPropSelection toPropSelection();

    /**
     * Helper converts this instance to its native type.
     *
     * \return the property as color property
     */
    WPropColor toPropColor();

    /**
     * Helper converts this instance to its native type.
     *
     * \return the property as position property
     */
    WPropPosition toPropPosition();

    /**
     * Helper converts this instance to its native type.
     *
     * \return the property as trigger property
     */
    WPropTrigger toPropTrigger();

    /**
     * Helper converts this instance to its native type.
     *
     * \return the property as group
     */
    WPropGroup toPropGroup();

    /**
     * Helper converts this instance to an arbitrary type.
     *
     * \return the property of given type of NULL if not valid type
     */
    template< typename T >
    boost::shared_ptr< WPropertyVariable< T > > toPropertyVariable();

    /**
     * Signal signature emitted during set operations
     */
    typedef boost::function<void ( boost::shared_ptr< WPropertyBase > )> PropertyChangeNotifierType;

protected:

    /**
     * Name of the property.
     */
    std::string m_name;

    /**
     * Description of the property.
     */
    std::string m_description;

    /**
     * Flag denoting whether the property is hidden or not.
     */
    bool m_hidden;

    /**
     * Type of the PropertyVariable instance
     */
    PROPERTY_TYPE m_type;

    /**
     * Calculates the type of the property. This has to be done by the implementing class.
     */
    virtual void updateType();

    /**
     * Signal used for firing change signals
     */
    typedef boost::signals2::signal<void ( boost::shared_ptr< WPropertyBase >  )>  PropertyChangeSignalType;

    /**
     * Signal getting fired whenever the property changes.
     */
    PropertyChangeSignalType signal_PropertyChange;

    /**
     * Condition notified whenever something changes. See getUpdateCondition for more details.
     * \see getUpdateCondition
     */
    boost::shared_ptr< WConditionSet > m_updateCondition;

private:
};

template< typename T >
boost::shared_ptr< WPropertyVariable< T > > WPropertyBase::toPropertyVariable()
{
    return boost::shared_dynamic_cast< WPropertyVariable< T > >( shared_from_this() );
}

#endif  // WPROPERTYBASE_H

