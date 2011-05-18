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

#ifndef WROIASSOCEVENT_H
#define WROIASSOCEVENT_H

#include <boost/shared_ptr.hpp>

#include <QtCore/QEvent>

#include "core/graphicsEngine/WROI.h"


/**
 * Event signalling a new roi has been associated with the roi manager in the kernel.
 */
class WRoiAssocEvent: public QEvent
{
public:
    /**
     * constructor
     * \param roi
     */
    explicit WRoiAssocEvent( osg::ref_ptr< WROI > roi );

    /**
     * destructor
     */
    virtual ~WRoiAssocEvent();

    /**
     * Getter for the roi that got associated.
     *
     * \return the roi.
     */
    osg::ref_ptr< WROI > getRoi();


protected:
    /**
     * The roi that got associated.
     */
    osg::ref_ptr< WROI > m_roi;
private:
};

#endif  // WROIASSOCEVENT_H
