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

#ifndef WQTLINEEDIT_H
#define WQTLINEEDIT_H

#include <string>

#include <QtGui/QLineEdit>

/**
 * implements a QLineEdit with a boost signal
 */
class WQtLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    /**
     * standard constructor
     */
    WQtLineEdit();

    /**
     * destructor
     */
    virtual ~WQtLineEdit();

    /**
     * Setter for name.
     *
     * \param name the new name for the line edit
     */
    void setName( QString name );

private:

    /**
     * Name of the edit and its value.
     */
    QString m_name;

public slots:

    /**
     * Value of the edit field changed.
     */
    void emitStateChanged();

signals:

    /**
     * Signal a state change in the line edit.
     *
     * \param name name of the line edit.
     * \param text new text inside the edit field.
     */
    void lineEditStateChanged( QString name, QString text );
};

#endif  // WQTLINEEDIT_H
