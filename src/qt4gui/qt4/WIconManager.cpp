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
#include "core/common/WPathHelper.h"
#include "core/common/WLogger.h"
#include "core/common/exceptions/WFileNotFound.h"
#include "core/kernel/WModuleFactory.h"

#include "WIconManager.h"

void WIconManager::addMapping( const std::string& newName, const std::string& mapToThis )
{
    if( m_iconMappingList.count( newName ) == 0 )
    {
        m_iconMappingList.insert( std::make_pair( newName, mapToThis ) );
    }
    else
    {
        m_iconMappingList[ newName ] = mapToThis;
    }
}

QIcon WIconManager::getIcon( const std::string name )
{
    // ensure we have a fallback icon
    boost::filesystem::path fallback = WPathHelper::getPathHelper()->getSharePath() / ".." / "pixmaps" / "default.png";
    WAssert( boost::filesystem::exists( fallback ), "Found no icon named: " + name + " and no fallback icon. Installation broken?" );
    return getIcon( name, QIcon( QPixmap( QString::fromStdString( fallback.string() ) ) ) );
}

QIcon WIconManager::getIcon( const std::string name, const QIcon& defaultIcon )
{
    std::string iconFile = name;

    // is there a mapping for this icon name?
    if( m_iconMappingList.count( name ) > 0 )
    {
        iconFile = m_iconMappingList[ name ];
    }

    // search file
    boost::filesystem::path p = WPathHelper::getPathHelper()->getSharePath() / ".." / "pixmaps" / std::string( iconFile + ".png" );
    if( boost::filesystem::exists( p ) )
    {
        try
        {
            return QIcon( QPixmap( QString::fromStdString( p.string() ) ) );
        }
        catch( ... )
        {
            return defaultIcon;
        }
    }
    else if( WModuleFactory::getModuleFactory()->isPrototypeAvailable( name ) )
    {
        // get module icon from meta info if available
        WModuleMetaInformation::ConstSPtr meta = WModuleFactory::getModuleFactory()->getPrototypeByName( name )->getMetaInformation();
        const char** xpm = WModuleFactory::getModuleFactory()->getPrototypeByName( name )->getXPMIcon();

        // prefer meta info icon
        if( meta->isIconAvailable() && boost::filesystem::exists( meta->getIcon() ) )
        {
            try
            {
                return QIcon( QPixmap( QString::fromStdString( meta->getIcon().string() ) ) );
            }
            catch( ... )
            {
                if( xpm )
                {
                    return QIcon( QPixmap( xpm ) );
                }
                else
                {
                    return defaultIcon;
                }
            }
        }
        else
        {
            if( xpm )
            {
                return QIcon( QPixmap( xpm ) );
            }
            else
            {
                return defaultIcon;
            }
        }
    }
    else
    {
        wlog::debug( "WIconManager" ) << "Icon \"" << name << "\" not found. Falling back to default.";
        return defaultIcon;
    }
}
