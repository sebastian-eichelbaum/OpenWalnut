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

#ifndef WQTMESSAGEDIALOG_H
#define WQTMESSAGEDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QLabel>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QtCore/QSettings>

/**
 * General purpose message dialog, able to show arbitrary content. It additionally allows the user to decide whether he wants to see this again
 * or not.
 */
class WQtMessageDialog: public QDialog
{
    Q_OBJECT
public:
    /**
     * Construct a message dialog.
     *
     * \param msgID the id used to check whether to show the message or not.
     * \param title the title of the dialog
     * \param content the widget representing the content
     * \param settings the object storing the info about disabled messages
     * \param parent parent window
     */
    WQtMessageDialog( QString msgID, QString title, QWidget* content, QSettings& settings, QWidget* parent ); // NOLINT - yes use a non-const ref

    /**
     * Destructor.
     */
    virtual ~WQtMessageDialog();

    /**
     * Shows the dialog if the user does not disabled it.
     */
    virtual void show();

    /**
     * Force showing the dialog.
     *
     * \param force true if forced to show (even if the user marked the do not show again flag).
     */
    virtual void show( bool force );

protected:
private slots:

    /**
     * Dialog closed
     */
    virtual void reject();

    /**
     * Dialog closed
     */
    virtual void accept();

private:
    /**
     * Dialog buttons.
     */
    QDialogButtonBox* m_buttonBox;

    /**
     * Do not show again - Checkbox
     */
    QCheckBox* m_checkBox;

    /**
     * The widget showing the content
     */
    QWidget* m_content;

    /**
     * The message ID
     */
    QString m_msgID;

    /**
     * Settings object of the main window
     */
    QSettings& m_settings;

    /**
     * Handles close and saves setting
     */
    void handleClose();
};

#endif  // WQTMESSAGEDIALOG_H

