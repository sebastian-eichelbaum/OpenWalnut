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

#include <iostream>
#include <string>
#include <vector>

#include "WIGTLinkRemote.h"

#include "core/dataHandler/WDataSet.h"
#include "core/dataHandler/WValueSet.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/common/WIOTools.h"
#include "core/common/WLogger.h"

#include "igtlOSUtil.h"
#include "igtlImageMessage.h"
#include "igtlImageMetaMessage.h"
#include "igtlServerSocket.h"
#include "igtlTransformMessage.h"

WIGTLinkRemote::WIGTLinkRemote()
    : checkCRC( false ), port( 0 )
{
    receiversCondition.reset( new WCondition );
    statusCondition.reset( new WCondition );
}

WIGTLinkRemote::~WIGTLinkRemote()
{
    receiversMutex.lock();
    //receiveQueue.clear();
    receiversCondition->notify();
    receiversMutex.unlock();
    if( socket.IsNotNull() )
    {
        socket->CloseSocket();
    }
}

void WIGTLinkRemote::createSocketAndWaitForConnection( uint32_t port )
{
    this->port = port;
}

void WIGTLinkRemote::createSocketAndConnect( std::string hostname, uint32_t port )
{
    std::cout << "setting up connection to: \"" <<  hostname << ":" <<  port <<  "\"" << std::endl;
    this->port = 0;
    socket = igtl::ClientSocket::New();
    int r =  socket->ConnectToServer( hostname.c_str(), port );
    if( r != 0 )
    {
        throw WException( "Cannot connect to server" );
    }
    std::cout << "connection established" <<  std::endl;
    statusCondition->notify(); // link established
}

void WIGTLinkRemote::threadMain()
{
    readDataLoop();
}

void WIGTLinkRemote::listenLoop()
{
    igtl::ServerSocket::Pointer serverSocket;
    serverSocket = igtl::ServerSocket::New();
    serverSocket->CreateServer( port );

    // wait for connection
    while( !m_shutdownFlag() && socket.IsNull() )
    {
        socket = serverSocket->WaitForConnection( 2000 );
    }
    if( !m_shutdownFlag() && socket.IsNotNull() )
    {
        statusCondition->notify(); // passive connection established
    }
}

void WIGTLinkRemote::readDataLoop()
{
    igtl::MessageHeader::Pointer headerMsg;
    headerMsg = igtl::MessageHeader::New();

    headerMsg->InitPack();

    if( port > 0 )
    {
        listenLoop();
    }

    WAssert( socket.IsNotNull(), "Something failed when setting up the socket" );

    while( !m_shutdownFlag() )
    {
        try
        {
            std::cout << "Waiting for data." <<  std::endl;
            int r = socket->Receive( headerMsg->GetPackPointer(), headerMsg->GetPackSize() );
            if( r == 0 )
            {
                // socket failed!!!
                throw WException( "Socket Failed" );
            }
            std::cout << "got something." <<  std::endl;

            if( r !=  headerMsg->GetPackSize() )
            {
                throw WException( "Invalid data size" );
            }

            // deserialize the header
            headerMsg->Unpack( checkCRC ? 1:0 );

            // check data type
            if( strcmp( headerMsg->GetDeviceType(), "TRANSFORM" ) == 0 )
            {
                //debugLog() << "Received TRANSFORM";
                receiveTransform( headerMsg );
            }
            else if( strcmp( headerMsg->GetDeviceType(), "IMAGE" ) == 0 )
            {
                std::cout << "got an image!!!" << std::endl;
                WDataSetScalarSPtr ds = receiveImage( headerMsg );
                receiversMutex.lock();
                receiveQueue.push( ds );
                receiversCondition->notify();
                receiversMutex.unlock();
            }
            else if( strcmp( headerMsg->GetDeviceType(), "POSITION" ) ==  0 )
            {
                //readPosition( headerMsg );
                socket->Skip( headerMsg->GetBodySizeToRead(), 0 );
            }
            else if( strcmp( headerMsg->GetDeviceType(), "STATUS" ) == 0 )
            {
                //readStatus( headerMsg );
                socket->Skip( headerMsg->GetBodySizeToRead(), 0 );
            }
            else
            {
                //debugLog() << "Unknown header received";
                socket->Skip( headerMsg->GetBodySizeToRead(), 0 );
            }
        }
        catch( const std::exception &e )
        {
            if( socket.IsNotNull() )
            {
                socket->CloseSocket();
            }
            if( port == 0 )
            {
                // we are in client mode and the connection failed
                // notify the main program and quit this thread
                // we have to start a new connection if this happens
                throw e;
            }
            else
            {
                // just continue listening
                socket = igtl::ClientSocket::Pointer();
                listenLoop();
            }
        }
    }
}

void WIGTLinkRemote::receiveTransform( igtl::MessageHeader::Pointer headerMsg )
{
    // TODO(mario): OpenWalnut needs a nice interface for transforms to display tracking devices etc

    // message body handler for transform
    igtl::TransformMessage::Pointer transMsg;
    transMsg =  igtl::TransformMessage::New();
    transMsg->SetMessageHeader( headerMsg );
    transMsg->AllocatePack();

    socket->Receive( transMsg->GetPackBodyPointer(), transMsg->GetPackBodySize() );

    // deserialize the message body
    int c =  transMsg->Unpack( checkCRC ? 1:0 ); // 1 indicates to perform CRC
    if( c & igtl::MessageHeader::UNPACK_BODY )
    {
        // if CRC check is OK, read transform data
        igtl::Matrix4x4 matrix;
        transMsg->GetMatrix( matrix );

        // TODO(mario): add better debugging
        igtl::PrintMatrix( matrix );
    }
}

WDataSetScalarSPtr WIGTLinkRemote::receiveImage( igtl::MessageHeader::Pointer headerMsg )
{
    igtl::ImageMessage::Pointer imgMsg;
    imgMsg = igtl::ImageMessage::New();
    imgMsg->SetMessageHeader( headerMsg );
    imgMsg->AllocatePack();

    // receive data from socket
    socket->Receive( imgMsg->GetPackBodyPointer(), imgMsg->GetPackBodySize() );

    // deserialize the data
    int c =  imgMsg->Unpack( checkCRC ? 1:0 ); // 1 stands for do CRC

    if( c & igtl::MessageHeader::UNPACK_BODY )
    {
        int size[ 3 ];
        float spacing[ 3 ];
        int svsize[ 3 ];
        int svoffset[ 3 ];
        int scalarType;
        igtl::Matrix4x4 mat;

        scalarType = imgMsg->GetScalarType();
        imgMsg->GetDimensions( size );
        imgMsg->GetSpacing( spacing );
        imgMsg->GetSubVolume( svsize, svoffset );
        imgMsg->GetMatrix( mat );

        WMatrix<double> owmat( 4, 4 );
        for( int i = 0; i < 4; ++i )
        {
            for( int j = 0; j < 4; ++j )
            {
                owmat( i, j ) = mat[ i ][ j ];
            }
        }
        WGridRegular3D::SPtr grid( new WGridRegular3D( size[ 0 ], size[ 1 ], size[ 2 ], WGridTransformOrtho( owmat ) ) );

        boost::shared_ptr< WValueSetBase > valueSet = createValueSet( imgMsg );

        WDataSetScalarSPtr ds( new WDataSetScalar( valueSet, grid ) );
        // if( length( imgMsg->GetDeviceName() > 0 ) )
        // {
        //    ds->setFileName( imgMsg->GetDeviceName() );
        // }

        return ds;
    }
    else
    {
        std::cerr << "decoding image failed" << std::endl;
    }
    return WDataSetScalarSPtr ();
}

void WIGTLinkRemote::injectMessage()
{
}

void WIGTLinkRemote::sendTransform( const std::string& name, const WMatrix<double> & matrix )
{
    if( socket.IsNotNull() )
    {
        igtl::TransformMessage::Pointer transMsg;
        transMsg = igtl::TransformMessage::New();
        transMsg->SetDeviceName( name.c_str() );
        igtl::Matrix4x4 igtlMatrix;
        for( int i = 0; i < 4; ++i )
        {
            for( int j = 0; j < 4; ++j )
            {
                igtlMatrix[ i ][ j ] =  matrix( i, j );
            }
        }
        transMsg->SetMatrix( igtlMatrix );

        transMsg->Pack();

        socket->Send( transMsg->GetPackPointer(), transMsg->GetPackSize() );
    }
}

namespace
{
    int convertTypeOWtoIGTL( int type )
    {
        switch( type )
        {
            case igtl::ImageMessage::TYPE_INT8:
                    return W_DT_INT8;
            case igtl::ImageMessage::TYPE_UINT8:
                    return W_DT_UINT8;
            case igtl::ImageMessage::TYPE_INT16:
                    return W_DT_INT16;
            case igtl::ImageMessage::TYPE_UINT16:
                    return W_DT_UINT16;
            case igtl::ImageMessage::TYPE_INT32:
                    return W_DT_INT16;
            case igtl::ImageMessage::TYPE_UINT32:
                    return W_DT_UINT32;
            default:
                // TODO(mario): throw exception?
                return W_DT_UNKNOWN;
        }
    }
}

void WIGTLinkRemote::sendImageMetaData( const std::vector < WDataSetScalarSPtr >& dataSets )
{
    igtl::ImageMetaMessage::Pointer imgMetaMsg;
    imgMetaMsg =  igtl::ImageMetaMessage::New();

    imgMetaMsg->SetDeviceName( "OpenWalnut" );

    // create meta data for each data set
    for( size_t i = 0; i < dataSets.size(); ++i )
    {
        igtl::ImageMetaElement::Pointer imgMeta;
        imgMeta = igtl::ImageMetaElement::New();
        imgMeta->SetName( dataSets[ i ]->getFileName().c_str() );
        imgMeta->SetDeviceName( dataSets[ i ]->getFileName().c_str() );
        imgMeta->SetModality( "UNKNOWN_MODALITY" );
        imgMeta->SetPatientName( dataSets[ i ]->getFileName().c_str() );
        imgMeta->SetPatientID( "PATIENT_ID_0" );

        igtl::TimeStamp::Pointer ts0;
        ts0 = igtl::TimeStamp::New();
        ts0->SetTime( 123456.78 );

        imgMeta->SetTimeStamp( ts0 );
        boost::shared_ptr < WGridRegular3D > g3dr( boost::shared_dynamic_cast < WGridRegular3D > ( dataSets[ i ]->getGrid() ) );
        imgMeta->SetSize( g3dr->getNbCoordsX(), g3dr->getNbCoordsY(), g3dr->getNbCoordsZ() );
        imgMeta->SetScalarType( convertTypeOWtoIGTL( dataSets[ i ]->getValueSet()->getDataType() ) );
        imgMetaMsg->AddImageMetaElement( imgMeta );
    }
    imgMetaMsg->Pack();

    socket->Send( imgMetaMsg->GetPackPointer(), imgMetaMsg->GetPackSize() );
}

namespace Ugly
{
    template < int DT >
    size_t getRawSizeT( boost::shared_ptr < WValueSetBase >  valueSet )
    {
        typedef typename DataTypeRT<DT>::type type;
        boost::shared_ptr < WValueSet < type > > v = boost::shared_dynamic_cast < WValueSet < type > >( valueSet );
        WAssert( v, "Type cast failed" );
        return valueSet->rawSize() * sizeof( type );
    }

    template < int DT >
    const void* getRawPtrT( boost::shared_ptr < WValueSetBase > valueSet )
    {
        typedef typename DataTypeRT<DT>::type type;
        boost::shared_ptr < WValueSet < type > > v;
        v = boost::shared_dynamic_cast < WValueSet < type > >( valueSet );
        WAssert( v, "Type cast failed" );
        const void* ptr = v->rawData();
        WAssert( ptr != 0, "Trying to query raw data, got null pointer" );
        return ptr;
    }

    size_t getRawSize( boost::shared_ptr < WValueSetBase > valueSet )
    {
        int type = valueSet->getDataType();
#define CASE( A ) case A: return getRawSizeT < A > ( valueSet );
        switch( type )
        {
            CASE( W_DT_UNSIGNED_CHAR );
            CASE( W_DT_INT8 );
            CASE( W_DT_SIGNED_SHORT ); // INT16
            CASE( W_DT_SIGNED_INT ); // INT32
            CASE( W_DT_FLOAT );
            CASE( W_DT_DOUBLE );
            CASE( W_DT_UINT16 );
            CASE( W_DT_UINT32 );
            CASE( W_DT_INT64 );
            CASE( W_DT_UINT64 );
            CASE( W_DT_FLOAT128 );
            default:
                throw WException( "Not implemented for given data type" );
        }
#undef CASE
    }

    const void* getRawPtr( boost::shared_ptr < WValueSetBase > valueSet )
    {
        int type = valueSet->getDataType();
#define CASE( A ) case A: return getRawPtrT < A > ( valueSet );
        switch( type )
        {
            CASE( W_DT_UNSIGNED_CHAR );
            CASE( W_DT_INT8 );
            CASE( W_DT_SIGNED_SHORT ); // INT16
            CASE( W_DT_SIGNED_INT ); // INT32
            CASE( W_DT_FLOAT );
            CASE( W_DT_DOUBLE );
            CASE( W_DT_UINT16 );
            CASE( W_DT_UINT32 );
            CASE( W_DT_INT64 );
            CASE( W_DT_UINT64 );
            CASE( W_DT_FLOAT128 );
            default:
                throw WException( "Not implemented for given data type" );
        }
#undef CASE
    }
}

void WIGTLinkRemote::sendImageData( WDataSetScalarSPtr dataSetScalar )
{
    boost::shared_ptr< WValueSetBase > valueSet = dataSetScalar->getValueSet();
    //size_t rawSize = valueSet->rawSize();

    int scalarType = 0;
    switch( valueSet->getDataType() )
    {
        case W_DT_FLOAT:
            scalarType = igtl::ImageMessage::TYPE_FLOAT32;
            break;
        case W_DT_DOUBLE:
            scalarType = igtl::ImageMessage::TYPE_FLOAT64;
            break;
        case W_DT_UINT16:
            scalarType = igtl::ImageMessage::TYPE_UINT16;
            break;
        case W_DT_UINT32:
            scalarType = igtl::ImageMessage::TYPE_UINT32;
            break;
        case W_DT_UNSIGNED_CHAR:
            scalarType = igtl::ImageMessage::TYPE_UINT8;
            break;
        case W_DT_INT8:
            scalarType = igtl::ImageMessage::TYPE_INT8;
            break;
        case W_DT_INT64:
        case W_DT_FLOAT128:
            throw WException( "Unsupported scalar type: not supported by igtl?" );
            break;
        case W_DT_SIGNED_INT:
            scalarType = igtl::ImageMessage::TYPE_INT32;
            break;
        case W_DT_SIGNED_SHORT:
            scalarType = igtl::ImageMessage::TYPE_INT16;
            break;
        case W_DT_BINARY:
        case W_DT_UINT64:
            throw WException( "Unsupported scalar type" );
            break;
        case W_DT_RGB:
        case W_DT_RGBA32:
            throw WException( "RGB not supported" );
            break;
        case W_DT_COMPLEX:
        case W_DT_COMPLEX128:
        case W_DT_COMPLEX256:
            throw WException( "Complex types are not supported, yet" );
            break;
        case W_DT_NONE:
            throw WException( "W_DT_NONE should never occur as a type." );
            break;
        case W_DT_ALL:
            throw WException( "W_DT_ALL should never occur as a type." );
            break;
    }

    if( socket.IsNotNull() )
    {
        int size[ 3 ];
        boost::shared_ptr < WGridRegular3D > g3dr( boost::shared_dynamic_cast < WGridRegular3D > ( dataSetScalar->getGrid() ) );
        size[ 0 ] = g3dr->getNbCoordsX();
        size[ 1 ] = g3dr->getNbCoordsY();
        size[ 2 ] = g3dr->getNbCoordsZ();

        int svsize[ 3 ] = { size[ 0 ], size[ 1 ], size[ 2 ]};
        int svoffset[] = { 0, 0, 0 };

        igtl::ImageMessage::Pointer imgMsg =  igtl::ImageMessage::New();
        imgMsg->SetDimensions( size );
        imgMsg->SetSpacing( g3dr->getOffsetX(), g3dr->getOffsetY(), g3dr->getOffsetZ() );
        imgMsg->SetDeviceName( "OpenWalnut" );
        imgMsg->SetSubVolume( svsize, svoffset );
        imgMsg->SetScalarType( scalarType );
        imgMsg->AllocateScalars();

        size_t rawsize =  Ugly::getRawSize( valueSet );
        std::cout << "Transfering " << rawsize << " = " << imgMsg->GetImageSize() << " bytes of data." << std::endl;
        memcpy( imgMsg->GetScalarPointer(), Ugly::getRawPtr( valueSet ), rawsize );

        igtl::Matrix4x4 matrix;
        igtl::IdentityMatrix( matrix );
        imgMsg->SetMatrix( matrix ); // TODO(mario): get the matrix from the data set
        imgMsg->Pack();

        socket->Send( imgMsg->GetPackPointer(), imgMsg->GetPackSize() );
    }
}

boost::shared_ptr < WValueSetBase > WIGTLinkRemote::createValueSet( const igtl::ImageMessage::Pointer& imgMsg )
{
    boost::shared_ptr<WValueSetBase> valueSet;
    int size[ 3 ];
    imgMsg->GetDimensions( size );
    size_t sz =  size[ 0 ] * size[ 1 ] * size[ 2 ];
#define CASE( igtltype, ctype, owtype )\
    case igtltype: \
            {\
                boost::shared_ptr < std::vector < ctype > > values( new std::vector < ctype >( sz ) );\
                memcpy( ( void* )&( ( *values )[ 0 ] ), imgMsg->GetScalarPointer(), sizeof( ctype ) * sz );\
                valueSet.reset( new WValueSet < ctype >( 0, 1, values, owtype ) );\
            }\
            break

     switch( imgMsg->GetScalarType() )
    {
        CASE( igtl::ImageMessage::TYPE_INT8, int8_t, W_DT_INT8 );
        CASE( igtl::ImageMessage::TYPE_UINT8, uint8_t, W_DT_UINT8 );
        CASE( igtl::ImageMessage::TYPE_INT16, int16_t, W_DT_INT16 );
        CASE( igtl::ImageMessage::TYPE_UINT16, uint16_t, W_DT_UINT16 );
        CASE( igtl::ImageMessage::TYPE_INT32, int32_t, W_DT_SIGNED_INT );
        CASE( igtl::ImageMessage::TYPE_UINT32, uint32_t, W_DT_UINT32 );
         default:
            break;
            // TODO(mario): throw exception?
    }
#undef CASE
    return valueSet;
}
