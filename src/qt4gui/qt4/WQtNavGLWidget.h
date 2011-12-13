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

#include <map>
#include <string>

#include <QtGui/QDockWidget>
#include <QtGui/QSlider>

#include "core/common/WPropertyVariable.h"
#include "core/graphicsEngine/WGEGroupNode.h"
#include "controlPanel/WPropertyDoubleWidget.h"

#include "WQtGLDockWidget.h"

/**
 * container widget to hold as GL widget and a slider
 */
class WQtNavGLWidget : public WQtGLDockWidget
{
    Q_OBJECT

public:
    /**
     * default constructor
     *
     * \param viewTitle Title will be used as view title
     * \param dockTitle The title of the dock widget.
     * \param parent The widget that manages this widget
     * \param sliderTitle Name of the slider corresponding to the property it manipulates
     * \param shareWidget this widget will share OpenGL display lists and texture objects with shareWidget
     */
    WQtNavGLWidget( QString viewTitle, QString dockTitle, QWidget* parent, std::string sliderTitle="pos", const QWidget * shareWidget = 0 );

    /**
     * destructor.
     */
    virtual ~WQtNavGLWidget();

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
    void setSliderProperty( boost::shared_ptr< WPropertyBase > prop );

    /**
     * Remove the property to control by the slider.
     *
     * \param prop the property
     */
    void removeSliderProperty( boost::shared_ptr< WPropertyBase > prop );

protected:
private:
    /**
     * The slider's title.
     */
    QString m_sliderTitle;

    /**
     * the scene which is displayed by the GL widget
     */
    osg::ref_ptr< WGEGroupNode > m_scene;

    /**
     * Update slider if property changed.
     */
    void handleChangedPropertyValue();

    /**
     * Map holding the widgets for module properties added automatically. So they can be removed again automatically
     * if the module is removed.
     */
    std::map< boost::shared_ptr< WPropertyBase >, WPropertyDoubleWidget* > propertyWidgetMap;
};

#endif  // WQTNAVGLWIDGET_H
