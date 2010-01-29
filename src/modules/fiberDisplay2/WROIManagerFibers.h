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
#include "WMFiberDisplay2.h"

#include "WRMBranch.h"

/**
 * Class to store and manage different ROI's for fiber selection
 */
class WROIManagerFibers: public boost::enable_shared_from_this< WROIManagerFibers >
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
     */
    void addRoi( boost::shared_ptr< WROI > newRoi );

    /**
     * adds a new ROI below a master ROI
     *
     * \param newRoi
     * \param parentRoi
     */
    void addRoi( boost::shared_ptr< WROI > newRoi, boost::shared_ptr< WROI > parentRoi );

    /**
     * removes a roi
     *
     * \param roi
     */
    void removeRoi( boost::shared_ptr< WROI > roi );

    /**
     * connects a fiber dataset to the roi manager
     *
     * \param fibers
     */
    void addFiberDataset( boost::shared_ptr< const WDataSetFibers2 > fibers );

    /**
     * removes a fiber dataset from the roi manager
     *
     * \param fibers
     */
    void removeFiberDataset( boost::shared_ptr< const WDataSetFibers2 > fibers );

    /**
     * returns a bit vector for the selected fiber dataset that is the result of all active ROI's
     * on that fiber dataset
     *
     * \param fibers
     * \return std::vector<bool>*
     */
    boost::shared_ptr< std::vector< bool > > getBitField( boost::shared_ptr< const WDataSetFibers2 > fibers );

    /**
     *
     */
    boost::shared_ptr< const WDataSetFibers2 > getDataSet( unsigned int index );

    /**
     *
     */
    boost::shared_ptr< WKdTree > getKdTree( unsigned int index );

    /**
     *
     */
    void addBitField( size_t size );

    /**
     *
     */
    void setDirty();

    /**
     *
     */
    bool isDirty();

    /**
     * Add a specified notifier to the list of default notifiers which get connected to each added module.
     *
     * \param signal    the signal the notifier should get connected to
     * \param notifier  the notifier function
     */
    virtual void addDefaultNotifier( boost::function< void( boost::shared_ptr< WRMROIRepresentation > ) > notifier );

protected:
private:
    /**
     *
     */
    void recalculate();

    bool m_dirty;

    std::list< boost::shared_ptr< const WDataSetFibers2 > > m_fiberList;

    std::list< boost::shared_ptr< std::vector< bool > > > m_bitFields;

    std::list< boost::shared_ptr< WRMBranch > > m_branches;

    /**
     * Stores a pointer to the kdTree used for fiber selection
     */
    std::list< boost::shared_ptr< WKdTree > > m_kdTreeList;

    /**
     * Lock for associated notifiers set.
     */
    boost::shared_mutex m_associatedNotifiersLock;

    /**
     * The notifiers connected to added rois by default.
     */
    std::list< boost::function< void( boost::shared_ptr< WRMROIRepresentation > ) > > m_notifiers;
};

#endif  // WROIMANAGERFIBERS_H
