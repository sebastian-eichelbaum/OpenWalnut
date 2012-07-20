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

#include <cassert>
#include <string>

#include <QtGui/QPixmap>

#include "core/common/WAssert.h"
#include "core/common/exceptions/WFileNotFound.h"
#include "core/kernel/WModuleFactory.h"
#include "WIconManager.h"

void WIconManager::addIcon( std::string name, boost::filesystem::path filename )
{
    QPixmap img;
    bool failed = !img.load( QString::fromStdString( filename.string() ) );
    if( failed )
    {
        throw WFileNotFound( "Image file for icon in \"" + filename.string() + "\" could not be loaded." );
    }

    QIcon* icon = new QIcon( img );
    m_iconList[name] = icon;
}

void WIconManager::addIcon( std::string name, const char* const xpm[] )
{
    QIcon* icon = new QIcon( QPixmap( xpm ) );
    m_iconList[name] = icon;
}

void WIconManager::addIcon( std::string name, const QPixmap& pixmap )
{
    QIcon* icon = new QIcon( QPixmap( pixmap ) );
    m_iconList[name] = icon;
}

QIcon WIconManager::getIcon( const std::string name )
{
    QIcon icon;
    if( m_iconList.count( name ) != 0 )
    {
        icon = *m_iconList[name];
    }
    else if( WModuleFactory::getModuleFactory()->getPrototypeByName( name ) )
    {
        // get module icon from meta info if available
        WModuleMetaInformation::ConstSPtr meta = WModuleFactory::getModuleFactory()->getPrototypeByName( name )->getMetaInformation();
        if( meta->isIconAvailable() && boost::filesystem::exists( meta->getIcon() ) )
        {
            try
            {
                icon = QIcon( QPixmap( QString::fromStdString( meta->getIcon().string() ) ) );
            }
            catch( ... )
            {
                icon = QIcon( QPixmap( WModuleFactory::getModuleFactory()->getPrototypeByName( name )->getXPMIcon() ) );
            }
        }
        else
        {
            icon = QIcon( QPixmap( WModuleFactory::getModuleFactory()->getPrototypeByName( name )->getXPMIcon() ) );
        }
    }
    else
    {
        WAssert( 0, "Found no icon named: " + name );
    }

    return icon;
}

QIcon WIconManager::getIcon( const std::string name, const QIcon& defaultIcon )
{
    if( m_iconList.count( name ) != 0 )
    {
        return *m_iconList[name];
    }
    else if( WModuleFactory::getModuleFactory()->getPrototypeByName( name ) )
    {
        // get module icon from meta info if available
        WModuleMetaInformation::ConstSPtr meta = WModuleFactory::getModuleFactory()->getPrototypeByName( name )->getMetaInformation();
        if( meta->isIconAvailable() && boost::filesystem::exists( meta->getIcon() ) )
        {
            try
            {
                return QIcon( QPixmap( QString::fromStdString( meta->getIcon().string() ) ) );
            }
            catch( ... )
            {
                return QIcon( QPixmap( WModuleFactory::getModuleFactory()->getPrototypeByName( name )->getXPMIcon() ) );
            }
        }
        else
        {
            return QIcon( QPixmap( WModuleFactory::getModuleFactory()->getPrototypeByName( name )->getXPMIcon() ) );
        }
    }
    else
    {
        return QIcon( defaultIcon );
    }
}
