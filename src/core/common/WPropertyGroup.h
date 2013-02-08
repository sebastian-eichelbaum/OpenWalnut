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

#ifndef WPROPERTYGROUP_H
#define WPROPERTYGROUP_H

#include <map>
#include <string>
#include <vector>

#include "WConditionSet.h"
#include "WPropertyBase.h"
#include "WPropertyGroupBase.h"
#include "WPropertyTypes.h"
#include "WPropertyVariable.h"
#include "WSharedSequenceContainer.h"
#include "exceptions/WPropertyNotUnique.h"



/**
 * Class to manage properties of an object and to provide convenience methods for easy access and manipulation. It also allows
 * thread safe iteration on its elements. The main purpose of this class is to group properties together and to allow searching properties by a
 * given name. The name of each property in a group has to be unique and is constructed using the group names containing them: hello/you/property
 * is the property with the name "property" in the group "you" which against is in the group "hello".
 * \note The root group of each module does not have a name.
 */
class WPropertyGroup: public WPropertyGroupBase
{
friend class WPropertiesTest;
public:
    // the following typedefs are for convenience.
    typedef boost::shared_ptr< WPropertyGroup > SPtr; //!< shared pointer to object of this type
    typedef boost::shared_ptr< const WPropertyGroup > ConstSPtr; //!< const shared pointer to object of this type
    typedef WPropertyGroup* Ptr; //!< pointer to object of this type
    typedef const WPropertyGroup* ConstPtr; //!< const pointer to object of this type
    typedef WPropertyGroup& Ref; //!< ref to object of this type
    typedef const WPropertyGroup& ConstRef; //!< const ref to object of this type

    /**
     * For shortening: a type defining a shared vector of WSubject pointers.
     */
    typedef WPropertyGroupBase::PropertyContainerType PropertyContainerType;

    /**
     * The alias for a shared container.
     */
    typedef WPropertyGroupBase::PropertySharedContainerType PropertySharedContainerType;

    /**
     * The const iterator type of the container.
     */
    typedef WPropertyGroupBase::PropertyConstIterator PropertyConstIterator;

    /**
     * The iterator type of the container.
     */
    typedef WPropertyGroupBase::PropertyIterator PropertyIterator;

    /**
     * Constructor. Creates an empty list of properties.
     *
     * \note WModule::getProperties always returns an unnamed instance.
     *
     * \param name the name of the property group. The GUI is using this name for naming the tabs/group boxes
     * \param description the description of the group.
     */
    WPropertyGroup( std::string name = "unnamed group", std::string description = "an unnamed group of properties" );

    /**
     * Copy constructor. Creates a deep copy of this property. As boost::signals2 and condition variables are non-copyable, new instances get
     * created. The subscriptions to a signal are LOST as well as all listeners to a condition.
     * The conditions you can grab using getValueChangeConditon and getCondition are not the same as in the original! This is because
     * the class corresponds to the observer/observable pattern. You won't expect a clone to fire a condition if a original flag is changed
     * (which after cloning is completely decoupled from the clone).
     *
     * \note the properties inside this list are also copied deep
     *
     * \param from the instance to copy.
     */
    explicit WPropertyGroup( const WPropertyGroup& from );

    /**
     * destructor
     */
    virtual ~WPropertyGroup();

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // The WPropertyBase interface
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * This method clones a property and returns the clone. It does a deep copy and, in contrast to a copy constructor, creates property with the
     * correct type without explicitly requiring the user to specify it. It creates a NEW change condition and change signal. This means, alls
     * subscribed signal handlers are NOT copied.
     *
     * \note this simply ensures the copy constructor of the runtime type is issued.
     *
     * \return the deep clone of this property.
     */
    virtual boost::shared_ptr< WPropertyBase > clone();

    /**
     * Gets the real type of this instance. In this case, PV_GROUP.
     *
     * \return the real type.
     */
    virtual PROPERTY_TYPE getType() const;

    /**
     * This methods allows properties to be set by a string value. This method does nothing here, as groups can not be set in any kind.
     *
     * \param value the new value to set. IGNORED.
     *
     * \return always true
     */
    virtual bool setAsString( std::string value );

    /**
     * Returns the current value as a string. This is useful for debugging or project files. It is not implemented as << operator, since the <<
     * should also print min/max constraints and so on. This simply is the value.
     *
     * \return the value as a string.
     */
    virtual std::string getAsString();

    /**
     * Sets the value from the specified property to this one. This is especially useful to copy a value without explicitly casting/knowing the
     * dynamic type of the property.
     * For WPropertyGroup, this tries to set the contained properties to the value of the given group. It does not add/remove properties. It
     * simply sets the children values to the ones given.
     *
     * \param value the new value.
     * \param recommendedOnly if true, property types which support recommended values apply the given value as recommendation.
     *
     * \return true if the values of the children could be set. If one could not be set, false
     */
    virtual bool set( boost::shared_ptr< WPropertyBase > value, bool recommendedOnly = false );

    /**
     * This method is a special version of \ref WPropertyBase::set for groups. It allows to set the values of the contained properties. It does
     * not add nor remove properties. It searches the property by name (recursively) and sets the value from the specified property group's
     * property. You can use the exclusion list to exclude a certain property.
     *
     * \param value the source values
     * \param exclude a list of property names to exclude. Use complete names here, which means, when having nested groups, apply the rules
     *                defined in \ref WPropertyGroupBase::findProperty.
     * \param recommendedOnly if true, property types which support recommended values apply the given value as recommendation.
     *
     * \return true if the values of the children could be set. If one could not be set, false
     */
    virtual bool set( boost::shared_ptr< WPropertyGroup > value, std::vector< std::string > exclude = std::vector< std::string >(),
                                                                 bool recommendedOnly = false );

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Extend the WPropertyGroupBase to allow the property list to be modified
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Removes all properties from the list.
     */
    virtual void clear();

    /**
     * Insert the specified property into the list.
     *
     * \param prop the property to add
     *
     * \return The given prop.
     */
    template< typename PropType >
    PropType addProperty( PropType prop );

    /**
     * Remove the specified property from the list. If the given property is not in the list, nothing happens.
     *
     * \param prop the property to remove.
     */
    void removeProperty( boost::shared_ptr< WPropertyBase > prop );

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Convenience methods to create and add properties
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Create and add a new property group. Use these groups to structure your properties.
     *
     * \param name the name of the group.
     * \param description the description of the group.
     * \param hide true if group should be completely hidden.
     *
     * \return The newly created property group.
     */
    WPropGroup addPropertyGroup( std::string name, std::string description, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    template< typename T>
    boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    template< typename T>
    boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
                                                             boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    template< typename T>
    boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
                                                             WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    template< typename T>
    boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
                                                             boost::shared_ptr< WCondition > condition,
                                                             WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Convenience methods to create and add properties
    // NOTE: these methods use the type of the initial parameter to automatically use the proper type.
    // This works, since the compiler always calls the function with the best matching parameter types.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // convenience methods for
    // template< typename T>
    // boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial, bool hide = false );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropBool      addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     * It also sets the min and max constraint to [0,100].
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropInt       addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     * It also sets the min and max constraint to [0,100].
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropDouble    addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropString    addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropFilename  addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropSelection      addProperty( std::string name, std::string description, const WPVBaseTypes::PV_SELECTION&   initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropPosition      addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropColor         addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&  initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropTrigger       addProperty( std::string name, std::string description, const WPVBaseTypes::PV_TRIGGER&  initial, bool hide = false );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // convenience methods for
    // template< typename T>
    // boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
    //                                                          boost::shared_ptr< WCondition > condition, bool hide = false );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropBool      addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     * It also sets the min and max constraint to [0,100].
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropInt       addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     * It also sets the min and max constraint to [0,100].
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropDouble    addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropString    addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropFilename  addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropSelection addProperty( std::string name, std::string description, const WPVBaseTypes::PV_SELECTION&   initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropPosition  addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropColor     addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&   initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropTrigger   addProperty( std::string name, std::string description, const WPVBaseTypes::PV_TRIGGER&   initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // convenience methods for
    // template< typename T>
    // boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
    //                                                          WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropBool      addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     * It also sets the min and max constraint to [0,100].
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropInt       addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     * It also sets the min and max constraint to [0,100].
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropDouble    addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropString    addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropFilename  addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropSelection addProperty( std::string name, std::string description, const WPVBaseTypes::PV_SELECTION&   initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropPosition  addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION& initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropColor     addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&  initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropTrigger   addProperty( std::string name, std::string description, const WPVBaseTypes::PV_TRIGGER&  initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // convenience methods for
    // template< typename T>
    // boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
    //                                                          boost::shared_ptr< WCondition > condition,
    //                                                          WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropBool      addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     * It also sets the min and max constraint to [0,100].
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropInt       addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     * It also sets the min and max constraint to [0,100].
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropDouble    addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );


    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropString    addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropFilename  addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropSelection addProperty( std::string name, std::string description, const WPVBaseTypes::PV_SELECTION&   initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropPosition  addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropColor     addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&   initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropTrigger   addProperty( std::string name, std::string description, const WPVBaseTypes::PV_TRIGGER&   initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

protected:
    /**
     * This function implements the set functionality. It works recursively and keeps track of the current path.
     *
     * \param value the value source
     * \param path the current path inside the source property group
     * \param exclude exclude list with paths relative to the original source group
     * \param recommendedOnly recommendation flag.
     *
     * \return true if everything was set successfully.
     */
    virtual bool setImpl( boost::shared_ptr< WPropertyGroup > value, std::string path = "",
                                                                     std::vector< std::string > exclude = std::vector< std::string >(),
                                                                     bool recommendedOnly = false );

private:
};

template< typename PropType >
PropType WPropertyGroup::addProperty( PropType prop )
{
    addArbitraryProperty( prop );
    return prop;
}

template< typename T>
boost::shared_ptr< WPropertyVariable< T > > WPropertyGroup::addProperty( std::string name, std::string description, const T& initial, bool hide )
{
    boost::shared_ptr< WPropertyVariable< T > > p = boost::shared_ptr< WPropertyVariable< T > >(
            new WPropertyVariable< T >( name, description, initial )
    );
    p->setHidden( hide );
    addProperty( p );
    return p;
}

template< typename T>
boost::shared_ptr< WPropertyVariable< T > > WPropertyGroup::addProperty( std::string name, std::string description, const T& initial,
                                                                       boost::shared_ptr< WCondition > condition, bool hide )
{
    boost::shared_ptr< WPropertyVariable< T > > p = boost::shared_ptr< WPropertyVariable< T > >(
            new WPropertyVariable< T >( name, description, initial, condition )
    );
    p->setHidden( hide );
    addProperty( p );
    return p;
}

template< typename T>
boost::shared_ptr< WPropertyVariable< T > > WPropertyGroup::addProperty( std::string name, std::string description, const T& initial,
                                                                       WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    boost::shared_ptr< WPropertyVariable< T > > p = boost::shared_ptr< WPropertyVariable< T > >(
            new WPropertyVariable< T >( name, description, initial, notifier )
    );
    p->setHidden( hide );
    addProperty( p );
    return p;
}

template< typename T>
boost::shared_ptr< WPropertyVariable< T > > WPropertyGroup::addProperty( std::string name, std::string description, const T& initial,
                                                                       boost::shared_ptr< WCondition > condition,
                                                                       WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    boost::shared_ptr< WPropertyVariable< T > > p = boost::shared_ptr< WPropertyVariable< T > >(
            new WPropertyVariable< T >( name, description, initial, condition, notifier )
    );
    p->setHidden( hide );
    addProperty( p );
    return p;
}

#endif  // WPROPERTYGROUP_H
