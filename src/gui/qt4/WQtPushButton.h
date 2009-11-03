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

#ifndef WQTPUSHBUTTON_H
#define WQTPUSHBUTTON_H

#include <string>

#include <QtGui/QPushButton>

/**
 * implements a QPushButton with a boost signal
 */
class WQtPushButton : public QPushButton
{
    Q_OBJECT

public:
     /**
     * standard constructor
     */
    explicit WQtPushButton( QIcon icon, QString label );

    /**
     * destructor
     */
    virtual ~WQtPushButton();

    /**
     * setter for name
     */
    void setName( QString name );

private:
    QString m_name;



public slots:
    void emitPressed();

signals:
    void pushButtonPressed( QString name, bool value );
    void pushButtonToggled( QString name, bool value );
};

#endif  // WQTPUSHBUTTON_H
