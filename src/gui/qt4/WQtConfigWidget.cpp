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

#include <list>
#include <vector>
#include <string>

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>

#include "WCfgOperations.h"
#include "../../kernel/WModuleFactory.h"
#include "../../common/WConditionOneShot.h"
#include "../../common/WPathHelper.h"

#include "WQtConfigWidget.h"

WQtConfigWidget::WQtConfigWidget() :
        QWidget()
{
    this->setWindowTitle( "walnut.cfg Editor" );

    QVBoxLayout *verticalLayout = new QVBoxLayout( this );
    m_tabWidget = new QTabWidget();
    verticalLayout->addWidget( m_tabWidget );
    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    verticalLayout->addLayout( horizontalLayout );
    m_okButton = new QPushButton( "Save" );
    m_cancelButton = new QPushButton( "Cancel" );
    m_previewButton = new QPushButton( "Preview" );
    m_resetDefaultButton = new QPushButton( "Reset to defaults" );
    m_resetBackupButton = new QPushButton( "Reset to Loaded" );

    horizontalLayout->addWidget( m_okButton );
    horizontalLayout->addWidget( m_cancelButton );
    horizontalLayout->addWidget( m_previewButton );
    horizontalLayout->addWidget( m_resetDefaultButton );
    horizontalLayout->addWidget( m_resetBackupButton );

    connect( m_okButton,           SIGNAL( clicked() ), this, SLOT( save() ) );
    connect( m_cancelButton,       SIGNAL( clicked() ), this, SLOT( cancel() ) );
    connect( m_previewButton,      SIGNAL( clicked() ), this, SLOT( preview() ) );
    connect( m_resetDefaultButton, SIGNAL( clicked() ), this, SLOT( resetToDefaults() ) );
    connect( m_resetBackupButton,  SIGNAL( clicked() ), this, SLOT( resetToBackup() ) );

    connect( this,                 SIGNAL( closeWidget()   ), this, SLOT( cancel() ) );

    // allocate the propertys
    m_defaultProperties = boost::shared_ptr< WProperties >( new WProperties );
    m_loadedProperties = boost::shared_ptr< WProperties >( new WProperties );
    m_properties = boost::shared_ptr< WProperties >( new WProperties );

    registerComponents();

    m_previewed = false;
}

WQtConfigWidget::~WQtConfigWidget()
{
}

void WQtConfigWidget::getAvailableModuleNames()
{
    m_moduleNames.clear();

    // read all prototypes
    WModuleFactory::PrototypeSharedContainerType::ReadTicket pa = WModuleFactory::getModuleFactory()->getPrototypes();
    for ( WModuleFactory::PrototypeContainerConstIteratorType itr = pa->get().begin(); itr != pa->get().end(); ++itr )
    {
        m_moduleNames.push_back( ( *itr )->getName() );
    }
}

void WQtConfigWidget::updatePropertyGroups( boost::shared_ptr< WProperties > properties, std::string groupName, bool fromConfig )
{
    // Here we assume we already registered our properties
    // then we set them one way or another

    // the bool flag can be used to determine which way the update shall be executed

    // Colors:
    // this may also be updated when either color or one of the values gets updated
    if ( groupName == "ge.bgColor" )
    {
        WPropGroup backgroundColor = properties->findProperty( "ge.bgColor" )->toPropGroup();
        WPropDouble r, g, b;
        r = backgroundColor->findProperty( "ge.bgColor.r" )->toPropDouble();
        g = backgroundColor->findProperty( "ge.bgColor.g" )->toPropDouble();
        b = backgroundColor->findProperty( "ge.bgColor.b" )->toPropDouble();
        WPropColor wColorProp = backgroundColor->findProperty( "ge.sColorWidget" )->toPropColor();

        if ( fromConfig )
        {
            WColor color( static_cast< double >( r->get( true ) ), static_cast< double >( g->get( true ) ), static_cast< double >( b->get( true ) ) );
            wColorProp->set( color, true );
        }
        else
        {
            WColor color = wColorProp->get( true );
            r->set( static_cast< double >( color.getRed() ), true );
            g->set( static_cast< double >( color.getGreen() ), true );
            b->set( static_cast< double >( color.getBlue() ), true );
        }
    }
    size_t i;
    if ( groupName == "modules.whiteListGroup" )
    {
        WPropGroup moduleWhiteList = properties->findProperty( "modules.whiteListGroup" )->toPropGroup();
        WPropString whiteList = moduleWhiteList->findProperty( "modules.whiteList" )->toPropString();

        if ( fromConfig )
        {
            for ( i = 0; i < m_moduleNames.size(); ++i )
            {
                bool isWhiteListed = false;
                std::string whiteString = whiteList->get();
                if ( whiteString.find( m_moduleNames[i] ) != std::string::npos )
                {
                    isWhiteListed = true;
                }
                moduleWhiteList->findProperty( "modules.whitelist." + m_moduleNames[i] )->toPropBool()->set( isWhiteListed, true );
            }
        }
        else
        {
            std::string setStr;

            for ( i = 0; i < m_moduleNames.size(); ++i )
            {
                bool isWhiteListed = moduleWhiteList->findProperty( "modules.whitelist." + m_moduleNames[i] )->toPropBool()->get( true );
                if ( isWhiteListed )
                {
                    if ( setStr.length() > 0 )
                    {
                        setStr += ',';
                    }
                    setStr += m_moduleNames[i];
                }
            }
            whiteList->set( setStr, true );
        }
    }

    if ( groupName == "modules.defaultList" )
    {
        WPropGroup moduleDefaultList = properties->findProperty( "modules.defaultList" )->toPropGroup();
        WPropString defaultList = moduleDefaultList->findProperty( "modules.default" )->toPropString();
        if ( fromConfig )
        {
            for ( i = 0; i < m_moduleNames.size(); ++i )
            {
                bool isDefaultListed = false;
                std::string defaultString = defaultList->get();
                if ( defaultString.find( m_moduleNames[i] ) != std::string::npos )
                {
                    isDefaultListed = true;
                }
                moduleDefaultList->findProperty( "modules.defaultList." + m_moduleNames[i] )->toPropBool()->set( isDefaultListed, true );
            }
        }
        else
        {
            std::string setStr;

            for ( i = 0; i < m_moduleNames.size(); ++i )
            {
                bool isDefaultListed = moduleDefaultList->findProperty( "modules.defaultList." + m_moduleNames[i] )->toPropBool()->get( true );
                if ( isDefaultListed )
                {
                    if ( setStr.length() > 0 )
                    {
                        setStr += ',';
                    }
                    setStr += m_moduleNames[i];
                }
            }
            defaultList->set( setStr, true );
        }
    }
}

void WQtConfigWidget::registerComponents()
{
    getAvailableModuleNames();

    //////////////////////////////////////////////////////////////////////////
    //  Tutorial on how to add properties to the config editor              //
    //////////////////////////////////////////////////////////////////////////
    //  1. Make sure the section you use is registered at the beginning     //
    //  2. Add your Properties to m_defaultProperties with a short          //
    //     description and default value                                    //
    //     Note: You can also groups to show the properties in different    //
    //     ways like the background color group                             //
    //  3. every property that isn't represented in the config file needs   //
    //     to be added to m_skipPropertyWrite                               //
    //  4. to react on changes in groups like the color group, add an if    //
    //     section to updatePropertyGroups() and call them in the           //
    //     initialization and at the condition change if clause in the      //
    //     threadMain()                                                     //
    //  5. to make changes directly to openWalnut you need to make sure     //
    //     that there is an appropriate interface and call it from within   //
    //     setWalnutFromProperties()                                        //
    //////////////////////////////////////////////////////////////////////////

    // Register config properties
    m_registeredSections.push_back( "general" );
    m_registeredSections.push_back( "qt4gui" );
    m_registeredSections.push_back( "ge" );
    m_registeredSections.push_back( "modules" );

    m_skipPropertyWrite.clear();

    // to set and get properties use the get() and set() methods of WFlag
    // copy properties through getAccessObject()

    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // add every property existing with its default values
    m_defaultProperties->addProperty( "ge.zoomTrackballManipulator.allowThrow", "Allow auto rotation", false, m_propCondition );
    m_defaultProperties->addProperty( "ge.multiThreadedViewers", "Multithreaded Viewers", true, m_propCondition );
    m_defaultProperties->addProperty( "qt4gui.hideAxial", "Hide Axial View", false, m_propCondition );
    m_defaultProperties->addProperty( "qt4gui.hideCoronal", "Hide Coronal View", false, m_propCondition );
    m_defaultProperties->addProperty( "qt4gui.hideSagittal", "Hide Sagittal View", false, m_propCondition );

    // those are connected, so we put them into a group
    WPropGroup backgroundColor = m_defaultProperties->addPropertyGroup( "ge.bgColor", "color group" );
    m_skipPropertyWrite.push_back( "ge.bgColor" );
    WPropDouble r, g, b;
    r = backgroundColor->addProperty( "ge.bgColor.r", "red value of Background Color", 0.9, m_propCondition );
    g = backgroundColor->addProperty( "ge.bgColor.g", "green value of Background Color", 0.9, m_propCondition );
    b = backgroundColor->addProperty( "ge.bgColor.b", "blue value of Background Color", 0.9, m_propCondition );
    r->setMin( 0.0 );
    r->setMax( 1.0 );
    g->setMin( 0.0 );
    g->setMax( 1.0 );
    b->setMin( 0.0 );
    b->setMax( 1.0 );
    WColor color( static_cast< double >( r->get() ), static_cast< double >( g->get() ), static_cast< double >( b->get() ) );
    backgroundColor->addProperty( "ge.sColorWidget", "Color pick widget", color, m_propCondition );
    m_skipPropertyWrite.push_back( "ge.sColorWidget" );

    m_defaultProperties->addProperty( "qt4gui.useAutoDisplay", "use Auto Display", true, m_propCondition );
    m_defaultProperties->addProperty( "qt4gui.useToolBarBreak", "use ToolBarBreak", true, m_propCondition );
    m_defaultProperties->addProperty( "general.allowOnlyOneFiberDataSet", "allow only one FiberDataSet", false, m_propCondition );
    WPropInt tbs = m_defaultProperties->addProperty( "qt4gui.toolBarStyle", "The style of all toolbars in OpenWalnut", 0, m_propCondition );
    WPropInt ctbs = m_defaultProperties->addProperty( "qt4gui.compatiblesToolBarStyle", "The style of all compatibles toolbar in OpenWalnut", 0,
                                                      m_propCondition );
    tbs->setMin( 0 );
    tbs->setMax( 3 );
    ctbs->setMin( 0 );
    ctbs->setMax( 3 );
    WPropInt tbpos = m_defaultProperties->addProperty( "qt4gui.toolBarPos", "The position of the toolbar in OpenWalnut", 0,
                                                       m_propCondition );
    WPropInt ctbpos = m_defaultProperties->addProperty( "qt4gui.compatiblesToolBarPos", "The position of the compatibles toolbar in OpenWalnut", 0,
                                                        m_propCondition );
    tbpos->setMin( 0 );
    tbpos->setMax( 5 );
    ctbpos->setMin( 0 );
    ctbpos->setMax( 5 );
    m_defaultProperties->addProperty( "qt4gui.hideMenuBar", "Hide the menu bar.", false, m_propCondition );

    WPropInt dsbWidth = m_defaultProperties->addProperty( "qt4gui.dsbWidth", "The width of the dataset browser.", 250, m_propCondition );
    dsbWidth->setMin( 0 );
    dsbWidth->setMax( 1000 );

    m_defaultProperties->addProperty( "qt4gui.dsbInvisibleByDefault", "Hide the dataset browser on startup?", false, m_propCondition );
    m_defaultProperties->addProperty( "qt4gui.dsbFloatingByDefault", "Undock the dataset browser on startup?", false, m_propCondition );

    WPropGroup moduleWhiteList =  m_defaultProperties->addPropertyGroup( "modules.whiteListGroup", "moduleWhiteList" );
    m_skipPropertyWrite.push_back( "modules.whiteListGroup" );
    WPropString whiteList = moduleWhiteList->addProperty( "modules.whiteList", "Modules that are whitelisted",
        std::string( "Isosurface,Bounding Box, Direct Volume Rendering,Distance Map Isosurface,Gauss Filtering,HUD,Vector Plot,Write NIfTI" ),
        m_propCondition, true );
    size_t i;
    for ( i = 0; i < m_moduleNames.size(); ++i )
    {
        bool isWhiteListed = false;
        std::string whiteString = whiteList->toPropString()->get();
        if ( whiteString.find( m_moduleNames[i] ) != std::string::npos )
        {
            isWhiteListed = true;
        }
        moduleWhiteList->addProperty( "modules.whitelist." + m_moduleNames[i],
            "Module " + m_moduleNames[i] + " is on whitelist", isWhiteListed, m_propCondition );
        m_skipPropertyWrite.push_back( "modules.whitelist." + m_moduleNames[i] );
    }

    WPropGroup moduleDefaultList = m_defaultProperties->addPropertyGroup( "modules.defaultList", "default Modules List" );
    m_skipPropertyWrite.push_back( "modules.defaultList" );
    WPropString defaultList = moduleDefaultList->addProperty( "modules.default", "default Modules", std::string( "" ), m_propCondition, true );
    for ( i = 0; i < m_moduleNames.size(); ++i )
    {
        bool isDefaultListed = false;
        std::string defaultString = defaultList->toPropString()->get();
        if ( defaultString.find( m_moduleNames[i] ) != std::string::npos )
        {
            isDefaultListed = true;
        }
        moduleDefaultList->addProperty( "modules.defaultList." + m_moduleNames[i],
            "Module " + m_moduleNames[i] + " is on defaultlist", isDefaultListed, m_propCondition );
        m_skipPropertyWrite.push_back( "modules.defaultList." + m_moduleNames[i] );
    }

    m_defaultProperties->addProperty( "modules.MC.isoValue", "Marching Cubes isoValue", 100.0, m_propCondition );
}

boost::shared_ptr< WProperties > WQtConfigWidget::copyProperties( boost::shared_ptr< WProperties > from )
{
    boost::shared_ptr< WProperties > res = boost::shared_static_cast< WProperties >( from->clone() );

    std::list< boost::shared_ptr< WProperties > > propertyStack;
    std::list< WProperties::PropertyIterator > iteratorStack;

    propertyStack.push_back( res );

    WProperties::PropertyIterator iter;
    WProperties::PropertyAccessType accessObject = propertyStack.back()->getAccessObject();

    iter = accessObject->get().begin();
    iteratorStack.push_back( iter );

    while ( !propertyStack.empty() )
    {
        // check if at the end of group, if so pop and continue
        if ( iteratorStack.back() == propertyStack.back()->getAccessObject()->get().end() )
        {
            propertyStack.pop_back();
            iteratorStack.pop_back();
            continue;
        }

        // add group to stack, and move iterator
        if ( ( *( iteratorStack.back() ) )->getType() == PV_GROUP )
        {
            propertyStack.push_back( ( *iteratorStack.back() )->toPropGroup() );
            ++iteratorStack.back();
            iteratorStack.push_back( propertyStack.back()->getAccessObject()->get().begin() );
            continue;
        }
        else
        {
            boost::shared_ptr< WProperties > prop = ( *( iteratorStack.back() ) )->toPropGroup();

            m_configState.add( prop->getUpdateCondition() );
        }

        ++iteratorStack.back();
    } // LOOP over properties

    return res;
}

void WQtConfigWidget::copyPropertiesContents( boost::shared_ptr< WProperties > from, boost::shared_ptr< WProperties > to, bool lock )
{
    // we assume from and to got the same structure
    // this is needed to keep the pointers, only set the values
    WProperties::PropertyAccessType accesObject = from->getAccessObject();
    WProperties::PropertyAccessType accesObject2 = to->getAccessObject();

    // in recursive calls, this avoids concurrent locks (even if concurrent read locks are allowed).
    if ( lock )
    {
        // Temporarily disabled since locking causes several problems here :-/
        // accesObject->beginRead();
        // accesObject2->beginRead();
    }

    WProperties::PropertyIterator iter;
    WProperties::PropertyIterator iter2;
    for ( iter = accesObject->get().begin(), iter2 = accesObject2->get().begin();
          iter != accesObject->get().end() && iter2 != accesObject2->get().end();
          ++iter, ++iter2 )
    {
        // so far we only need those 5 types
        switch( (*iter)->getType() )
        {
        case PV_GROUP:
            {
                // recurse all groups
                copyPropertiesContents( ( *iter )->toPropGroup(), ( *iter2 )->toPropGroup() );
                break;
            }
        default:
            {
                // copy value
                ( *iter2 )->set( *iter );

                break;
            }
        }
    }

    if ( lock )
    {
        // Temporarily disabled since locking causes several problems here :-/
        // accesObject2->endRead();
        // accesObject->endRead();
    }
}

void WQtConfigWidget::addLineToProperty( boost::shared_ptr< WProperties > var, size_t lineNumber )
{
    LinePropertySet::iterator lPSitr;
    lPSitr = m_lineAssociationList.find( var );
    if ( lPSitr != m_lineAssociationList.end() )
    {
        lPSitr->second.push_back( lineNumber );
    }
    else
    {
        std::vector< size_t > lineList;
        lineList.push_back( lineNumber );
        m_lineAssociationList[ var ] = lineList;
    }
}

boost::shared_ptr< WPropertyBase > WQtConfigWidget::findPropertyRecursive( boost::shared_ptr< WProperties > searchIn, std::string name, bool lock )
{
    WProperties::PropertyAccessType accesObject = searchIn->getAccessObject();

    // avoid concurrent locks in recursive calls (even if concurrent read locks are allowed)
    if ( lock )
    {
        // Temporarily disabled since locking causes several problems here :-/
        // accesObject->beginRead();
    }

    boost::shared_ptr< WPropertyBase > result = boost::shared_ptr< WPropertyBase >();

    // TODO(ledig): WProperties::findProperty already does the job of searching recursively. Better use this.
    WProperties::PropertyIterator iter;
    for ( iter = accesObject->get().begin(); iter != accesObject->get().end(); ++iter )
    {
        if ( ( *iter )->getName() == name )
        {
            result = ( *iter )->toPropGroup();
            // avoid concurrent locks in recursive calls (even if concurrent read locks are allowed)
            if ( lock )
            {
                // Temporarily disabled since locking causes several problems here :-/
                // accesObject->endRead();
            }
            return result;
        }
        if ( ( *iter )->getType() == PV_GROUP )
        {
            boost::shared_ptr< WPropertyBase > tmp = findPropertyRecursive( ( *iter )->toPropGroup(), name );
            if ( tmp )
            {
                // avoid concurrent locks in recursive calls (even if concurrent read locks are allowed)
                if ( lock )
                {
                    // Temporarily disabled since locking causes several problems here :-/
                    // accesObject->endRead();
                }
                return tmp->toPropGroup();
            }
        }
    }

     // avoid concurrent locks in recursive calls (even if concurrent read locks are allowed)
    if ( lock )
    {
        // Temporarily disabled since locking causes several problems here :-/
        // accesObject->endRead();
    }
    return result;
}

void WQtConfigWidget::createLineAssociation()
{
    m_lineAssociationList.clear();
    // this property gets all lines associated that can't be associated with a property
    boost::shared_ptr< WProperties > noVar = boost::shared_ptr< WProperties >();

    // iterate over every line
    // if its a comment without an assignment or a section just add it to the "noVar"
    // otherwise add it the fitting Property from m_loadedProperties, make sure to traverse groups
    // also set the read value if its not commented

    std::string lastSection = std::string( "" );

    size_t i;
    for ( i = 0; i < m_configLines.size(); ++i )
    {
        if ( !WCfgOperations::isAssignment( m_configLines[i] ) &&
             !WCfgOperations::isCommentedAssignment( m_configLines[i] ) )
        {
            if ( WCfgOperations::isSection( m_configLines[i] ) )
            {
                lastSection = WCfgOperations::getSectionName( m_configLines[i] );
            }

            addLineToProperty( noVar, i );
        }
        else
        {
            std::string curLine = m_configLines[i];
            std::string propertyVal;
            bool commentedAssignment = WCfgOperations::isCommentedAssignment( curLine );
            if ( commentedAssignment )
            {
                curLine = WCfgOperations::uncommentLine( curLine );
            }
            std::string propertyNameInConfig;
            WCfgOperations::getAssignementComponents( curLine, &propertyNameInConfig, &propertyVal );
            std::string propertyName = lastSection + '.' + propertyNameInConfig;

            boost::shared_ptr< WPropertyBase > foundProperty = findPropertyRecursive( m_loadedProperties, propertyName );

            if ( !foundProperty )
            {
                addLineToProperty( noVar, i );
            }
            else
            {
                addLineToProperty( foundProperty->toPropGroup() , i );
                if ( !commentedAssignment )
                {
                    // set the variable
                    switch ( foundProperty->getType() )
                    {
                    case PV_BOOL:
                        {
                            if ( WCfgOperations::isBool( propertyVal ) )
                            {
                                foundProperty->toPropBool()->set( WCfgOperations::getAsBool( propertyVal ), true );
                            }
                            break;
                        }
                    case PV_INT:
                        {
                            if ( WCfgOperations::isInt( propertyVal ) )
                            {
                                foundProperty->toPropInt()->set( WCfgOperations::getAsInt( propertyVal ), true );
                            }
                            break;
                        }
                    case PV_DOUBLE:
                        {
                            if ( WCfgOperations::isDouble( propertyVal ) )
                            {
                                foundProperty->toPropDouble()->set( WCfgOperations::getAsDouble( propertyVal ), true );
                            }
                            break;
                        }
                    case PV_STRING:
                        {
                            if ( WCfgOperations::isString( propertyVal ) )
                            {
                                foundProperty->toPropString()->set( WCfgOperations::getAsString( propertyVal ), true );
                            }
                            break;
                        }
                    default:
                        {
                            break;
                        }
                    }
                }
            }
        }
    }
}

bool WQtConfigWidget::isPropertyEqual( boost::shared_ptr< WProperties > prop1, boost::shared_ptr< WProperties > prop2, std::string propName )
{
    boost::shared_ptr< WPropertyBase > found1 = findPropertyRecursive( prop1, propName );
    boost::shared_ptr< WPropertyBase > found2 = findPropertyRecursive( prop2, propName );

    bool result = false;

    if ( !( found1.get() && found2.get() ) )
    {
        return result;
    }

    switch ( found1->getType() )
    {
    case PV_BOOL:
        {
            result = found1->toPropBool()->get() == found2->toPropBool()->get();
            break;
        }
    case PV_DOUBLE:
        {
            result = found1->toPropDouble()->get() == found2->toPropDouble()->get();
            break;
        }
    case PV_INT:
        {
            result = found1->toPropInt()->get() == found2->toPropInt()->get();
            break;
        }
    case PV_STRING:
        {
            result = found1->toPropString()->get() == found2->toPropString()->get();
            break;
        }
    default:
        {
            result = false;
            break;
        }
    }

    return result;
}

void WQtConfigWidget::saveToConfig()
{
    // empty property pointer
    boost::shared_ptr< WProperties > noVar = boost::shared_ptr< WProperties >();

    // keeps track of the property names which got added to the out file
    std::vector< std::string > propertyWriten;

    // the lines of the config file to write
    std::vector< std::string > configOut;

    std::string current_section = std::string( "" );

    // keep track of the already written sections
    std::vector< std::string > sectionsWriten;

    // a little helper to associate lines to properties
    std::vector< boost::shared_ptr< WProperties > > propertyInLine;

    propertyInLine.resize( m_configLines.size() );

    // first transform into another representation
    LinePropertySet::iterator itr;
    size_t i;
    for ( itr = m_lineAssociationList.begin(); itr != m_lineAssociationList.end(); ++itr )
    {
        for ( i = 0; i < itr->second.size(); ++i )
        {
            propertyInLine[ itr->second[i] ] = itr->first;
        }
    }

    // current line
    size_t cLine = 0;

    bool finished = false;

    // stack implementation to avoid recursion of property groups
    std::list< boost::shared_ptr< WProperties > > propertyStack;
    std::list< WProperties::PropertyIterator > iteratorStack;

    // to properly fulfill finished we need to make sure:
    //  - all lines from the config have been written
    //  - all registered sections have been written
    //  - all from default different properties have been written

    while ( !finished )
    {
        // not associated with a property, so either its a comment (without assignment), a section or an empty line
        if ( cLine < propertyInLine.size() )
        {
            if ( propertyInLine[cLine].get() == noVar.get() )
            {
                if ( WCfgOperations::isSection( m_configLines[cLine] ) )
                {
                    // check whether we switch from empty section or another section
                    if ( current_section != std::string( "" ) )
                    {
                        // find all so far unwritten none-default properties from the previous section
                        propertyStack.push_back( m_properties );

                        WProperties::PropertyIterator iter;
                        WProperties::PropertyAccessType accessObject = propertyStack.back()->getAccessObject();
                        // Temporarily disabled since locking causes several problems here :-/
                        // accessObject->beginRead();

                        iter = accessObject->get().begin();
                        iteratorStack.push_back( iter );

                        while ( !propertyStack.empty() )
                        {
                            // check if at the end of group, if so pop and continue
                            if ( iteratorStack.back() == propertyStack.back()->getAccessObject()->get().end() )
                            {
                                propertyStack.pop_back();
                                iteratorStack.pop_back();
                                continue;
                            }

                            // add group to stack, and move iterator
                            if ( ( *( iteratorStack.back() ) )->getType() == PV_GROUP )
                            {
                                propertyStack.push_back( ( *iteratorStack.back() )->toPropGroup() );
                                ++iteratorStack.back();
                                iteratorStack.push_back( propertyStack.back()->getAccessObject()->get().begin() );
                                continue;
                            }
                            else
                            {
                                // check the property and write it in case its valid
                                boost::shared_ptr< WProperties > prop = ( *( iteratorStack.back() ) )->toPropGroup();
                                std::string propName = prop->getName();
                                std::string sectionName = propName;
                                sectionName.erase( sectionName.find( '.' ), sectionName.length() - sectionName.find( '.' ) );
                                if ( sectionName == current_section && !isPropertyEqual( m_properties, m_defaultProperties, propName ) )
                                {
                                    size_t j;
                                    bool skip = false;
                                    for ( j = 0; j < m_skipPropertyWrite.size(); ++j )
                                    {
                                        if ( propName == m_skipPropertyWrite[j] )
                                        {
                                            skip = true;
                                            break;
                                        }
                                    }
                                    bool written = false;
                                    for ( j = 0; j < propertyWriten.size(); ++j )
                                    {
                                        if ( propName == propertyWriten[j] )
                                        {
                                            written = true;
                                            break;
                                        }
                                    }
                                    // only write the variable if its not already written, is not in the skip list
                                    // has the same section name as the section we just left and its value does not equal the default value
                                    if ( !skip && !written )
                                    {
                                        propertyWriten.push_back( propName );
                                        propName = propName.erase( 0, propName.find( '.' ) + 1 );
                                        configOut.push_back( propName + " = " + WCfgOperations::getPropValAsString( prop ) );
                                    }
                                }
                            } // END property is not a group

                            ++iteratorStack.back();
                        } // LOOP over properties

                        sectionsWriten.push_back( current_section );

                        // Temporarily disabled since locking causes several problems here :-/
                        // accessObject->endRead();
                    } // IF switch from none-empty section

                    std::string sn = WCfgOperations::getSectionName( m_configLines[cLine] );
                    if ( sn != std::string( "" ) )
                    {
                        current_section = sn;
                    }
                } // IF isSectionSwitch
                // if it was an empty section don't write the line
                if ( m_configLines[cLine] != std::string( "[]" ) )
                {
                    configOut.push_back( m_configLines[cLine] );
                }
                ++cLine;
                //continue;
            } // IF line not associated to property
            else
            {
                // -> line associated to property
                // though the line is associated we can assume the following:
                // - the line is in the correct section
                // - the property is not in the skip list
                boost::shared_ptr< WProperties > curProp = propertyInLine[cLine];
                std::string curPropName = curProp->getName();
                // get the same properties as the one we are at from default and currently set
                boost::shared_ptr< WProperties > propDefault = findPropertyRecursive( m_defaultProperties, curPropName )->toPropGroup();
                boost::shared_ptr< WProperties > propSet = findPropertyRecursive( m_properties, curPropName )->toPropGroup();
                std::string propDefaultValAsString = WCfgOperations::getPropValAsString( propDefault );
                std::string propSetValAsString = WCfgOperations::getPropValAsString( propSet );
                bool isLastLineOfProperty = false;
                // that cryptic line does the following:
                // m_lineAssociationList[ curProp ] = is the linelist which corresponds to the property we are at
                // m_lineAssociationList[ curProp ].size() - 1 = is the last index of the linelist
                // so if the last index, which must be at least 1 (because the line has to be associated at least once here),
                // is the same as the current line number, we are at the last line of the property

                // TODO(ledig): use this information to count the comments for a property
                // and therefore reduce them to a special count so the cfg file won't explode of comments
                if ( m_lineAssociationList[ curProp ][ m_lineAssociationList[ curProp ].size() - 1 ] == cLine )
                {
                    isLastLineOfProperty = true;
                }

                // to prevent multiple comments we just keep track of the current property
                bool isWritten = false;
                size_t j;
                for ( j = 0; j < propertyWriten.size(); ++j )
                {
                    if ( propertyWriten[j] == curPropName )
                    {
                        isWritten = true;
                        break;
                    }
                }

                // is comment:
                if ( WCfgOperations::isComment( m_configLines[cLine] ) )
                {
                    //  - comment val equals default -> keep as comment, but mark as written if the current value also is equal to default
                    //  - comment val equals current val, write it uncommented and mark as written
                    //  - otherwise just keep the commented line
                    std::string lineUncommented = WCfgOperations::uncommentLine( m_configLines[cLine] );
                    std::string propVal;
                    std::string dummy;
                    WCfgOperations::getAssignementComponents( lineUncommented, &dummy, &propVal );

                    if ( propVal == propDefaultValAsString )
                    {
                        configOut.push_back( m_configLines[cLine] );

                        if ( propDefaultValAsString == propSetValAsString && !isWritten )
                        {
                            propertyWriten.push_back( curPropName );
                            isWritten = true;
                        }
                    }
                    else
                    {
                        if ( propVal == propSetValAsString )
                        {
                            configOut.push_back( lineUncommented );
                            if ( !isWritten )
                            {
                                propertyWriten.push_back( curPropName );
                                isWritten = true;
                            }
                        }
                        else
                        {
                            configOut.push_back( m_configLines[cLine] );
                        }
                    }
                } // IF is comment
                else
                {
                    // is assignment:
                    //  - equal to default -> write as comment if not written
                    //  - equal to property value -> write and mark as written
                    //  - otherwise -> write as comment
                    // - has been last line associated to the property but not yet written
                    //  -> write if unequal to default and in both cases mark as written
                    std::string line = m_configLines[cLine];
                    std::string propVal;
                    std::string dummy;
                    WCfgOperations::getAssignementComponents( line, &dummy, &propVal );

                    if ( propVal == propDefaultValAsString )
                    {
                        if ( !isWritten )
                        {
                            configOut.push_back( "# " + line );
                            if ( propDefaultValAsString == propSetValAsString && !isWritten )
                            {
                                propertyWriten.push_back( curPropName );
                                isWritten = true;
                            }
                        }
                    }
                    else
                    {
                        if ( propVal == propSetValAsString )
                        {
                            configOut.push_back( line );
                            if ( !isWritten )
                            {
                                propertyWriten.push_back( curPropName );
                                isWritten = true;
                            }
                        }
                        else
                        {
                            configOut.push_back( "# " + line );
                        }
                    }
                    // if we didn't write the property so far, we will write it here, because we are at the only assignment line
                    if ( !isWritten )
                    {
                        std::string writePropName = curPropName;
                        writePropName = writePropName.erase( 0, writePropName.find( '.' ) + 1 );
                        configOut.push_back( writePropName + " = " + propSetValAsString );
                        propertyWriten.push_back( curPropName );
                    }
                } // end of assignment
                ++cLine;
            }
        } // IF not all lines written

        if ( cLine == propertyInLine.size() )
        {
            // at the end of the previous config file make sure
            // - we wrote all registered sections
            // - and if previous that we wrote all relevant properties, if so we are finished

            // could be accomplished with a little hack:
            // add all those sections not yet written and an empty section to the end
            // so that the section switch check will take all the work to write the leftover properties
            // -> here we just need to check that all sections are written at the end of the old config and we are finished

            size_t j, k;
            bool allSectionsWritten = true;
            for ( j = 0; j < m_registeredSections.size(); ++j )
            {
                // check if all registered sections are written
                bool sectionWritten = false;
                for ( k = 0; k < sectionsWriten.size(); ++k )
                {
                    if ( m_registeredSections[j] == sectionsWriten[k] )
                    {
                        sectionWritten = true;
                        break;
                    }
                }
                // is this sections not written?
                if ( !sectionWritten )
                {
                    allSectionsWritten = false;
                    // add the missing section and an empty section to the config file which will invoke flush of all unwritten
                    // properties for that section

                    // if current section is not yet registered just close it, otherwise force the
                    // section that didn't get written to flush
                    bool currentSectionWriten = false;
                    for ( k = 0; k < sectionsWriten.size(); ++k )
                    {
                        if ( sectionsWriten[k] == current_section )
                        {
                            currentSectionWriten = true;
                            break;
                        }
                    }

                    if ( currentSectionWriten || current_section == std::string( "" ) )
                    {
                        m_configLines.push_back( "[" + m_registeredSections[j] + "]" );
                        propertyInLine.push_back( noVar );
                    }
                    m_configLines.push_back( "[]" );
                    propertyInLine.push_back( noVar );
                    break;
                }
            }
            if ( allSectionsWritten )
            {
                finished = true;
            }
        }
    } // END write not finished loop

    WCfgOperations::writeCfg( WPathHelper::getConfigFile().file_string(), configOut );
}

void WQtConfigWidget::loadConfigFile()
{
    namespace fs = boost::filesystem;
    if ( fs::exists( WPathHelper::getConfigFile() ) )
    {
        m_configLines = WCfgOperations::readCfg( WPathHelper::getConfigFile().file_string() );
    }
    // copy all default properties into the loaded properties
    m_loadedProperties = copyProperties( m_defaultProperties );
    // parse the config file
    createLineAssociation();
    // update the loaded properties
    updateGui( m_loadedProperties );
    // copy all loaded properties into the current properties where we create the gui from
    m_properties = copyProperties( m_loadedProperties );

    m_configState.setResetable( true, true );
    m_configState.add( m_propCondition );
    m_configState.add( m_shutdownFlag.getCondition() );
}

void WQtConfigWidget::updateGui( boost::shared_ptr< WProperties > properties )
{
    updatePropertyGroups( properties, "ge.bgColor", true );
    updatePropertyGroups( properties, "modules.whiteListGroup", true );
    updatePropertyGroups( properties, "modules.defaultList", true );
}

WQtDSBWidget *WQtConfigWidget::createTabForSection( boost::shared_ptr< WProperties > from, std::string sectionName )
{
    WProperties::PropertyAccessType accesObject = from->getAccessObject();
    // Temporarily disabled since locking causes several problems here :-/
    // accesObject->beginRead();

    std::string name = std::string( "" );
    if ( from->getType() == PV_GROUP )
    {
        name = from->getName();
    }

    WQtDSBWidget* tab = new WQtDSBWidget( name );

    WProperties::PropertyIterator iter;
    for ( iter = accesObject->get().begin(); iter != accesObject->get().end(); ++iter )
    {
        std::string propName = ( *iter )->getName();
        if ( propName.find( '.' ) == std::string::npos || ( *iter )->isHidden() )
        {
            continue;
        }
        std::string propSectionName = propName.erase( propName.find( '.' ), propName.length() - propName.find( '.' ) );
        if ( sectionName == propSectionName )
        {
            switch ( ( *iter )->getType() )
            {
            case PV_BOOL:
                tab->addProp( ( *iter )->toPropBool() );
                break;
            case PV_INT:
                tab->addProp( ( *iter )->toPropInt() );
                break;
            case PV_DOUBLE:
                tab->addProp( ( *iter )->toPropDouble() );
                break;
            case PV_STRING:
                tab->addProp( ( *iter )->toPropString() );
                break;
            case PV_PATH:
                tab->addProp( ( *iter )->toPropFilename() );
                break;
            case PV_SELECTION:
                WLogger::getLogger()->addLogMessage( "This property type \"PV_SELECTION\" is not yet supported by the GUI.", "DatasetBrowser",
                    LL_WARNING );
                break;
            case PV_COLOR:
                tab->addProp( ( *iter )->toPropColor() );
                break;
            case PV_POSITION:
                WLogger::getLogger()->addLogMessage( "This property type \"PV_POSITION\" is not yet supported by the GUI.", "DatasetBrowser",
                    LL_WARNING );
                break;
            case PV_TRIGGER:
                tab->addProp( ( *iter )->toPropTrigger() );
                break;
            case PV_GROUP:
                tab->addGroup( createTabForSection( ( *iter )->toPropGroup(), sectionName ), true );
                break;
            default:
                WLogger::getLogger()->addLogMessage( "This property type is not yet supported.", "DatasetBrowser", LL_WARNING );
                break;
            }
        }
    }

    // Temporarily disabled since locking causes several problems here :-/
    // accesObject->endRead();

    tab->addSpacer();
    return tab;
}

void WQtConfigWidget::createGuiFromProperties( boost::shared_ptr< WProperties > from )
{
    // iterate over all sections
    for ( size_t i = 0; i < m_registeredSections.size(); ++i )
    {
        // iterate over all properties and use those which are in this section

        m_tabWidget->addTab( createTabForSection( from, m_registeredSections[i] ), QString::fromStdString( m_registeredSections[i] ) );
    }
}

void WQtConfigWidget::initAndShow()
{
    loadConfigFile();
    // create the widgets and their connections in the corresponding tabs
    m_tabWidget->clear();

    createGuiFromProperties( m_properties );

    this->show();

    run();
}

void WQtConfigWidget::threadMain()
{
    size_t i;
    while ( !m_shutdownFlag() )
    {
        m_configState.wait();

        if ( m_shutdownFlag() )
        {
            break;
        }

        // react on the changes of gui elements which influence other gui values
        if ( findPropertyRecursive( m_properties, "ge.bgColor.r" )->toPropDouble()->changed() ||
             findPropertyRecursive( m_properties, "ge.bgColor.g" )->toPropDouble()->changed() ||
             findPropertyRecursive( m_properties, "ge.bgColor.b" )->toPropDouble()->changed() )
        {
            updatePropertyGroups( m_properties, "ge.bgColor", true );
        }
        if ( findPropertyRecursive( m_properties, "ge.sColorWidget" )->toPropColor()->changed() )
        {
            updatePropertyGroups( m_properties, "ge.bgColor", false );
        }
        for ( i = 0; i < m_moduleNames.size(); ++i )
        {
            if ( findPropertyRecursive( m_properties, "modules.whitelist." + m_moduleNames[i] )->toPropBool()->changed() )
            {
                updatePropertyGroups( m_properties, "modules.whiteListGroup", false );
            }
            if ( findPropertyRecursive( m_properties, "modules.defaultList." + m_moduleNames[i] )->toPropBool()->changed() )
            {
                updatePropertyGroups( m_properties, "modules.defaultList", false );
            }
        }
    }
}

bool WQtConfigWidget::setWalnutFromProperties()
{
    bool result = false;

    boost::shared_ptr< WPropertyBase > propertyToSet, propertyLoaded;

    std::string propertyName = "ge.sColorWidget";
    propertyToSet = findPropertyRecursive( m_properties, propertyName );
    propertyLoaded = findPropertyRecursive( m_loadedProperties, propertyName );

    if ( propertyToSet->toPropColor()->get() != propertyLoaded->toPropColor()->get() )
    {
        // set the color

        result = true;
    }
    WColor color = propertyToSet->toPropColor()->get();

    boost::shared_ptr< WGEViewer > view;
    view = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "main" );
    if ( view )
    {
        view->setBgColor( color );
    }
    view = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Axial View" );
    if ( view )
    {
        view->setBgColor( color );
    }
    view = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Coronal View" );
    if ( view )
    {
        view->setBgColor( color );
    }
    view = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Sagittal View" );
    if ( view )
    {
        view->setBgColor( color );
    }

    return result;
    // check if
}


void WQtConfigWidget::preview()
{
    m_previewed = setWalnutFromProperties();
}

void WQtConfigWidget::resetToDefaults()
{
    // load the default value of every property and display it
    copyPropertiesContents( m_defaultProperties, m_properties );
    updateGui( m_properties );

    //
    if ( m_previewed )
    {
        // reset the preview
        setWalnutFromProperties();

        // this is also some sort of preview because when we cancel we want the loaded config still active
        //m_previewed = false;
    }
}

void WQtConfigWidget::resetToBackup()
{
    // get the backup of the loaded file and show its values
    copyPropertiesContents( m_loadedProperties, m_properties );
    updateGui( m_properties );

    if ( m_previewed )
    {
        // reset the preview
        setWalnutFromProperties();
        m_previewed = false;
    }
}

void WQtConfigWidget::save()
{
    // save the config changes
    // and use every trigger that can be executed
    saveToConfig();
    setWalnutFromProperties();
    this->hide();
    requestStop();
}

void WQtConfigWidget::cancel()
{
    // reset to the loaded state
    if ( m_previewed )
    {
        copyPropertiesContents( m_loadedProperties, m_properties );

        // reset the preview
        setWalnutFromProperties();
        m_previewed = false;
    }

    // and hide the window
    this->hide();

    requestStop();
}

void WQtConfigWidget::closeEvent( QCloseEvent *event )
{
    emit closeWidget();
    event->accept();
}
