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

#ifndef WIGTLINKREMOTE_H
#define WIGTLINKREMOTE_H

#include <vector>
#include <queue>
#include <string>

#include "core/common/WCondition.h"

#include "core/common/WThreadedRunner.h"

#include "core/common/math/WMatrix.h"

#include "igtlMessageHeader.h"
#include "igtlImageMessage.h"
#include "igtlClientSocket.h"
#include "core/dataHandler/WDataSetScalar.h"

class WDataSet;
class WDataSetScalar;
class WValueSetBase;

// there is no WDataSetScalar::SPtr definition, yet
// so I use this workaround here for better readability
typedef boost::shared_ptr<WDataSetScalar> WDataSetScalarSPtr;

/**
 * Common base class for a IGT Link connection.
 * Provides some helper functions to handle and convert OW data.
 */
class WIGTLinkRemote : public WThreadedRunner
{
public:
    WIGTLinkRemote();

    virtual ~WIGTLinkRemote();

    /**
     * Use this as a server that waits for client connections.
     *
     * \param port Port number for the connection
     */
    void createSocketAndWaitForConnection( uint32_t port );

    /**
     * Use this as a client that connects to a remote server
     *
     * \param server Name of the remote server.
     * \param port Port number for the connection.
     */
    void createSocketAndConnect( std::string server, uint32_t port );

    /**
     * main loop for reading
     */
    void readDataLoop();

    /**
     * Receive a transform
     *
     * \param headerMsg The message conatining the transform.
     */
    void receiveTransform( igtl::MessageHeader::Pointer headerMsg );

    /**
     * Receive an image
     *
     * \param headerMsg The image in this message is used to create a dataset
     * \return Dataset created from message.
     */
    WDataSetScalarSPtr receiveImage( igtl::MessageHeader::Pointer headerMsg );

    /**
     * TODO: inject a message in the send queue
     */
    void injectMessage();

    /**
     * Send metadata of a list of data sets
     *
     * \param dataSets A vector containing pointer to the datasets whose metadata we want to send.
     */
    void sendImageMetaData( const std::vector<WDataSetScalarSPtr>& dataSets );

    /**
     * Send image data of a single data set
     *
     * \param dataSetScalar Pointer to the dataset we want to send
     */
    void sendImageData( WDataSetScalarSPtr  dataSetScalar );

    /**
     * send a matrix as an igtl transform
     * \param name: the name to use for the transform
     * \param matrix: the matrix that will be send
     * \pre: socket is valid. If not, nothing will be sent and no error will be reported.
     */
    void sendTransform( const std::string& name, const WMatrix <double> & matrix );

    /**
     * Internal helper to create a value set from a message
     *
     * \param imgMsg The image behind this pointer is used to create the dataset
     * \return Created value set.
     */
    boost::shared_ptr < WValueSetBase > createValueSet( const igtl::ImageMessage::Pointer& imgMsg );

    /**
     * Set whether we check the CRC. Otherwise, we just ignore it while unpacking the data.
     * The default for checkCRC is false, so there is no CRC checking when this function
     * has not been called.
     *
     * \param doCheckCRC Should CRC of the data be checked?
     */
    void setCheckCRC( bool doCheckCRC = true )
    {
        checkCRC = doCheckCRC;
    }

    /**
     * setup listening socket and listen
     */
    void listenLoop();

    /**
     * Send a message containing the metadata of the data sets
     *
     * \param dataSets A vector containing pointers to the datasets whose metadata we want to send.
     */
    void sendImageMetaData( std::vector < WDataSetScalarSPtr > dataSets );

    /**
     * mutex has to be locked during access to receiveQueue
     */
    boost::mutex receiversMutex;

    /**
     * condition to notify receivers that new data is waiting
     */
    boost::shared_ptr < WCondition > receiversCondition;

    /**
     * condition to notify a status change
     */
    boost::shared_ptr < WCondition > statusCondition;

    /**
     * queue of received data sets that should be read by the module
     */
    std::queue < WDataSetScalarSPtr > receiveQueue;

protected:
    /**
     * the main thread doing passive connections or receiving data
     */
    virtual void threadMain();

    /** true if we check CRC sums in incoming packets */
    bool checkCRC;

    /** the port for listening connections */
    uint32_t port;
private:

    /**
     * Points to the socket used by this connection.
     */
    igtl::ClientSocket::Pointer socket;
};

#endif  // WIGTLINKREMOTE_H
