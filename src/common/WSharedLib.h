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

#ifndef WSHAREDLIB_H
#define WSHAREDLIB_H

#include <algorithm>
#include <string>

// Use filesystem version 2 for compatibility with newer boost versions.
#ifndef BOOST_FILESYSTEM_VERSION
    #define BOOST_FILESYSTEM_VERSION 2
#endif
#include <boost/filesystem.hpp>

#include "WExportCommon.h"

/**
 * This class loads shared libraries and provides function pointers. This is especially useful for dynamic loading of shared libraries during
 * runtime. This works on Windows, Linux and Mac OS and is based on the openbug shared_lib implementation by
 * Christian Heine <heine@informatik.uni-leipzig.de>. For more details, see http://www.informatik.uni-leipzig.de/~hg/openbug .
 *
 * \note This class performs locking so that under any system variables of shared_lib may be used in multi-threaded environments.
 * \warning Because the POSIX standard does not enforce thread safety for the functions dlopen, dlclose, dlerror, and dlsym, these should not
 *          be used simultaneously with variables of this class.
 */
class OWCOMMON_EXPORT WSharedLib // NOLINT
{
public:

    /**
     * Constructor. Loads the specified library.
     *
     * \param lib the library to load. Can be a DLL,SO or DYLIB (depending on system). This can be an absolut or relative path. Otherwise
     * standard library search directory may be searched.
     *
     * \note If the shared library is already loaded, this constructor just
     *       increases its reference count. This is detected even if different
     *       paths were used (e.g. "./somelib.so", "../libs/somelib.so").
     * \throw WLibraryLoadFailed if the lib could not be loaded. Maybe because of file not found or link errors.
     */
    explicit WSharedLib( boost::filesystem::path lib );

    /**
     * Copies this instance by increasing the reference counter of the loaded library by 1.
     *
     * \param rhs the other Lib.
     */
    WSharedLib( const WSharedLib& rhs );

    /**
     * Destructor. Decreases the reference counter and unloads the library if the reference count drops to zero.
     */
    virtual ~WSharedLib();

    /**
     * Copy assignment for shared libraries.
     *
     * \param rhs the one to assign
     *
     * \return this instance copied from the specified one.
     */
    WSharedLib& operator=( const WSharedLib& rhs );

    /**
     * Swap to shared libraries.
     *
     * \param lhs the one
     * \param rhs the other
     */
    friend
    void swap( WSharedLib& lhs, WSharedLib& rhs );

    /** Search for a function in the shared library.
     * \tparam FuncType a function type
     * \param name the name of the function
     * \param func will be set to the function pointer
     *
     * \throw WLibraryFetchFailed if the symbol was not found
     *
     * \warning type unsafe, make sure the symbol actually is of the proper type
     */
    template < typename FuncType >
    void fetchFunction( const std::string& name, FuncType& func ) const;

    /** Search for an variable in the shared library
     * \tparam PtrType a pointer type
     * \param name the name of the variable
     * \param variable will be set to the variable pointer
     *
     * \throw WLibraryFetchFailed if the symbol was not found
     *
     * \warning type unsafe, make sure the symbol actually is of the proper type
     */
    template < typename PtrType >
    void fetchVariable( const std::string& name, PtrType& variable ) const;

    /**
     * Returns the prefix used for libraries on the system. On Unix this mostly is "lib".
     *
     * \return the prefix.
     */
    static std::string getSystemPrefix();

    /**
     * Returns the suffix for libraries used on the system. On Unix this mostly is "so", Windows uses "dll" and Mac something like "dylib".
     *
     * \return the suffix.
     */
    static std::string getSystemSuffix();

    /**
     * Returns the default path for libraries on the current system. This is the directory where to search for .so,.dll or .dylib files. On Unix,
     * this will be "../lib", on Windows ".".
     *
     * \return the path on the system.
     */
    static std::string getSystemLibPath();

protected:

private:

    //! neutral function pointer type
    typedef void (*func_ptr_type)(void);

    /**
     * Find the specified function pointer in the library.
     *
     * \param name the symbol to search
     *
     * \return the pointer to the symbol as function pointer
     */
    func_ptr_type findFunction( const std::string& name ) const;

    /**
     * Find the specified symbol in the library.
     *
     * \param name the symbol to search
     *
     * \return the pointer to the symbol as function pointer.
     */
    void* findVariable( const std::string& name ) const;

    //! internal data
    class data;

    //! internal data
    data* m_data;
};

template < typename FuncType >
void WSharedLib::fetchFunction( const std::string& name, FuncType& func ) const
{
    func = reinterpret_cast< FuncType >( findFunction( name ) );
}

template < typename PtrType >
void WSharedLib::fetchVariable( const std::string& name, PtrType& variable ) const
{
    variable = static_cast< PtrType >( findVariable( name ) );
}

#endif  // WSHAREDLIB_H

