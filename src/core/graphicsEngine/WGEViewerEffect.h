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

#ifndef WGEVIEWEREFFECT_H
#define WGEVIEWEREFFECT_H

#include <string>

#include <osg/Camera>
#include <osg/Geode>
#include <osg/StateSet>

#include "core/common/WObjectNDIP.h"
#include "core/common/WProperties.h"

#include "WGECamera.h"

/**
 * Base class for implementing basic fullscreen effects for the WGEViewer. It uses properties for configuration.
 */
class WGEViewerEffect: public WObjectNDIP< WGECamera >
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WGEViewerEffect >.
     */
    typedef osg::ref_ptr< WGEViewerEffect > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WGEViewerEffect >.
     */
    typedef osg::ref_ptr< const WGEViewerEffect > ConstSPtr;

    /**
     * Create the effect.
     *
     * \param name the name
     * \param description the description
     * \param icon an icon in XPM format. Can be NULL if no icon is required.
     */
    WGEViewerEffect( std::string name, std::string description, const char** icon = NULL );

    /**
     * Destructor.
     */
    virtual ~WGEViewerEffect();

    /**
     * Check whether the effect is active or not.
     *
     * \return true if active
     */
    virtual bool isEnabled() const;

    /**
     * Set the effect enabled.
     *
     * \param enable true to enable. False to disable.
     */
    virtual void setEnabled( bool enable = true );

    /**
     * Use this to activate an effect by default. This sets the default value of the active property. It can be overwritten by user settings.
     *
     * \param enableByDefault activate this effect by default if true.
     */
    virtual void setEnabledByDefault( bool enableByDefault = true );
protected:
    /**
     * The fullscreen quad
     */
    osg::ref_ptr< osg::Geode > m_geode;

    /**
     * The stateset of the cam
     */
    osg::ref_ptr< osg::StateSet > m_state;

    /**
     * Enable or disable effect.
     */
    WPropBool m_active;
private:
};

#endif  // WGEVIEWEREFFECT_H

