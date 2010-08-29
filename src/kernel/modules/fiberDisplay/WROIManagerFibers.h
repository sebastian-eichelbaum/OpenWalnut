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

#ifndef WROIMANAGERFIBERS_H
#define WROIMANAGERFIBERS_H

#include <list>
#include <vector>

#include <boost/enable_shared_from_this.hpp>

#include "WKdTree.h"
#include "WMFiberDisplay.h"

#include "WRMBranch.h"
#include "WUpdateThread.h"

#include "../../WExportKernel.h"

/**
 * Class to store and manage different ROI's for fiber selection
 */
class OWKERNEL_EXPORT WROIManagerFibers: public boost::enable_shared_from_this< WROIManagerFibers >
{
public:
    /**
     * standard constructor
     */
    WROIManagerFibers();

    /**
     * destructor
     */
    ~WROIManagerFibers();

    /**
     * adds a new master ROI
     *
     * \param newRoi
     * \return ROI representation which can be used to remove the ROI
     */
    boost::shared_ptr< WRMROIRepresentation > addRoi( osg::ref_ptr< WROI > newRoi );

    /**
     * adds a new ROI below a master ROI
     *
     * \param newRoi
     * \param parentRoi
     * \return ROI representation which can be used to remove the ROI
     */
    boost::shared_ptr< WRMROIRepresentation > addRoi( osg::ref_ptr< WROI > newRoi, osg::ref_ptr< WROI > parentRoi );

    /**
     * removes a roi
     *
     * \param roi
     */
    void removeRoi( boost::shared_ptr< WRMROIRepresentation > roi );

    /**
     * removes a branch
     *
     * \param roi the first roi in the branch
     */
    void removeBranch( boost::shared_ptr< WRMROIRepresentation > roi );

    /**
     * connects a fiber dataset to the roi manager
     *
     * \param fibers
     */
    void addFiberDataset( boost::shared_ptr< const WDataSetFibers > fibers );

    /**
     * removes a fiber dataset from the roi manager
     *
     * \param fibers
     */
    void removeFiberDataset( boost::shared_ptr< const WDataSetFibers > fibers );

    /**
     * returns a bit vector for the selected fiber dataset that is the result of all active ROI's
     * on that fiber dataset
     *
     * \return std::vector<bool>*
     */
    boost::shared_ptr< std::vector< bool > > getBitField();

    /**
     * getter for a registered dataset
     *
     * \return the dataset
     */
    boost::shared_ptr< const WDataSetFibers > getDataSet();

    /**
     * getter for a kd tree
     * return the kd tree
     */
    boost::shared_ptr< WKdTree > getKdTree();

    /**
     * sets the dirty flag which will cause recalculation of the bit field
     */
    void setDirty();

    /**
     *  getter
     *  \return the dirty flag
     */
    bool dirty();

    /**
     * Add a specified notifier to the list of default notifiers which get connected to each added roi.
     *
     * \param notifier  the notifier function
     */
    virtual void addAddNotifier( boost::function< void( boost::shared_ptr< WRMROIRepresentation > ) > notifier );

    /**
     * Add a specified notifier to the list of default notifiers which get connected to each removed roi.
     *
     * \param notifier  the notifier function
     */
    virtual void addRemoveNotifier( boost::function< void( boost::shared_ptr< WRMROIRepresentation > ) > notifier );

    /**
     * updates the bit fields
     */
    void recalculate();

    /**
     * creates a color array, that is used for coloring of fibers
     */
    void createCustomColorArray();

    /**
     * getter
     */
    boost::shared_ptr< std::vector< float > >getCustomColors();

    /**
     * updates the custom color array with the color of a selected branch
     *
     * \param branch
     * \param color
     */
    void updateBundleColor( boost::shared_ptr<WRMBranch> branch, WColor color );

    /**
     * setter
     * \param roi
     */
    void setSelectedRoi( boost::shared_ptr< WRMROIRepresentation > roi );

    /**
     * getter
     */
    boost::shared_ptr< WRMROIRepresentation > getSelectedRoi();

    /**
     * getter
     */
    size_t size();

protected:
private:
    bool m_dirty; //!< dirty flag

    size_t m_size; //!< number of fibers in the dataset

    boost::shared_ptr< const WDataSetFibers >m_fibers; //!< registered fiber dataset

    boost::shared_ptr< std::vector< bool > >m_outputBitfield; //!< bit field of activated fibers

    boost::shared_ptr< std::vector< bool > >m_workerBitfield; //!< bit field of activated fibers

    std::list< boost::shared_ptr< WRMBranch > > m_branches; //!< list of branches in the logical tree structure

    std::list< boost::shared_ptr< WUpdateThread > > m_updateThreads; //!< list of currently running update threads.

    boost::shared_mutex m_updateListLock; //!< Lock to prevent concurrent threads trying to update the list

    /**
     * Stores a pointer to the kdTree used for fiber selection
     */
    boost::shared_ptr< WKdTree > m_kdTree;

    /**
     * Lock for associated notifiers set.
     */
    boost::shared_mutex m_associatedNotifiersLock;

    /**
     * The notifiers connected to added rois by default.
     */
    std::list< boost::function< void( boost::shared_ptr< WRMROIRepresentation > ) > > m_addNotifiers;

    /**
     * The notifiers connected to removed rois by default.
     */
    std::list< boost::function< void( boost::shared_ptr< WRMROIRepresentation > ) > > m_removeNotifiers;

    /**
     * lock to prevent concurrent threads trying to update the branch
     */
    boost::shared_mutex m_updateLock;

    /**
     * bool to lock the update function, will be removed again once we found out why the boost lock isnt working
     */
    bool m_recalcLock;

    boost::shared_ptr< std::vector< float > >m_customColors; //!< vector to store custom colors

    boost::shared_ptr< WRMROIRepresentation > m_selectedRoi; //!< stores a pointer to the currently selected roi
};

inline bool WROIManagerFibers::dirty()
{
    return m_dirty;
}

inline size_t WROIManagerFibers::size()
{
    return m_size;
}

#endif  // WROIMANAGERFIBERS_H
