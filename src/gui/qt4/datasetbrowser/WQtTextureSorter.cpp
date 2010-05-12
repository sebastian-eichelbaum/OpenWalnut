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

#include "../../../dataHandler/WDataSet.h"
#include "../../../dataHandler/WSubject.h"
#include "../../../dataHandler/WDataHandler.h"
#include "../../../dataHandler/exceptions/WDHNoSuchSubject.h"
#include "../../../kernel/WModuleFactory.h"

#include "WQtTextureSorter.h"


WQtTextureSorter::WQtTextureSorter( QWidget* parent )
    : QWidget( parent )
{
    m_textureListWidget = new QListWidget( this );
    m_textureListWidget->setToolTip( "List of available textures. Only the upper <b>eight</b> textures will be applied." );

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

    setLayout( m_layout );
}

WQtTextureSorter::~WQtTextureSorter()
{
}

void WQtTextureSorter::handleTextureClicked()
{
    unsigned int index =  m_textureListWidget->currentIndex().row();

    DatasetAccess ta = m_textures.getAccessObject();
    ta->beginRead();

    QListWidgetItem* ci = m_textureListWidget->item( index );
    WAssert( ci, "Problem in source code." );
    boost::shared_ptr< WDataSet > dataSet = ta->get()[index];

    ta->endRead();

    emit textureSelectionChanged( dataSet );
}

// TODO(wiebel): have a second look on this begin/end read/write mess.
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


    DatasetAccess ta = m_textures.getAccessObject();
    ta->beginRead();

    if( ta->get().empty() )
    {
        ta->endRead(); // end for the begin needed for the if condition

        DatasetAccess da = subject->getAccessObject();
        da->beginRead();

        for( DatasetContainerType::iterator it = da->get().begin(); it != da->get().end(); ++it )
        {
            if( ( *it )->isTexture() )
            {
                da->endRead();
                ta->beginWrite();
                ta->get().insert( ta->get().begin(), *it );
                ta->endWrite();
                da->beginRead();
            }
        }
        da->endRead();
    }
    else
    {
        ta->endRead(); // end for the begin needed for the if condition

        // insert the new datasets into the texture sorter.
        DatasetAccess da = subject->getAccessObject();
        da->beginRead();
        for( DatasetContainerType::iterator it = da->get().begin(); it != da->get().end(); ++it )
        {
            if( ( *it )->isTexture() )
            {
                ta->beginRead();
                if( std::find( ta->get().begin(), ta->get().end(), *it ) == ta->get().end() )
                {
                    ta->endRead();
                    ta->beginWrite();
                    ta->get().insert( ta->get().begin(), *it );
                    ta->endWrite();
                }
                else
                {
                    ta->endRead();
                }
            }
        }
        da->endRead();

        // remove deregistered datasets from the texture sorter.
        ta->beginRead();
        for( DatasetContainerType::iterator it = ta->get().begin(); it != ta->get().end(); ++it )
        {
            ta->endRead();
            da->beginRead();
            DatasetContainerType::iterator fIt = std::find( da->get().begin(), da->get().end(), *it );
            if( fIt == da->get().end() )
            {
                ta->beginWrite();
                ta->get().erase( it );
                ta->endWrite();
                ta->beginRead();
                break;
            }
            da->endRead();
            ta->beginRead();
        }
        ta->endRead();
    }

    int index =  m_textureListWidget->currentIndex().row();
    m_textureListWidget->clear();

    ta->beginRead();
    for( DatasetContainerType::iterator it = ta->get().begin(); it != ta->get().end(); ++it )
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
    ta->endRead();

    m_textureListWidget->setCurrentRow( index );
    sort();
}

void WQtTextureSorter::moveItemDown()
{
    unsigned int index =  m_textureListWidget->currentIndex().row();

    DatasetAccess ta = m_textures.getAccessObject();
    ta->beginRead();

    if( index < ta->get().size() - 1 )
    {
        ta->endRead();
        QListWidgetItem* ci = m_textureListWidget->takeItem( index );

        if( ci )
        {
            m_textureListWidget->insertItem( index + 1, ci );
            m_textureListWidget->clearSelection();
            m_textureListWidget->setCurrentItem( ci );
            ci->setSelected( true );
            ta->beginWrite();
            boost::shared_ptr< WDataSet > tmp = ta->get()[index+1];
            ta->get()[index+1] = ta->get()[index];
            ta->get()[index] = tmp;
            ta->endWrite();
        }
    }
    else
    {
        ta->endRead();
    }
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
            DatasetAccess ta = m_textures.getAccessObject();
            ta->beginWrite();

            m_textureListWidget->insertItem( index - 1, ci );
            m_textureListWidget->clearSelection();
            m_textureListWidget->setCurrentItem( ci );
            ci->setSelected( true );
            boost::shared_ptr< WDataSet > tmp = ta->get()[index-1];
            ta->get()[index-1] = ta->get()[index];
            ta->get()[index] = tmp;

            ta->endWrite();
        }
    }
    sort();
}

void WQtTextureSorter::selectTexture( boost::shared_ptr< WDataSet > dataSet )
{
    DatasetAccess ta = m_textures.getAccessObject();
    ta->beginRead();
    size_t i = 0;
    for( ; i < ta->get().size(); ++i )
    {
        if( ta->get()[i] == dataSet )
        {
            break;
        }
    }
    ta->endRead();

    if( i < ta->get().size() )
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
    DatasetAccess ta = m_textures.getAccessObject();
    ta->beginRead();

    DatasetContainerType::iterator itLHS = std::find( ta->get().begin(), ta->get().end(), lhs );
    DatasetContainerType::iterator itRHS = std::find( ta->get().begin(), ta->get().end(), rhs );

    bool result = itLHS < itRHS;
    ta->endRead();

    return result;
}

void WQtTextureSorter::sort()
{
    DatasetAccess da = WDataHandler::getDefaultSubject()->getAccessObject();
    da->beginWrite();

    std::sort( da->get().begin(), da->get().end(), boost::bind( boost::mem_fn( &WQtTextureSorter::isLess ), this, _1, _2 ) );

    da->endWrite();
    WDataHandler::getDefaultSubject()->getChangeCondition()->notify();
}
