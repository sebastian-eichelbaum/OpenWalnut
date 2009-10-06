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

#ifndef WQTSLIDERWITHEDIT_H
#define WQTSLIDERWITHEDIT_H

#include "signalslib.hpp"

#include "WQtNumberEdit.h"
#include <QtGui/QSlider>
#include <QtGui/QHBoxLayout>

/**
 * TODO(schurade): Document this!
 */
class WQtSliderWithEdit : public QWidget
{
public:
    explicit WQtSliderWithEdit( QWidget* parent = 0 );
    virtual ~WQtSliderWithEdit();

    /**
     *
     */
    boost::signal1< void, int >* getboostSignalObject();

    void setMin( int min );
    void setMax( int max );
    void setValue( int value );


protected:
private:
    QSlider m_slider;
    WQtNumberEdit m_edit;
    QHBoxLayout m_layout;
};

#endif  // WQTSLIDERWITHEDIT_H
