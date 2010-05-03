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

#include <Qt/qobject.h>

#include <map>
#include <string>
#include <vector>

#include <QtGui/QWidget>
#include <QtGui/QTabWidget>
#include <QtGui/QPushButton>
#include <QtGui/QClipboardEvent>

#include <boost/shared_ptr.hpp>

#include "../../common/WProperties.h"
#include "../../common/WThreadedRunner.h"
#include "datasetbrowser/WQtDSBWidget.h"

class WQtConfigWidget : public QWidget, WThreadedRunner
{
    Q_OBJECT

public:
    WQtConfigWidget();

    virtual ~WQtConfigWidget();

    void initAndShow();

protected:
    void threadMain();

    void closeEvent( QCloseEvent *event );

private:
    void registerComponents();

    void loadConfigFile();

    void createLineAssociation();

    bool isPropertyEqual( boost::shared_ptr< WProperties > prop1, boost::shared_ptr< WProperties > prop2, std::string propName );

    void saveToConfig();

    void updatePropertyGroups( boost::shared_ptr< WProperties > properties, std::string groupName, bool fromConfig );

    void addLineToProperty( boost::shared_ptr< WProperties > var, size_t lineNumber );

    void getAvailibleModuleNames();

    void updateGui( boost::shared_ptr< WProperties > properties );

    void copyProperties( boost::shared_ptr< WProperties > from, boost::shared_ptr< WProperties > to );

    void copyPropertiesContents( boost::shared_ptr< WProperties > from, boost::shared_ptr< WProperties > to );

    WQtDSBWidget *createTabForSection( boost::shared_ptr< WProperties > from, std::string sectionName );

    void createGuiFromProperties( boost::shared_ptr< WProperties > from );

    bool setWalnutFromProperties();

    boost::shared_ptr< WPropertyBase > findPropertyRecursive( boost::shared_ptr< WProperties > searchIn, std::string name );

    QWidget *m_window;
    QTabWidget *m_tabWidget;
    QPushButton *m_cancelButton;
    QPushButton *m_okButton;
    QPushButton *m_previewButton;
    QPushButton *m_resetDefaultButton;
    QPushButton *m_resetBackupButton;

    boost::shared_ptr< WProperties > m_defaultProperties;
    boost::shared_ptr< WProperties > m_loadedProperties;
    boost::shared_ptr< WProperties > m_properties;

    typedef std::map< boost::shared_ptr< WProperties >, std::vector< size_t > > linePropertySet;

    linePropertySet m_lineAssociationList;

    std::vector< std::string > m_configLines;

    std::vector< std::string > registeredSections;

    boost::shared_ptr< WCondition > m_propCondition;

    std::vector< std::string > m_moduleNames;

    std::vector< std::string > m_skipPropertyWrite;

    WConditionSet m_configState;

    bool m_previewed;
public slots:

    void preview();

    void resetToDefaults();

    void resetToBackup();

    void save();

    void cancel();

signals:
    void closeWidget();
};

#endif  // WQTCONFIGWIDGET_H
