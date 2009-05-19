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

#ifndef WMETAINFO_H
#define WMETAINFO_H

#include <string>

/**
 * Stores meta information such as types, file names, etc. associated with a
 * WDataSet loaded into OpenWalnut.
 */
class WMetaInfo
{
/**
 * Only UnitTests are allowed to be friends of this class
 */
friend class WMetaInfoTest;

public:
    /**
     * \return Name of the WDataSet
     */
    std::string getName() const;

    /**
     * Reset the name of the DataSet
     */
    void setName( std::string name );

    /**
     * \return The file name from which the DataSet got its data.
     */
    std::string getFileName() const;

    /**
     * Reset the file name from which the data is gathered for the WDataSet.
     */
    void setFileName( std::string fileName );

protected:

private:
    /**
     * Stores the name of the WDataSet this MetaInfo is associated with
     */
    std::string m_name;

    /**
     * Stores the name of the input file from which this DataSet gets its data.
     */
    std::string m_fileName;
};

#endif  // WMETAINFO_H
