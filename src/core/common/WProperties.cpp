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

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include "WLogger.h"
#include "exceptions/WPropertyUnknown.h"

#include "WPropertyHelper.h"

#include "WProperties.h"

WProperties::WProperties( std::string name, std::string description ):
    WPropertyBase( name, description ),
    m_properties(),
    m_childUpdateCondition( new WConditionSet() )
{
    m_updateCondition->add( m_properties.getChangeCondition() );
}

WProperties::~WProperties()
{
}

WProperties::WProperties( const WProperties& from ):
    WPropertyBase( from ),
    m_properties(),
    m_childUpdateCondition( new WConditionSet() )
{
    // copy the properties inside

    // lock, unlocked if l looses focus
    PropertySharedContainerType::ReadTicket l = from.m_properties.getReadTicket();

    // we need to make a deep copy here.
    for( PropertyConstIterator iter = l->get().begin(); iter != l->get().end(); ++iter )
    {
        // clone them to keep dynamic type
        addProperty( ( *iter )->clone() );
    }

    // unlock explicitly
    l.reset();

    // add the change condition of the prop list
    m_updateCondition->add( m_properties.getChangeCondition() );
}

boost::shared_ptr< WPropertyBase > WProperties::clone()
{
    // class copy constructor.
    return boost::shared_ptr< WProperties >( new WProperties( *this ) );
}

PROPERTY_TYPE WProperties::getType() const
{
    return PV_GROUP;
}

bool WProperties::setAsString( std::string /*value*/ )
{
    // groups can't be set in any way. -> ignore it.
    return true;
}

std::string WProperties::getAsString()
{
    // groups can't be set in any way. -> ignore it.
    return "";
}

/**
 * Add the default constraints for a certain type of property. By default, nothing is added.
 *
 * \note Information properties never get constraints by default
 *
 * \param prop the property
 *
 * \return the property inserted gets returned.
 */
template< typename T >
T _addDefaultConstraints( T prop )
{
    return prop;
}

/**
 * Add the default constraints for a certain type of property. For ints, a min and max is set to 0 and 100.
 *
 * \note Information properties never get constraints by default
 *
 * \param prop the property
 *
 * \return the property inserted gets returned.
 */
WPropInt _addDefaultConstraints( WPropInt prop )
{
    prop->setMin( 0 );
    prop->setMax( 100 );
    return prop;
}

/**
 * Add the default constraints for a certain type of property. For doubles, a min and max is set to 0 and 100.
 *
 * \note Information properties never get constraints by default
 *
 * \param prop the property
 *
 * \return the property inserted gets returned.
 */
WPropDouble _addDefaultConstraints( WPropDouble prop )
{
    prop->setMin( 0.0 );
    prop->setMax( 100.0 );
    return prop;
}

/**
 * Add the default constraints for a certain type of property. For selections, the PC_ISVALID constraint is added.
 *
 * \note Information properties never get constraints by default
 *
 * \param prop the property
 *
 * \return the property inserted gets returned.
 */
WPropSelection _addDefaultConstraints( WPropSelection prop )
{
    WPropertyHelper::PC_ISVALID::addTo( prop );
    return prop;
}

/**
 * Add the default constraints for a certain type of property. For filenames, the PC_NOTEMPTY constraint is added.
 *
 * \note Information properties never get constraints by default
 *
 * \param prop the property
 *
 * \return the property inserted gets returned.
 */
WPropFilename _addDefaultConstraints( WPropFilename prop )
{
    WPropertyHelper::PC_NOTEMPTY::addTo( prop );
    return prop;
}

/**
 * Add the default constraints for a certain type of property. Please specialize _addDefaultConstraints for your special needs and prop types.
 *
 * \note Information properties never get constraints by default
 *
 * \param prop the property to add the constraints to
 *
 * \return the property inserted
 */
template< typename T >
T addDefaultConstraints( T prop )
{
    if( prop->getPurpose() == PV_PURPOSE_INFORMATION )
    {
        return prop;
    }

    return _addDefaultConstraints( prop );
}

bool WProperties::set( boost::shared_ptr< WPropertyBase > /*value*/ )
{
    return true;
}

bool WProperties::propNamePredicate( boost::shared_ptr< WPropertyBase > prop1, boost::shared_ptr< WPropertyBase > prop2 ) const
{
    return ( prop1->getName() == prop2->getName() );
}
void WProperties::removeProperty( boost::shared_ptr< WPropertyBase > prop )
{
    if( !prop )
    {
        return;
    }

    // lock, unlocked if l looses focus
    PropertySharedContainerType::WriteTicket l = m_properties.getWriteTicket();
    l->get().erase( std::remove( l->get().begin(), l->get().end(), prop ), l->get().end() );
    m_childUpdateCondition->remove( prop->getUpdateCondition() );
}

boost::shared_ptr< WPropertyBase > WProperties::findProperty( const WProperties* const props, std::string name ) const
{
    boost::shared_ptr< WPropertyBase > result = boost::shared_ptr< WPropertyBase >();

    // lock, unlocked if l looses focus
    PropertySharedContainerType::ReadTicket l = props->m_properties.getReadTicket();

    // iterate over the items
    for( PropertyContainerType::const_iterator it = l->get().begin(); it != l->get().end(); ++it )
    {
        if( ( *it )->getName() == name )
        {
            result = ( *it );
            break;
        }
    }

    // done. Unlocked after l looses focus.
    return result;
}

boost::shared_ptr< WPropertyBase > WProperties::findProperty( std::string name ) const
{
    boost::shared_ptr< WPropertyBase > result = boost::shared_ptr< WPropertyBase >();

    // tokenize the name -> contains any paths?
    typedef boost::tokenizer<boost::char_separator< char > > tokenizer;
    boost::char_separator< char > sep( "/" );   // separate by /
    tokenizer tok( name, sep );

    // iterate along the path
    const WProperties* curProps = this;       // the group currently in while traversing the path
    for( tokenizer::iterator it = tok.begin(); it != tok.end(); ++it )
    {
        // was the last token not a group?
        if( result && ( result->getType() != PV_GROUP ) )
        {
            // no it wasn't. This means that one token inside the path is no group, but it needs to be one
            return boost::shared_ptr< WPropertyBase >();
        }

        // get the properties along the path
        result = findProperty( curProps, boost::lexical_cast< std::string >( *it ) );
        if( !result )
        {
            // not found? Return NULL.
            return boost::shared_ptr< WPropertyBase >();
        }
        else if( result && ( result->getType() == PV_GROUP ) )
        {
            // it is a group. Go down
            curProps = result->toPropGroup().get();
        }
    }

    return result;
}

bool WProperties::existsProperty( std::string name )
{
    return ( findProperty( name ) != boost::shared_ptr< WPropertyBase >() );
}

boost::shared_ptr< WPropertyBase > WProperties::getProperty( std::string name )
{
    boost::shared_ptr< WPropertyBase > p = findProperty( name );
    if( p == boost::shared_ptr< WPropertyBase >() )
    {
        throw WPropertyUnknown( std::string( "Property \"" + name + "\" can't be found." ) );
    }

    return p;
}

WProperties::PropertySharedContainerType::ReadTicket WProperties::getProperties() const
{
    return m_properties.getReadTicket();
}

WProperties::PropertySharedContainerType::ReadTicket WProperties::getReadTicket() const
{
    return m_properties.getReadTicket();
}

WPropGroup WProperties::addPropertyGroup( std::string name, std::string description, bool hide )
{
    WPropGroup p = WPropGroup( new WProperties( name, description ) );
    p->setHidden( hide );
    addProperty( p );
    return p;
}

void WProperties::clear()
{
    // lock, unlocked if l looses focus
    PropertySharedContainerType::WriteTicket l = m_properties.getWriteTicket();
    l->get().clear();
}

boost::shared_ptr< WCondition > WProperties::getChildUpdateCondition() const
{
    return m_childUpdateCondition;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// convenience methods for
// template< typename T>
// boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial, bool hide = false );
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WPropBool WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_BOOL >( name, description, initial, hide ) );
}

WPropInt WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_INT >( name, description, initial, hide ) );
}

WPropDouble WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_DOUBLE >( name, description, initial, hide ) );
}

WPropString WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_STRING >( name, description, initial, hide ) );
}

WPropFilename WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_PATH >( name, description, initial, hide ) );
}

WPropSelection WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_SELECTION&   initial, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_SELECTION >( name, description, initial, hide ) );
}

WPropPosition WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_POSITION >( name, description, initial, hide ) );
}

WPropColor WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&   initial, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_COLOR >( name, description, initial, hide ) );
}

WPropTrigger WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_TRIGGER&   initial, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_TRIGGER >( name, description, initial, hide ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// convenience methods for
// template< typename T>
// boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
//                                                          boost::shared_ptr< WCondition > condition, bool hide = false );
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WPropBool WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial,
                                     boost::shared_ptr< WCondition > condition, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_BOOL >( name, description, initial, condition, hide ) );
}

WPropInt WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial,
                                    boost::shared_ptr< WCondition > condition, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_INT >( name, description, initial, condition, hide ) );
}

WPropDouble WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial,
                                       boost::shared_ptr< WCondition > condition, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_DOUBLE >( name, description, initial, condition, hide ) );
}

WPropString WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial,
                                       boost::shared_ptr< WCondition > condition, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_STRING >( name, description, initial, condition, hide ) );
}

WPropFilename WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial,
                                         boost::shared_ptr< WCondition > condition, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_PATH >( name, description, initial, condition, hide ) );
}

WPropSelection WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_SELECTION&   initial,
                                          boost::shared_ptr< WCondition > condition, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_SELECTION >( name, description, initial, condition, hide ) );
}

WPropPosition WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial,
                                         boost::shared_ptr< WCondition > condition, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_POSITION >( name, description, initial, condition, hide ) );
}

WPropColor WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&   initial,
                                     boost::shared_ptr< WCondition > condition, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_COLOR >( name, description, initial, condition, hide ) );
}

WPropTrigger WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_TRIGGER&   initial,
                                       boost::shared_ptr< WCondition > condition, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_TRIGGER >( name, description, initial, condition, hide ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// convenience methods for
// template< typename T>
// boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
//                                                          WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WPropBool WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_BOOL >( name, description, initial, notifier, hide ) );
}

WPropInt WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial,
                                    WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_INT >( name, description, initial, notifier, hide ) );
}

WPropDouble WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial,
                                       WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_DOUBLE >( name, description, initial, notifier, hide ) );
}

WPropString WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial,
                                       WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_STRING >( name, description, initial, notifier, hide ) );
}

WPropFilename WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial,
                                         WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_PATH >( name, description, initial, notifier, hide ) );
}

WPropSelection WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_SELECTION&   initial,
                                          WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_SELECTION >( name, description, initial, notifier, hide ) );
}

WPropPosition WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial,
                                         WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_POSITION >( name, description, initial, notifier, hide ) );
}

WPropColor WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&   initial,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_COLOR >( name, description, initial, notifier, hide ) );
}

WPropTrigger WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_TRIGGER&   initial,
                                       WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_TRIGGER >( name, description, initial, notifier, hide ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// convenience methods for
// template< typename T>
// boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
//                                                          boost::shared_ptr< WCondition > condition,
//                                                          WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WPropBool WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_BOOL >( name, description, initial, condition, notifier, hide ) );
}

WPropInt WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_INT >( name, description, initial, condition, notifier, hide ) );
}

WPropDouble WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_DOUBLE >( name, description, initial, condition, notifier, hide ) );
}

WPropString WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_STRING >( name, description, initial, condition, notifier, hide ) );
}

WPropFilename WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_PATH >( name, description, initial, condition, notifier, hide ) );
}

WPropSelection WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_SELECTION&   initial,
                                          boost::shared_ptr< WCondition > condition,
                                          WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_SELECTION >( name, description, initial, condition, notifier, hide ) );
}

WPropPosition WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial,
                                         boost::shared_ptr< WCondition > condition,
                                         WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_POSITION >( name, description, initial, condition, notifier, hide ) );
}

WPropColor WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&   initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_COLOR >( name, description, initial, condition, notifier, hide ) );
}

WPropTrigger WProperties::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_TRIGGER&   initial,
                                       boost::shared_ptr< WCondition > condition,
                                       WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_TRIGGER >( name, description, initial, condition, notifier, hide ) );
}

