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

#include <vector>

#include "QGraphicsView"


class QGraphicsScene;
class WTransferFunctionPoint;
class WTransferFunctionColorPoint;
class WTransferFunctionHistogram;
class WTransferFunctionBackground;

class WTransferFunction; // I want to remove this if possible to keep the code clean from OW datatypes

/**
 * The class managing the widget that wants to receive updates whenever
 * a change occurrs. This is only here to provide an interface and a better
 * separation between gui code and the transfer function code
 */
struct WTransferFunctionGuiNotificationClass
{
    /**
     * default destructor
     */
    virtual ~WTransferFunctionGuiNotificationClass()
    {
    }

    /**
     * update the gui
     * \param tf the new transfer function
     */
    virtual void guiUpdate( const WTransferFunction& tf ) = 0;
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
    /** type of our base class for easier coding */
    typedef QGraphicsView BaseClass;

    /**
     * Constructor. Create a TF widget with a given parent.
     *
     * \param qparent parent widgeet
     * \param parent  a class that receives notifications
     */
    WTransferFunctionWidget( QWidget* qparent = NULL, WTransferFunctionGuiNotificationClass* parent = NULL );

    /**
     * Destructor.
     */
    virtual ~WTransferFunctionWidget();

    /**
     * Called from external to set histogram.
     *
     * \param histogram The histogram data
     */
    void setHistogram( const std::vector< double > &histogram );

    /**
     * Redraw the widget
     */
    void forceRedraw();

    /**
     * Set the current active point => this should be changed to QGraphicsScene
     * object selection.
     *
     * \param current the new selection
     */
    void setCurrent( WTransferFunctionPoint* current )
    {
        this->current = current;
        this->ccurrent = 0x0;
    }

    /**
     * Similar to setCurrent but for Color control points.
     *
     * \param ccurrent the new seelection
     */
    void setCurrentColor( WTransferFunctionColorPoint* ccurrent )
    {
        this->ccurrent = ccurrent;
        this->current = 0x0;
    }

    /**
     * Sample the transfer function into a 1D RGBA texture.
     *
     * \param array target byte array of RGBA values
     * \param width width of array in number of color values
     */
    void sample1DTransferFunction( unsigned char*array, int width );

    /**
     * Sample the transfer function into a 1D RGBA, ABGR, ARGB, ... whatever...
     * This function should work on the graphics native system, but I did not find the
     * function to get information about the alignment, yet.
     * (X11 Linux, little endian: ABRG. OSX: ARGB, X11 on OSX from Linux host->???)
     *
     * \param array target byte array of RGBA values
     * \param width width of array in number of color values
     */
    void sample1DTransferFunctionForDisplay( unsigned char*array, int width );

    /**
     * Insert a new alpha control point at scene position position.
     * \param position the position of the point with position.x = iso and position.y = alpha in window coordinates.
     */
    void insertPoint( const QPointF& position );

    /**
     * Same as insertPoint but in normalized coordinates and not in screen space.
     *
     * \param position: same as in insertPoint, but scaled to [ 0..1 ]x[ 0..1 ]
     */
    void insertPointNormalized( const QPointF& position );


    /**
     * Insert a new color control point at scene position pos (only the x-value is relevant)
     * If a color is provided, it is used. Otherwise, a new color is used interpolating the
     * current color map.
     *
     * \param pos: the clicked position where pos.x = iso and pos.y is ignored
     * \param color: the color of the new point. If color = 0, a new color is created by interpolating the neighbors.
     */
    void insertColor( const QPointF& pos, QColor const* const color = 0 );

    /**
     * Same as insertColor but in normalized coordinates, i.e., [ 0...1 ] along x.
     *
     * \param pos the position
     * \param color the new color ( my be 0 and will then be interpolated linearly )
     */
    void insertColorNormalized( const QPointF& pos, QColor const *const color = 0 );

    /**
     * Remove all points from the transfer function widget to be able to insert new points.
     */
    void clearTransferFunction();

public slots:

    /**
     * Notification that the data changed, i.e., a control point has been moved or a color changed.
     */
    void dataChanged();

protected:
    /**
     * Draws the background.
     *
     * \param painter the painter to use
     * \param rect the QRectF to repaint
     */
    virtual void drawBackground( QPainter *painter, const QRectF &rect );

    /**
     * Interactions implemented so far:
     * right click: insert new object (Ctrl+left click or two-finger tap on OSX)
     * left click on objects: (handled by individual objects) move control points and select
     *     object
     * double click on objects: (handled by individual objects) open parameter dialog, e.g.,
     *     to change the color
     * "Delete" or "Backspace": delete selected itom
     * \param event the event to handle
     */
    virtual void keyPressEvent( QKeyEvent *event );

    /**
     * For a documentation of the implemented actions confer the keyPressEvent documentation.
     * \param event the event to handle
     */
    virtual void mousePressEvent( QMouseEvent *event );

    /**
     * Internal helper function: Find the point to the left of the given point.
     *
     * \param position in the widget
     * \returns an alpha point left of the given position, i.e., with return.x <  x_position
     */
    WTransferFunctionPoint* findPointOnLeft( QPointF position );

    /**
     * Internal helper function: Find the point to the left of the given color control point.
     *
     * \param position a position
     * \returns a color point left of the given position, i.e., with return.x <  x_position
     */
    WTransferFunctionColorPoint* findCPointOnLeft( QPointF position );

    /**
     * Updates the transfer function.
     */
    void updateTransferFunction();

    /**
     * Internal helper function to update the QGraphicsPixmapItem that holds a representation
     * of the current color map and displays it as a background of the widget.
     * This function samples the texture into a QPixmap and updates the QGraphicsPixmapItem.
     */
    void setMyBackground();

private:
    /** the class that receives our update notifications */
    WTransferFunctionGuiNotificationClass *parent;

    /** our scene */
    QGraphicsScene *scene;

    /** linked list of alpha items */
    WTransferFunctionPoint *first;           //! first element
    WTransferFunctionPoint *last;            //! last element in list
    WTransferFunctionPoint *current;         //! currently selected/active element

    /** linked list of color items */
    WTransferFunctionColorPoint *cfirst;     //! first element
    WTransferFunctionColorPoint *clast;      //! last element
    WTransferFunctionColorPoint *ccurrent;   //! currently selected/active color element

    WTransferFunctionBackground *background; //! background that displays the color map
    WTransferFunctionHistogram *histogram;   //! item responsible for displaying histogram data

    bool initialized; //< set to true after initialization
};

#endif  // WTRANSFERFUNCTIONWIDGET_H
