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

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <algorithm>

#include <boost/bind.hpp>

#include <QtCore/QList>
#include <QtGui/QScrollArea>
#include <QtGui/QVBoxLayout>
#include <QtGui/QListWidgetItem>

#include "../../../common/WLimits.h"
#include "../../../dataHandler/WDataSet.h"
#include "../../../dataHandler/WSubject.h"
#include "../../../dataHandler/WDataHandler.h"
#include "../../../dataHandler/exceptions/WDHNoSuchSubject.h"
#include "../../../kernel/WModuleFactory.h"

#include "WQtTextureSorter.h"


WQtTextureSorter::WQtTextureSorter( QWidget* parent )
    : QDockWidget( "Texture Sorter", parent )
{
    setObjectName( "Texture Sorter Dock" );

    m_textureListWidget = new QListWidget( this );
    m_textureListWidget->setToolTip( "List of available textures. Only the upper <b>"
                                     + QString().setNum( wlimits::MAX_NUMBER_OF_TEXTURES )
                                     + "</b> textures will be applied." );
    this->setAllowedAreas( Qt::AllDockWidgetAreas );
    this->setFeatures( QDockWidget::DockWidgetClosable |QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );

    QWidget* panel = new QWidget( this );

    m_layout = new QVBoxLayout();

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_downButton = new QPushButton();
    m_downButton->setText( QString( "down" ) );
    m_upButton = new QPushButton();
    m_upButton->setText( QString( "up" ) );

    buttonLayout->addWidget( m_downButton );
    buttonLayout->addWidget( m_upButton );

    connect( m_upButton, SIGNAL( pressed() ), this, SLOT( moveItemUp() ) );
    connect( m_downButton, SIGNAL( pressed() ), this, SLOT( moveItemDown() ) );

    m_layout->addWidget( m_textureListWidget );
    m_layout->addLayout( buttonLayout );

    connect( m_textureListWidget, SIGNAL( itemClicked( QListWidgetItem* ) ), this, SLOT( handleTextureClicked() ) );

    panel->setLayout( m_layout );
    setWidget( panel );
}

WQtTextureSorter::~WQtTextureSorter()
{
}

void WQtTextureSorter::handleTextureClicked()
{
    unsigned int index =  m_textureListWidget->currentIndex().row();

    DatasetSharedContainerType::ReadTicket r = m_textures.getReadTicket();

    QListWidgetItem* ci = m_textureListWidget->item( index );
    WAssert( ci, "Problem in source code." );
    boost::shared_ptr< WDataSet > dataSet = r->get()[index];

    emit textureSelectionChanged( dataSet );
}

void WQtTextureSorter::update()
{
    boost::shared_ptr< WSubject > subject;
    try
    {
        subject =  WDataHandler::getDefaultSubject();
    }
    catch( WDHNoSuchSubject )
    {
        return;
    }

    if( !subject )
    {
        return;
    }


    DatasetSharedContainerType::WriteTicket writeTex =  m_textures.getWriteTicket();
    DatasetSharedContainerType::WriteTicket writeDataSet = subject->getDatasetsForWriting();

    if( writeTex->get().empty() )
    {
        for( DatasetContainerType::iterator it = writeDataSet->get().begin(); it != writeDataSet->get().end(); ++it )
        {
            if( ( *it )->isTexture() )
            {
                writeTex->get().insert( writeTex->get().begin(), *it );
            }
        }
    }
    else
    {
        // insert the new datasets into the texture sorter.
        for( DatasetContainerType::iterator it = writeDataSet->get().begin(); it != writeDataSet->get().end(); ++it )
        {
            if( ( *it )->isTexture() )
            {
                if( std::find( writeTex->get().begin(), writeTex->get().end(), *it ) == writeTex->get().end() )
                {
                    writeTex->get().insert( writeTex->get().begin(), *it );
                }
            }
        }

        // remove deregistered datasets from the texture sorter.
        for( DatasetContainerType::iterator it = writeTex->get().begin(); it != writeTex->get().end(); ++it )
        {
            DatasetContainerType::iterator fIt = std::find( writeDataSet->get().begin(), writeDataSet->get().end(), *it );
            if( fIt == writeDataSet->get().end() )
            {
                writeTex->get().erase( it );
                break;
            }
        }
    }

    int index =  m_textureListWidget->currentIndex().row();
    m_textureListWidget->clear();

    for( DatasetContainerType::iterator it = writeTex->get().begin(); it != writeTex->get().end(); ++it )
    {
        std::vector< std::string > names =  string_utils::tokenize( ( *it )->getFileName().c_str(), "/" );
        std::string name;
        if( names.size() )
        {
            name = names.back();
        }
        else
        {
            name = "No name given.";
        }
        m_textureListWidget->addItem( name.c_str() );
    }

    m_textureListWidget->setCurrentRow( index );

    writeTex.reset();
    writeDataSet.reset();
    sort();
}

void WQtTextureSorter::moveItemDown()
{
    unsigned int index =  m_textureListWidget->currentIndex().row();

    DatasetSharedContainerType::WriteTicket writeTex =  m_textures.getWriteTicket();

    if( index < writeTex->get().size() - 1 )
    {
        QListWidgetItem* ci = m_textureListWidget->takeItem( index );

        if( ci )
        {
            m_textureListWidget->insertItem( index + 1, ci );
            m_textureListWidget->clearSelection();
            m_textureListWidget->setCurrentItem( ci );
            ci->setSelected( true );
            boost::shared_ptr< WDataSet > tmp = writeTex->get()[index+1];
            writeTex->get()[index+1] = writeTex->get()[index];
            writeTex->get()[index] = tmp;
        }
    }
    else
    {
    }

    writeTex.reset();
    sort();
}

void WQtTextureSorter::moveItemUp()
{
    unsigned int index =  m_textureListWidget->currentIndex().row();

    if( index > 0 )
    {
        QListWidgetItem* ci = m_textureListWidget->takeItem( index );

        if( ci )
        {
            DatasetSharedContainerType::WriteTicket writeTex = m_textures.getWriteTicket();

            m_textureListWidget->insertItem( index - 1, ci );
            m_textureListWidget->clearSelection();
            m_textureListWidget->setCurrentItem( ci );
            ci->setSelected( true );
            boost::shared_ptr< WDataSet > tmp = writeTex->get()[index-1];
            writeTex->get()[index-1] = writeTex->get()[index];
            writeTex->get()[index] = tmp;
        }
    }
    sort();
}

void WQtTextureSorter::selectTexture( boost::shared_ptr< WDataSet > dataSet )
{
    DatasetSharedContainerType::ReadTicket readTex =  m_textures.getReadTicket();

    size_t i = 0;
    for( ; i < readTex->get().size(); ++i )
    {
        if( readTex->get()[i] == dataSet )
        {
            break;
        }
    }

    if( i < readTex->get().size() )
    {
        // select appropriate texture if the data set has a corresponding texture
        QListWidgetItem* ci = m_textureListWidget->item( i );
        m_textureListWidget->setCurrentItem( ci );
    }
    else
    {
        // unselect all if the dataset has no corresponding texture
        m_textureListWidget->clearSelection();
    }
}

bool WQtTextureSorter::isLess( boost::shared_ptr< WDataSet > lhs, boost::shared_ptr< WDataSet > rhs )
{
    DatasetSharedContainerType::ReadTicket readTex =  m_textures.getReadTicket();

    DatasetContainerType::const_iterator itLHS = std::find( readTex->get().begin(), readTex->get().end(), lhs );
    DatasetContainerType::const_iterator itRHS = std::find( readTex->get().begin(), readTex->get().end(), rhs );

    bool result = itLHS < itRHS;

    return result;
}

void WQtTextureSorter::sort()
{
    DatasetSharedContainerType::WriteTicket writeDataSet = WDataHandler::getDefaultSubject()->getDatasetsForWriting();

    std::sort( writeDataSet->get().begin(), writeDataSet->get().end(), boost::bind( boost::mem_fn( &WQtTextureSorter::isLess ), this, _1, _2 ) );

    WDataHandler::getDefaultSubject()->getChangeCondition()->notify();
}
