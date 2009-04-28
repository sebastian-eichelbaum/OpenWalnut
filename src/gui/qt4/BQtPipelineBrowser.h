//---------------------------------------------------------------------------
//
// Project: BrainCognize
//
// Copyright 2009 SomeCopyrightowner
//
// This file is part of BrainCognize.
//
// BrainCognize is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// BrainCognize is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with BrainCognize. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

#ifndef BQTPIPELINEBROWSER_H
#define BQTPIPELINEBROWSER_H

#include <QtGui/QDockWidget>

/**
 * \ingroup gui
 * Represents the browser showing the loaded data sets
 * and applied filter/methods/algorithms/plugins
 */
class BQtPipelineBrowser: public QDockWidget
{
    public:
        BQtPipelineBrowser();
        virtual ~BQtPipelineBrowser();

        /**
         * returns the recommended size for the widget to allow
         * parent widgets to give it a proper initial layout
         */
        QSize sizeHint() const;

    private:
        /**
         * Holds the recommended size for the widget
         */
        QSize m_recommendedSize;
};

#endif  // BQTPIPELINEBROWSER_H
