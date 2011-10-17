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

#ifndef WSELECTORBRANCH_H
#define WSELECTORBRANCH_H

#include <list>
#include <vector>

#include "WSelectorRoi.h"
#include "../kernel/WRMBranch.h"

/**
 * TODO(schurade): Document this!
 */
class WSelectorBranch
{
public:
    /**
     * constructor
     * \param fibers pointer to the fiber dataset to work on
     * \param branch pointer to the branch object in the roi manager
     */
    WSelectorBranch( boost::shared_ptr< const WDataSetFibers > fibers, boost::shared_ptr< WRMBranch > branch );

    /**
     * destructor
     */
    ~WSelectorBranch();

    /**
     * getter
     * \return the bitfield that is created from all rois in this branch
     */
    boost::shared_ptr< std::vector<bool> > getBitField();

    /**
     * getter
     * \return pointer to the branch object, mainly for deletion and update purposes
     */
    boost::shared_ptr<WRMBranch> getBranch();

    /**
     * adds a roi to the branch
     * \param roi
     */
    void addRoi( boost::shared_ptr< WSelectorRoi > roi );


    /**
     * Queries the ROIs.
     *
     * \return A copy of the list of WSelectorRois
     */
    std::list< boost::shared_ptr< WSelectorRoi > > getROIs();

    /**
     * Removes a roi fromt he branch.
     *
     * \param roi
     */
    void removeRoi( osg::ref_ptr< WROI > roi );

    /**
     * Checks if empty.
     *
     * \return true when this branch contains no rois
     */
    bool empty();

    /**
     * Sets the dirty flag.
     */
    void setDirty();

    /**
     * Checks if branch is dirty.
     *
     * \return true if dirty
     */
    bool dirty();

protected:
    /**
     * function gets called when the color property of the roi branch has changed, it will write this color
     * into the custom color array of the fiber dataset
     */
    void colorChanged();
private:
    /**
     * updates the output bitfield with the information from all rois in this branch
     */
    void recalculate();

    /**
     * Pointer to the fiber data set
     */
    boost::shared_ptr< const WDataSetFibers > m_fibers;

    /**
     * size of the fiber dataset, stored for convinience
     */
    size_t m_size;

    bool m_dirty; //!< dirty flag

    /**
     * the bitfield given to the outside world
     */
    boost::shared_ptr< std::vector< bool > > m_bitField;

    /**
     * the bitfield we work on
     */
    boost::shared_ptr< std::vector< bool > > m_workerBitfield;

    /**
     * list of rois in this branch
     */
    std::list< boost::shared_ptr< WSelectorRoi > > m_rois;

    /**
     * pointer to the branch object in the roi manager
     */
    boost::shared_ptr< WRMBranch > m_branch;

    boost::shared_ptr< boost::function< void() > > m_changeSignal; //!< Signal that can be used to update the selector branch
    boost::shared_ptr< boost::function< void() > > m_changeRoiSignal; //!< Signal that can be used to update the selector branch
};

inline boost::shared_ptr< std::vector<bool> > WSelectorBranch::getBitField()
{
    if( m_dirty )
    {
        recalculate();
    }
    return m_bitField;
}

inline boost::shared_ptr< WRMBranch > WSelectorBranch::getBranch()
{
    return m_branch;
}

inline bool WSelectorBranch::empty()
{
    return m_rois.empty();
}

inline bool WSelectorBranch::dirty()
{
    return m_dirty;
}

#endif  // WSELECTORBRANCH_H
