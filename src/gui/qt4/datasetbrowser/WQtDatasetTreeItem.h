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

#ifndef WQTDATASETTREEITEM_H
#define WQTDATASETTREEITEM_H

#include <QtGui/QTreeWidgetItem>

#include "../signalslib.hpp"
/**
 * tree widget item to represent a dataset in the dataset browser tree widget
 */
class WQtDatasetTreeItem : public QTreeWidgetItem
{
public:
    /**
     * standard constructor
     */
    explicit WQtDatasetTreeItem( QTreeWidgetItem * parent );

    /**
     * destructor
     */
    virtual ~WQtDatasetTreeItem();

    /**
     * getter for the signal object
     */
    boost::signal0< void >* getSignalSelect();

    /**
     * helper function to signal this item has been selected
     */
    void emitSelect();
protected:
private:
    /**
     * the boost signal object
     * TODO(schurade): change this to a signal with parameters
     */
    boost::signal0< void > m_signalSelect;
};

#endif  // WQTDATASETTREEITEM_H
