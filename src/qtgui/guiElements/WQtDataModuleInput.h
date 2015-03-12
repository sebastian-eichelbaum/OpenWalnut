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

#ifndef WQTDATAMODULEINPUT_H
#define WQTDATAMODULEINPUT_H

#include <boost/signals2/signal.hpp>

#include <QWidget>
#include <QHBoxLayout>
#include <QToolButton>

#include "WScaleLabel.h"

#include "core/kernel/WDataModuleInput.h"
#include "core/kernel/WDataModule.h"

/**
 * Widget that handles WDataModuleInputs of WDtaModules. Right now it only handles WDataModuleInputFile.
 * See #32 for details on how this will be changed.
 */
class WQtDataModuleInput: public QWidget
{
    Q_OBJECT
public:
    /**
     * Constructor.
     *
     * \param module the data module to handle
     * \param parent the parent widget
     */
    WQtDataModuleInput( WDataModule::SPtr module, QWidget* parent = NULL );

    /**
     * Destructor.
     */
    virtual ~WQtDataModuleInput();

public slots:
    /**
     * Called when the user presses the change button
     */
    void onChange();

    /**
     * Triggered for reload
     */
    void onReload();

    /**
     * Clear the source
     */
    void onClear();

protected:
private:
    /**
     * The input to manage
     */
    WDataModule::SPtr m_module;

    /**
     * Container for all
     */
    QWidget* m_container;

    /**
     * This is the main widget containing the reload button, label and the change button it is nested in the container
     */
    QWidget* m_mainWidget;

    /**
     * Layout
     */
    QHBoxLayout* m_mainLayout;

    /**
     * Layout
     */
    QVBoxLayout* m_layout;

    /**
     * The button to open the input dialog.
     */
    QToolButton* m_changeButton;

    /**
     * The reload button
     */
    QToolButton* m_reloadButton;

    /**
     * The clear button
     */
    QToolButton* m_clearButton;

    /**
     * The label
     */
    WScaleLabel* m_label;

    /**
     * Stay informed about changes in the input
     */
    boost::signals2::connection m_inputChangeConnection;

    /**
     * Called by the input change condition.
     */
    void onInputChange();

    /**
     * Called by input change condition in GUI thread.
     */
    void onInputChangeGUI();
};

#endif  // WQTDATAMODULEINPUT_H
