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

#ifndef WQTCONTROLPANEL_H
#define WQTCONTROLPANEL_H

#include <list>
#include <string>
#include <vector>

#include <QtGui/QDockWidget>
#include <QtGui/QSplitter>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtGui/QListWidget>

#include "core/dataHandler/WDataSet.h"
#include "core/graphicsEngine/WROI.h"

#include "../WQtModuleConfig.h"

#include "../WQtCombinerToolbar.h"
#include "WQtPropertyGroupWidget.h"
#include "WQtModuleHeaderTreeItem.h"
#include "WQtModuleTreeItem.h"
#include "WQtRoiHeaderTreeItem.h"
#include "WQtSubjectTreeItem.h"
#include "WQtTreeWidget.h"

class WMainWindow;
class WQtColormapper;

/**
 * container widget for a tree widget with context menu and some control widgets
 */
class WQtControlPanel : public QDockWidget
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * \param parent Parent widget.
     */
    explicit WQtControlPanel( WMainWindow* parent = 0 );

    /**
     * Default Destructor.
     */
    virtual ~WQtControlPanel();

    /**
     * Complete own GUI setup. This is useful when not all the information is available during construction. Called by the WMainWindow.
     */
    void completeGuiSetup();

    /**
     * Adds a page to the context widget
     *
     * \param content A widget with controls
     *
     * \return the index of the new tab
     */
    int addTabWidgetContent( WQtPropertyGroupWidget* content );

    /**
     * Adds a subject entry to the tree widget
     *
     * \param name The entry name of the subjectin the tree widget
     * \return A pointer to the tree widget item
     */
    WQtSubjectTreeItem* addSubject( std::string name );

    /**
     * Adds a dataset entry to any given subject in the tree widget
     *
     * \param module shared pointer to the module associated with this tree widget entry
     * \param subjectId subject id this dataset belongs to
     *
     * \return A pointer to the tree widget item
     */
    WQtDatasetTreeItem* addDataset( boost::shared_ptr< WModule > module, int subjectId = 0 );

    /**
     * Adds a module to the control panel.
     *
     * \param module the module to add.
     *
     * \return the representation in control panel.
     */
    WQtModuleTreeItem* addModule( boost::shared_ptr< WModule > module );

    /**
     * Adds a ROI entry to the control panel
     *
     * \param roi pointer to the ROI representation object
     */
    void addRoi( osg::ref_ptr< WROI > roi );

    /**
     * Removes a ROI entry from the control panel
     *
     * \param roi pointer to the ROI representation object
     */
    void removeRoi( osg::ref_ptr< WROI > roi );

    /**
     * helper funtion to connect all qt widgets with their functions
     */
    void connectSlots();

    /**
     * Returns the module currently selected in control panel.
     *
     * \return the module.
     */
    boost::shared_ptr< WModule > getSelectedModule();

    /**
     * Returns the currently selected ROI.
     *
     * \return pointer to ROI representation
     */
    osg::ref_ptr< WROI > getSelectedRoi();

    /**
     * Returns the first ROI in the currently selected branch.
     *
     * \return pointer to ROI representation
     */
    osg::ref_ptr< WROI > getFirstRoiInSelectedBranch();

    /**
     * Returns a checkable action that can be used to show or close this dock widget.
     * The action's text is set to the dock widget's window title.
     *
     * This member function is overwritten to add a keyboard shortcut to this action.
     *
     * \return Modified QAction
     */
    QAction* toggleViewAction() const;

    /**
     * Selects the uppermost entry in the module tree.
     */
    void selectUpperMostEntry();

    /**
     * Gets the ROI dock widget.
     *
     * \return the ROI dock.
     */
    QDockWidget* getRoiDock() const;

    /**
     * Gets the module dock widget.
     *
     * \return the module dock
     */
    QDockWidget* getModuleDock() const;

    /**
     * Gets the colormapper dock
     *
     * \return the dock
     */
    QDockWidget* getColormapperDock() const;

    /**
     * Returns the module excluder. It then can be used for configuration.
     *
     * \return the module excluder.
     */
    WQtModuleConfig& getModuleConfig() const;

    /**
     * Returns an action which can be triggered by the user if some module are missing.
     *
     * \return the action.
     */
    QAction* getMissingModuleAction() const;

    /**
     * Sets the module which is now active. Updates the GUI accordingly. Can be NULL which causes the GUI to remove all module specific stuff.
     *
     * \param module the module to activate
     * \param forceUpdate force update even if the module is the same as the current one.
     */
    void setActiveModule( WModule::SPtr module, bool forceUpdate = false );

    /**
     * Used to clean the GUI from any module specify widgets.
     *
     * \param selectTopmost select top element or keep current
     */
    void deactivateModuleSelection( bool selectTopmost = true );
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
     *
     * \return index of first subject in tree
     */
    int getFirstSubject();

    /**
     * fills the modul toolbar with buttons for modules that are compatible with the currently
     * selected dataset
     *
     * \param module pointer to the currently selected module
     *
     */
    void createCompatibleButtons( boost::shared_ptr< WModule > module );

    /**
     * Reference to the main window of the application.
     */
    WMainWindow* m_mainWindow;

    /**
     * Searches the specified tree for all tree items matching the specified module.
     *
     * \param module the module uses as search criteria.
     * \param where in which subtree to search.
     *
     * \return a list of all matching items.
     */
    std::list< WQtTreeItem* > findItemsByModule( boost::shared_ptr< WModule > module, QTreeWidgetItem* where );

    /**
     * Searches the tree for all tree items matching the specified module.
     *
     * \param module the module uses as search criteria.
     *
     * \return a list of all matching items.
     */
    std::list< WQtTreeItem* > findItemsByModule( boost::shared_ptr< WModule > module );

    /**
     * As QTabWidget::clear() does not delete tabs, we have to manage that ourselves.
     * So this function clears all tabs and deletes the data as well.
     */
    void clearAndDeleteTabs();

private:
    WQtTreeWidget* m_moduleTreeWidget; //!< pointer to the tree widget

    WQtTreeWidget* m_roiTreeWidget; //!< pointer to the tree widget

    WQtColormapper* m_colormapper; //!< the colormapper control widget

    QTabWidget* m_tabWidget; //!< pointer to the tab widget

    QTabWidget* m_tabWidget2; //!< pointer to the tab widget

    QSplitter* m_splitter; //!< splitter to have resizable widgets in the control panel

    WQtModuleHeaderTreeItem* m_tiModules; //!< header for modules

    WQtRoiHeaderTreeItem* m_tiRois; //!< header for ROIs

    bool m_showToolBarText; //!< Show tool bar icons with text

    QDockWidget* m_roiDock;     //!< the dock widget with the ROI tree
    QDockWidget* m_moduleDock;  //!< the dock widget with the module tree

    /**
     * The action to remove a module from the tree.
     */
    QAction* m_deleteModuleAction;

    /**
     * The action to remove a ROI from the tree.
     */
    QAction* m_deleteRoiAction;

    /**
     * Action which uses a compatibles list (submenu) to connect a selected item with other existing modules.
     */
    QAction* m_connectWithModuleAction;

    /**
     * Action which uses a compatibles list (submenu) to connect a selected item with other prototypes.
     */
    QAction* m_connectWithPrototypeAction;

    /**
     * Action which uses a list of all modules allowing them to be added without any connections.
     */
    QAction* m_addModuleAction;

    /**
     * Action which disconnects a connector from the module.
     */
    QAction* m_disconnectAction;

    /**
     * List all actions created for applying a prototype. Is needed for m_addModuleAction.
     *
     * \note We need to store this action list here as Qt is not able to delete the actions if they get replaced. We need to handle this
     * manually.
     */
    WQtCombinerActionList m_addModuleActionList;

    /**
     * List all actions created for applying a prototype. Is needed for m_connectWithPrototypeAction.
     *
     * \note We need to store this action list here as Qt is not able to delete the actions if they get replaced. We need to handle this
     * manually.
     */
    WQtCombinerActionList m_connectWithPrototypeActionList;

    /**
     * List all actions created for applying a prototype. Is needed for m_connectWithModuleAction.
     *
     * \note We need to store this action list here as Qt is not able to delete the actions if they get replaced. We need to handle this
     * manually.
     */
    WQtCombinerActionList m_connectWithModuleActionList;

    /**
     * List all actions created for applying a prototype. Is needed for m_disconnectAction.
     *
     * \note We need to store this action list here as Qt is not able to delete the actions if they get replaced. We need to handle this
     * manually.
     */
    WQtCombinerActionList m_disconnectActionList;

    /**
     * If true, a selection change does not cause the property tab to rebuild. This is useful if multiple items get selected at once
     * programatically.
     */
    bool m_ignoreSelectionChange;

    /**
     * Ignore recursive selection update in network editor.
     */
    bool m_ignoreSelectionChangeNWE;

    /**
     * The WQtCombinerActionList needs some predicate which decides whether to exclude a certain module from the list or not. We use this
     * predicate here. It is configured internally using a white and blacklist.
     */
    WQtModuleConfig* m_moduleExcluder;

    /**
     * Action giving the user fast access to the module config dialog.
     */
    QAction* m_missingModuleAction;

    /**
     * The module currently active
     */
    WModule::SPtr m_activeModule;

    /**
     * The title of the last selected tab in the control panel. This needs to be done using the tab name as the tab index is not consistent(
     * depending on the number of tabs. Sometimes, some tabs are not visible).
     */
    QString m_previousTab;
private slots:
    /**
     * function that gets called when a tree item is selected, on a new select that tab widget
     * is rebuilt with the controls provided by the tree item
     */
    void selectTreeItem();

    /**
     * This de-selects and re-selects the current item. This is useful for updating compatibles bar and similar if data changes
     */
    void reselectTreeItem();

    /**
     * function that gets called when a tree item is selected, on a new select that tab widget
     * is rebuilt with the controls provided by the tree item
     */
    void selectRoiTreeItem();

    /**
     * Will be called to select the data module for the given texture.
     * \param texture the texture currently selected.
     */
    void selectDataModule( osg::ref_ptr< WGETexture3D > texture );

    /**
     * Search the tree item representing this module
     *
     * \param module the module
     *
     * \return the item or null if not found
     */
    QTreeWidgetItem* findModuleItem( WModule::SPtr module ) const;

    /**
     * function that builds the property tab
     *
     * \param props the properties.
     * \param infoProps the information properties shown on a separate tab
     */
    void buildPropTab( boost::shared_ptr< WProperties > props, boost::shared_ptr< WProperties > infoProps );

    /**
     * Function gets change when a change to a tree item occurs.
     *
     * \param item the item
     * \param column column index
     */
    void changeTreeItem( QTreeWidgetItem* item, int column );

    /**
     * delete a ROI tree item
     */
    void deleteROITreeItem();

    /**
     * delete a module
     */
    void deleteModule();

    /**
     * function to notify the ROI manager of any drag&drop action in the ROI tree
     */
    void handleRoiDragDrop();
};

#endif  // WQTCONTROLPANEL_H
