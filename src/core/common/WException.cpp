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

#if (( defined( __linux__ ) && defined( __GNUC__ )) || defined ( __APPLE__ ))
// This is highly platform dependent. Used for backtrace functionality.
#include <execinfo.h>
#include <cxxabi.h>
#endif

#include <iostream>
#include <list>
#include <sstream>
#include <stdexcept>
#include <string>

#include <boost/algorithm/string.hpp>

#include "WException.h"

/**
 * initialize static member.
 */
bool WException::noBacktrace = false;

WException::WException( const std::string& msg ):
    exception(),
    m_labelColor( WTerminalColor( WTerminalColor::Bold, WTerminalColor::FGRed ) ),
    m_functionColor( WTerminalColor( WTerminalColor::Bold, WTerminalColor::FGBlue ) ),
    m_symbolColor( WTerminalColor() ),
    m_headlineColor( WTerminalColor( WTerminalColor::Bold, WTerminalColor::FGYellow ) )
{
    // initialize members
    m_msg = msg;

    // print stacktrace and message
    // no backtrace?
    if( !noBacktrace )
    {
        std::cerr << m_headlineColor( std::string( "Exception thrown! Callstack's backtrace:" ) ) << std::endl << getBacktrace() << std::endl;
    }
}

WException::WException( const std::exception& e ):
    exception( e ),
    m_labelColor( WTerminalColor( WTerminalColor::Bold, WTerminalColor::FGRed ) ),
    m_functionColor( WTerminalColor( WTerminalColor::Off, WTerminalColor::FGBlue ) ),
    m_symbolColor( WTerminalColor() ),
    m_headlineColor( WTerminalColor( WTerminalColor::Bold, WTerminalColor::FGYellow ) )
{
    m_msg = e.what();

    // print stacktrace and message
    // no backtrace?
    if( !noBacktrace )
    {
        std::cerr << m_headlineColor( std::string( "Exception thrown! Callstack's backtrace:" ) ) << std::endl << getBacktrace() << std::endl;
    }
}

WException::~WException() throw()
{
    // cleanup
}

const char* WException::what() const throw()
{
    // return it
    return m_msg.c_str();
}

std::string WException::getTrace() const
{
    std::string result( what() );
    result += "\n\n";
    std::list< std::string >::const_iterator citer;
    for( citer = m_trace.begin(); citer != m_trace.end(); ++citer )
        result += "trace: " + *citer + "\n";
    boost::trim( result );
    return result;
}

std::string WException::getBacktrace() const
{
    // print trace here
    std::ostringstream o;

#if (( defined( __linux__ ) && defined( __GNUC__ )) || defined( __APPLE__))
    // This is highly platform dependent. It MIGHT also work on BSD and other unix.

    // Automatic callstack backtrace
    const size_t maxDepth = 100;
    size_t stackDepth;
    void* stackAddrs[maxDepth];
    char** stackSymbols;

    // acquire stacktrace
    stackDepth = backtrace( stackAddrs, maxDepth );
    stackSymbols = backtrace_symbols( stackAddrs, stackDepth );

    // for each stack element -> demangle and print
    for( size_t i = 1; i < stackDepth; ++i )
    {
        // need some space for function name
        // just a guess, especially template names might be even longer
        size_t functionLength = 512;
        char* function = new char[functionLength];

        // find mangled function name in stackSymbols[i]
        char* begin = 0;
        char* end = 0;

        // find the parentheses and address offset surrounding the mangled name
        for( char* j = stackSymbols[i]; *j; ++j )
        {
            if( *j == '(' )
            {
                begin = j;
            }
            else if( *j == '+' )
            {
                end = j;
            }
        }

        // found?
        if( begin && end )
        {
            *begin++ = '(';
            *end = '\0';    // temporarily end string there (since \0 is string delimiter)

            // found our mangled name, now in [begin, end)
            int status;
            char* ret = abi::__cxa_demangle( begin, function, &functionLength, &status );

            if( ret )
            {
                // return value may be a realloc() of the input
                function = ret;
            }
            else
            {
                // demangling failed, just pretend it's a C function with no args
                std::strncpy( function, begin, functionLength );
                std::strncat( function, "()", functionLength );
                function[functionLength-1] = '\0';
            }
            *end = '+';
            o << m_labelColor( std::string( "trace: " ) )
              << m_functionColor( function )
              << "\t->\t"
              << m_symbolColor( stackSymbols[i] ) << std::endl;
        }
        else
        {
            // didn't find the mangled name, just print the whole line
            o << m_labelColor( std::string( "trace: " ) )
              << m_functionColor( std::string( "??? " ) )
              << "\t->\t"
              << m_symbolColor( stackSymbols[i] ) << std::endl;
        }

        delete[] function;
    }

    // backtrace_symbols malloc()ed some mem -> we NEED to use free()
    free( stackSymbols );
#else
    o << "Backtrace not supported on your platform. Currently just works on Linux with GCC. Sorry!";
#endif

    return o.str();
}

void WException::disableBacktrace()
{
    noBacktrace = true;
}

