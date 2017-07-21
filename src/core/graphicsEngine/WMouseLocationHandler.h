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

#ifndef WMOUSELOCATIONHANDLER_H
#define WMOUSELOCATIONHANDLER_H

#include <string>
#include <utility>

#include <boost/signals2/signal.hpp>

#include <osgViewer/View>

#include "../common/math/linearAlgebra/WVectorFixed.h"


/**
 * Class to handle providing information about pixel position of mouse.
 */
class WMouseLocationHandler: public osgGA::GUIEventHandler
{
public:
    /**
     * Constructor that initalizes members with sensible defaults.
     */
    WMouseLocationHandler();

    /**
     * Constructor that initalizes members with sensible defaults and sets the name of the viewer
     *
     * \param viewerName name of the viewer
     */
    explicit WMouseLocationHandler( std::string viewerName );

    /**
     * Deals with the events found by the osg.
     * \param ea Event class for storing Keyboard, mouse and window events.
     * \param aa Interface by which GUIEventHandlers may request actions of the GUI system
     *
     * \return true if the event was handled.
     */
    bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );

    /**
     * This class holds the information about the mouse pointer location in a certain viewe.
     */
    class WMouseLocationInfo
    {
    public:
        /**
         * Initializing members.
         *
         * \param viewerName Name of the osg::viewer to which the infos belong
         * \param pixelCoords The pixel position of the mouse pointer in the viewer.
         */
        WMouseLocationInfo( std::string viewerName,
                            std::pair< float, float > pixelCoords ) :
            m_viewerName( viewerName ),
            m_pixelCoords( pixelCoords )
            {
            }
        /**
         * Default initializing members.
         */
        WMouseLocationInfo() :
            m_viewerName( "" ),
            m_pixelCoords( std::make_pair( 0.0, 0.0 ) )
            {
            }
        /**
         * Obtain the mouse location in pixel coordinates
         *
         * \return The position of the mouse pointer in the viewer.
         */
        inline WVector2d getPixel() const
            {
                WVector2d pos;
                pos[0] = m_pixelCoords.first;
                pos[1] = m_pixelCoords.second;
                return pos;
            }
    private:
        std::string m_viewerName; //!< name of the viewer
        std::pair< float, float > m_pixelCoords; //!< Pixel coordinates of the mouse.
    };

    /**
     * returns the m_locationSignal to for registering to it.
     */
    boost::signals2::signal< void( WMouseLocationInfo ) >* getLocationSignal();

    /**
     * Gives information about the mouse location.
     *
     * \return info the location information
     */
    WMouseLocationInfo getLocationInfo();


protected:
    /**
     * Virtual destructor needed because of virtual function.
     *
     * This desctructor is protected to avoid accidentally deleting
     * an instance of WMouseLocationHandler.
     * This follows the philosophy of OSG to avoid problems in multithreaded
     * environments, since these pointers are used deep in the OSG where
     * a deletion could cause a segfault.
     */
    virtual ~WMouseLocationHandler();

private:
    boost::signals2::signal< void( WMouseLocationInfo ) > m_locationSignal; //!< One can register to this signal to receive location events.

    WMouseLocationInfo m_mouseLocation; //!< Representation of mouse location
    std::string m_viewerName; //!< which viewer sends the signal
};

#endif  // WMOUSELOCATIONHANDLER_H
