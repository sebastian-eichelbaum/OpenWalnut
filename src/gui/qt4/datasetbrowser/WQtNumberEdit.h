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

#ifndef WQTNUMBEREDIT_H
#define WQTNUMBEREDIT_H

#include <string>

#include <QtGui/QLineEdit>

/**
 * a QLineEdit modified to deal only with integer numbers
 */
class WQtNumberEdit : public QLineEdit
{
    Q_OBJECT

public:
    /**
     * Default constructor
     *
     * \param parent The parent widget of this widget, i.e. the widget that manages it.
     */
    explicit WQtNumberEdit( QWidget* parent = 0 );

    /**
     * destructor
     */
    virtual ~WQtNumberEdit();

public slots:

    /**
     * Set the value of the number edit.
     *
     * \param number the new number of the number edit.
     */
    void setInt( int number );

    /**
     * Slot getting called whenever the value of the edit changed.
     */
    void numberChanged();

signals:

    /**
     * Signals a new value of the edit
     *
     * \param value the new value.
     */
    void signalNumber( int value );

protected:
private:
};

#endif  // WQTNUMBEREDIT_H
