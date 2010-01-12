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

#ifndef WPROGRESSCOMBINER_H
#define WPROGRESSCOMBINER_H

#include <string>

#include "WProgress.h"

/**
 * Base class for all kinds of progress combinations. You might want to derive from this class to implement some special progress
 * combination.
 */
class WProgressCombiner: public WProgress
{
public:

    /**
     * Default constructor. It creates a empty combiner.
     *
     * \param name the (optional) name of this progress.
     */
    explicit WProgressCombiner( std::string name = "" );

    /**
     * Destructor.
     */
    virtual ~WProgressCombiner();

protected:

    /**
     * The name of the combiner.
     */
    std::string m_name;

private:
};

#endif  // WPROGRESSCOMBINER_H

