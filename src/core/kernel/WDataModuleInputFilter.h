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

#ifndef WDATAMODULEINPUTFILTER_H
#define WDATAMODULEINPUTFILTER_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "WDataModuleInput.h"

/**
 * This class is the abstract interface allowing \ref WDataModule to define possible supported \ref WDataModuleInput.
 */
class WDataModuleInputFilter
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WDataModuleInputFilter >.
     */
    typedef boost::shared_ptr< WDataModuleInputFilter > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WDataModuleInputFilter >.
     */
    typedef boost::shared_ptr< const WDataModuleInputFilter > ConstSPtr;

    /**
     * Default constructor. Always provide a useful description!
     *
     * \param description a useful description
     */
    explicit WDataModuleInputFilter( std::string description );

    /**
     * Destructor.
     */
    virtual ~WDataModuleInputFilter();

    /**
     * Check whether the filter matches the input or not. You can check contents of the file/stream/... if you like to be more accurate.
     *
     * \param input the input to check against.
     *
     * \return true if the filter matches.
     */
     virtual bool matches( WDataModuleInput::ConstSPtr input ) const = 0;

    /**
     * Return the description of this filter.
     *
     * \return the description
     */
    const std::string& getDescription() const;
protected:
private:
    /**
     * The description of the filter
     */
    std::string m_description;
};

#endif  // WDATAMODULEINPUTFILTER_H

