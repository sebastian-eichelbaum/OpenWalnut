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

#ifndef WQTMESSAGEPOPUP_H
#define WQTMESSAGEPOPUP_H

#include <QWidget>
#include <QDialog>
#include <QtCore/QString>
#include <QtCore/QTimer>

#include "core/common/WLogEntry.h"

/**
 * Nice looking message popup.
 */
class WQtMessagePopup: public QDialog
{
    Q_OBJECT
public:
    /**
     * Describe the kind of message
     */
    typedef LogLevel MessageType;

    /**
     * Constructor
     *
     * \param parent parent widget
     * \param title title
     * \param message message
     * \param type the message type
     */
    WQtMessagePopup( QWidget* parent, const QString& title, const QString& message, MessageType type );

    /**
     * Destructor.
     */
    virtual ~WQtMessagePopup();

    /**
     * When set to true, the widget gets closed when loosing focus or when clicking the detail button. If not, the widget needs to be closed
     * manually.
     *
     * \param autoClose close flag
     */
    void setAutoClose( bool autoClose = true );

    /**
     * Show or hide the close button
     *
     * \param showCloseButton true to show
     */
    void setShowCloseButton( bool showCloseButton = true );

    /**
     * If true, the widget moves itself to the bottom of its parent widget. As this is not useful sometimes, you can disable this by using
     * false here.
     *
     * \param autoPosition false if widget should not move automatically
     */
    void setAutoPosition( bool autoPosition = true );

    /**
     * Get this popups message type.
     *
     * \return the type
     */
    MessageType getType() const;

signals:
    /**
     * Called when closing the popup
     *
     * \param me the pointer to the widget getting close
     */
    void onClose( WQtMessagePopup* me );

public slots:
    /**
     * Shows the message
     */
    void showMessage();

    /**
     * Cloes this popup. Use instead of close() slot.
     */
    void closePopup();
protected:
    /**
     * On show. Called when opening the widget.
     *
     * \param event
     */
    virtual void showEvent( QShowEvent* event );
private:
    /**
     * Title Text
     */
    QString m_title;

    /**
     * The message text.
     */
    QString m_message;

    /**
     * What kind of message is this.
     */
    MessageType m_type;

    /**
     * Close button
     */
    QPushButton* m_closeBtn;

    /**
     * See \ref setAutoClose.
     */
    bool m_autoClose;

    /**
     * See \ref setAutoPosition.
     */
    bool m_autoMove;
};

#endif  // WQTMESSAGEPOPUP_H

