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

#ifndef WDATAMODULEINPUTFILTERFILE_H
#define WDATAMODULEINPUTFILTERFILE_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "WDataModuleInputFilter.h"

/**
 * Checks a given \ref WDataModuleInput against a file type.
 */
class WDataModuleInputFilterFile: public WDataModuleInputFilter
{
public:

    /**
     * Convenience typedef for a boost::shared_ptr< WDataModuleInputFilterFile >.
     */
    typedef boost::shared_ptr< WDataModuleInputFilterFile > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WDataModuleInputFilterFile >.
     */
    typedef boost::shared_ptr< const WDataModuleInputFilterFile > ConstSPtr;

    /**
     * Default constructor. Provide an extension and description for it.
     *
     * \param extension the extension, without "." (i.e. "nii.gz")
     * \param description the description for this kind of file.
     */
    WDataModuleInputFilterFile( std::string extension, std::string description );

    /**
     * Destructor.
     */
    virtual ~WDataModuleInputFilterFile();

    /**
     * Get the defined extension.
     *
     * \return the extension
     */
    const std::string& getExtension() const;

    /**
     * Check whether the filter matches the input or not. This filter requires the input to be a file with a certain extension.
     *
     * \param input the input to check against.
     *
     * \return true if the last extension of the file matches.
     */
    bool virtual matches( WDataModuleInput::ConstSPtr input ) const;
protected:
private:
    /**
     * Extension to check against
     */
    std::string m_extension;
};

#endif  // WDATAMODULEINPUTFILTERFILE_H

