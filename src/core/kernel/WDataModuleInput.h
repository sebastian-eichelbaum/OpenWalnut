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

#include <string>
#include <ostream>

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
     * Return a unique name for this input type. This is used to identify a certain input later.
     *
     * \return the name.
     */
    virtual std::string getName() const = 0;

    /**
     * Return a human-readable form of the input. Like filenames, server names and similar.
     *
     * \return the input as string
     */
    virtual std::string asString() const = 0;

    /**
     * Write a machine-readable string which allows to restore your specific input later. No line-breaks, no ":" and it must not be empty.
     *
     * \param out the stream to serialize to
     *
     * \return the stream
     */
    virtual std::ostream& serialize( std::ostream& out ) const = 0;     // NOLINT: yes, it is an intended non-const ref

    /**
     * Create an instance by using a parameter string. This is the string you define by the \ref serialize() call.
     *
     * \param parameter the parameter string
     * \param name the name string
     *
     * \return an instance of WDataModuleInput
     */
    static SPtr create( std::string name, std::string parameter );
protected:
private:
};

#endif  // WDATAMODULEINPUT_H

