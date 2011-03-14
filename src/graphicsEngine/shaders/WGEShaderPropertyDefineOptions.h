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

#ifndef WGESHADERPROPERTYDEFINEOPTIONS_H
#define WGESHADERPROPERTYDEFINEOPTIONS_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include "boost/tuple/tuple.hpp"
#include <boost/signals2.hpp>

#include "../../common/WProperties.h"
#include "../../common/WPropertyTypes.h"
#include "../../common/exceptions/WPreconditionNotMet.h"

#include "WGEShaderDefineOptions.h"

template< typename PropType >
class WGEShaderPropertyDefineOptionsIndexAdapter;


/**
 * This is a WGEShaderDefineOptions class which additionally uses a property to automatically control the active options. This is very useful if
 * you have some WPropInt or WPropSelection which controls some features in your shader. Especially with WPropSelection Instances, you can even
 * activate multiple options if your selection allows this ( see WPropertyVariable<>::PropertyConstraint for details ). If used with a WPropBool,
 * it is useful to switch on/off an option for example.
 *
 * \note You can use inherited WGEShaderDefineOptions methods too. This might create some kind of inconsistency since they of course do not
 * update the property.
 */
template< typename PropType = WPropSelection, typename PropIndexAdapter = WGEShaderPropertyDefineOptionsIndexAdapter< PropType > >
class WGEShaderPropertyDefineOptions: public WGEShaderDefineOptions
{
public:

    /**
     * Convenience typedef for a boost_shared_ptr< WGEShaderPropertyDefineOptions >.
     */
    typedef boost::shared_ptr< WGEShaderPropertyDefineOptions > SPtr;

    /**
     * Convenience typedef for a boost_shared_ptr< const WGEShaderPropertyDefineOptions >.
     */
    typedef boost::shared_ptr< const WGEShaderPropertyDefineOptions > ConstSPtr;

    /**
     * Create a new instance of this class. The first option is mandatory and is set as default. The specified property controls the activations.
     *
     * \param prop the property controlling this thing.
     *
     * \param first fist option. Is default.
     * \param option2 another option
     * \param option3 another option
     * \param option4 another option
     * \param option5 another option
     * \param option6 another option
     * \param option7 another option
     * \param option8 another option
     * \param option9 another option
     * \param option10 another option
     */
    WGEShaderPropertyDefineOptions( PropType prop, std::string first,
                                    std::string option2 = "", std::string option3 = "", std::string option4 = "", std::string option5 = "",
                                    std::string option6 = "", std::string option7 = "", std::string option8 = "", std::string option9 = "",
                                    std::string option10 = "" );

    /**
     * Create a new instance of this class. The first option is mandatory and is set as default. The specified property controls the activations.
     *
     * \param prop the property controlling this thing.
     *
     * \param options the list of options. Must have a size greater 0.
     */
    WGEShaderPropertyDefineOptions( PropType prop, std::vector< std::string > options );

    /**
     * Destructor.
     */
    virtual ~WGEShaderPropertyDefineOptions();

    /**
     * Returns the property associated with this instance.
     *
     * \return
     */
    PropType getProperty() const;

protected:

private:

    /**
     * The property controlling this instance and the active options list.
     */
    PropType m_property;

    /**
     * The connection associated with the properties update condition.
     */
    boost::signals2::connection m_connection;

    /**
     * Called by the property update mechanism. This handles the new value in the property.
     */
    void propUpdated();
};

/**
 * Contains some utility functions related to the WGEShaderPropertyDefineOptions class.
 */
namespace WGEShaderPropertyDefineOptionsTools
{
    /**
     * This tuple contains name, description and define-name of an option.
     */
    typedef boost::tuple< std::string, std::string, std::string > NameDescriptionDefineTuple;

    /**
     * A little bit more comfortable way to create a list of shader-defines and the corresponding property.
     *
     * \param propName the name of the property to create
     * \param propDescription the description of the property to create
     * \param propGroup the owning group of the property
     * \param defines the list of names, descriptions and defines
     *
     * \return a WGEShaderPropertyDefineOptions instance associated with a new property. This can be acquired using getProperty().
     */
    WGEShaderPropertyDefineOptions< WPropSelection >::SPtr createSelection( std::string propName, std::string propDescription,
                                                                                               WProperties::SPtr propGroup,
                                                                                               std::vector< NameDescriptionDefineTuple > defines );
}

/**
 * Class converts the specified property value to an index list. The generic case for all int-castable property types is trivial. WPropSelection
 * is a specialization of this class.
 *
 * \tparam PropType The property. WPropInt for example.
 */
template< typename PropType >
class WGEShaderPropertyDefineOptionsIndexAdapter
{
public:
    /**
     * The type of the index-list to create.
     */
    typedef typename WGEShaderPropertyDefineOptions< PropType >::IdxList IdxList;

    /**
     *  Converts the specified property value to an index list.
     *
     * \param in the value to convert to an index list
     *
     * \return the new index list
     */
    IdxList operator()( const typename PropType::element_type::ValueType& in ) const
    {
        return IdxList( 1, typename IdxList::value_type( in ) );
    }
};

/**
 * Class converts the specified property value to an index list. The generic case for all int-castable property types is trivial. This is the
 * specialization for WPropSelection which allows multiple options to be active if the selection has multiple selected items.
 *
 * \tparam PropType The property. WPropInt for example.
 */
template<>
class WGEShaderPropertyDefineOptionsIndexAdapter< WPropSelection >
{
public:
    /**
     * The type of the index-list to create.
     */
    typedef WGEShaderPropertyDefineOptions< WPropSelection >::IdxList IdxList;

    /**
     *  Converts the specified property value to an index list.
     *
     * \param in the value to convert to an index list
     *
     * \return the new index list
     */
    IdxList operator()( const WPVBaseTypes::PV_SELECTION& in ) const
    {
        return in.getIndexList();
    }
};

template< typename PropType, typename PropIndexAdapter >
WGEShaderPropertyDefineOptions< PropType, PropIndexAdapter >::WGEShaderPropertyDefineOptions( PropType prop, std::string first,
                            std::string option2, std::string option3, std::string option4, std::string option5,
                            std::string option6, std::string option7, std::string option8, std::string option9,
                            std::string option10 ):
    WGEShaderDefineOptions( first, option2, option3, option4, option5, option6, option7, option8, option9, option10 ),
    m_property( prop )
{
    // if the prop changes -> update options
    m_connection = m_property->getValueChangeCondition()->subscribeSignal(
        boost::bind( &WGEShaderPropertyDefineOptions< PropType >::propUpdated, this )
    );
    propUpdated();
}

template< typename PropType, typename PropIndexAdapter >
WGEShaderPropertyDefineOptions< PropType, PropIndexAdapter >::WGEShaderPropertyDefineOptions( PropType prop, std::vector< std::string > options ):
    WGEShaderDefineOptions( options ),
    m_property( prop )
{
    // if the prop changes -> update options
    m_connection = m_property->getValueChangeCondition()->subscribeSignal(
        boost::bind( &WGEShaderPropertyDefineOptions< PropType >::propUpdated, this )
    );
    propUpdated();
}

template< typename PropType, typename PropIndexAdapter >
WGEShaderPropertyDefineOptions< PropType, PropIndexAdapter >::~WGEShaderPropertyDefineOptions()
{
    // cleanup
    m_connection.disconnect();
}

template< typename PropType, typename PropIndexAdapter >
void WGEShaderPropertyDefineOptions< PropType, PropIndexAdapter >::propUpdated()
{
    PropIndexAdapter functor;
    setActivationList( functor( m_property->get() ) );
}

template< typename PropType, typename PropIndexAdapter >
PropType WGEShaderPropertyDefineOptions< PropType, PropIndexAdapter >::getProperty() const
{
    return m_property;
}

#endif  // WGESHADERPROPERTYDEFINEOPTIONS_H

