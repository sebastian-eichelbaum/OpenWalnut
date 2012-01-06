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

#ifndef WTRANSFERFUNCTIONHISTOGRAM_H
#define WTRANSFERFUNCTIONHISTOGRAM_H

#include <vector>
#include "QtGui/QGraphicsItem"

class WTransferFunctionWidget;

/**
 * Display a semi-transparent line graph as the histogram of the current data set.
 */
class WTransferFunctionHistogram : public QGraphicsItem
{
    public:
        /** type of the base class */
        typedef QGraphicsItem BaseClass;

        /**
         * default constructor
         * \param parent the parent widget
         */
        explicit WTransferFunctionHistogram( WTransferFunctionWidget* parent = 0x0 );

        /**
         * default destructor
         */
        virtual ~WTransferFunctionHistogram();

        /** \returns the bounding rectange */
        QRectF boundingRect() const;

        /**
         * Paint a semi-transparent histogram on top of the transfer function but below the
         * input handles
         */
        virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget* );

        /** the histogram data */
        std::vector< double > data;
};

#endif  // WTRANSFERFUNCTIONHISTOGRAM_H

