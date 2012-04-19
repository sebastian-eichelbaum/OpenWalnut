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

#ifndef WOBJECTNDIP_H
#define WOBJECTNDIP_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "WProperties.h"

/**
 * This is a base class for everything which has a Name,Description,Icon and Properties (=NDIP). Just derive from this class and you get
 * the NDIP stuff for free. Managed for you.
 *
 * \note This is a useful base class for strategies in \ref WModule and with \ref WStrategyHelper.
 */
template< typename T >
class WObjectNDIP: public T
{
public:
   /**
     * Shared ptr to an instance.
     */
    typedef boost::shared_ptr< WObjectNDIP > SPtr;

    /**
     * Shared ptr to a const instance.
     */
    typedef boost::shared_ptr< const WObjectNDIP > ConstSPtr;

    /**
     * Destructor. Implement if you have non trivial cleanup stuff.
     */
    virtual ~WObjectNDIP();

    /**
     * The name of the object.
     *
     * \return the name
     */
    virtual std::string getName() const;

    /**
     * The description of this object.
     *
     * \return description text.
     */
    virtual std::string getDescription() const;

    /**
     * The icon of this object.
     *
     * \return the icon in XPM format. Can be NULL.
     */
    virtual const char** getIcon() const;

    /**
     * Return the property group of this object.
     *
     * \note the method is non-const to allow returning the properties as non-const
     *
     * \return  the properties.
     */
    virtual WProperties::SPtr getProperties();

protected:
    /**
     * Construct a NDIP'ed object.
     *
     * \param name the name
     * \param description the description
     * \param icon an icon in XPM format. Can be NULL if no icon is required.
     */
    WObjectNDIP( std::string name, std::string description, const char** icon = NULL );

private:
    std::string m_name; //!< the name
    std::string m_description; //!< the description
    const char** m_icon; //!< the icon
    WProperties::SPtr m_properties; //!< the properties of the object.
};

template< typename T >
WObjectNDIP< T >::WObjectNDIP( std::string name, std::string description, const char** icon ):
    m_name( name ),
    m_description( description ),
    m_icon( icon ),
    m_properties( new WProperties( name, description ) )
{
    // init
}

template< typename T >
WObjectNDIP< T >::~WObjectNDIP()
{
    // cleanup
}

template< typename T >
std::string WObjectNDIP< T >::getName() const
{
    return m_name;
}

template< typename T >
std::string WObjectNDIP< T >::getDescription() const
{
    return m_description;
}

template< typename T >
const char** WObjectNDIP< T >::getIcon() const
{
    return m_icon;
}

template< typename T >
WProperties::SPtr WObjectNDIP< T >::getProperties()
{
    return m_properties;
}


#endif  // WOBJECTNDIP_H

