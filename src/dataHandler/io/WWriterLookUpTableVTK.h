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

#ifndef WWRITERLOOKUPTABLEVTK_H
#define WWRITERLOOKUPTABLEVTK_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "WWriter.h"

/**
 * Can write a look up table to a file in VTK format.
 */
class WWriterLookUpTableVTK : public WWriter
{
public:
    /**
     * Creates a writer object for FiberVTK file writing. On parameter documentation
     * take a look into the WWriter class.
     */
    WWriterLookUpTableVTK( std::string fname, bool overwrite = false );

    /**
     * Actually perform writing to file.
     *
     * \param table The data in that table will be saved
     * \param dim the dimensionality of the table
     */
    void writeTable( const std::vector< double > &table ) const;

protected:
private:
};

#endif  // WWRITERLOOKUPTABLEVTK_H
