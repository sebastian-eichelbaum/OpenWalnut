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

#ifndef WTRANSFERFUNCTIONWIDGET_H
#define WTRANSFERFUNCTIONWIDGET_H

//#include "WTransferFunctionPoint.hpp"
//#include "WTransferFunctionPointColor.hpp"

#include "QtGui/QGraphicsView"


class QGraphicsScene;
class WTransferFunctionPoint;
class WTransferFunctionColorPoint;
class WTransferFunctionHistogram;
class WTransferFunctionBackground;

//#include "core/common/WTransferFunction.h"
class WTransferFunction; // I want to remove this if possible to keep the code clean from OW datatypes

/**
 * The class managing the widget that wants to receive updates whenever
 * a change occurrs. This is only here to provide an interface and a better
 * separation between gui code and the transfer function code
 */
struct WTransferFunctionGuiNotificationClass
{
    virtual void guiUpdate( const WTransferFunction& ) = 0;
};

/**
 * A widget that holds a scene to display and modify the transfer function.
 *
 * Currently most data storage is done in this class, which is not a great
 * design. Thus, we should try to split the model and the view a bit more,
 * even though this is not intended by QGraphicsView.
 */
class WTransferFunctionWidget :public QGraphicsView
{
    Q_OBJECT

    public:
        typedef QGraphicsView BaseClass;

        WTransferFunctionWidget( QWidget* qparent = 0x0, WTransferFunctionGuiNotificationClass* parent=0x0 );

        virtual ~WTransferFunctionWidget();

        /**
         * called to notify the widget of achange of data
         */
        //void externalDataChange( const WTransferFunction &tf );

        /**
         * called from external to set histogram
         */
        void setHistogram( const std::vector< double > &histogram );

        /**
         * Redraw the widget
         */
        void forceRedraw();

        void setTransferFunction(); // FIXME

        /** set the current active point => this should be changed to QGraphicsScene
         * object selection */
        void setCurrent( WTransferFunctionPoint* current )
        { this->current = current; this->ccurrent = 0x0; }

        /** similart to setCurrent but for Color control points */
        void setCurrentColor( WTransferFunctionColorPoint* ccurrent )
        { this->ccurrent = ccurrent; this->current = 0x0; }

        /** sample the transfer function into a 1D RGBA texture */
        void sample1DWTransferFunction( unsigned char*array, int width );

        /** sample the transfer function into a 1D RGBA, ABGR, ARGB, ... whatever...
          This function should work on the graphics native system, but I did not find the
          function to get information about the alignment, yet.
          (X11 Linux, little endian: ABRG. OSX: ARGB, X11 on OSX from Linux host->???)
          */
        void sample1DWTransferFunctionForDisplay( unsigned char*array, int width );

        //void dropEvent( QGraphicsSceneDragDropEvent* event );
        //void dragEnterEvent( QGraphicsSceneDragDropEvent* event );

        /**
         * Insert a new alpha control point at scene position position.
         */
        void insertPoint( const QPointF& position );

        /**
         * same as insertPoint but in normalized coordinates and not in screen space
         */
        void insertPointNormalized( const QPointF& position );


        /**
         * Insert a new color control point at scene position pos (only the x-value is relevant)
         * If a color is provided, it is used. Otherwise, a new color is used interpolating the
         * current color map.
         */
        void insertColor( const QPointF& pos, QColor const *const color = 0);

        /**
         * Same as insertColor but in normalized coordinates, i.e., [ 0...1 ] along x
         */
        void insertColorNormalized( const QPointF& pos, QColor const *const color = 0);

        /**
         * Remove all points from the transfer function widget to be able to insert new points
         */
        void clearTransferFunction();

    public slots:

        /**
         * Notification that the data changed, i.e., a control point has been moved or a color changed
         * */
        void dataChanged();

    protected:
        virtual void drawBackground( QPainter *painter, const QRectF &rect );

        /**
         * Interactions implemented so far:
         * right click: insert new object (Ctrl+left click or two-finger tap on OSX)
         * left click on objects: (handled by individual objects) move control points and select
         *     object
         * double click on objects: (handled by individual objects) open parameter dialog, e.g.,
         *     to change the color
         * "Delete" or "Backspace": delete selected itom
         */
        virtual void keyPressEvent( QKeyEvent *event );

        /**
         * for a documentation of the implemented actions confer the keyPressEvent documentation
         */
        virtual void mousePressEvent( QMouseEvent *event );

        /**
         * internal helper function: Find the point to the left of the given point
         */
        WTransferFunctionPoint* findPointOnLeft( QPointF position );

        /**
         * internal helper function: Find the point to the left of the given color control point
         */
        WTransferFunctionColorPoint* findCPointOnLeft( QPointF position );

        void updateTransferFunction();

        //void drawLineStrip( QPainter *painter );

        /**
         * internal helper function to update the QGraphicsPixmapItem that holds a representation
         * of the current color map and displays it as a background of the widget.
         * This function samples the texture into a QPixmap and updates the QGraphicsPixmapItem.
         */
        void setMyBackground();

    private:
        WTransferFunctionGuiNotificationClass *parent;
        QGraphicsScene *scene;

        WTransferFunctionPoint *first;
        WTransferFunctionPoint *last;
        WTransferFunctionPoint *current;

        WTransferFunctionColorPoint *cfirst;
        WTransferFunctionColorPoint *clast;
        WTransferFunctionColorPoint *ccurrent;

        WTransferFunctionBackground *background; //< background that displays the color map
        WTransferFunctionHistogram *histogram;   //< item responsible for displaying histogram data

        bool initialized;
};

#endif
