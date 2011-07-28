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

#ifndef WGESHADERDEFINE_H
#define WGESHADERDEFINE_H

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include "WGEShaderPreprocessor.h"

/**
 * This class is able to provide arbitrary values as define statements in GLSL code.
 */
template< typename ValueType = bool >
class WGEShaderDefine: public WGEShaderPreprocessor
{
public:
    /**
     * Shared pointer for this class.
     */
    typedef boost::shared_ptr< WGEShaderDefine< ValueType > > SPtr;

    /**
     * A const shared pointer for this class.
     */
    typedef boost::shared_ptr< const WGEShaderDefine< ValueType > > ConstSPtr;

    /**
     * Constructs a define with a given name and initial value.
     *
     * \param name name of the define
     * \param value the initial value.
     */
    WGEShaderDefine( std::string name, ValueType value = ValueType( 0 ) );

    /**
     * Destructor.
     */
    virtual ~WGEShaderDefine();

    /**
     * Process the whole code. It is not allowed to modify some internal state in this function because it might be called by several shaders.
     *
     * \param code the code to process
     * \param file the filename of the shader currently processed. Should be used for debugging output.
     *
     * \return the resulting new code
     */
    virtual std::string process( const std::string& file, const std::string& code ) const;

    /**
     * Returns the name of the define.
     *
     * \return the name
     */
    std::string getName() const;

    /**
     * Returns the current value.
     *
     * \return the current value
     */
    const ValueType& getValue() const;

    /**
     * Sets the new value for this define. Causes an reload of all associated shaders.
     *
     * \param value the new value.
     */
    void setValue( const ValueType& value );

protected:

private:

    /**
     * The name of the define.
     */
    std::string m_name;

    /**
     * The value of the define as a string.
     */
    ValueType m_value;
};

template< typename ValueType >
WGEShaderDefine< ValueType >::WGEShaderDefine( std::string name, ValueType value ):
    WGEShaderPreprocessor(),
    m_name( name ),
    m_value( value )
{
    // initialize
}

template< typename ValueType >
WGEShaderDefine< ValueType >::~WGEShaderDefine()
{
    // cleanup
}

template< typename ValueType >
std::string WGEShaderDefine< ValueType >::process( const std::string& /*file*/, const std::string& code ) const
{
    if( !getActive() )
    {
        return code;
    }

    return "#define " + getName() + " " + boost::lexical_cast< std::string >( getValue() ) + "\n" + code;
}

template< typename ValueType >
std::string WGEShaderDefine< ValueType >::getName() const
{
    return m_name;
}

template< typename ValueType >
const ValueType& WGEShaderDefine< ValueType >::getValue() const
{
    return m_value;
}

template< typename ValueType >
void WGEShaderDefine< ValueType >::setValue( const ValueType& value )
{
    m_value = value;
    updated();
}

///////////////////////////////////////////////////////////////////////////////
// Some typedefs
///////////////////////////////////////////////////////////////////////////////

typedef WGEShaderDefine< bool > WGEShaderDefineSwitch;

#endif  // WGESHADERDEFINE_H

