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
#include <string>
#include <vector>

#include "../graphicsEngine/WGraphicsEngine.h"

#include "WROIManager.h"
#include "WRMBranch.h"


WRMBranch::WRMBranch( boost::shared_ptr< WROIManager > roiManager ) :
    m_roiManager( roiManager )
{
    properties();
}

WRMBranch::~WRMBranch()
{
}

void WRMBranch::properties()
{
    m_properties = boost::shared_ptr< WProperties >( new WProperties( "Properties", "This branch's properties" ) );

    m_dirty = m_properties->addProperty( "Dirty", "", true, boost::bind( &WRMBranch::propertyChanged, this ) );
    m_dirty->setHidden( true );
    m_name = m_properties->addProperty( "Name", "The name of this branch.", std::string( "Branch" ) );
    m_isNot = m_properties->addProperty( "Not", "Negate the effect of this branch.", false, boost::bind( &WRMBranch::propertyChanged, this ) );
    m_bundleColor = m_properties->addProperty( "Bundle color", "Color the selected fibers using this color.", WColor( 1.0, 0.0, 0.0, 1.0 ),
                                               boost::bind( &WRMBranch::propertyChanged, this ) );

    m_changeRoiSignal = boost::shared_ptr< boost::function< void() > >( new boost::function< void() >( boost::bind( &WRMBranch::setDirty, this ) ) );
}

WPropertyGroup::SPtr WRMBranch::getProperties() const
{
    return m_properties;
}

void WRMBranch::propertyChanged()
{
    setDirty();
}

WPropString WRMBranch::nameProperty()
{
    return m_name;
}

WPropBool WRMBranch::invertProperty()
{
    return m_isNot;
}

WPropColor WRMBranch::colorProperty()
{
    return m_bundleColor;
}

void WRMBranch::addRoi( osg::ref_ptr< WROI > roi )
{
    m_rois.push_back( roi );
    roi->addROIChangeNotifier( m_changeRoiSignal );
    setDirty();
}

bool WRMBranch::contains( osg::ref_ptr< WROI > roi )
{
    for( std::list< osg::ref_ptr< WROI > >::iterator iter = m_rois.begin(); iter != m_rois.end(); ++iter )
    {
        if( ( *iter ) == roi )
        {
            return true;
        }
    }
    return false;
}

void WRMBranch::removeRoi( osg::ref_ptr< WROI > roi )
{
    roi->removeROIChangeNotifier( m_changeRoiSignal );
    for( std::list< osg::ref_ptr< WROI > >::iterator iter = m_rois.begin(); iter != m_rois.end(); ++iter )
    {
        if( ( *iter ) == roi )
        {
            m_rois.erase( iter );
            setDirty();
            break;
        }
    }
}

void WRMBranch::getRois( std::vector< osg::ref_ptr< WROI > >& roiVec ) // NOLINT
{
    for( std::list< osg::ref_ptr< WROI > >::iterator iter = m_rois.begin(); iter != m_rois.end(); ++iter )
    {
        roiVec.push_back( ( *iter ) );
    }
}

WROIManager::ROIs WRMBranch::getRois() const
{
    WROIManager::ROIs ret;
    for( std::list< osg::ref_ptr< WROI > >::const_iterator iter = m_rois.begin(); iter != m_rois.end(); ++iter )
    {
        ret.push_back( ( *iter ) );
    }
    return ret;
}

void WRMBranch::removeAllRois()
{
    for( std::list< osg::ref_ptr< WROI > >::iterator iter = m_rois.begin(); iter != m_rois.end(); ++iter )
    {
        WGraphicsEngine::getGraphicsEngine()->getScene()->remove( ( *iter ) );
    }

    m_rois.clear();
}

void WRMBranch::setDirty()
{
    m_dirty->set( true );
    m_roiManager->setDirty();

    for( std::list< boost::shared_ptr< boost::function< void() > > >::iterator iter = m_changeNotifiers.begin();
                iter != m_changeNotifiers.end(); ++iter )
    {
        ( **iter )();
    }
}

osg::ref_ptr< WROI > WRMBranch::getFirstRoi()
{
    return m_rois.front();
}

boost::shared_ptr< WROIManager > WRMBranch::getRoiManager()
{
    return m_roiManager;
}

boost::shared_ptr< WProperties > WRMBranch::getProperties()
{
    return m_properties;
}

void WRMBranch::addChangeNotifier( boost::shared_ptr< boost::function< void() > > notifier )
{
    boost::unique_lock< boost::shared_mutex > lock;
    lock = boost::unique_lock< boost::shared_mutex >( m_associatedNotifiersLock );
    m_changeNotifiers.push_back( notifier );
    lock.unlock();
}

void WRMBranch::removeChangeNotifier( boost::shared_ptr< boost::function< void() > > notifier )
{
    boost::unique_lock< boost::shared_mutex > lock;
    lock = boost::unique_lock< boost::shared_mutex >( m_associatedNotifiersLock );
    std::list<  boost::shared_ptr< boost::function< void() > > >::iterator it;
    it = std::find( m_changeNotifiers.begin(), m_changeNotifiers.end(), notifier );
    if( it != m_changeNotifiers.end() )
    {
        m_changeNotifiers.erase( it );
    }
    lock.unlock();
}
