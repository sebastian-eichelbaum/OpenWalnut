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

#ifndef WQTNETWORKPORT_H
#define WQTNETWORKPORT_H

#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsLineItem>
#include <QtGui/QGraphicsScene>

#include "WQtNetworkArrow.h"

// forward declaration
class WQtNetworkArrow;

/**
 * This class represents the ports a module have. Two ports from different
 * modules can be connected by the corresponding ports.
 */ 
class WQtNetworkPort : public QGraphicsRectItem
{
    public:

        /**
         * Constructor
         */
        //WQtNetworkPort();

        /**
         * Destructor
         */
        virtual ~WQtNetworkPort();
        
        
        /**
         * This customize the return value of type()
         *
        enum
        {
            Type = UserType + 10
        };

        **
         * Reimplementation from QGraphicsItem
         * \return the type of the item as int
         */
        virtual int type() const = 0;
        

        /**
         * Every Arrow connected with this port is updating its position in the
         * scene.
         */
        virtual void updateArrows() = 0;

        /**
         * Removes a specific arrow
         * \param arrow an arrow
         */
        virtual void removeArrow( WQtNetworkArrow *arrow ) = 0;

        /**
         * Removes all connected arrows
         */
        virtual void removeArrows() = 0;

        /**
         * Calculates the position inside a item for each port to get a correct
         * alignment
         * \param size the total number of ports ( distinguished by in- and
         *          outport
         * \param portNumber the number of the current port (distinguised by in-
         *          and outport
         * \param rect the rect of the parent item
         * \param inOut is it an in- or outport
         */
        void alignPosition( int size, int portNumber, QRectF rect, bool outPort );

        /**
         * Set the type of the port.
         * \param type true if out / false if in
         */
        virtual void setOutPort( bool type ) = 0;

        /**
         * Returns the porttype.
         * \return is it a outport
         */
        virtual bool isOutPort() = 0;

        /**
         * Returns the Name.
         * \return Name
         */
        virtual QString getPortName() = 0;

        /**
         * Set the Name
         * \param str Name as string
         */
        virtual void setPortName( QString str ) = 0;

        virtual int getNumberOfArrows() = 0;

        /**
         * Adds an arrow to the port
         */
        virtual void addArrow( WQtNetworkArrow *arrow ) = 0;

    protected:

        /**
         * Start drawing an arrow
         *
         * \param mouseEvent the mouse event
         */
//        void mousePressEvent( QGraphicsSceneMouseEvent *mouseEvent );

        /**
         * Updates the arrows endpoint.
         *
         * \param mouseEvent the mouse event
         */
//        void mouseMoveEvent( QGraphicsSceneMouseEvent *mouseEvent );

        /**
         * Draw the arrow if its the correct port
         *
         * \param mouseEvent the mouse event
         */
//        void mouseReleaseEvent( QGraphicsSceneMouseEvent *mouseEvent );

    private:
    
        QGraphicsLineItem *line; //!< the temporary line when u connect two ports

};
#endif  // WQTNETWORKPORT_H
