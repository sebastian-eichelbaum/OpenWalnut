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

#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QWidget>
#include <QtGui/QLCDNumber>

/**
 * TODO(schurade): Document this!
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
     *
     */
    QPushButton* addPushButton( std::string label = "button" );

    /**
     *
     */
    QCheckBox* addCheckBox( std::string label = "checkbox:", bool isChecked = false );

    /**
     *
     */
    QLineEdit* addLineEdit( std::string label = "edit text:", std::string text = "" );

    /**
     *
     */
    QSlider* addSliderInt( std::string label = "", int value = 0, int min = 0, int max = 100 );

    /**
     *
     */
    QString getName();

protected:
private:
    QGridLayout* m_layout;
    QString m_name;
};

#endif  // WQTDSBWIDGET_H
