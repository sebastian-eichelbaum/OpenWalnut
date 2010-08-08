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

#ifndef WGESCENE_H
#define WGESCENE_H

#include "WGEGroupNode.h"
#include "WExportWGE.h"

/**
 * Class for managing the OpenSceneGraph root node. It can handle new nodes, removing nodes and so on.
 * \ingroup ge
 */
class WGE_EXPORT WGEScene: public WGEGroupNode
{
public:

    /**
     * Default constructor.
     */
    WGEScene();

protected:

    /**
     * Destructor. Protected. To avoid accidental destruction of these root nodes.
     */
    virtual ~WGEScene();

private:
//    /**
//     * Node callback to handle updates properly
//     */
//    class bbCallback : public osg::Node::ComputeBoundingSphereCallback
//    {
//    public: // NOLINT
//        /**
//         * operator ()
//         *
//         * \param node the osg node
//         * \param nv the node visitor
//         */
//        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
//        {
//            osg::ref_ptr< WMFiberDisplay > module = static_cast< WMFiberDisplay* > ( node->getUserData() );
//
//            if ( module )
//            {
//                module->update();
//                module->toggleTubes();
//                module->toggleColoring();
//            }
//            traverse( node, nv );
//        }
//    };
};



#endif  // WGESCENE_H

