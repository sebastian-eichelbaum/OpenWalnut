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

#ifndef WPROTOTYPEUNKNOWN_H
#define WPROTOTYPEUNKNOWN_H

#include <string>

#include "WKernelException.h"

#include "../WExportKernel.h"

/**
 * Thrown whenever an unknown prototype is specified.
 * \ingroup kernel
 */
class OWKERNEL_EXPORT WPrototypeUnknown: public WKernelException
{
public:

    /**
     * Default constructor.
     * \param msg the exception message.
     */
    explicit WPrototypeUnknown( const std::string& msg = "Prototype unknown" );

    /**
     * Destructor.
     */
    virtual ~WPrototypeUnknown() throw();

protected:

private:
};

#endif  // WPROTOTYPEUNKNOWN_H

