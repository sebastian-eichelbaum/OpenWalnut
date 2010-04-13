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

#ifndef WQTAPPLYMODULEPUSHBUTTON_H
#define WQTAPPLYMODULEPUSHBUTTON_H

#include <string>

#include <boost/shared_ptr.hpp>

#include <QtGui/QPushButton>

#include "../WIconManager.h"
#include "../../../kernel/combiner/WApplyPrototypeCombiner.h"

/**
 * Implements a QPushButton with a boost signal for convenience.
 */
class WQtApplyModulePushButton : public QPushButton
{
    Q_OBJECT

public:

    /**
     * Constructor creating a module application button.
     *
     * \param parent the parent
     * \param iconManager the icon manager to use
     * \param combiner the combiner that is represented by this button
     * \param useText  true if a text should be shown.
     */
    WQtApplyModulePushButton( QWidget* parent, WIconManager* iconManager,
                              boost::shared_ptr< WApplyPrototypeCombiner > combiner, bool useText = true );

    /**
     * destructor
     */
    virtual ~WQtApplyModulePushButton();

protected:

    /**
     * The combiner used in this button.
     */
    boost::shared_ptr< WApplyPrototypeCombiner > m_combiner;

private:
public slots:

    /**
     * Slot getting called when the button got pressed. It applies the combiner.
     */
    void emitPressed();
};

#endif  // WQTAPPLYMODULEPUSHBUTTON_H
