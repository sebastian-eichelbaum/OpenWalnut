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

#ifndef WQTDSBWIDGET_H
#define WQTDSBWIDGET_H

#include <string>

#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QWidget>
#include <QtGui/QLCDNumber>

#include "WQtCheckBox.h"
#include "WQtLineEdit.h"
#include "WQtSliderWithEdit.h"


/**
 * container widget to contain the context widget for the dataset browser
 */
class WQtDSBWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * default constructor
     */
    explicit WQtDSBWidget( std::string name, QWidget* parent = 0 );

    /**
     * destructor
     */
    virtual ~WQtDSBWidget();

    /**
     * helper function to add a generic button
     */
    QPushButton* addPushButton( std::string label = "button" );

    /**
     * helper function to add a generic checkbox
     */
    WQtCheckBox* addCheckBox( std::string label = "checkbox:", bool isChecked = false );

    /**
     * helper function to add a generic line edit
     */
    WQtLineEdit* addLineEdit( std::string label = "edit text:", std::string text = "" );

    /**
     * helper function to add a slider with a connect line edit
     */
    WQtSliderWithEdit* addSliderInt( std::string label = "", int value = 0, int min = 0, int max = 100 );

    /**
     * getter for m_name
     */
    QString getName();

protected:
private:
    QString m_name;
    QGridLayout m_layout;
};

#endif  // WQTDSBWIDGET_H
