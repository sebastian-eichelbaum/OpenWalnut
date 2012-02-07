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

#ifndef WINVALIDBOUNDINGBOX_H
#define WINVALIDBOUNDINGBOX_H

#include <string>

#include "../WException.h"


/**
 * Indicates that a bounding box is not valid, meaning its valid() member function delivers false.
 */
class WInvalidBoundingBox : public WException
{
public:
    /**
     * Default constructor.
     * \param msg the exception message.
     */
    explicit WInvalidBoundingBox( const std::string& msg = "A bounding box is not valid" );

    /**
     * Destructor.
     */
    virtual ~WInvalidBoundingBox() throw();

protected:
private:
};

#endif  // WINVALIDBOUNDINGBOX_H
