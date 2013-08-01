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

#include <algorithm>
#include <cassert>
#include <string>

#ifdef _WIN32
    #include <iostream>
    #include <windows.h>        // NOLINT
#endif

#include <boost/filesystem.hpp>

#include "exceptions/WLibraryFetchFailed.h"
#include "exceptions/WLibraryLoadFailed.h"
#include "WSharedLib.h"

#ifdef _WIN32

/**
 * Simple class holding an opened library.
 */
struct WSharedLib::data
{
    /**
     * Path of lib.
     */
    const std::string m_path;

    /**
     * Handle describing the loaded lib.
     */
    HMODULE m_hDLL;

    /**
     * Constructor. Opens and loads the library.
     *
     * \see WSharedLib::WSharedLib for details.
     *
     * \param path the lib to open
     */
    explicit data( const std::string& path ):
        m_path( path ),
        m_hDLL( LoadLibrary( path.c_str() ) )
    {
        if( !m_hDLL )
        {
            throw WLibraryLoadFailed( std::string( "Could not load library \"" + m_path + "\" due to the error: " + errmsg() ) );
        }
    }

    /**
     * Destructor. Closes the previously opened library handle.
     */
    ~data()
    {
        FreeLibrary( m_hDLL );
    }

    /**
     * Searches the lib for the specified function symbol and returns it.
     *
     * \param name the name of the function
     *
     * \return the pointer to the requested function
     *
     * \throw WLibraryFetchFailed thrown if the symbol could not be found.
     */
    func_ptr_type findFunction( const std::string& name )
    {
        func_ptr_type func_ptr = reinterpret_cast< func_ptr_type >( GetProcAddress( m_hDLL, name.c_str() ) );
        if( !func_ptr )
        {
            throw WLibraryFetchFailed( std::string( "Could not fetch symbol \"" + name + "\"" + " due to the error: " + errmsg() ) );
        }
        return func_ptr;
    }

    /**
     * Check for existence of a given function pointer symbol.
     *
     * \param name the symbol
     *
     * \return true if it exists.
     */
    bool existsFunction( const std::string& name )
    {
        func_ptr_type func_ptr = reinterpret_cast< func_ptr_type >( GetProcAddress( m_hDLL, name.c_str() ) );
        return func_ptr;
    }

    /**
     * Searches the lib for the specified function symbol and returns it.
     *
     * \param name the name of the function
     *
     * \return the pointer to the requested function
     *
     * \throw WLibraryFetchFailed thrown if the symbol could not be found.
     */
    void* findVariable( const std::string& name )
    {
        return reinterpret_cast< void* >( findFunction( name ) );
    }

    /**
     * Constructs a nice looking error message for the last error occurred.
     *
     * \return the last error message
     */
    static std::string errmsg()
    {
        std::string msg;
        LPTSTR lpMsgBuf = 0;
        FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 0, GetLastError(), 0,
                       reinterpret_cast< LPTSTR >( &lpMsgBuf ), 0, 0 );
        LPTSTR p = lpMsgBuf;
        while( *p )
        {
            msg.push_back( *p++ );
        }
        LocalFree( lpMsgBuf );
        return msg;
    }
};
#else
#include <dlfcn.h>      // NOLINT
#include <pthread.h>    // NOLINT

namespace
{
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    struct auto_lock
    {
        auto_lock()
        {
            pthread_mutex_lock( &mutex );
        }
        ~auto_lock()
        {
            pthread_mutex_unlock( &mutex );
        }
    };
}

/**
 * Simple class holding an opened library.
 */
struct WSharedLib::data
{
    /**
     * Path of lib.
     */
    const std::string m_path;

    /**
     * Handle describing the loaded lib.
     */
    void* m_dl;

    /**
     * Destructor. Closes the previously opened library handle.
     */
    ~data()
    {
        assert( dlclose( m_dl ) == 0 );
    }

    /**
     * Constructor. Opens and loads the library.
     *
     * \see WSharedLib::WSharedLib for details.
     *
     * \param path the lib to open
     */
    explicit data( const std::string& path )
        : m_path( path ), m_dl( 0 )
    {
        auto_lock lock;
        m_dl = dlopen( m_path.c_str(), RTLD_LAZY );
        if( !m_dl )
        {
            throw WLibraryLoadFailed( std::string( "Could not load library \"" + m_path + "\" due to the error: " + dlerror() ) );
        }
    }

    /**
     * Searches the lib for the specified function symbol and returns it.
     *
     * \param name the name of the function
     *
     * \return the pointer to the requested function
     *
     * \throw WLibraryFetchFailed thrown if the symbol could not be found.
     */
    func_ptr_type findFunction( const std::string& name )
    {
        // This cast is supposed to throw a warning because the cast
        // of void* to function pointers is not defined or required
        // in ISO C. Nevertheless, it works on most current compilers.
        //
        // man dlsym talks about introducing a new function to circumvent
        // this problem.
        return reinterpret_cast< func_ptr_type >( findVariable( name ) );
    }

    /**
     * Searches the lib for the specified symbol and returns it.
     *
     * \param name the name of the symbol to search.
     * \param suppressThrow set to true to suppress the exception. NULL is returned if the symbol does not exists
     *
     * \return pointer to the symbol.
     *
     * \throw WLibraryFetchFailed thrown if the symbol could not be found.
     *
     */
    void* findVariable( const std::string& name, bool suppressThrow = false )
    {
        auto_lock lock;
        dlerror();
        void* variable_ptr = dlsym( m_dl, name.c_str() );
        const char *err = dlerror();
        if( !suppressThrow && err )
        {
            throw WLibraryFetchFailed( std::string( "Could not fetch symbol \"" + name + "\"" + " due to the error: " + err ) );
        }
        return variable_ptr;
    }

    /**
     * Check for existence of a given function pointer symbol.
     *
     * \param name the symbol
     *
     * \return true if it exists.
     */
    bool existsFunction( const std::string& name )
    {
        return findVariable( name, true );
    }
};
#endif

WSharedLib::WSharedLib( boost::filesystem::path lib ):
    m_data( new data( lib.string() ) )
{
}

WSharedLib::WSharedLib( const WSharedLib& rhs ):
    m_data( new data( rhs.m_data->m_path ) )
{
}

WSharedLib::~WSharedLib()
{
    delete m_data;
}

WSharedLib& WSharedLib::operator=( const WSharedLib& rhs )
{
    WSharedLib o( rhs );
    swap( *this, o );
    return *this;
}

void swap( WSharedLib& lhs, WSharedLib& rhs )
{
    std::swap( lhs.m_data, rhs.m_data );
}

WSharedLib::func_ptr_type WSharedLib::findFunction( const std::string& name ) const
{
    return m_data->findFunction( name );
}

void* WSharedLib::findVariable( const std::string& name ) const
{
    return m_data->findVariable( name );
}

bool WSharedLib::existsFunction( const std::string& name ) const
{
    return m_data->existsFunction( name );
}

std::string WSharedLib::getSystemPrefix()
{
    return W_LIB_PREFIX;
}

std::string WSharedLib::getSystemSuffix()
{
    return W_LIB_SUFFIX;
}

std::string WSharedLib::getSystemLibPath()
{
    return "../lib";
}

