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

#include <QtGui/QWidget>
#include <QtGui/QDialog>
#include <QtCore/QString>
#include <QtCore/QTimer>

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
    enum MessageType
    {
        ERROR,          //!< error messgae
        WARNING,        //!< warning
        INFO            //!< info only
    };

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

public slots:
    /**
     * Shows the message
     */
    void showMessage();
protected:
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
};

#endif  // WQTMESSAGEPOPUP_H

