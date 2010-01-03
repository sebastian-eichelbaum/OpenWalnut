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
#include "WQtNumberEditDouble.h"
#include "WQtSliderWithEdit.h"


/**
 * container widget to contain the context widget for the dataset browser
 */
class WQtDSBWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * Creates new data set browser widget
     * \param name Name of the widget
     * \param parent The widget managing this widget
     */
    explicit WQtDSBWidget( std::string name, QWidget* parent = 0 );

    /**
     * destructor
     */
    virtual ~WQtDSBWidget();

    /**
     * helper function to add a generic button
     * \param label Text on the push button
     */
    QPushButton* addPushButton( QString label );

    /**
     * helper function to add a generic checkbox
     * \param label The text next to the check box
     * \param isChecked the initial state of the check box
     */
    WQtCheckBox* addCheckBox( QString label, bool isChecked = false );

    /**
     * helper function to add a generic line edit
     * \param label The text next to the line edit
     * \param text The text intially in the line edit
     */
    WQtLineEdit* addLineEdit( QString label, QString text = QString( "" ) );

    /**
     * helper function to add a double number edit control
     * \param label name of the control
     * \param value value of the control
     */
    WQtNumberEditDouble* addNumberEditDouble( QString label, double value );

    /**
     * helper function to add a slider with a connect line edit
     * \param label The text next to the slider and the edit field
     * \param value Initial value of slider and field
     * \param min Minimum value for slider
     * \param max Maximum value for slider
     */
    WQtSliderWithEdit* addSliderInt( QString label, int value = 0, int min = 0, int max = 100 );

    /**
     * helper function to add a spacer at the end
     */
    void addSpacer();



    /**
     * getter for m_name
     */
    QString getName();

protected:
private:
    QString m_name;
    QGridLayout m_controlLayout;
    QVBoxLayout m_pageLayout;
};

#endif  // WQTDSBWIDGET_H
