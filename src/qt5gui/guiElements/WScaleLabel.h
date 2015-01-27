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

#include <QLabel>
#include <QtCore/QString>

#include "../WGuiConsts.h"

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
     * \param length the minimal number of characters visible.
     */
    WScaleLabel( size_t length = WPREFERRED_LABEL_LENGTH, QWidget* parent = NULL );

    /**
     * Constructor. Creates the label with its original text
     *
     * \param text text of the label
     * \param parent the widgets parent
     * \param length the minimal number of characters visible.
     */
    WScaleLabel( const QString &text, size_t length = WPREFERRED_LABEL_LENGTH, QWidget* parent = NULL );

    /**
     * Constructor
     *
     * \param parent the widgets parent
     */
    explicit WScaleLabel( QWidget* parent );

    /**
     * Constructor. Creates the label with its original text
     *
     * \param text text of the label
     * \param parent the widgets parent
     */
    WScaleLabel( const QString &text, QWidget* parent );

    /**
     * overwritten from QLabel, returning the widgets prefered size
     *
     * \return prefered size of the label
     */
    virtual QSize sizeHint() const;

    /**
    * overwritten from QLabel, returning the widgets prefered size
    *
    * \return minimum size of the label
    */
    virtual QSize minimumSizeHint() const;

    /**
     * reimplemented function to setText
     *
     * \param text text of the label
     */
    virtual void setText( const QString &text );

    /**
     * Set this to reserve extra space for a margin. This function does not set the margin. This is still your task, using stylesheets. You will
     * not need this when using QLabel::setMargin().
     *
     * \param margin the margin to keep in mind for size calculations
     */
    virtual void addAdditionalWidth( int margin );

    /**
     * How many characters should be visible all the time?
     *
     * \param chars the number of chars
     */
    virtual void setMinimalLength( size_t chars );

    /**
     * Get the current minimal number of characters
     *
     * \return the number of chars visible all the time
     */
    virtual size_t getMinimalLength() const;

    /**
     * Calculate the size that is needed for the given number of chars.
     *
     * \param chars number of chars
     *
     * \return the size in pixels
     */
    virtual size_t calculateSize( size_t chars ) const;
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
     * Deferred construction.
     */
    void construct();

    /**
     * set the actual text which is shown on the QLabel
     */
    void fitTextToSize();

    /**
     * QString to remember the original unshortend text of the widget
     */
    QString m_orgText;

    /**
     * The additional width we need to reserver (like for margins).
     */
    int m_additionalWidth;

    /**
     * Minimal character number
     */
    size_t m_minLength;
};

#endif  // WSCALELABEL_H
