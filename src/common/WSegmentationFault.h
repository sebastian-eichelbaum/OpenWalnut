//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#ifndef WSEGMENTATIONFAULT_H
#define WSEGMENTATIONFAULT_H

#ifdef __linux__
// This is highly platform dependent. Used for backtrace functionality.
#include <signal.h>
#endif  // __linux__

#include <string>

#include "WException.h"

#ifdef __linux__
// This is highly platform dependent. Used for backtrace functionality.

/** 
 * \par Description
 * Template class for every signal which can be handled.
 */
template <class SignalExceptionClass> class SignalTranslator
{
public:
    SignalTranslator()
    {
        static SingleTonTranslator s_objTranslator;
    }

protected:

private:
    class SingleTonTranslator
    {
    public:  // NOLINT
        SingleTonTranslator()
        {
            signal( SignalExceptionClass::getSignalNumber(), SignalHandler );
        }

        static void SignalHandler( int signum )
        {
            throw SignalExceptionClass();
        }
    };
};
#endif  // __linux__

/**
 * \par Description:
 * Base exception class for handling segmentation faults.
 * It throws segmentation faults as exceptions. Please remember that SIGSEGV is not
 * recoverable, which means it can NOT be catched!
 * Also note that this will only work on Linux.
 */
class WSegmentationFault: public WException
{
public:

    /**
     * \par Description
     * Default constructor.
     */
    explicit WSegmentationFault( const std::string& msg = "Segmentation Fault" );

    /**
     * \par Description
     * Destructor.
     */
    virtual ~WSegmentationFault() throw();

    /** 
     * \par Description
     * Defines signal type to handle. 
     * @return The signal number.
     */
    static int getSignalNumber() throw();

    /** 
     * \par Description
     * Installs this exception as signal handler for SIGSEGV. 
     * This will just work on Linux.
     */
    static void installSignalHandler() throw();

protected:

private:
};

#endif  // WSEGMENTATIONFAULT_H

