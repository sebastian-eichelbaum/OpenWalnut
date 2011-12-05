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

#ifndef WPROPERTYSTRUCT_H
#define WPROPERTYSTRUCT_H

#include <vector>
#include <string>

#include <boost/shared_ptr.hpp>

#include <boost/tuple/tuple.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/at.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>

#include "WStringUtils.h"
#include "WCondition.h"
#include "WPropertyGroupBase.h"
#include "WPropertyBase.h"
#include "WPropertyTypes.h"
#include "exceptions/WPropertyUnknown.h"

/**
 * This contains some helping classes for compile time type conversion and similar.
 */
namespace WPropertyStructHelper
{
    /**
     * Class to push a type from a sequence to the front of a tuple type
     *
     * \tparam T the sequence to convert.
     * \tparam Tuple the tuple type, getting extended with the sequence types.
     */
    template< typename T, typename Tuple>
    struct PushTypeToTupleFront;

    /**
     * Class to push a type from a sequence to the front of a tuple type. This is a specialization allowing to create a tuple from a list of
     * types.
     *
     * \tparam T the sequence to convert.
     * \tparam list of types to add to the tuple.
     */
    template< typename T, BOOST_PP_ENUM_PARAMS( 10, typename T )>
    struct PushTypeToTupleFront< T, boost::tuple< BOOST_PP_ENUM_PARAMS( 10, T ) > >
    {
        /**
         * The resulting tuple type
         */
        typedef boost::tuple< T, BOOST_PP_ENUM_PARAMS( 9, T ) > type;
    };

    /**
     * Converts a boost mpl sequence to a boost tuple
     *
     * \tparam Sequence the sequence to convert
     */
    template< typename Sequence >
    struct SequenceToTuple
    {
        /**
         * This is the tuple type for the sequence
         */
        typedef typename boost::mpl::reverse_copy<
            Sequence,
            boost::mpl::inserter<
                boost::tuple<>,
                PushTypeToTupleFront< boost::mpl::_2, boost::mpl::_1 >
            >
        >::type type;
    };

    /**
     * Alias for default type to emulate variadic templates
     */
    typedef  boost::mpl::na NOTYPE;

    /**
     * Convert a list of template parameters to a boost::mpl::vector. This is currently done using the boost::mpl no-type type. This might get a
     * problem some day?!
     *
     * \tparam T0 first type. Mandatory.
     * \tparam T1 additional type. Optional.
     * \tparam T2 additional type. Optional.
     * \tparam T3 additional type. Optional.
     * \tparam T4 additional type. Optional.
     * \tparam T5 additional type. Optional.
     * \tparam T6 additional type. Optional.
     * \tparam T7 additional type. Optional.
     * \tparam T8 additional type. Optional.
     * \tparam T9 additional type. Optional.
     */
    template<
        typename T0,
        typename T1 = NOTYPE,
        typename T2 = NOTYPE,
        typename T3 = NOTYPE,
        typename T4 = NOTYPE,
        typename T5 = NOTYPE,
        typename T6 = NOTYPE,
        typename T7 = NOTYPE,
        typename T8 = NOTYPE,
        typename T9 = NOTYPE
    >
    struct AsVector
    {
        /**
         * The template types as mpl vector
         */
        typedef boost::mpl::vector< BOOST_PP_ENUM_PARAMS( 10, T ) > type;
    };
}

/**
 * Specialization which does nothing for the NOTYPE default template parameters of \ref WPropertyStruct.
 */
template<>
struct WPropertyGroupBase::PropertyCreatorAndGroupAdder< WPropertyStructHelper::NOTYPE >
{
    /**
     * The type of the initial value.
     */
    typedef WPropertyStructHelper::NOTYPE ValueType;

    /**
     * Dummy method which does nothing for NOTYPE types.
     */
    static void createAndAdd( WPropertyGroupBase*, std::string, std::string, const ValueType& )
    {
        // NOTYPE will not cause any property creation.
    }

    /**
     * Dummy method which does nothing for NOTYPE types.
     */
    static void createAndAdd( WPropertyGroupBase*, std::string, std::string )
    {
        // NOTYPE will not cause any property creation.
    }
};


/**
 *  This is a property which encapsulates a given, fixed number of other properties. You can specify up to 10 properties. This can be seen
 *  similar to the "struct" in the C++ language. A WPropertyStruct can basically seen as \ref WPropertyGroup, but is different in a certain way:
 *  it is fixed size (defined on compile time), it allows getting each property with their correct type and provides the appearance as if this
 *  property is only ONE object and not a group of multiple objects.
 *
 *  \note the limitation to 10 types is due to the boost::tuple. If you need more, you need to replace the tuple type as storage-backend.
 *  \note if we use C++11 some day, we could use variadic templates here.
 *
 * \tparam T0 first type. Mandatory.
 * \tparam T1 additional type. Optional.
 * \tparam T2 additional type. Optional.
 * \tparam T3 additional type. Optional.
 * \tparam T4 additional type. Optional.
 * \tparam T5 additional type. Optional.
 * \tparam T6 additional type. Optional.
 * \tparam T7 additional type. Optional.
 * \tparam T8 additional type. Optional.
 * \tparam T9 additional type. Optional.
 */
template<
    typename T0,
    typename T1 = WPropertyStructHelper::NOTYPE,
    typename T2 = WPropertyStructHelper::NOTYPE,
    typename T3 = WPropertyStructHelper::NOTYPE,
    typename T4 = WPropertyStructHelper::NOTYPE,
    typename T5 = WPropertyStructHelper::NOTYPE,
    typename T6 = WPropertyStructHelper::NOTYPE,
    typename T7 = WPropertyStructHelper::NOTYPE,
    typename T8 = WPropertyStructHelper::NOTYPE,
    typename T9 = WPropertyStructHelper::NOTYPE
>
class WPropertyStruct: public WPropertyGroupBase
{
friend class WPropertyStructTest;
public:

   /**
    * The type of this template instantiation.
    */
    typedef WPropertyStruct< BOOST_PP_ENUM_PARAMS( 10, T ) > WPropertyStructType;

    /**
     * Convenience typedef for a boost::shared_ptr< WPropertyStructType >
     */
    typedef boost::shared_ptr< WPropertyStructType > SPtr;

    /**
     * Convenience typedef for a  boost::shared_ptr< const WPropertyStructType >
     */
    typedef boost::shared_ptr< const WPropertyStructType > ConstSPtr;

    /**
     * The boost mpl vector for all the types specified.
     */
    typedef typename WPropertyStructHelper::AsVector< BOOST_PP_ENUM_PARAMS( 10, T ) >::type TypeVector;

    /**
     * The type vector as a boost tuple.
     */
    typedef typename WPropertyStructHelper::SequenceToTuple< TypeVector >::type TupleType;

    /**
     * Create an empty named property.
     *
     * \param name  the name of the property
     * \param description the description of the property
     */
    WPropertyStruct( std::string name, std::string description ):
        WPropertyGroupBase( name, description )
    {
        // now create the property instances
        PropertyCreatorAndGroupAdder< T0 >::createAndAdd( this, name + "_Prop0", "No description for Property 0 in struct \"" + name + "\"." );
        PropertyCreatorAndGroupAdder< T1 >::createAndAdd( this, name + "_Prop1", "No description for Property 1 in struct \"" + name + "\"." );
        PropertyCreatorAndGroupAdder< T2 >::createAndAdd( this, name + "_Prop2", "No description for Property 2 in struct \"" + name + "\"." );
        PropertyCreatorAndGroupAdder< T3 >::createAndAdd( this, name + "_Prop3", "No description for Property 3 in struct \"" + name + "\"." );
        PropertyCreatorAndGroupAdder< T4 >::createAndAdd( this, name + "_Prop4", "No description for Property 4 in struct \"" + name + "\"." );
        PropertyCreatorAndGroupAdder< T5 >::createAndAdd( this, name + "_Prop5", "No description for Property 5 in struct \"" + name + "\"." );
        PropertyCreatorAndGroupAdder< T6 >::createAndAdd( this, name + "_Prop6", "No description for Property 6 in struct \"" + name + "\"." );
        PropertyCreatorAndGroupAdder< T7 >::createAndAdd( this, name + "_Prop7", "No description for Property 7 in struct \"" + name + "\"." );
        PropertyCreatorAndGroupAdder< T8 >::createAndAdd( this, name + "_Prop8", "No description for Property 8 in struct \"" + name + "\"." );
        PropertyCreatorAndGroupAdder< T9 >::createAndAdd( this, name + "_Prop9", "No description for Property 9 in struct \"" + name + "\"." );
    }

    /**
     * Copy constructor. Creates a deep copy of this property. As boost::signals2 and condition variables are non-copyable, new instances get
     * created. The subscriptions to a signal are LOST as well as all listeners to a condition.
     *
     * \param from the instance to copy.
     */
    explicit WPropertyStruct( const WPropertyStructType& from ):
        WPropertyGroupBase( from )
    {
        // this created a NEW update condition and NEW property instances (clones)
    }

    /**
     * Destructor.
     */
    virtual ~WPropertyStruct()
    {
        // the storing tuple is destroyed automatically and the properties if not used anymore
    }

    /**
     * Get the N'th property in the struct.
     *
     * \tparam N the number of the property to get.
     *
     * \return the property.
     */
    template< int N >
    typename boost::mpl::at< TypeVector, boost::mpl::size_t< N > >::type getProperty()
    {
        typedef typename boost::mpl::at< TypeVector, boost::mpl::size_t< N > >::type::element_type TargetType;
        return boost::shared_dynamic_cast< TargetType >( getProperty( N ) );
    }

    /**
     * Get the N'th property in the struct.
     *
     * \tparam N the number of the property to get.
     *
     * \return the property.
     */
    template< int N >
    typename boost::mpl::at< TypeVector, boost::mpl::size_t< N > >::type::element_type::ConstSPtr getProperty() const
    {
        typedef typename boost::mpl::at< TypeVector, boost::mpl::size_t< N > >::type::element_type TargetType;
        return boost::shared_dynamic_cast< const TargetType >( getProperty( N ) );
    }

    /**
     * Returns the property with the given number, but only as base type. The advantage is that the property number can be specified during
     * runtime.
     *
     * \param n the number of the property
     *
     * \return the property
     */
    const WPropertyBase::SPtr& getProperty( size_t n ) const
    {
        WPropertyStruct* nonconst = const_cast< WPropertyStruct* >( this );
        return nonconst->getProperty( n );
    }

    /**
     * Returns the property with the given number, but only as base type. The advantage is that the property number can be specified during
     * runtime.
     *
     * \param n the number of the property
     *
     * \return the property
     */
    WPropertyBase::SPtr getProperty( size_t n )
    {
       // lock, unlocked if l looses focus
        PropertySharedContainerType::ReadTicket l = m_properties.getReadTicket();
        return l->get()[ n ];
    }

    /**
     * The size of the WPropertyStruct. This returns the number of properties encapsulated.
     *
     * \return number of properties in struct
     */
    size_t size() const
    {
        return m_size;
    }

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
        return WPropertyStructType::SPtr( new WPropertyStructType( *this ) );
    }

    /**
     * Gets the real WPropertyVariable type of this instance.
     *
     * \return the real type.
     */
    virtual PROPERTY_TYPE getType() const
    {
        return PV_STRUCT;
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
        // this method splits the given string and simply forwards the call to the other properties
        std::vector< std::string > propsAsString = string_utils::tokenize( value, "|", false );
        if( size() != propsAsString.size() )
        {
            return false;
        }

        // lock, unlocked if l looses focus
        PropertySharedContainerType::ReadTicket l = m_properties.getReadTicket();
        // give the string to each property
        size_t curPropNb = 0;
        bool success = true;
        for( std::vector< std::string >::const_iterator iter = propsAsString.begin(); iter != propsAsString.end(); ++iter )
        {

            success = success && l->get()[ curPropNb ]->setAsString( *iter );
            curPropNb++;
        }
        return success;
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

        // go through and append each prop
        std::string result = "";
        for( size_t i = 0; i < size(); ++i )
        {
            result += l->get()[ i ]->getAsString() + "|";
        }
        // strip last "|"
        result.erase( result.length() - 1, 1 );
        return result;
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
        typename WPropertyStructType::SPtr v = boost::shared_dynamic_cast< WPropertyStructType >( value );
        if( !v )
        {
            // it is not a WPropertyStruct with the same type
            return false;
        }

        // lock, unlocked if l looses focus
        PropertySharedContainerType::ReadTicket l = m_properties.getReadTicket();
        PropertySharedContainerType::ReadTicket lother = v->m_properties.getReadTicket();
        bool success = true;
        // set each property
        for( size_t curPropNb = 0; curPropNb < size(); ++curPropNb )
        {
            success = success && l->get()[ curPropNb ]->set( lother->get()[ curPropNb ] );
        }

        return success;
    }

protected:

private:

    /**
     * How many elements are in this WPropertyStruct?
     */
    static const size_t m_size = boost::mpl::size< TypeVector >::value;
};

#endif  // WPROPERTYSTRUCT_H

