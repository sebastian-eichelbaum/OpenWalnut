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

#include <string>
#include <vector>
#include <iostream>

#include <QVBoxLayout>

#include <QWebView>
#include <QWebFrame>
#include <QWebPage>
#include <QToolBar>
#include <QToolButton>
#include <QHBoxLayout>
#include <QWidget>
#include <QPixmap>
#include <QIcon>
#include <QAction>
#include <QtCore/QUrl>
#include <QTextEdit>

#include "core/common/WIOTools.h"
#include "core/common/WLogger.h"
#include "core/common/WPathHelper.h"

#include "../WQtGui.h"
#include "../WMainWindow.h"

#include "WQtModuleMetaInfo.h"

std::string htmlify( WModuleMetaInformation::ConstSPtr meta )
{
    typedef std::vector< WModuleMetaInformation::Author > AuthorType;
    AuthorType authors = meta->getAuthors();
    typedef std::vector< std::string > TagType;
    TagType tags = meta->getTags();
    boost::filesystem::path help = meta->getHelp();
    std::string website = meta->getWebsite();
    std::string description = meta->getDescription();
    typedef std::vector< WModuleMetaInformation::Online > OnlineType;
    OnlineType online = meta->getOnlineResources();
    typedef std::vector< WModuleMetaInformation::Screenshot > ScreenshotType;
    ScreenshotType screenshots = meta->getScreenshots();

    // where to put in the html code
    std::ostringstream ss;

    // the website URL is the module dir -> so we need to explicitly specify the src for icons
    std::string iconPath = WPathHelper::getSharePath().string() + "/qt5gui/";

    ss << "<div style='font-family:sans-serif;'>"
    // The title
       << "  <div style='padding:10px; border:0px solid #000; border-bottom-width: 10px;color:#fff;background:#0c67a8;'>"
       << "    <h1>"
       <<        meta->getName()
       << "    </h1>"
       << "  </div>";

    // Description
    ss << "  <div style='padding:10px; background:#ddd;'>"
       << "    <h3>DESCRIPTION</h3>"
       << "    <p align='justify'>"
       <<        description
       << "    </p>"
       << "  </div>";

    // this div contains tags, help, website and authors
    ss << "  <div style='padding:10px;background:#f3f3f3;'>";
    // Taglist
    if( !tags.empty() )
    {
        ss << "    <h3>TAGS</h3>";
        for( TagType::const_iterator iter = tags.begin(); iter != tags.end(); ++iter )
        {
            ss << *iter;
            if( iter+1 != tags.end() )
            {
                ss << ", ";
            }
        }
    }

    // Website
    if( !website.empty() )
    {
        ss << "    <h3>WEBSITE</h3>"
           << "    <a href='" <<  website << "'>" << website << "</a>";
    }

    // Help
    if( !help.empty() && boost::filesystem::exists( help ) )
    {
        ss << "    <h3>HELP</h3>"
           << "    <a href='" << help.string() << "'>Module Help</a>";
    }
    // The authors
    if( !authors.empty() )
    {
        ss << "    <h3>AUTHORS</h3>"
           << "    <table width='100%' style='padding:10px'>";
        for( AuthorType::const_iterator iter = authors.begin(); iter != authors.end(); ++iter )
        {
            ss << "<tr> <td style='padding:10px; background:#ddd;'>";

            if( !( *iter ).m_email.empty() )
            {
                ss << "<a href='mailto:" << ( *iter ).m_email << "'>" << "<img width='24px' src='" << iconPath << "/email.png' />" << "</a>";
            }
            if( !( *iter ).m_url.empty() )
            {
                ss << "<a href='" << ( *iter ).m_url << "'>" <<  "<img width='24px' src='" << iconPath << "website.png' />" << "</a>";
            }

            ss << "&nbsp;";
            ss << ( *iter ).m_name;
            ss << "</td><td style='padding:10px; background:#e7e7e7;'><p align='justify'>" << ( *iter ).m_what << "</p></td>";
            ss << "</tr>";
        }
        ss << "    </table>";
    }

    // thats it. No more authors, help and similar
    ss << "  </div>";

    // The online resources
    if( !online.empty() )
    {
        ss << "  <div style='padding:10px;background:#fff;'>"
           << "    <h3>ONLINE RESOURCES</h3>"
           << "    <table width='100%' style='padding:10px'>";

        for( OnlineType::const_iterator iter = online.begin(); iter != online.end(); ++iter )
        {
            ss << "      <tr>"
               << "        <td style='padding:10px; background:#ddd;'>";

            ss << "          <a href='" << ( *iter ).m_url << "'> " <<  ( *iter ).m_name << "</a>";

            ss << "        </td>"
               << "        <td style='padding:10px; background:#e7e7e7;'>";

            ss << "          <p align='justify'>" << ( *iter ).m_description << "</p>";

            ss << "        </td>"
               << "      </tr>";
        }

        ss << "    </table>"
           << "  </div>";
    }

    // screenshots
    if( !screenshots.empty() )
    {
        ss << "  <div  style='padding:10px;background:#f5f5f5;'>"
           << "    <h3>SCREENSHOTS</h3>"
           << "    <table width='100%' style='padding:10px'>";

        for( ScreenshotType::const_iterator iter = screenshots.begin(); iter != screenshots.end(); ++iter )
        {
            ss << "      <tr>"
               << "        <td width='1%' style='padding:10px; background:#ddd;'>";

            ss << "          <img src='" << ( *iter ).m_filename.string() << "' width=200px>";

            ss << "        </td>"
               << "        <td style='padding:10px; background:#e7e7e7;'>";

            ss << "          <p align='justify'>" << ( *iter ).m_description << "</p>";

            ss << "        </td>"
               << "      </tr>";
        }

        ss << "    </table>"
           << "  </div>"
           << "</div>";
    }
    return ss.str();
}

WQtModuleMetaInfo::WQtModuleMetaInfo( WModule::SPtr module, QWidget* parent ):
    QWidget( parent ),
    m_module( module )
{
    // some layout to place the webview
    QVBoxLayout *layout = new QVBoxLayout();
    setLayout( layout );
    layout->setSpacing( 0 );
    layout->setContentsMargins( 0, 0, 0, 0 );

    // create the QT webview
    QWebView* view = new QWebView( this );

    // create a webpage and add it to the view
    QWebPage* page = new QWebPage( this );
    page->setLinkDelegationPolicy( QWebPage::DelegateExternalLinks );
    m_frame = page->mainFrame();
    view->setPage( page );

    // add a toolbar for basic navigation
    QWidget* toolbar = new QWidget( this );
    QHBoxLayout* tbLayout = new QHBoxLayout( toolbar );
    toolbar->setLayout( tbLayout );

    // we need a special home-action to set the html content again
    QAction* homeAction = new QAction( WQtGui::getMainWindow()->getIconManager()->getIcon( module->getName() ),
                                       QString( "Back to " ) + QString::fromStdString( module->getName() ),
                                       toolbar );
    homeAction->setIconText( QString( "Back to " ) + QString::fromStdString( module->getName() ) );

    // add the buttons
    QToolButton* homeBtn = new QToolButton( toolbar );
    homeBtn->setDefaultAction( homeAction );
    homeBtn->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );

    QToolButton* backBtn = new QToolButton( toolbar );
    backBtn->setDefaultAction( page->action( QWebPage::Back ) );
    QToolButton* fwdBtn = new QToolButton( toolbar );
    fwdBtn->setDefaultAction( page->action( QWebPage::Forward ) );

    tbLayout->addWidget( backBtn );
    tbLayout->addWidget( fwdBtn );
    tbLayout->addStretch( 100000 );
    tbLayout->addWidget( homeBtn );

    // build widget
    layout->addWidget( toolbar );
    layout->addWidget( view );

    layout->setStretchFactor( toolbar, 0 );
    layout->setStretchFactor( view, 1 );

    resetContent();

    // the home action triggers reseContent
    connect( homeAction, SIGNAL( triggered() ), this, SLOT( resetContent() ) );
}

WQtModuleMetaInfo::~WQtModuleMetaInfo()
{
    // cleanup
}

void WQtModuleMetaInfo::resetContent()
{
    // we use the module resource path as search URL
    std::string moduleLocation( m_module->getLocalPath().string() );
    QString locationURL( QString( "file://" ) + QString::fromStdString( moduleLocation ) + "/" );

    // set content
    std::string processedContent = htmlify( m_module->getMetaInformation() );
    m_frame->setHtml( processedContent.c_str(), QUrl( locationURL ) );
}
