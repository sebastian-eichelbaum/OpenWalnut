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

#ifndef WQTNETWORKINPUTPORT_H
#define WQTNETWORKINPUTPORT_H

#include <QtGui/QGraphicsRectItem>

#include "../../../kernel/WModuleInputConnector.h"
#include "WQtNetworkPort.h"
#include "WQtNetworkArrow.h"

// forward declaration
class WQtNetworkArrow;

/**
 * This class represents the ports a module have. Two ports from different
 * modules can be connected by the corresponding ports.
 */
class WQtNetworkInputPort : public WQtNetworkPort
{
public:

    /**
     * Constructor
     * \param connector the related WModuleInputConnector
     */
    explicit WQtNetworkInputPort( boost::shared_ptr<WModuleInputConnector> connector );

    /**
     * Destructor
     */
    ~WQtNetworkInputPort();

    /**
     * This customize the return value of type()
     */
    enum
    {
        Type = UserType + 11
    };

    /**
     * Reimplementation from QGraphicsItem
     * \return the type of the item as int
     */
    int type() const;

    /**
     * Every Arrow connected with this port is updating its position in the
     * scene.
     */
    void updateArrows();

    /**
     * Removes a specific arrow
     * \param arrow an arrow
     */
    void removeArrow( WQtNetworkArrow *arrow );

    /**
     * Removes all connected arrows
     */
    void removeArrows();

    /**
     * Calculates the position inside a item for each port to get a correct
     * alignment
     * \param size the total number of ports ( distinguished by in- and
     *          outport
     * \param portNumber the number of the current port (distinguised by in-
     *          and outport
     * \param rect the rect of the parent item
     * \param outPort is it an in- or outport
     */
    void alignPosition( int size, int portNumber, QRectF rect, bool outPort );

    /**
     * Set the type of the port.
     * \param type true if out / false if in
     */
    void setOutPort( bool type );

    /**
     * Returns the porttype.
     * \return is it a outport
     */
    bool isOutPort();

    /**
     * Returns the Name.
     * \return Name
     */
    QString getPortName();

    /**
     * Set the Name
     * \param str Name as string
     */
    void setPortName( QString str );

    /**
     * Return the number of connections
     * \return number of connections
     */
    int getNumberOfArrows();

    /**
     * Returns the WModuleInputConnecter that belongs to this object.
     * \return a WModuleInputConnector
     */
    boost::shared_ptr<WModuleInputConnector> getConnector();

    /**
     * Adds an arrow to the port
     *
     * \param arrow the arrow to add
     */
    void addArrow( WQtNetworkArrow* arrow );

    /**
     * Get a QList of all arrows connected to this port
     * \return a QList of WQtNetworkArrows
     */
    QList< WQtNetworkArrow *> getArrowList();

private:

    bool m_isOutPort; //!< is the port an outport

    QList< WQtNetworkArrow *> m_arrows; //!< the connected arrows

    QString m_name; //!< the portname

    boost::shared_ptr<WModuleInputConnector> m_connector; //!< the related WModuleInputConnector
};
#endif  // WQTNETWORKINPUTPORT_H
