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

#ifndef WFIBERSELECTOR_H
#define WFIBERSELECTOR_H

#include <list>
#include <vector>

#include "../dataHandler/WDataSetFibers.h"
#include "../common/WCondition.h"

#include "WSelectorRoi.h"
#include "WSelectorBranch.h"

#include "WKdTree.h"

/**
 * Adaptor class between the roi manager and the fiber display
 */
class  WFiberSelector // NOLINT
{
public:
    /**
     * Fiber selector pointer
     */
    typedef boost::shared_ptr< WFiberSelector > SPtr;

    /**
     * Const fiber selector pointer.
     */
    typedef boost::shared_ptr< const WFiberSelector > ConstSPtr;

    /**
     * constructor
     * \param fibers pointer to the datset this selector works on
     */
    explicit WFiberSelector( boost::shared_ptr< const WDataSetFibers > fibers );

    /**
     * destructor
     */
    ~WFiberSelector();

    /**
     * Return the number of fibers in the dataset.
     *
     * \return number of fibers
     */
    size_t size();

    /**
     * getter
     * \return the bitfield calculated from all active rois
     */
    boost::shared_ptr< std::vector< bool > > getBitfield();

    /**
     * getter for the line start index array
     * \return line starts
     */
    boost::shared_ptr< std::vector< size_t > > getStarts();

    /**
     * getter for the line length array
     * \return line lengths
     */
    boost::shared_ptr< std::vector< size_t > > getLengths();

    /**
     * setter
     * sets the dirty flag
     */
    void setDirty();

    /**
     * Get the current dirty-state.
     */
    bool getDirty();

    /**
     * Condition that fires upon a recalculation of the fiber selection.
     *
     * \return the condition
     */
    WCondition::SPtr getDirtyCondition();

protected:
    /**
     * listener function for inserting rois
     * \param roi new roi inserted into the roi structure
     */
    void slotAddRoi( osg::ref_ptr< WROI > roi );

    /**
     * listener function for removing rois
     * \param roi roi that is being removed
     */
    void slotRemoveRoi( osg::ref_ptr< WROI > roi );

    /**
     * listener function for removing rois
     * \param branch branch that is being removed
     */
    void slotRemoveBranch( boost::shared_ptr< WRMBranch > branch );

private:
    /**
     * update the bitfield when there was a change in the roi structure
     */
    void recalculate();

    /**
     * Pointer to the fiber data set
     */
    boost::shared_ptr< const WDataSetFibers > m_fibers;

    size_t m_size; //!< number of fibers in the dataset

    bool m_dirty; //!< dirty flag

    /**
     * Stores a pointer to the kdTree used for fiber selection
     */
    boost::shared_ptr< WKdTree > m_kdTree;

    boost::shared_ptr< std::vector< bool > >m_outputBitfield; //!< bit field of activated fibers

    boost::shared_ptr< std::vector< bool > >m_workerBitfield; //!< bit field of activated fibers

    std::list< boost::shared_ptr<WSelectorBranch> >m_branches; //!< list of branches int he roi structure

    boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > m_assocRoiSignal; //!< Signal that can be used to update the selector
    boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > m_removeRoiSignal; //!< Signal that can be used to update the selector
    boost::shared_ptr< boost::function< void( boost::shared_ptr< WRMBranch > ) > > m_removeBranchSignal; //!< Signal for updating the selector
    boost::shared_ptr< boost::function< void() > > m_changeRoiSignal; //!< Signal that can be used to update the selector

    /**
     * Condition that fires on setDirty.
     */
    WCondition::SPtr m_dirtyCondition;
};

inline size_t WFiberSelector::size()
{
    return m_size;
}

inline boost::shared_ptr< std::vector< size_t > > WFiberSelector::getStarts()
{
    return m_fibers->getLineStartIndexes();
}

inline boost::shared_ptr< std::vector< size_t > > WFiberSelector::getLengths()
{
    return m_fibers->getLineLengths();
}

#endif  // WFIBERSELECTOR_H
