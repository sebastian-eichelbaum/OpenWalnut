//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#ifndef WGRAPHICSENGINE_H
#define WGRAPHICSENGINE_H

#include <list>

#include <boost/shared_ptr.hpp>
#include <osg/Camera>
#include <osgViewer/Viewer>

/**
 * \par Description:
 * Base class for initializing the graphics engine. This Class also serves as adaptor to access the graphics
 * engine.
 */
class WGraphicsEngine
{
public:

    /**
     * \par Description
     * Default constructor.
     */
    WGraphicsEngine();

    /**
     * \par Description
     * Destructor.
     */
    virtual ~WGraphicsEngine();

    /**
     * \par Description
     * Copy constructor
     * \param other Reference on object to copy.
     */
    WGraphicsEngine( const WGraphicsEngine& other );

protected:

    /**
     * \par Description
     * List of all OSG views.
     */
    std::list<boost::shared_ptr<osgViewer::Viewer> > m_Views;
private:
};

#endif  // WGRAPHICSENGINE_H

