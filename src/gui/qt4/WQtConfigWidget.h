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

#ifndef WQTCONFIGWIDGET_H
#define WQTCONFIGWIDGET_H

#include <map>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <QtCore/QObject>
#include <QtGui/QWidget>
#include <QtGui/QTabWidget>
#include <QtGui/QPushButton>
#include <QtGui/QClipboardEvent>

#include "../../common/WProperties.h"
#include "../../common/WThreadedRunner.h"
#include "datasetbrowser/WQtDSBWidget.h"

/**
* Class that provides registration, loading and writing of the standard config file walnut.cfg
* This also creates a gui from all registered components and makes it possible to apply the changes
* made in the gui directly to the main program
*/
class WQtConfigWidget : public QWidget, public WThreadedRunner
{
    Q_OBJECT

public:
    /**
    * constructor
    */
    WQtConfigWidget();

    /**
    * destructor
    */
    virtual ~WQtConfigWidget();

    /**
    * Simple initialization which calls all necessary functions to register the components,
    * loads the config file, creates and shows the gui and starts the thread
    */
    void initAndShow();

protected:
    /**
    * start the thread and handle the changes in the widgets which correspond to properties
    */
    void threadMain();

    /**
    * catch the closing event of the small x in the right upper corner to react on the closing
    * of the dialog
    *
    * \param event event to catch
    */
    void closeEvent( QCloseEvent *event );

private:
    /**
    * registers the components which then get a widget element
    */
    void registerComponents();

    /**
    * reads the config file from disc into m_loadedProperties
    */
    void loadConfigFile();

    /**
    * assign a boost::shared_ptr< WProperties > to every line in the config file reed
    */
    void createLineAssociation();

    /**
    * determine if to WPropertieVariable have the same value
    * \note also subgroups ar searched and if the property name doesn't exist in either of both groups
    * and their subgroups the function returns false
    *
    * \param prop1 first property group
    * \param prop2 second property group
    * \param propName name of the property inside the group
    *
    * \return both properties got the same value
    */
    bool isPropertyEqual( boost::shared_ptr< WProperties > prop1, boost::shared_ptr< WProperties > prop2, std::string propName );

    /**
    * write the m_properties to the walnut.cfg with respect to the file we loaded at the initialization
    */
    void saveToConfig();

    /**
    * reacts on the changes of a property inside the gui and is mainly used if a property is represented in different
    * ways, we usually call it after initially setting up the gui and when one of the property components changes
    * so the other representations show the same content
    *
    * \param properties the list on which the change should occur
    * \param groupName the name of the group ( this should be added manually inside the function for every group added, so tutorial
    * inside of registerComponents() )
    * \param fromConfig do we update from the m_loadedProperties or from the m_properties
    */
    void updatePropertyGroups( boost::shared_ptr< WProperties > properties, std::string groupName, bool fromConfig );

    /**
    * helper function to assign a line number from the reed config file to a property
    *
    * \param var the property we want the line associated to
    * \param lineNumber the line number to associate
    */
    void addLineToProperty( boost::shared_ptr< WProperties > var, size_t lineNumber );

    /**
    * initializes the module list by asking the module factory for available prototypes
    */
    void getAvailableModuleNames();

    /**
    * updates the group contents of the gui
    *
    * \param properties the property group to update
    */
    void updateGui( boost::shared_ptr< WProperties > properties );

    /**
    * recursive copy a property group, also copies constraints
    * \note some errors may occur with some costume constraints, e.g. min and max have to be set by hand,
    * see implementation for details
    *
    * \param from source property group
    * \return cloned property group
    */
    boost::shared_ptr< WProperties > copyProperties( boost::shared_ptr< WProperties > from );

    /**
    * recursive copy of the values of property groups
    * \note both property groups need to be initialized the same way e.g. with copyProperties
    * so that they got the same WPropertyVariables
    *
    * \param from source property group
    * \param to target property group
    * \param lock true if locking should be used. For recursive calls, this needs to be false.
    */
    void copyPropertiesContents( boost::shared_ptr< WProperties > from, boost::shared_ptr< WProperties > to, bool lock = true );

    /**
    * create a tab widget for a give section
    *
    * \param from property group to create the tab from
    * \param sectionName the name of the section where from all the properties will get a widget
    *
    * \result pointer to the widget
    */
    WQtDSBWidget *createTabForSection( boost::shared_ptr< WProperties > from, std::string sectionName );

    /**
    * creates the whole gui for a give property group
    *
    * \param from property group to create the gui from
    */
    void createGuiFromProperties( boost::shared_ptr< WProperties > from );

    /**
    * applies changes to the main program from changed properties
    *
    * \note if you want to add the effect of a property to the main program implement it in here
    * see tutorial and example for details
    *
    * \result have any properties been set different from the loaded properties and therefore have changes been made to the
    * main program
    */
    bool setWalnutFromProperties();

    /**
    * search after a concrete property inside of a property group, will also find properties in sub groups
    *
    * \param searchIn property group to search in
    * \param name name of the property to find
    * \param lock use locking. This is needed for recursive calls.
    *
    * \result the property if found otherwise an empty shared_ptr
    */
    boost::shared_ptr< WPropertyBase > findPropertyRecursive( boost::shared_ptr< WProperties > searchIn, std::string name, bool lock = true );

    /**
    * the tab widget
    */
    QTabWidget *m_tabWidget;

    /**
    * the cancel button
    */
    QPushButton *m_cancelButton;

    /**
    * the ok button
    */
    QPushButton *m_okButton;

    /**
    * the preview button
    */
    QPushButton *m_previewButton;

    /**
    * the default reset button
    */
    QPushButton *m_resetDefaultButton;

    /**
    * the backup reset button
    */
    QPushButton *m_resetBackupButton;

    /**
    * default property group
    */
    boost::shared_ptr< WProperties > m_defaultProperties;

    /**
    * loaded property group
    */
    boost::shared_ptr< WProperties > m_loadedProperties;

    /**
    * property group shown by gui
    */
    boost::shared_ptr< WProperties > m_properties;

    /**
    * map to associate WProperties to line numbers
    */
    typedef std::map< boost::shared_ptr< WProperties >, std::vector< size_t > > LinePropertySet;

    /**
    * an association container for properties and line numbers
    */
    LinePropertySet m_lineAssociationList;

    /**
    * the plain text from the config file as loaded
    */
    std::vector< std::string > m_configLines;

    /**
    * all the registered sections
    */
    std::vector< std::string > m_registeredSections;

    /**
    * property conditions which are changed if any of the properties get changed
    */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
    * list of available modules
    */
    std::vector< std::string > m_moduleNames;

    /**
    * list of properties which wont be written to the config file
    */
    std::vector< std::string > m_skipPropertyWrite;

    /**
    * state set which will be waited for in the main loop to capture changes in properties
    */
    WConditionSet m_configState;

    /**
    * is there a preview active so we can reset changes to main program after changes not applied
    */
    bool m_previewed;
public slots:

    /**
    * preview the changes made in config editor
    */
    void preview();

    /**
    * reset the properties to their default value (also resets the preview)
    */
    void resetToDefaults();

    /**
    * reset the properties to the loaded config file values (also resets the preview)
    */
    void resetToBackup();

    /**
    * apply the changes and save them to the config
    */
    void save();

    /**
    * close the editor and stop the thread, reset the preview if made
    */
    void cancel();

signals:
    /**
    * react on a close command
    */
    void closeWidget();
};

#endif  // WQTCONFIGWIDGET_H
