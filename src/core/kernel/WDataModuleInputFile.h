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

#ifndef WDATAMODULEINPUTFILE_H
#define WDATAMODULEINPUTFILE_H

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/path.hpp>

#include "WDataModuleInput.h"

/**
 * Implements a file based input for the \ref WDataModule.
 */
class WDataModuleInputFile: public WDataModuleInput
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WDataModuleInputFile >.
     */
    typedef boost::shared_ptr< WDataModuleInputFile > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WDataModuleInputFile >.
     */
    typedef boost::shared_ptr< const WDataModuleInputFile > ConstSPtr;

    /**
     * Default constructor.
     *
     * \param fname the filename to use.
     */
    explicit WDataModuleInputFile( boost::filesystem::path fname );

    /**
     * Destructor.
     */
    virtual ~WDataModuleInputFile();

    /**
     * Get the filename to load.
     *
     * \return the filename
     */
    boost::filesystem::path getFilename() const;

    /**
     * The file input as human readable string.
     *
     * \return the readable form.
     */
    virtual std::string asString() const;
protected:
private:
    /**
     * The filename to load.
     */
    boost::filesystem::path m_filename;
};

#endif  // WDATAMODULEINPUTFILE_H

