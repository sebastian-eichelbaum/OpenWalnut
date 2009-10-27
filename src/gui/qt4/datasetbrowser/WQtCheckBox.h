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

#ifndef WQTCHECKBOX_H
#define WQTCHECKBOX_H

#include <string>

#include <QtGui/QCheckBox>

/**
 * implements a QCheckBox with a boost signal
 */
class WQtCheckBox : public QCheckBox
{
    Q_OBJECT

public:
     /**
     * standard constructor
     */
    explicit WQtCheckBox();

    /**
     * destructor
     */
    virtual ~WQtCheckBox();

    /**
     * setter for name
     */
    void setName( QString name );

private:
    QString m_name;



public slots:
    void emitStateChanged();

signals:
    void checkBoxStateChanged( QString name, bool value );
};

#endif  // WQTCHECKBOX_H
