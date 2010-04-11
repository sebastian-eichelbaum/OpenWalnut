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

#ifndef WQTNAVGLWIDGET_H
#define WQTNAVGLWIDGET_H

#include <string>

#include <QtGui/QDockWidget>
#include <QtGui/QSlider>

#include "../../common/WPropertyVariable.h"
#include "../../graphicsEngine/WGEGroupNode.h"

#include "WQtGLWidget.h"

/**
 * container widget to hold as GL widget and a slider
 */
class WQtNavGLWidget : public QDockWidget
{
    Q_OBJECT

public:
    /**
     * default constructor
     *
     * \param title Title will be displayed above the widget
     * \param parent The widget that manages this widget
     * \param maxValue Maximum value for the corresponding nav slice slider
     * \param sliderTitle Name of the slider corresponding to the property it manipulates
     */
    explicit WQtNavGLWidget( QString title, QWidget* parent, int maxValue = 100, std::string sliderTitle="pos" );

    /**
     * destructor.
     */
    virtual ~WQtNavGLWidget();

    /**
     * Gets the contained GL widget instance.
     *
     * \return pointer to GL widget
     */
    boost::shared_ptr< WQtGLWidget > getGLWidget();

    /**
     * Set the title of the slider used in this nav widget
     *
     * \param title the title
     */
    void setSliderTitle( std::string title );

    /**
     * Sets the property to control by the slider.
     *
     * \param prop the property
     */
    void setSliderProperty( WPropInt prop );

protected:

    /**
     * Event handler for close events.
     *
     * \param event the event description.
     */
    virtual void closeEvent( QCloseEvent* event );

    /**
     * Custom event dispatcher. Gets called by QT's Event system every time an event got sent to this widget. This event handler
     * processes property change events.
     *
     * \note QT Doc says: use event() for custom events.
     * \param event the event that got transmitted.
     *
     * \return true if the event got handled properly.
     */
    virtual bool event( QEvent* event );

private:

    /**
     * The property which is controlled by the slider
     */
    WPropInt m_sliderProp;

    /**
     * The slider's title.
     */
    QString m_sliderTitle;

    /**
     * The slider representing the property.
     * \todo this has to be replaced by a WPropertyIntWidget!
     */
    QSlider *m_slider;

    /**
     * My GL widget.
     */
    boost::shared_ptr<WQtGLWidget> m_glWidget;

    /**
     * the scene which is displayed by the GL widget
     */
    osg::ref_ptr< WGEGroupNode > m_scene;

    /**
     * Update slider if property changed.
     */
    void handleChangedPropertyValue();

private slots:

    /**
     * Slot getting called whenever the slider gets moved.
     *
     * \param value the new value of the slider.
     */
    void sliderValueChanged( int value );

signals:

    /**
     * Signals a value change event inside the slider.
     *
     * \param name name of the slider.
     * \param value value of the slider.
     */
    void navSliderValueChanged( QString name, int value );
};

#endif  // WQTNAVGLWIDGET_H
