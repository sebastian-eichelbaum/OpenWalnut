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

#ifndef WQTDATASETBROWSER_H
#define WQTDATASETBROWSER_H

#include <string>
#include <vector>

#include <QtGui/QDockWidget>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

#include "../../../dataHandler/WDataSet.h"
#include "../../../graphicsEngine/WROI.h"
#include "../../../modules/fiberDisplay/WRMROIRepresentation.h"
#include "WQtDSBWidget.h"
#include "WQtModuleHeaderTreeItem.h"
#include "WQtModuleTreeItem.h"
#include "WQtRoiHeaderTreeItem.h"
#include "WQtSubjectTreeItem.h"
#include "WQtTreeWidget.h"

class WMainWindow;

/**
 * container widget for a tree widget with context menu and some control widgets
 */
class WQtDatasetBrowser : public QDockWidget
{
    Q_OBJECT

public:
    /**
     * Default constructor.
     *
     * \param parent Parent widget.
     *
     * \return
     */
    explicit WQtDatasetBrowser( WMainWindow* parent = 0 );

    /**
     * Destructor.
     */
    virtual ~WQtDatasetBrowser();

    /**
     * adds a page to the context widget
     *
     * \param content A widget with controls
     */
    void addTabWidgetContent( WQtDSBWidget* content );

    /**
     * adds a subject entry to the tree widget
     *
     * \param name The entry name of the subjectin the tree widget
     * \return A pointer to the tree widget item
     */
    WQtSubjectTreeItem* addSubject( std::string name );

    /**
     * adds a dataset entry to any given subject in the tree widget
     *
     * \param module shared pointer to the module associated with this tree widget entry
     * \param subjectId subject id this dataset belongs to
     *
     * \return A pointer to the tree widget item
     */
    WQtDatasetTreeItem* addDataset( boost::shared_ptr< WModule > module, int subjectId = 0 );

    /**
     * Adds a module to the dataset browser.
     *
     * \param module the module to add.
     *
     * \return the representation in dataset browser.
     */
    WQtModuleTreeItem* addModule( boost::shared_ptr< WModule > module );

    /**
     * Adds a roi entry to the dataset browser
     *
     * \param roi pointer to the roi representation object
     */
    void addRoi( boost::shared_ptr< WRMROIRepresentation > roi );

    /**
     * Adds a module to the dataset browser. This function will combine addDataset and addModule
     *
     * \param module the module to add.
     * \param subjectId the subject under which the entry is added
     */
    void addModule2( boost::shared_ptr< WModule > module, int subjectId = 0 );

    /**
     * Returns a vector of pointers to the loaded datasets for a given subject.
     *
     * \param subjectId The ID of the subject to get the list for.
     * \param onlyTextures True if only textures should be returned.
     *
     * \return the list of datasets.
     */
    std::vector< boost::shared_ptr< WDataSet > > getDataSetList( int subjectId, bool onlyTextures = false );

    /**
     * helper funtion to connect all qt widgets with their functions
     */
    void connectSlots();

    /**
     * Returns the module currently selected in dataset browser.
     *
     * \return the module.
     */
    boost::shared_ptr< WModule > getSelectedModule();

    /**
     * Returns the currently selected roi.
     *
     * \return pointer to roi representation
     */
    boost::shared_ptr< WRMROIRepresentation > getSelectedRoi();

public slots:
    /**
     * slot to connect dynamically created controls
     *
     * \param name The name of the control
     * \param value
     */
    void slotSetIntProperty( QString name, int value );

    /**
     * slot to connect dynamically created controls
     *
     * \param name The name of the control
     * \param value The value of the property
     */
    void slotSetDoubleProperty( QString name, double value );

    /**
     * slot to connect dynamically created controls
     *
     * \param name The name of the control
     * \param value
     */
    void slotSetBoolProperty( QString name, bool value );

    /**
     * slot to connect dynamically created controls
     *
     * \param name The name of the control
     * \param value
     */
    void slotSetStringProperty( QString name, QString value );

protected:

    /**
     * Custom event dispatcher. Gets called by QT's Event system every time an event got sent to this widget. This event handler
     * processes several custom events, like WModuleAssocEvent.
     *
     * \param event the event that got transmitted.
     *
     * \return true if the event got handled properly.
     */
    virtual bool event( QEvent* event );

    /**
     * Helper function to return the first subject id in the tree widget
     */
    int getFirstSubject();

    /**
     * fills the modul toolbar with buttons for modules that are compatible with the currently
     * selected dataset
     *
     * \param module pointer to the currently selected module
     */
    void createCompatibleButtons( boost::shared_ptr< WModule >module );

    /**
     * returns a pointer to the properties object of the currently selected tree item
     *
     * \return the properties
     */
    boost::shared_ptr< WProperties > getPropOfSelected();

    /**
     * Reference to the main window of the application.
     */
    WMainWindow* m_mainWindow;

private:
    WQtTreeWidget* m_treeWidget; //!< pointer to the tree widget

    QTabWidget* m_tabWidget; //!< pointer to the tab widget

    QPushButton* m_downButton; //!< button down

    QPushButton* m_upButton; //!< button up

    QWidget* m_tab1; //!< tab 1

    QWidget* m_tab2; //!< tab 2

    QWidget* m_tab3; //!< tab 3

    QWidget* m_panel; //!< panel

    QVBoxLayout* m_layout; //!< layout

    WQtModuleHeaderTreeItem* m_tiModules; //!< header for modules

    WQtRoiHeaderTreeItem* m_tiRois; //!< header for rois

private slots:
    /**
     * function that gets called when a tree item is selected, on a new select that tab widget
     * is rebuilt with the controls provided by the tree item
     */
    void selectTreeItem();

    /**
     * function gets called when a change to a tree item, eg. check box status, occurs
     */
    void changeTreeItem();

    /**
     * change order of items, move currently selected item down
     */
    void moveTreeItemDown();

    /**
     * change order of items, move currently selected item up
     */
    void moveTreeItemUp();


signals:
    /**
     * notifies the outside world of changes in the dsb
     *
     * \param name the name of the event
     * \param value bool
     */
    void dataSetBrowserEvent( QString name, bool value );
};

#endif  // WQTDATASETBROWSER_H
