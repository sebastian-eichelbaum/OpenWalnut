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

#ifndef WRMBRANCH_H
#define WRMBRANCH_H

#include <list>
#include <string>
#include <vector>

#include <boost/enable_shared_from_this.hpp>

#include "WRMROIRepresentation.h"

class WROIManagerFibers;

/**
 * TODO(schurade): Document this!
 */
class WRMBranch : public boost::enable_shared_from_this< WRMBranch >
{
public:
    /**
     * TODO(schurade): Document this!
     */
    explicit WRMBranch( boost::shared_ptr< WROIManagerFibers > roiManager );

    /**
     * TODO(schurade): Document this!
     */
    ~WRMBranch();

    /**
     *
     */
    void addRoi( boost::shared_ptr< WRMROIRepresentation > roi );

    /**
     *
     */
    boost::shared_ptr< std::vector< bool > > getBitField( unsigned int index );

    /**
     *
     */
    void addBitField( size_t size );

    /**
     *
     */
    void recalculate();

    /**
     * getter for dirty flag
     */
    bool isDirty();

    /**
     * sets dirty flag true and notifies the branch
     */
    void setDirty();

    /**
     * returns a pointer to the first roi in the branch
     */
    boost::shared_ptr< WRMROIRepresentation >getFirstRoi();

    /**
     *
     */
    boost::shared_ptr< WROIManagerFibers > getRoiManager();

protected:
private:
    bool m_dirty;

    boost::shared_ptr< WROIManagerFibers > m_roiManager;

    std::list< boost::shared_ptr< std::vector<bool> > >m_bitFields;

    std::list< boost::shared_ptr< WRMROIRepresentation> > m_rois;
};

#endif  // WRMBRANCH_H
