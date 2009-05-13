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
 * Stores meta information for data sets loaded into OpenWalnut
 */
class WMetaInfo
{
public:
    std::string getName() const;
    void setName( std::string name );
    std::string getFileName() const;
    void setFileName( std::string fileName );

protected:
private:
    std::string m_name;
    std::string m_fileName;
};

#endif  // WMETAINFO_H
