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
#include <cassert>

#include "QtGui/QMouseEvent"
#include "QtGui/QPaintEngine"
#include "QtGui/QGraphicsSceneMouseEvent"


#include "core/common/WTransferFunction.h"

#include "WTransferFunctionPoint.h"
#include "WTransferFunctionColorPoint.h"
#include "WTransferFunctionBackground.h"
#include "WTransferFunctionHistogram.h"
#include "WTransferFunctionLine.h"
#include "WTransferFunctionScene.h"

#include "WTransferFunctionWidget.h"
#include "WTransferFunctionWidget.moc"

WTransferFunctionWidget::WTransferFunctionWidget( QWidget*qparent, WTransferFunctionGuiNotificationClass* parent ): BaseClass( qparent ),
    parent( parent ),
    scene( 0x0 ),
    first( 0x0 ),
    last( 0x0 ),
    current( 0x0 ),
    cfirst( 0x0 ),
    clast( 0x0 ),
    ccurrent( 0x0 ),
    background( 0x0 ),
    histogram( 0x0 ),
    initialized( false )
{
    // std::cout << "new widget" << std::endl;
    const int xMin( 0 );
    const int yMin( 0 );
    const int xMax( 300 );
    const int yMax( 100 );

    // set up the scene and the parameters that define how we paint things
    setMinimumSize( xMax-xMin+20, yMax - yMin + 30 );
    this->setViewportUpdateMode( QGraphicsView::FullViewportUpdate );

    scene = new WTransferFunctionScene( this );
    scene->setItemIndexMethod( QGraphicsScene::NoIndex );
    scene->setSceneRect( xMin, yMin, xMax, yMax );
    this->setScene( scene );


    this->setCacheMode( CacheNone );
    this->setRenderHint( QPainter::Antialiasing );
    //this->setTransformationAnchor( AnchorUnderMouse );
    //this->setResizeAnchor( AnchorViewCenter );

    // insert background and histogram items
    scene->addItem( background = new WTransferFunctionBackground( this ) );
    scene->addItem( histogram  = new WTransferFunctionHistogram( this ) );

    // // insert first and last alpha point
    // first = new WTransferFunctionPoint( this );
    // last = new WTransferFunctionPoint( this );
    //
    // first->setRight( last );
    // last->setLeft( first );
    //
    // scene->addItem( first );
    // scene->addItem( last );
    //
    // first->setPos( QPointF( xMin, yMax/3*2 ) );
    // last->setPos( QPointF( xMax, yMax/8 ) );

    // // connect points by a line
    // WTransferFunctionLine* line = new WTransferFunctionLine();
    // line->setLeft( first );
    // line->setRight( last );
    // first->setLine( line );
    //
    // scene->addItem( line );
    //
    // // create the control points for the color points
    // cfirst = new WTransferFunctionColorPoint( this );
    // clast  = new WTransferFunctionColorPoint( this );
    //
    // cfirst->setRight( clast );
    // clast->setLeft( cfirst );
    //
    // cfirst->colorSelected( Qt::black );
    // clast->colorSelected( Qt::white );
    //
    // cfirst->setPos( xMin, 0 );
    // clast->setPos( xMax, 0 );
    //
    // scene->addItem( cfirst );
    //scene->addItem( clast );

    initialized = true;
    // initialize the color map (aka. background)
    setMyBackground(); // trigger first paint of transfer function
}

namespace
{
    /**
     * linear blending of two colors in RGB space. Alpha is ignored
     */
    QColor blend( const QColor&a, double ta, const QColor &b, double tb )
    {
        return QColor(
                ta*a.red()+tb*b.red(),
                ta*a.green()+tb*b.green(),
                ta*a.blue()+tb*b.blue() );
    }
}

void WTransferFunctionWidget::sample1DWTransferFunction( unsigned char*array, int width )
{
    if ( !first || !cfirst)
        return;

    WTransferFunctionPoint *acurrent( first );
    WTransferFunctionColorPoint *ccurrent( cfirst );

    for ( int i = 0; i < width; ++i )
    {
        double normalized = ( double )i/( double )scene->width();
        while (acurrent && acurrent->getRight() && normalized > acurrent->getRight()->pos().x() )
        {
            acurrent = acurrent->getRight();
        }

        while ( ccurrent && ccurrent->getRight() && normalized > ccurrent->getRight()->pos().x() )
        {
            ccurrent = ccurrent->getRight();
        }

        if ( !acurrent || !acurrent->getRight() )
        {
            break;
        }
        if ( !ccurrent || !ccurrent->getRight() )
        {
            break;
        }

        double awidth = acurrent->getRight()->pos().x() - acurrent->pos().x();
        double cwidth = ccurrent->getRight()->pos().x() - ccurrent->pos().x();

        double at = ( normalized - acurrent->pos().x() )/awidth;
        double ct = ( normalized - ccurrent->pos().x() )/cwidth;

        double alpha = ( double )( acurrent->getRight()->pos().y() )/scene->height()*at + ( double )( acurrent->pos().y() )/scene->height()*( 1.-at );
        QColor rgb = blend( ccurrent->getRight()->getColor(), ct, ccurrent->getColor(), ( 1.-ct ) );

        array[ i*4 + 0 ] = rgb.red();
        array[ i*4 + 1 ] = rgb.green();
        array[ i*4 + 2 ] = rgb.blue();
        array[ i*4 + 3 ] = alpha;
    }
}


//! inaternal representation needs ARGB, but we do not display alpha component, so set it to 255
void WTransferFunctionWidget::sample1DWTransferFunctionForDisplay( unsigned char*array, int width )
{
    double scenewidth = scene->width();
    //WTransferFunctionPoint *acurrent( first );
    WTransferFunctionColorPoint *ccurrent( cfirst );

    for ( int i = 0; i < width; ++i )
    {
        double normalized = ( double )i/( double )width * scenewidth;

        while ( ccurrent && ccurrent->getRight() && normalized > ccurrent->getRight()->pos().x() )
        {
            ccurrent = ccurrent->getRight();
        }

        if ( ccurrent && ccurrent->getRight() )
        {
            double cwidth = ccurrent->getRight()->pos().x() - ccurrent->pos().x();

            double ct = ( normalized - ccurrent->pos().x() )/cwidth;

            QColor rgb = blend( ccurrent->getRight()->getColor(), ct, ccurrent->getColor(), ( 1.-ct ) );

            // FIXME: this encoding differs from platform to platform. Even though I asked for ARGB, it is RGBA on Linux
#ifdef Q_WS_MAC
            array[ i*4 + 0 ] = 255; //alpha;
            array[ i*4 + 1 ] = rgb.red();
            array[ i*4 + 2 ] = rgb.green();
            array[ i*4 + 3 ] = rgb.blue();
#else
            array[ i*4 + 2 ] = rgb.red();
            array[ i*4 + 1 ] = rgb.green();
            array[ i*4 + 0 ] = rgb.blue();
            array[ i*4 + 3 ] = 255; //alpha;
#endif
        }
    }
}



WTransferFunctionWidget::~WTransferFunctionWidget()
{
    // debug loop
    // std::cout << "~WTransferFunctionWidget()" << std::endl;
    WTransferFunctionPoint *current( first );
    while ( current )
    {
        // std::cout << "item: " << current->pos().x() << ", " << current->pos().y() << std::endl;
        current = current->getRight();
    }
    // std::cout << "done." << std::endl;

    // hopefully, the QScene will delete all its items.
}

void WTransferFunctionWidget::setMyBackground()
{
    const int transferFunctionSize = 100;
    static unsigned char texturearray[ 4*transferFunctionSize ];

    if ( background )
    {
        sample1DWTransferFunctionForDisplay( texturearray, transferFunctionSize );

        QImage image( texturearray, transferFunctionSize, 1, QImage::Format_ARGB32 );
        QPixmap pixmap( transferFunctionSize, 1 );
        pixmap.convertFromImage( image );

        background->setMyPixmap( pixmap );
    }
}

void WTransferFunctionWidget::drawBackground( QPainter *painter, const QRectF &rect )
{
    BaseClass::drawBackground( painter, rect );

    // paint the border
    // painter->setBrush(  Qt::NoBrush );
    // painter->drawRect( rect );
}

void WTransferFunctionWidget::setHistogram( const std::vector< double > &newHistogram )
{
    // std::cout << "Histogram updated." << std::endl;
    histogram->data = newHistogram;
    // std::cout << "Histogram size" << histogram->data.size() << std::endl;
    histogram->update();
    forceRedraw();
}

void WTransferFunctionWidget::dataChanged()
{
    if ( !initialized ) return;
    // std::cout << "dataChanged()" << std::endl;
    this->updateTransferFunction();
    this->setMyBackground();
    forceRedraw();
}

void WTransferFunctionWidget::forceRedraw()
{
    if ( !initialized ) return;
    QRectF viewport( scene->sceneRect() );
    scene->invalidate( viewport );
    this->update();
}

void WTransferFunctionWidget::clearTransferFunction()
{
    while ( cfirst )
    {
        WTransferFunctionColorPoint *next = cfirst->getRight();
        delete cfirst;
        cfirst = next;
    }
    clast = 0x0;

    while ( first )
    {
        WTransferFunctionPoint *next = first->getRight();
        if ( first->getLine() )
        {
            delete ( first->getLine() );
        }
        delete first;
        first = next;
    }
    last = 0x0;
    ccurrent = 0x0;
    current = 0x0;
}

void WTransferFunctionWidget::keyPressEvent( QKeyEvent *event )
{
    if ( event->key() == Qt::Key_Backspace
            || event->key() == Qt::Key_Delete )
    {
        if ( current )
        {
            if ( current->getRight() && current->getLeft() )
            {
                current->getLeft()->getLine()->setRight( current->getRight() );
                delete current->getLine();

                WTransferFunctionPoint *next = 0;
                if ( current->getLeft() && current->getLeft()->getLeft( ) )
                    next = current->getLeft();
                else if ( current->getRight() && current->getRight()->getRight() )
                    next = current->getRight();

                current->getLeft()->setRight( current->getRight() );
                current->getRight()->setLeft( current->getLeft() );
                delete current;
                current = next;
                this->dataChanged();
            }
        }
        if ( ccurrent )
        {
            if ( ccurrent->getRight() && ccurrent->getLeft() )
            {
                WTransferFunctionColorPoint *next = 0;
                if ( ccurrent->getLeft() && ccurrent->getLeft()->getLeft( ) )
                    next = ccurrent->getLeft();
                else if ( ccurrent->getRight() && ccurrent->getRight()->getRight() )
                    next = ccurrent->getRight();

                ccurrent->getLeft()->setRight( ccurrent->getRight() );
                ccurrent->getRight()->setLeft( ccurrent->getLeft() );
                delete ccurrent;
                ccurrent = next;
                this->dataChanged();
            }
        }
    }
}

void WTransferFunctionWidget::insertColorNormalized( const QPointF& pos, QColor const *const color  )
{
    insertColor( QPointF( pos.x()*scene->width(), 0 ), color );
}

void WTransferFunctionWidget::insertColor( const QPointF& pos, QColor const *const color  )
{
    WTransferFunctionColorPoint *point( new WTransferFunctionColorPoint( this ) );
    scene->addItem( point );

    point->setPos( QPointF( pos.x(), 0 ) );

    WTransferFunctionColorPoint* left( this->findCPointOnLeft( pos ) );
    if ( left )
    {
        WTransferFunctionColorPoint* right( left->getRight() );

        left->setRight( point );
        point->setLeft( left );
        point->setRight( right );
        if ( right )
        {
            right->setLeft( point );
        }
        if ( color )
        {
            point->colorSelected( *color );
        }
        else
        {
            QColor a = left->getColor();
            if ( right )
            {
                QColor b = right->getColor();
                double p = ( point->pos().x() - left->pos().x() )/( right->pos().x() - left->pos().x() );
                point->colorSelected( blend( a, ( 1.-p ), b, ( p ) ) );
            }
            else
            {
                point->colorSelected( a );
            }
        }
    }
    else
    {
        point->setRight( cfirst );
        if ( cfirst )
        {
            cfirst->setLeft( point );
        }
        cfirst = point;
        if ( !clast )
        {
            clast = cfirst;
        }
    }

    point->update();
}

void WTransferFunctionWidget::insertPointNormalized( const QPointF& position_ )
{
    insertPoint( QPointF( position_.x()*scene->width(), ( 1.-position_.y() )*scene->height() ) );
}

void WTransferFunctionWidget::insertPoint( const QPointF& position )
{
    WTransferFunctionLine *line( new WTransferFunctionLine( this ) );
    WTransferFunctionPoint *point( new WTransferFunctionPoint( this ) );
    scene->addItem( point );
    scene->addItem( line );


    // insert into list
    WTransferFunctionPoint* left( this->findPointOnLeft( position ) );
    if ( left )
    {
        WTransferFunctionPoint* right( left->getRight() );

        left->setRight( point );
        point->setLeft( left );
        point->setRight( right );

        if ( right )
        {
            right->setLeft( point );
        }

        // if we are the rightmost point
        // add the line to the new point
        // otherwise, add the line to the left point.
        // because we are the last point in the list
        if ( left->getLine() )
        {
            assert( right );

            left->getLine()->setRight( point );

            point->setLine( line );
            line->setLeft( point );
            line->setRight( right );
        }
        else
        {
            left->setLine( line );
            line->setLeft( left );
            line->setRight( point );
        }
    }
    else
    {
        // there is nothing left of su, so we are the leftmost element
        // now, add pointers to the right and we are first.
        point->setRight( first );
        if ( first )
        {
            // if there is already a point to our right, we have to add a line
            first->setLeft( point );
            line->setLeft( point );
            line->setRight( first );
        }
        else
        {
            // otherwise, we do not need the line
            delete line;
        }
        first = point;
        if ( !clast )
        {
            clast = cfirst;
        }
    }
    point->setPos( position );
    //left->update();
    //point->update();
}

void WTransferFunctionWidget::mousePressEvent( QMouseEvent *event )
{
    if ( event->button() == Qt::RightButton )
    {
        QPointF position( this->mapToScene( event->pos() ) );
        if ( position.y() < 0 )
        {
            insertColor( position, 0 );
        }
        else
        {
            insertPoint( position );
        }
        this->forceRedraw();
        event->accept();
    }
    else
    {
        BaseClass::mousePressEvent( event );
    }
}

WTransferFunctionPoint* WTransferFunctionWidget::findPointOnLeft( QPointF position )
{
    WTransferFunctionPoint *current( first );
    if ( !current || current->pos().x() > position.x() )
        return 0x0;
    while ( current && current->getRight() )
    {
        WTransferFunctionPoint *right( current->getRight() );
        if ( right->pos().x() > position.x() )
        {
            return current;
        }
        current = right;
    }
    // seems like we are larger than the largest point, so return the last point
    return current;
}

WTransferFunctionColorPoint* WTransferFunctionWidget::findCPointOnLeft( QPointF position )
{
    WTransferFunctionColorPoint *current( cfirst );
    if ( !current || current->pos().x() > position.x() )
    {
        return 0x0;
    }

    while ( current && current->getRight() )
    {
        WTransferFunctionColorPoint *right( current->getRight() );
        if ( right->pos().x() > position.x() )
        {
            return current;
        }
        current = right;
    }
    return current;
}

namespace
{
    WColor toWColor( const QColor& q )
    {
        return WColor( q.redF(), q.greenF(), q.blueF(), q.alphaF() );
    }
}

void WTransferFunctionWidget::updateTransferFunction()
{
    WTransferFunction tf;
    std::vector < double > hist( histogram->data ); //< copy data, this will be deleted
    tf.setHistogram( hist ); // get the data back because we need this for comparison

    QRectF bb = scene->sceneRect();

    WTransferFunctionColorPoint *cp( cfirst );
    while ( cp )
    {
        double iso = ( cp->pos().x() - bb.x() )/bb.width();
        tf.addColor( iso, toWColor( cp->getColor() ) );
        cp = cp->getRight();
    }

    WTransferFunctionPoint *p( first );
    while ( p )
    {
        double iso = ( p->pos().x() - bb.x() )/bb.width();
        double alpha = 1.-( ( p->pos().y() - bb.y() )/bb.height() );
        tf.addAlpha( iso, alpha );
        p = p->getRight();
    }

    // std::cout << "updating gui" << parent << std::endl;
    if ( parent )
        parent->guiUpdate( tf );
    // std::cout << "done updating gui" << std::endl;
}

