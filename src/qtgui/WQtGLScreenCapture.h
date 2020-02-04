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

#ifndef WQTGLSCREENCAPTURE_H
#define WQTGLSCREENCAPTURE_H

#include <limits>

#include <boost/signals2.hpp>

#include <QWidget>
#include <QToolBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>

#include <osg/Camera>
#include <osg/Image>
#include <osg/RenderInfo>

#include "core/graphicsEngine/WGEViewer.h"

#include "WIconManager.h"

class WQtGLDockWidget;

/**
 * This class is a screen recorder adapter in QT. It uses WGEScreenCapture and provides a nice widget around it.
 */
class WQtGLScreenCapture: public QWidget
{
    Q_OBJECT
public:
    /**
     * Creates screen capture gui for the specified capture callback of a certain view.
     *
     * \param parent the parent
     */
    explicit WQtGLScreenCapture( WQtGLDockWidget* parent );

    /**
     * Destructor. Cleans up.
     */
    virtual ~WQtGLScreenCapture();

    /**
     * Returns the trigger used for screenshotting.
     *
     * \return the action.
     */
    QAction* getScreenshotTrigger() const;

    /**
     * Save settings.
     */
    virtual void saveSettings();

    /**
     * Restore settings
     */
    virtual void restoreSettings();

protected:
    /**
     * Custom event dispatcher. Gets called by QT's Event system every time an event got sent to this widget. This event handler
     * processes several custom events, like WModuleAssocEvent.
     *
     * \note QT Doc says: use event() for custom events.
     * \param event the event that got transmitted.
     *
     * \return true if the event got handled properly.
     */
    virtual bool event( QEvent* event );

private slots:

    /**
     * Initiates taking a screenshot
     */
    void screenShot();

    /**
     * Slot triggered by m_moveRecButton
     */
    void startRec();

    /**
     * Slot triggered by m_moveStopButton
     */
    void stopRec();

    /**
     * Slot triggered by m_configFrameResetButton
     */
    void resetFrames();

    /**
     * Someone changed the open tab in the toolbox.
     *
     * \param index the new index
     */
    void toolBoxChanged( int index );

    /**
     * Changes resolution of gl widget.
     *
     * \param force if true, the resolution is set. If not, it gets restored.
     */
    void resolutionChange( bool force );

private:
    /**
     * My parent.
     */
    WQtGLDockWidget* m_glDockWidget;

    /**
     * The actual screen capture instance.
     */
    WGEViewer::SPtr m_viewer;

    /**
     * Called by the screencapture callback to notify this widget about recording
     */
    void recCallback();

    /**
     * The function handles new images. Implement it.
     *
     * \param framesLeft how much frames to come
     * \param totalFrames the total number of frames until now
     * \param image the image
     */
    void handleImage( size_t framesLeft, size_t totalFrames, osg::ref_ptr< osg::Image > image ) const;

    /**
     * Recording - callback connection
     */
    boost::signals2::connection m_recordConnection;

    /**
     * New image incoming - callback connection to handleImage.
     */
    boost::signals2::connection m_imageConnection;

    /**
     * The toolbox containing all the stuff
     */
    QToolBox* m_toolbox;

    /**
     * The configuration widget. It contains the common config stuff
     */
    QWidget* m_configWidget;

    /**
     * The filename for the screenshot
     */
    QLineEdit* m_configFileEdit;

    /**
     * Shows recorded frames.
     */
    QLabel* m_configFrameLabel;

    /**
     * Triggers frame counter reset.
     */
    QPushButton* m_configFrameResetButton;

    /**
     * Widget containing all the widgets to control movie recording
     */
    QWidget* m_screenshotWidget;

    /**
     * The trigger which triggers the screenshot.
     */
    QPushButton* m_screenshotButton;

    /**
     * This action triggers a screenshot.
     */
    QAction* m_screenshotAction;

    /**
     * Shows recorded time in movie-time.
     */
    QLabel* m_movieTimeLabel;

    /**
     * Widget containing all the widgets to control movie recording
     */
    QWidget* m_movieWidget;

    QPushButton* m_movieRecButton;  //!< record button
    QPushButton* m_movieStopButton;  //!< stop button

    /**
     * Stores the resolution the user wants to have.
     */
    QComboBox* m_resolutionCombo;

    QPushButton* m_resolutionButton;

    /**
     * Widget for entering a custom resolution width
     */
    QLineEdit* m_customWidth;

    /**
     * Widget for entering a custom resolution height
     */
    QLineEdit* m_customHeight;
};

#endif  // WQTGLSCREENCAPTURE_H
