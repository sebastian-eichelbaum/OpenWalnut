// OpenCV-based tool to test the OpenIGTLink in OpenWalnut by grabbing images from the camera
// and sending them to the specified IGTLink server
// (c) 2009-2011 Mario Hlawitschka

// compile using something like
// g++ main.cpp -I/opt/local/include -L/opt/local/lib -lopencv_highgui -I/usr/local/include/igtl/ -L/usr/local/lib/igtl -lOpenIGTLink -o OpenCVFramegrabber
// or whatever is appropriate on your platform

#include <iostream>
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/cvaux.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>

#include "igtlOSUtil.h"
#include "igtlImageMessage2.h"
#include "igtlClientSocket.h"

class FramegrabberDevice
{
public:
    FramegrabberDevice()
        : isInitialized( false )
    {
    }

    bool initFramegrabber( int camera )
    {
        if( isInitialized )
        {
            return false;
        }

        capture =  cvCreateCameraCapture( camera );

        sleep( 1.5 );
        if( !cvGrabFrame( capture ) )
        {
            std::cerr <<  "Cound not grab a frame" <<  std::endl;
            return false;
        }

        IplImage *img =  cvRetrieveFrame( capture );
        if( !img )
        {
            return false;
        }

        imageHeight =  img->height;
        imageWidth = img->width;
        step =  img->widthStep;
        channels = img->nChannels;
        dataOrder = img->dataOrder;
        alignment = img->align;
        std::cout << "\nwidth: " << img->width
                  << "\nheight: " <<  img->height
                  << "\nchannels: " << img->nChannels
                  << "\ndepth: " <<  img->depth
                  << "\ndataOrder: " <<  img->dataOrder
                  << "\nwidthStep: " <<  img->widthStep
                  << "\norigin: " << img->origin
                  << "\nalign: " << img->align
                  << "\n";

        isInitialized = true;
        return true;
    }

    bool releaseFramegrabber()
    {
        if( !isInitialized )
        {
            return false;
        }

        cvReleaseCapture( &capture );
        isInitialized = false;
        return true;
    }

    bool getFrameDimensions( int* resx, int* resy, int* channels )
    {
        if( !isInitialized )
        {
            return false;
        }
        *resx = imageWidth;
        *resy = imageHeight;
        *channels = this->channels;
        return true;
    }

    bool getFrame( unsigned char* data, int* resx, int* resy, int* rchannels )
    {
        if( !isInitialized )
        {
            return false;
        }

        if( !cvGrabFrame( capture ) )
        {
            std::cerr << "Could not grab a frame" << std::endl;
            return false;
        }

        IplImage* img = cvRetrieveFrame( capture );
        if( !img )
        {
            std::cerr << "Retrieve frame failed" << std::endl;
            return false;
        }

        memcpy( ( void* )data, img->imageData, imageWidth*imageHeight*channels*sizeof( unsigned char ) );
        *resx = imageWidth;
        *resy = imageHeight;
        *rchannels = channels;
        return true;
    }

    int getDataOrder()
    {
        return dataOrder;
    }

    int getDataAlignment()
    {
        return alignment;
    }

    int getDataStepWidth()
    {
        return step;
    }

    bool isInitialized;

    int imageHeight;
    int imageWidth;
    int dataOrder;
    int step;
    int channels;
    int alignment;
    CvCapture* capture;
};

void printHelp( int argc, char** argv )
{
        std::cout << argv[ 0 ] <<  ":\n"
            " -h or --help                        display this help message\n"
        //  " -server hostname port               run as a server, not implemented, yet\n"
            " -client hostname port               start in client mode and connect to given hostname:port (default=localhost:18944)\n"
            " -camera id                          OpenCV internal ID of the camera (default=0)\n"
            " -interval intervalInMs              interval to wait in ms until the next picture is captured (default=1000)\n"
            " -crop left right bottom top         dimensions are given in pixel. right and top can be absolute pixel or\n"
            "                                     -pixel to indicate an offset to left or bottom, respectively\n"
            "                                     a value of 0 for right or top indicates the last pixel of the capured image\n"
            "                                     default=0 0 0 0, which captures the whole image\n" 
            " -name YourNameForThisDevice         a name that is sent ofver OpenIGTLink to identify the device/the data.\n";
}

int main( int argc, char** argv )
{
    char mode = 'c';
    std::string hostname = "localhost";
    uint32_t port = 18944; // slicer default port

    int camera = 0;

    int interval =  1000;

    int left = -1;
    int right = 0;
    int bottom = -1;
    int top = 0;


    std::string deviceName = "OpenWalnut Framegrabber";

    int i = 1;
    while( i < argc )
    {
        if( ( strcmp( argv[ i ], "-h" ) == 0 )
         || ( strcmp( argv[ i ], "--help" ) == 0 )
         || ( strcmp( argv[ i ], "-help" ) ==  0 ) )
        {
            printHelp( argc, argv );
            return 0;
        }

        // server mode is not yet implemented and may never be
        // if( strcmp( argv[ i ], "-server" ) == 0 )
        // {
        //    mode = 's';
        //    if( argc <= i+1 )
        //    {
        //        printHelp( argc, argv );
        //        return -1;
        //    }
        //    port = atoi( argv[ i+1 ] );
        //    i += 2;
        // }
        // else
        if( strcmp( argv[ i ], "-client" ) == 0 )
        {
            if( argc <= i+2 )
            {
                printHelp( argc, argv );
                return -1;
            }
            hostname = argv[ i+1 ];
            port = atoi( argv[ i+2 ] );
            i += 3;
         }
        else if( strcmp( argv[ i ], "-camera" ) == 0 )
        {
            if( argc <= i+1 )
            {
                printHelp( argc, argv );
                return -1;
            }
            camera = atoi( argv[ i+1 ] );
            i += 2;
        }
        else if( strcmp( argv[ i ], "-interval" ) == 0 )
        {
            if( argc <= i+1 )
            {
                printHelp( argc, argv );
                return -1;
            }
            interval = atoi( argv[ i+1 ] );
            i += 2;
        }
        else if( strcmp( argv[ i ], "-crop" ) == 0 )
        {
            if( argc <= i+4 )
            {
                printHelp( argc, argv );
                return -1;
            }
            left   = atoi( argv[ i+1 ] );
            right  = atoi( argv[ i+2 ] );
            bottom = atoi( argv[ i+3 ] );
            top    = atoi( argv[ i+4 ] );
            i += 5;
        }
        else if( strcmp( argv[ i ], "-name" ) == 0 )
        {
             if( argc <= i+1 )
            {
                printHelp( argc, argv );
                return -1;
            }
             deviceName = argv[ i+1 ];
             i+= 2;
        }
        else
        {
            std::cerr << "Unknown argument " << argv[ i ] << std::endl;
            printHelp( argc, argv );
            return -1;
        }
    }

    FramegrabberDevice device;
    bool success = device.initFramegrabber( camera );
    if( !success )
    {
        std::cout << "failed to initialize camera no " << camera << 
            "\nTerminating." << std::endl;
        return -2;
    }

    int resx;
    int resy;
    int channels;
    device.getFrameDimensions( &resx, &resy, &channels );

    {
        if( right > resx )
        {
            std::cerr << "right crop larger than image size";
            return -1;
        }
        if( top > resy )
        {
            std::cerr << "top crop larger than image size";
            return -1;
        }
        if( right < 0)
        {
            right = left - right-1;
        }
        else if( right == 0 )
        {
            right = resx-1;
        }
        if( top < 0 )
        {
            top = bottom - top-1;
        }
        else if( top == 0 )
        {
            top = resy-1;
        }
        left = std::max( 0, std::min( resx-1, left ) );
        right = std::max( 0, std::min( resx-1, right ) );
        top = std::max( 0, std::min( resy-1, top ) );
        bottom = std::max( 0, std::min( resy-1, bottom ) );
    }

    std::cout <<  "cropping image to " <<  left <<  " " << right << "," << bottom << " " << top << std::endl;

    igtl::ClientSocket::Pointer socket;
    socket = igtl::ClientSocket::New();
    int r =  socket->ConnectToServer( hostname.c_str(), port );
    if( r != 0 )
    {
        std::cerr << "Error while connecting to remote host" << std::endl;
        return -1;
    }

    igtl::Matrix4x4 transform;
    transform[0][0] = 1.0;  transform[1][0] = 0.0;  transform[2][0] = 0.0; transform[3][0] = 0.0;
    transform[0][1] = 0.0;  transform[1][1] = -1.0;  transform[2][1] = 0.0; transform[3][1] = 0.0;
    transform[0][2] = 0.0;  transform[1][2] = 0.0;  transform[2][2] = 1.0; transform[3][2] = 0.0;
    transform[0][3] = 0.0;  transform[1][3] = 0.0;  transform[2][3] = 0.0; transform[3][3] = 1.0;



    unsigned char* data = new unsigned char[ resx*resy*4 ];
    int frame = 0;

    while( true )
    {
        std::cout << "Frame " <<  ( ++frame ) <<  std::endl;
        bool success = device.getFrame( data, &resx, &resy, &channels );

        if( !success )
        {
            std::cerr << "grabbing frame failed." << std::endl;
            break;
        }

        // we get an interleaved image, so copy the channels into the x-coordinate of the image
        igtl::ImageMessage2::Pointer imgMsg = igtl::ImageMessage2::New();
        imgMsg->SetDimensions( right-left+1, top-bottom+1, 1 );
        imgMsg->SetSpacing( 1, 1, 1 );
        imgMsg->SetScalarTypeToUint8();
        imgMsg->SetDeviceName( deviceName.c_str() );
        int svsize[] = { right-left+1, top-bottom+1, 1 };
        int svoffset[] = { 0, 0, 0 };

        imgMsg->SetSubVolume( svsize, svoffset );
        imgMsg->AllocateScalars();

        std::vector < uint8_t > data2( ( top-bottom+1 )*( right-left+1 ) );

        if( device.getDataOrder() ==  IPL_DATA_ORDER_PIXEL )
        {
            for( int j = 0; j <= top-bottom; ++j )
            {
                uint8_t* ptr = &data[ 0 ] + ( j+bottom )*device.getDataStepWidth();
                for( int i = 0; i <= ( right-left ); ++i )
                {
                    data2[ i + ( right-left+1 )*( j ) ] = ptr[ channels*( i+left )+1 ]; // copy the green channel
                }
            }
        }
        else // IPL_DATA_ORDER_PLANE
        {
            for( int j = 0; j <= top-bottom; ++j )
            {
                for( int i = 0; i <= right-left; ++i )
                {
                    data2[ i + ( right-left+1 )*j ] =  data[ ( ( j+bottom )*resx + ( i+left ) ) ]; // copies the red channel
                }
            }
         }

        imgMsg->SetScalarPointer( &data2[ 0 ] );

        imgMsg->SetMatrix( transform );

        imgMsg->Pack();
        std::cout <<  "sending " << imgMsg->GetNumberOfPackFragments() << " fragments." << std::endl;
        for( int i = 0; i < imgMsg->GetNumberOfPackFragments(); i++ )
        {
            std::cout << i << std::endl;
            socket->Send(imgMsg->GetPackFragmentPointer(i), imgMsg->GetPackFragmentSize(i));
            std::cout << i << std::endl;
        }

        std::cout <<  "waiting " << interval << "ms" << std::endl;
        igtl::Sleep(interval); // wait
    }
    socket->CloseSocket();
    delete[] data;
}

