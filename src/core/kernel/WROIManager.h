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

#ifndef WROIMANAGER_H
#define WROIMANAGER_H

#include <list>
#include <vector>

#include <boost/enable_shared_from_this.hpp>

#include "WRMBranch.h"



/**
 * Class to store and manage different ROI's for fiber selection
 */
class  WROIManager: public boost::enable_shared_from_this< WROIManager >
{
public:
    /**
     * standard constructor
     */
    WROIManager();

    /**
     * destructor
     */
    ~WROIManager();

    /**
     * Add a new branch.
     *
     * \return the new branch.
     */
    boost::shared_ptr< WRMBranch > addBranch();

    /**
     * adds a new master ROI
     *
     * \param newRoi
     * \return ROI representation which can be used to remove the ROI
     */
    void addRoi( osg::ref_ptr< WROI > newRoi );

    /**
     * adds a new ROI below a master ROI
     *
     * \param newRoi
     * \param parentRoi
     * \return ROI representation which can be used to remove the ROI
     */
    void addRoi( osg::ref_ptr< WROI > newRoi, osg::ref_ptr< WROI > parentRoi );

    /**
     * Add a ROI to a branch.
     *
     * \param newRoi the new ROI to add
     * \param toBranch the branch to add the ROI to.
     */
    void addRoi( osg::ref_ptr< WROI > newRoi, boost::shared_ptr< WRMBranch > toBranch );

    /**
     * removes a roi
     *
     * \param roi
     */
    void removeRoi( osg::ref_ptr< WROI > roi );

    /**
     * removes a branch
     *
     * \param roi the first roi in the branch
     */
    void removeBranch( osg::ref_ptr< WROI > roi );

    /**
     * getter
     * returns the branch item the roi is in
     * \param roi
     * \return branch
     */
    boost::shared_ptr< WRMBranch> getBranch( osg::ref_ptr< WROI > roi );

    /**
     * sets the dirty flag which will cause recalculation of the bit field
     */
    void setDirty();

    /**
     *  getter
     *   \param reset if true the dirty flag will be set to false
     *  \return the dirty flag
     */
    bool dirty( bool reset = false );

    /**
     * Add a specified notifier to the list of default notifiers which get connected to each added roi.
     *
     * \param notifier  the notifier function
     */
    void addAddNotifier( boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > notifier );

    /**
     * Remove a specified notifier from the list of default notifiers which get connected to each added roi.
     *
     * \param notifier  the notifier function
     */
    void removeAddNotifier( boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > notifier );


    /**
     * Add a specified notifier to the list of default notifiers which get connected to each removed roi.
     *
     * \param notifier  the notifier function
     */
    void addRemoveNotifier( boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > notifier );

    /**
     * Remove a specified notifier from the list of default notifiers which get connected to each removed roi.
     *
     * \param notifier  the notifier function
     */
    void removeRemoveNotifier( boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > notifier );

    /**
     * Add a specified notifier to the list of default notifiers which get connected to each removed branch.
     *
     * \param notifier  the notifier function
     */
    void addRemoveBranchNotifier( boost::shared_ptr< boost::function< void( boost::shared_ptr< WRMBranch > ) > > notifier );

    /**
     * Remove a specified notifier from the list of default notifiers which get connected to each removed branch.
     *
     * \param notifier  the notifier function
     */
    void removeRemoveBranchNotifier(  boost::shared_ptr< boost::function< void( boost::shared_ptr< WRMBranch > ) > > notifier );

    /**
     * setter
     * \param roi
     */
    void setSelectedRoi( osg::ref_ptr< WROI > roi );

    /**
     * getter
     *
     * \return Pointer to the currently (in the ROI manager) selected ROI
     */
    osg::ref_ptr< WROI > getSelectedRoi();

    /**
     * getter for the properties object
     * \return the properties object
     */
    boost::shared_ptr< WProperties > getProperties();

    /**
     * ROI list
     */
    typedef std::vector< osg::ref_ptr< WROI > > ROIs;

    /**
     * getter
     * \return all existing rois
     */
    ROIs getRois() const;

    /**
     * Branches list
     */
    typedef std::vector< boost::shared_ptr< WRMBranch > > Branches;

    /**
     * Get a copy of the current branch list. Please note that after getting the list, it might already have been changed by another thread.
     *
     * \return the list of current branches
     */
    Branches getBranches() const;

protected:
private:
    size_t m_size; //!< number of fibers in the dataset

    std::list< boost::shared_ptr< WRMBranch > > m_branches; //!< list of branches in the logical tree structure

    /**
     * Lock for associated notifiers set.
     */
    boost::shared_mutex m_associatedNotifiersLock;

    /**
     * The notifiers connected to added rois by default.
     */
    std::list<  boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > > m_addNotifiers;

    /**
     * The notifiers connected to removed rois by default.
     */
    std::list<  boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > > m_removeNotifiers;

    /**
     * The notifiers connected to removed rois by default.
     */
    std::list<  boost::shared_ptr< boost::function< void( boost::shared_ptr< WRMBranch > ) > > > m_removeBranchNotifiers;


    osg::ref_ptr< WROI > m_selectedRoi; //!< stores a pointer to the currently selected roi

    /**
     * The property object for the module.
     */
    boost::shared_ptr< WProperties > m_properties;

    /**
     * dirty flag
     */
    WPropBool m_dirty;
};

inline bool WROIManager::dirty( bool reset )
{
    bool ret = m_dirty->get();
    if( reset )
    {
        m_dirty->set( false );
    }
    return ret;
}

inline boost::shared_ptr< WProperties > WROIManager::getProperties()
{
    return m_properties;
}

#endif  // WROIMANAGER_H
