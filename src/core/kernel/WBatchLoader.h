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

#ifndef WBATCHLOADER_H
#define WBATCHLOADER_H

#include <string>
#include <vector>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include "../common/WThreadedRunner.h"

#include "WExportKernel.h"

class WModuleContainer;

/**
 * Class for loading many datasets. It runs in a separate thread.
 */
class OWKERNEL_EXPORT WBatchLoader: public WThreadedRunner,
                                  public boost::enable_shared_from_this< WBatchLoader >
{
public:
    /**
     * Initializes the batchloader but does not start it. Use run().
     *
     * \param fileNames the files to load.
     * \param targetContainer the container to which the data modules should be added.
     */
    WBatchLoader( std::vector< std::string > fileNames, boost::shared_ptr< WModuleContainer > targetContainer );

    /**
     * Destructor.
     */
    virtual ~WBatchLoader();

    /**
     * Run thread and load the data.
     */
    virtual void run();

protected:
    /**
     * Function that has to be overwritten for execution. It gets executed in a separate thread after run()
     * has been called.
     */
    virtual void threadMain();

    /**
     * List of files to load.
     */
    std::vector< std::string > m_fileNamesToLoad;

    /**
     * The container which later will contain the loaded datasets.
     */
    boost::shared_ptr< WModuleContainer > m_targetContainer;

private:
};

#endif  // WBATCHLOADER_H

