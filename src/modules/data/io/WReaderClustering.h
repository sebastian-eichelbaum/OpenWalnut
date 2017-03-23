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

#ifndef WREADERCLUSTERING_H_
#define WREADERCLUSTERING_H_

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "core/common/WStringUtils.h"
#include "core/dataHandler/datastructures/WTreeNode.h"
#include "core/dataHandler/exceptions/WDHIOFailure.h"
#include "core/dataHandler/exceptions/WDHNoSuchFile.h"
#include "core/dataHandler/exceptions/WDHParseError.h"
#include "core/dataHandler/io/WReader.h"
#include "core/dataHandler/WDataSetHierarchicalClustering.h"

/**
 * Reads a hierarchical clustering from a FDG file.
 *
 * \ingroup dataHandler
 */
class WReaderClustering : public WReader // NOLINT
{
/**
* Only UnitTests may be friends.
*/
friend class WReaderClusteringTest;
public:
    /**
     * Constructs and makes a new FDG reader for separate thread start.
     *
     * \param fname File name where to read data from
     * \throws WDHNoSuchFile
     */
    explicit WReaderClustering( std::string fname );

    /**
     * Destroys this instance and closes the file.
     */
    virtual ~WReaderClustering() throw();

    /**
     * Reads the clustering file and creates a dataset out of it.
     *
     * \return Reference to the dataset.
     */
    virtual boost::shared_ptr< WDataSetHierarchicalClustering > read();

protected:
    /**
     * Stores the hierarchical clustering map
     */
    std::map< size_t, WFiberCluster::SPtr > m_clustering;

    /**
     * Stores the hierarchical clustering map
     */
    std::map< size_t, WTreeNode::SPtr > m_nodes;

    /**
     * The lines of the fdg file
     */
    std::vector< std::string > m_lines;

    /**
     * The current hierarchy level the reader is reading
     */
    boost::shared_ptr<size_t> m_level;
};

#endif  // WREADERCLUSTERING_H
