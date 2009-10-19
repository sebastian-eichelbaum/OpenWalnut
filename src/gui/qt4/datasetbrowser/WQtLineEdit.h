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

#include "../signalslib.hpp"
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
    explicit WQtLineEdit();

    /**
     * destructor
     */
    virtual ~WQtLineEdit();

    /**
     * \return the boost signal object
     */
    boost::signal2< void, std::string, std::string >* getboostSignal();

    /**
     * setter for name
     */
    void setName( std::string name );

private:
    std::string m_name;

    boost::signal2< void, std::string, std::string > m_boostSignal;

public slots:
    void emitStateChanged();
};

#endif  // WQTLINEEDIT_H
