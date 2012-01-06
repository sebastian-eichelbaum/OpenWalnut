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

#ifndef WPROPERTYLIST_H
#define WPROPERTYLIST_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "WPropertyGroupBase.h"
#include "WPropertyTypes.h"

/**
 * This is a dynamic property list. With its help, users can dynamically add items.
 *
 * \tparam T This is a property type. The list will then contain several of these properties.
 */
template< typename T >
class WPropertyList: public WPropertyGroupBase
{
public:
    /**
     * The type of property to store in this list.
     */
    typedef T ValueType;

    /**
     * Abbreviation for this template with the current value type.
     */
    typedef WPropertyList< ValueType > WPropertyListType;

    /**
     * Convenience typedef for a boost::shared_ptr< WPropertyList >.
     */
    typedef boost::shared_ptr< WPropertyList< ValueType > > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WPropertyList >.
     */
    typedef boost::shared_ptr< const WPropertyList< ValueType > > ConstSPtr;

    /**
     * Create an empty named property.
     *
     * \param name  the name of the property
     * \param description the description of the property
     */
    WPropertyList( std::string name, std::string description ):
        WPropertyGroupBase( name, description )
    {
        // nothing to do here. The list is initially empty
    }

    /**
     * Copy constructor. Creates a deep copy of this property. As boost::signals2 and condition variables are non-copyable, new instances get
     * created. The subscriptions to a signal are LOST as well as all listeners to a condition.
     *
     * \param from the instance to copy.
     */
    explicit WPropertyList( const WPropertyListType& from ):
        WPropertyGroupBase( from )
    {
        // this created a NEW update condition and NEW property instances (clones)
    }

    /**
     * Destructor.
     */
    virtual ~WPropertyList()
    {
        // storing structure is destroyed automatically in WPropertyGroupBase.
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // The WPropertyList specific stuff
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // The WPropertyBase specific stuff
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * This method clones a property and returns the clone. It does a deep copy and, in contrast to a copy constructor, creates property with the
     * correct type without explicitly requiring the user to specify it. It creates a NEW change condition and change signal. This means, alls
     * subscribed signal handlers are NOT copied.
     *
     * \note this simply ensures the copy constructor of the runtime type is issued.
     *
     * \return the deep clone of this property.
     */
    virtual WPropertyBase::SPtr clone()
    {
        // just use the copy constructor
        return WPropertyListType::SPtr( new WPropertyListType( *this ) );
    }

    /**
     * Gets the real WPropertyVariable type of this instance.
     *
     * \return the real type.
     */
    virtual PROPERTY_TYPE getType() const
    {
        return PV_LIST;
    }

    /**
     * This methods allows properties to be set by a string value. This is especially useful when a property is only available as string and the
     * real type of the property is unknown. This is a shortcut for casting the property and then setting the lexically casted value.
     *
     * \param value the new value to set.
     *
     * \return true if value could be set.
     */
    virtual bool setAsString( std::string value )
    {
        return false;
    }

    /**
     * Returns the current value as a string. This is useful for debugging or project files. It is not implemented as << operator, since the <<
     * should also print min/max constraints and so on. This simply is the value.
     *
     * \return the value as a string.
     */
    virtual std::string getAsString()
    {
        // lock, unlocked if l looses focus
        PropertySharedContainerType::ReadTicket l = m_properties.getReadTicket();
        return "";
    }

    /**
     * Sets the value from the specified property to this one. This is especially useful to copy a value without explicitly casting/knowing the
     * dynamic type of the property.
     *
     * \param value the new value.
     *
     * \return true if the value has been accepted.
     */
    virtual bool set( boost::shared_ptr< WPropertyBase > value )
    {
        // is this the same type as we are?
        typename WPropertyListType::SPtr v = boost::shared_dynamic_cast< WPropertyListType >( value );
        if( !v )
        {
            // it is not a WPropertyList with the same type
            return false;
        }
    }

protected:
private:
};

#endif  // WPROPERTYLIST_H

