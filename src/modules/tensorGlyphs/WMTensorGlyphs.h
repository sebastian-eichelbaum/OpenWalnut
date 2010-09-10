//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#ifndef WMTENSORGLYPHS_H
#define WMTENSORGLYPHS_H

#include <boost/shared_ptr.hpp>

#include <osg/Geode>

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"

#include "../../dataHandler/WDataSetSingle.h"

#include "WGlyphRenderNode.h"

/**
*	Module for the GPU-based rendering of high order tensor glyphs.
*	TODO: include slices
*/
class WMTensorGlyphs: public WModule
{
	public:

		/**
		*	Default constructor.
		*/
		WMTensorGlyphs();

		/**
		*	Destructor.
		*/
		virtual ~WMTensorGlyphs();

		/**
		*	Gives back the name of this module.
		*
		*	\return the module's name.
		*/
		virtual const std::string getName() const;

		/**
		*	Gives back a description of this module.
		*
		*	\return description to module.
		*/
		virtual const std::string getDescription() const;

		/**
		*	Overrides WModule::factory.
		*/
		virtual boost::shared_ptr<WModule> factory() const;

		/**
		*	Overrides WModule::getXPMIcon.
		*/
		virtual const char** getXPMIcon() const;

	protected:

		/**
		*	Overrides WModule::moduleMain.
		*/
		virtual void moduleMain();

		/**
		*	Overrides WModule::connectors.
		*/
		virtual void connectors();

		/**
		*	Overrides WModule::properties.
		*/
		virtual void properties();

		/**
		*	Overrides WModule::activate.
		*/
		virtual void activate();

		/**
		*	The root node used for this modules graphics.
		*/
		osg::ref_ptr<WGlyphRenderNode> renderNode;

	private:

		/**
		*	The input dataset containing the tensor data.
		*/
		boost::shared_ptr<WModuleInputData<WDataSetSingle>> m_input;

		/**
		*	A condition used to notify about changes in several properties.
		*/
		boost::shared_ptr<WCondition> m_propertyChanged;

		/**
		*	Slice positions.
		*/
		WPropInt m_slices[3];

		/**
		*	Show slices.
		*/
		WPropBool m_sliceEnabled[3];
};

#endif