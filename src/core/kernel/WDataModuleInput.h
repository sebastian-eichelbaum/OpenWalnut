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

#ifndef WDATAMODULEINPUT_H
#define WDATAMODULEINPUT_H

#include <boost/shared_ptr.hpp>

/**
 * This class is the abstract interface to all the possible inputs a \ref WDataModule might handle. The classes can be specialized into streamed
 * inputs, file inputs and similar.
 */
class WDataModuleInput
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WDataModuleInput >.
     */
    typedef boost::shared_ptr< WDataModuleInput > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WDataModuleInput >.
     */
    typedef boost::shared_ptr< const WDataModuleInput > ConstSPtr;

    /**
     * Default constructor.
     */
    WDataModuleInput();

    /**
     * Destructor.
     */
    virtual ~WDataModuleInput();

    /**
     * Return a human-readable form of the input. Like filenames, servernames and similar.
     *
     * \return the input as string
     */
    virtual std::string asString() const = 0;
protected:
private:
};

#endif  // WDATAMODULEINPUT_H

