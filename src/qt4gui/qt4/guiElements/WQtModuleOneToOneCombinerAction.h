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

#ifndef WQTMODULEONETOONECOMBINERACTION_H
#define WQTMODULEONETOONECOMBINERACTION_H

#include <string>

#include <boost/shared_ptr.hpp>

#include <QtGui/QAction>

#include "core/kernel/WModule.h"
#include "core/kernel/combiner/WModuleOneToOneCombiner.h"

#include "../WIconManager.h"

/**
 * Implements a action which applies the specified combiners (of the specified group) if pressed.
 */
class WQtModuleOneToOneCombinerAction : public QAction
{
    Q_OBJECT

public:
    /**
     * Constructor creating a module application action.
     *
     * \param parent the parent
     * \param iconManager the icon manager to use
     * \param combiner the apply combiner which gets applied when triggered
     * \param advancedText if true a more complex text showing the used connectors is used instead of the plane target-module name
     */
    WQtModuleOneToOneCombinerAction( QWidget* parent, WIconManager* iconManager, boost::shared_ptr< WModuleOneToOneCombiner > combiner,
                          bool advancedText = false );

    /**
     * destructor
     */
    virtual ~WQtModuleOneToOneCombinerAction();

protected:
    /**
     * The combiner of this action
     */
    boost::shared_ptr< WModuleOneToOneCombiner > m_combiner;

private:
public slots:

    /**
     * Gets called by the action if it was triggered.
     */
    void applyCombiner();
};

#endif  // WQTMODULEONETOONECOMBINERACTION_H
