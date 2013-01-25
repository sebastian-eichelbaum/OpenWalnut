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

#include <vector>

#include <QtCore/QMimeData>
#include <QtCore/QUrl>
#include <QtGui/QGraphicsSceneDragDropEvent>

#include "core/common/WTransferFunction.h"
#include "WTransferFunctionScene.h"
#include "WTransferFunctionWidget.h"

WTransferFunctionScene::WTransferFunctionScene( QObject*parent )
    : BaseClass( parent ),
      myparent( dynamic_cast<WTransferFunctionWidget*>( parent ) )
{
    setBackgroundBrush( QBrush( Qt::lightGray, Qt::CrossPattern ) ); //Qt::white );
}

WTransferFunctionScene::~WTransferFunctionScene()
{
}

void WTransferFunctionScene::dragMoveEvent( QGraphicsSceneDragDropEvent* event )
{
    if( sceneRect().contains( event->scenePos() ) )
    {
        if( event->mimeData()->hasColor() )
        {
            if( myparent )
            {
                event->accept();
            }
            return;
        }
        if( event->mimeData()->hasImage() )
        {
            if( myparent )
            {
                event->accept();
            }
            return;
        }
        if( event->mimeData()->hasUrls() )
        {
            // this may be realtively slow but is the most-reliable way
            // to find out whether we accept drags of images:
            // Just load the image whenever it is offered and we hope that
            // nobody offers a really large image!
            QImage image( event->mimeData()->urls().first().toLocalFile() );
            if( !image.isNull() )
            {
                event->accept();
                return;
            }
        }
    }
    BaseClass::dragMoveEvent( event );
}

namespace
{
    inline QColor toQColor( const WColor &color )
    {
        QColor tmp;
        tmp.setRgbF( color[0],
                     color[1],
                     color[2],
                     color[3] );

        return tmp;
    }
}


void do_update( const WTransferFunction& tf, WTransferFunctionWidget* m_transferFunction )
{
    // check whether the values changed. If they did, propagate to the GUI
    {
        m_transferFunction->clearTransferFunction();
        //m_transferFunction->externalDataChange( tf );
        m_transferFunction->setHistogram( tf.getHistogram() );

        // initialize the widget with the new transfer function
        size_t nbColors = tf.numColors();
        QColor c;
        if( nbColors > 0 )
        {
          WColor cc = tf.getColor( 0 );
          c = toQColor( cc );
          double iso = tf.getColorIsovalue( 0 );
          m_transferFunction->insertColorNormalized( QPointF( iso, 0 ), &c );
        }
        if( nbColors > 1 )
        {
            WColor cc = tf.getColor( nbColors-1 );
            c = toQColor( cc );
            double iso = tf.getColorIsovalue( nbColors-1 );
            m_transferFunction->insertColorNormalized( QPointF( iso, 0 ), &c );
        }
        for( size_t i = 1; i< nbColors-1; ++i )
        {
          WColor cc = tf.getColor( i );
          c = toQColor( cc );
          double iso = tf.getColorIsovalue( i );
          m_transferFunction->insertColorNormalized( QPointF( iso, 0 ), &c );
        }
        size_t nbAlphas = tf.numAlphas();

        // add first alpha first
        if( nbAlphas > 0 )
        {
            double alpha = tf.getAlpha( 0 );
            double iso = tf.getAlphaIsovalue( 0 );
            m_transferFunction->insertPointNormalized( QPointF( iso,  alpha ) );
        }

        // add last alpha second to prevent the widget moving mid points to the right
        if( nbAlphas > 1 )
        {
            double alpha = tf.getAlpha( nbAlphas-1 );
            double iso = tf.getAlphaIsovalue( nbAlphas-1 );
            m_transferFunction->insertPointNormalized( QPointF( iso,  alpha ) );
        }

        for( size_t i = 1; i< nbAlphas-1; ++i )
        {
          double alpha = tf.getAlpha( i );
          double iso = tf.getAlphaIsovalue( i );
          m_transferFunction->insertPointNormalized( QPointF( iso,  alpha ) );
        }
    }
}


void WTransferFunctionScene::dropEvent( QGraphicsSceneDragDropEvent* event )
{
    if( sceneRect().contains( event->scenePos() ) )
    {
        if( event->mimeData()->hasColor() )
        {
            if( myparent )
            {
                event->accept();
                QColor color = qvariant_cast<QColor>( event->mimeData()->colorData() );
                {
                    myparent->insertColor( event->scenePos(), &color );
                }
            }
        }
        QImage myImage;
        if( event->mimeData()->hasImage() )
        {
            myImage =  qvariant_cast < QImage > ( event->mimeData()->imageData() );
            // std::cout <<  "found image" <<  std::endl;
        }
        if( event->mimeData()->hasUrls() )
        {
            QList < QUrl >  urls = event->mimeData()->urls();
            Q_ASSERT( !urls.empty() );
            myImage = QImage( urls.first().toLocalFile() );
            // std::cout <<  "found url" <<  std::endl;
        }
        if( !myImage.isNull() )
        {
            // std::cout <<  "work on image" <<  std::endl;
            QImage image = myImage;
            event->accept();
            std::vector < unsigned char > data( 4*image.width() );
            for( int i = 0; i < image.width( ); ++i )
            {
                QRgb pixel = image.pixel( i, 0 );
                data[ 4*i+0 ] = qRed( pixel );
                data[ 4*i+1 ] = qGreen( pixel );
                data[ 4*i+2 ] = qBlue( pixel );
                data[ 4*i+3 ] = qAlpha( pixel );
            }

            WTransferFunction tf =  WTransferFunction::createFromRGBA( &( data[ 0 ] ), image.width() );
            do_update( tf, myparent );
        }
        return;
    }
    BaseClass::dropEvent( event );
}

