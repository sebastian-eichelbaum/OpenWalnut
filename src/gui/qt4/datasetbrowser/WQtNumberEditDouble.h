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

#ifndef WQTNUMBEREDITDOUBLE_H
#define WQTNUMBEREDITDOUBLE_H

#include <string>

#include <QtGui/QLineEdit>

/**
 * a QLineEdit modified to deal only with double numbers
 */
class WQtNumberEditDouble : public QLineEdit
{
    Q_OBJECT

public:
    /**
     * Default constructor, that connects the slot of the edit.
     *
     * \param name The name of teh widget
     * \param parent The widget managing this widget
     */
    explicit WQtNumberEditDouble( QString name, QWidget* parent = 0 );

    /**
     * setter for name.
     *
     * \param name The new name for the widget
     */
    void setName( QString name );

    /**
     * destructor
     */
    virtual ~WQtNumberEditDouble();

public slots:

    /**
     * Sets the edit field to a certain number.
     *
     * \param number The double to be put into the field.
     */
    void setDouble( double number );

    /**
     * Updated the number in the lineEdit if the number has changed.
     */
    void numberChanged();

signals:

    /**
     * Signal to distribute the number set in the lineEdit.
     *
     * \param name name of the edit field.
     * \param number new value of the edit field.
     */
    void signalNumberWithName( QString name, double number );

protected:
private:
    /**
     * Name if the value.
     */
    QString m_name;
};

#endif  // WQTNUMBEREDITDOUBLE_H
