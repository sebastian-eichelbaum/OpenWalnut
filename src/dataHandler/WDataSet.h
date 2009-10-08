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

#ifndef WDATASET_H
#define WDATASET_H

#include <string>
#include <boost/shared_ptr.hpp>

/**
 * Base class for all data set types. This class has a number of subclasses
 * specifying the different types of data sets. Two of the dataset types
 * represent single and time-dependent datasets (compound of several time
 * steps) respectively.
 * \ingroup dataHandler
 */
class WDataSet
{
public:
    /**
     * This constructor should be used if a dataSet does not stem from a file.
     * It presets the its correpsonding fileName as empty string.
     */
    WDataSet();

    /**
     * Since WDataSet is a base class and thus should be polymorphic we add
     * virtual destructor.
     */
    virtual ~WDataSet()
    {
    }

    /**
     * Set the name of the file that this data set stems from.
     */
    void setFileName( const std::string fileName );

    /**
     * Get the name of the file that this data set stems from.
     */
    std::string getFileName() const;

protected:
private:
    /**
     * Name of the file this data set was loaded from. This information
     * may allow hollowing data sets later. DataSets that were not loaded
     * from a file should have the empty string stored here.
     */
    std::string m_fileName;
};

#endif  // WDATASET_H
