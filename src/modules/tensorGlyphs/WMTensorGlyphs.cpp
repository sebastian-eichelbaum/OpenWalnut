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

#include "../../kernel/WKernel.h"

#include "glyph.xpm"
#include "WMTensorGlyphs.h"

WMTensorGlyphs::WMTensorGlyphs()
{
	m_output = new osg::Geode();
}

WMTensorGlyphs::~WMTensorGlyphs()
{}

boost::shared_ptr<WModule> WMTensorGlyphs::factory() const
{
	return boost::shared_ptr<WModule>(new WMTensorGlyphs());
}

const char** WMTensorGlyphs::getXPMIcon() const
{
	return glyph_xpm;
}

const std::string WMTensorGlyphs::getName() const
{
	return "Tensor Glyphs";
}

const std::string WMTensorGlyphs::getDescription() const
{
	return "GPU based raytracing of high order tensor glyphs.";
}

void WMTensorGlyphs::connectors()
{
	m_input = boost::shared_ptr<WModuleInputData<WDataSetSingle>>
	(
		new WModuleInputData<WDataSetSingle>
		(
			shared_from_this(),
			"tensor input",
			"An input set of high order tensors on a regular 3D-grid."
		)
	);

	addConnector(m_input);

	WModule::connectors();
}

void WMTensorGlyphs::properties()
{
	m_propertyChanged = boost::shared_ptr<WCondition>(new WCondition());

	// etc. pp.
}

void WMTensorGlyphs::moduleMain()
{
	/* set conditions for data and property changes */

	m_moduleState.setResetable(true,true);
	m_moduleState.add(m_input->getDataChangedCondition());
	//m_moduleState.add(m_propertyChanged);

	ready();

	/* add render object */

	osg::ref_ptr<WGlyphRender> renderObject = new WGlyphRender();

	if (!renderObject->isSourceRead())
	{
		return;
	}

	m_output->addDrawable(renderObject.get());

	WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert(m_output);

	boost::shared_ptr<WDataSetSingle> newDataSet;
	unsigned int newDimension;
	unsigned int newOrder;
	bool dataChanged;
	bool dataValid;

	/* main loop */

	while (!m_shutdownFlag())
	{
		m_moduleState.wait();

		if (m_shutdownFlag()) break;

		/* test for data changes */

		newDataSet = m_input->getData();
		dataChanged = (m_dataSet != newDataSet);

		/* test for data validity */

		dataValid = (newDataSet != 0);

		if (dataValid)
		{
			/* check the order of the new tensor data */
			/* TODO: substitute this bullshit as soon as there is a working tensor data set class */

			newOrder = 0;
			newDimension = newDataSet->getValueSet()->dimension();

			while (true)
			{
				if (newDimension == (((newOrder + 1) * (newOrder + 2)) / 2)) break;

				if (newDimension < (((newOrder + 1) * (newOrder + 2)) / 2)) 
				{
					dataValid = false;

					break;
				}

				newOrder += 2;
			}

			if (newOrder == 0) dataValid = false;

			if (newDataSet->getValueSet()->order() != 1) dataValid = false;

			if (!dataValid)
			{
				warnLog()<<"Received data with order="<< newDataSet->getValueSet()->order()
						<<" and dimension="<<newDataSet->getValueSet()->dimension()
						<<" not compatible with this module. Ignoring Data.";
			}

			/* check the grid validity */

			dataValid = boost::shared_dynamic_cast<WGridRegular3D>(newDataSet->getGrid());

			if (!dataValid)
			{
				warnLog()<<"Dataset does not have a regular 3D grid. Ignoring Data.";
			}
		}
		
		/* handle data */

		if (dataChanged && dataValid)
		{
			debugLog() << "Received Data.";

			m_dataSet = newDataSet;

			renderObject->setTensorData(m_dataSet.get());
		}
		else
		{
			if (!m_dataSet) continue;
		}

		/* property changes */

		// etc. pp.
	}

	// remove allocated memory and all OSG nodes
	WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove(m_output);
}

void WMTensorGlyphs::activate()
{
	if (m_output)
	{
		if (m_active->get())
		{
			m_output->setNodeMask(0xFFFFFFFF);
		}
		else
		{
			m_output->setNodeMask(0x0);
		}
	}

	WModule::activate();
}