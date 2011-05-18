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

#ifndef WSCALELABEL_H
#define WSCALELABEL_H

#include <QtGui/QLabel>
#include <QtCore/QString>

/**
 * Special Label that can shrink and expand in a layout
 */
class WScaleLabel: public QLabel
{
    Q_OBJECT
public:
    /**
     * Constructor
     *
     * \param parent the widgets parent
     */
    explicit WScaleLabel( QWidget *parent = NULL );

    /**
     * Constructor. Creates the label with its original text
     *
     * \param text text of the label
     * \param parent the widgets parent
     */
    WScaleLabel( const QString &text, QWidget *parent = NULL );

    /**
     * overwritten from QLabel, returning the widgets prefered size
     *
     * \return prefered size of the label
     */
    QSize sizeHint() const;

    /**
    * overwritten from QLabel, returning the widgets prefered size
    *
    * \return minimum size of the label
    */
    QSize minimumSizeHint() const;

    /**
     * reimplemented function to setText
     *
     * \param text text of the label
     */
    void setText( const QString &text );
protected:
    /**
     * custom implementation of the resize event
     * to fit the QString into the labels current size
     *
     * \param event resize event passed from the parent widgets event handling
     */
    virtual void resizeEvent( QResizeEvent * event );
private:
    /**
     * set the actual text which is shown on the QLabel
     */
    void fitTextToSize();

    /**
     * QString to remember the original unshortend text of the widget
     */
    QString m_orgText;
};

#endif  // WSCALELABEL_H
