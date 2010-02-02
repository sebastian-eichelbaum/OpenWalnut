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
     * getter for a registered dataset
     *
     * \param index of the dataset
     * \return the dataset
     */
    boost::shared_ptr< const WDataSetFibers2 > getDataSet( unsigned int index );

    /**
     * getter for a kd tree
     * \param index
     * return the kd tree
     */
    boost::shared_ptr< WKdTree > getKdTree( unsigned int index );

    /**
     * adds a bit field list of bit fields
     *
     * \param size
     */
    void addBitField( size_t size );

    /**
     * sets the dirty flag which will cause recalculation of the bit field
     */
    void setDirty();

    /**
     *  getter
     *  \return the dirty flag
     */
    bool isDirty();

    /**
     * Add a specified notifier to the list of default notifiers which get connected to each added module.
     *
     * \param notifier  the notifier function
     */
    virtual void addDefaultNotifier( boost::function< void( boost::shared_ptr< WRMROIRepresentation > ) > notifier );

protected:
private:
    /**
     * updates the bit fields
     */
    void recalculate();

    bool m_dirty; //!< dirty flag

    std::list< boost::shared_ptr< const WDataSetFibers2 > > m_fiberList; //!< list of registered fiber datasets

    std::list< boost::shared_ptr< std::vector< bool > > > m_bitFields; //!< list of bitfields, one for each dataset

    std::list< boost::shared_ptr< WRMBranch > > m_branches; //!< list of branches in the logical tree structure

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
