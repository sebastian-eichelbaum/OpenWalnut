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

#ifndef WGESHADERPROPERTYDEFINE_H
#define WGESHADERPROPERTYDEFINE_H

#include <string>
#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>

#include "../../common/WPropertyTypes.h"
#include "../../common/WPropertyVariable.h"

#include "WGEShaderDefine.h"

#include "../WExportWGE.h"

/**
 * This class is able to provide arbitrary values as define statements in GLSL code. Unlike WGEShaderDefine, it is automatically controlled by a
 * WPropertyVariable. You might hope that WPropBool define and undefine this thing. Thats not the case. A WPropBool causes this to be 0 or 1. If
 * you need the first behavior, use WGEShaderDefineOptions or WGEShaderPropertyDefineOptions.
 */
template< typename PropertyType = WPropBool >
class WGEShaderPropertyDefine: public WGEShaderDefine< typename PropertyType::element_type::ValueType >
{
public:
    /**
     * Shared pointer for this class.
     */
    typedef boost::shared_ptr< WGEShaderPropertyDefine< PropertyType > > SPtr;

    /**
     * A const shared pointer for this class.
     */
    typedef boost::shared_ptr< const WGEShaderPropertyDefine< PropertyType > > ConstSPtr;

    /**
     * Constructs a define with a given name and initial value.
     *
     * \param name name of the define
     * \param prop the property controlling this define
     */
    WGEShaderPropertyDefine( std::string name, PropertyType prop  );

    /**
     * Destructor.
     */
    virtual ~WGEShaderPropertyDefine();

protected:
private:
    /**
     * The associated property.
     */
    PropertyType m_property;

    /**
     * Sets the value depending on the current value of the property.
     */
    void setNewValue();

    /**
     * The connection between the prop and the define
     */
    boost::signals2::connection m_connection;
};

template< typename PropertyType >
WGEShaderPropertyDefine< PropertyType >::WGEShaderPropertyDefine( std::string name, PropertyType prop ):
    WGEShaderDefine< typename PropertyType::element_type::ValueType >( name, prop->get() ),
    m_property( prop )
{
    // initialize
    m_connection = prop->getValueChangeCondition()->subscribeSignal( boost::bind( &WGEShaderPropertyDefine< PropertyType >::setNewValue, this ) );
}

template< typename PropertyType >
WGEShaderPropertyDefine< PropertyType >::~WGEShaderPropertyDefine()
{
    // cleanup
    m_connection.disconnect();
}

template< typename PropertyType >
void WGEShaderPropertyDefine< PropertyType >::setNewValue()
{
    WGEShaderDefine< typename PropertyType::element_type::ValueType >::setValue( m_property->get() );
}

#endif  // WGESHADERPROPERTYDEFINE_H

