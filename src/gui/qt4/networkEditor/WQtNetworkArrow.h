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

#ifndef WQTNETWORKARROW_H
#define WQTNETWORKARROW_H

#include <string>
#include <iostream>

#include <QtGui/QGraphicsLineItem>
#include <QtGui/QGraphicsScene>

#include "WQtNetworkPort.h"

class WQtNetworkPort;

class WQtNetworkArrow : public QGraphicsLineItem
{
    public:

        enum
        {
           Type = UserType + 4
        };

        explicit WQtNetworkArrow( WQtNetworkPort *outPort, WQtNetworkPort *inPort );

        ~WQtNetworkArrow();

        int type() const
        {
            return Type;
        }

        void updatePosition();

        WQtNetworkPort *startItem() const
        {
            return m_outPort;
        }

        WQtNetworkPort *endItem() const
        {
            return m_inPort;
        }

    protected:

    private:
        WQtNetworkPort *m_outPort;
        WQtNetworkPort *m_inPort;
};
#endif  // WQTNETWORKARROW_H
