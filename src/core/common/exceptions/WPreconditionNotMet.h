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

#ifndef WPRECONDITIONNOTMET_H
#define WPRECONDITIONNOTMET_H

#include <string>
#include <sstream>
#include <utility>

#include "../WException.h"


/**
 * An exception that gets thrown when preconditions of a function are not met.
 */
class WPreconditionNotMet : public WException
{
public:
    /**
     * Constructor.
     *
     * \param msg The message.
     */
    explicit WPreconditionNotMet( std::string const& msg );

    /**
     * Destructor.
     */
    virtual ~WPreconditionNotMet() throw();

protected:
private:
};

namespace utility
{
/**
 * Throws a WPreconditionNotMet.
 *
 * \param expr The expression that failed.
 * \param msg A message.
 */
inline void __WPrecondImpl( std::string const& expr, std::string const& msg )
{
    std::stringstream s;
    s << "Function precondition not met: " << expr;
    if( !msg.empty() )
    {
        s << "\n" << msg;
    }
    throw WPreconditionNotMet( s.str() );
}

/**
 * Check if two values differ.
 *
 * \tparam T1 The type of the first value.
 * \tparam T2 The type of the second value.
 * \param expr1 The first expression.
 * \param expr2 The second expression.
 * \param value1 The first value.
 * \param value2 The second value.
 */
template< typename T1, typename T2 >
void __WPrecondDiffersImpl( std::string const& expr1, std::string const& expr2, T1 const& value1, T2 const& value2 )
{
    if( value1 == value2 )
    {
        std::stringstream s;
        s << "Function precondition not met: \n";
        s << "Expected " << expr1 << " to differ from " << expr2 << ", yet " << value1
          << " == " << value2;
        throw WPreconditionNotMet( s.str() );
    }
}

/**
 * Check if two values are equal.
 *
 * \tparam T1 The type of the first value.
 * \tparam T2 The type of the second value.
 * \param expr1 The first expression.
 * \param expr2 The second expression.
 * \param value1 The first value.
 * \param value2 The second value.
 */
template< typename T1, typename T2 >
void __WPrecondEqualsImpl( std::string const& expr1, std::string const& expr2, T1 const& value1, T2 const& value2 )
{
    if( !( value1 == value2 ) )
    {
        std::stringstream s;
        s << "Function precondition not met: \n";
        s << "Expected " << expr1 << " to be equal to " << expr2 << ", yet " << value1
          << " != " << value2;
        throw WPreconditionNotMet( s.str() );
    }
}

/**
 * Check if a value is smaller than another value.
 *
 * \tparam T1 The type of the first value.
 * \tparam T2 The type of the second value.
 * \param expr1 The first expression.
 * \param expr2 The second expression.
 * \param value1 The first value.
 * \param value2 The second value.
 */
template< typename T1, typename T2 >
void __WPrecondLessImpl( std::string const& expr1, std::string const& expr2, T1 const& value1, T2 const& value2 )
{
    if( !( value1 < value2 ) )
    {
        std::stringstream s;
        s << "Function precondition not met: \n";
        s << "Expected " << expr1 << " to be smaller than " << expr2 << ", yet " << value1
          << " => " << value2;
        throw WPreconditionNotMet( s.str() );
    }
}

}  // namespace utility

#define WPrecond( expr, msg ) ( ( expr ) ? ( ( void )0 ) : ( ::utility::__WPrecondImpl( #expr, msg ) ) )

#define WPrecondDiffers( expr1, expr2 ) ( ::utility::__WPrecondDiffersImpl( #expr1, #expr2, ( expr1 ), ( expr2 ) ) ) // NOLINT

#define WPrecondEquals( expr1, expr2 ) ( ::utility::__WPrecondEqualsImpl( #expr1, #expr2, ( expr1 ), ( expr2 ) ) ) // NOLINT

#define WPrecondLess( expr1, expr2 ) ( ::utility::__WPrecondLessImpl( #expr1, #expr2, ( expr1 ), ( expr2 ) ) ) // NOLINT

#endif  // WPRECONDITIONNOTMET_H
