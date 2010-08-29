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

#ifndef WGLYPHRENDER_H
#define WGLYPHRENDER_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "../../dataHandler/WDataSetSingle.h"

#include "OpenCLRender.h"

/**
*	An osg::Drawable for rendering high order tensor glyphs with OpenCL.
*/
class WGlyphRender: public OpenCLRender
{
	public:

		/**
		*	Constructor.
		*/
		WGlyphRender(const boost::shared_ptr<WDataSetSingle>& data,const int& order,
					 const int& sliceX, const int& sliceY, const int& sliceZ,
					 const bool& enabledX,const bool& enabledY,const bool& enabledZ,
					 const boost::filesystem::path& search);

		/**
		*	Copy constructor.
		*/
		WGlyphRender(const WGlyphRender& wGlyphRender,const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

		/**
		*	Overrides Object::cloneType.
		*/
		virtual osg::Object* cloneType() const;

		/**
		*	Overrides Object::clone.
		*/
		virtual osg::Object* clone(const osg::CopyOp& copyop) const;

		/**
		*	Overrides Object::isSameKindAs.
		*/
		virtual bool isSameKindAs(const osg::Object* obj) const;

		/**
		*	Overrides Object::libraryName.
		*/
        virtual const char* libraryName() const;

		/**
		*	Overrides Object::className.
		*/
        virtual const char* className() const;

		/**
		*	Overrides Drawable::computeBound.
		*/
		virtual osg::BoundingBox computeBound() const;

		/**
		*	Has the kernel source code successfully been read?
		*/
		bool isSourceRead() const;

		/**
		*	Set the new tensor data to render. This method assumes data validity.
		*/
		void setTensorData(const boost::shared_ptr<WDataSetSingle>& data,const int& order,
						   const int& sliceX, const int& sliceY, const int& sliceZ,
						   const bool& enabledX,const bool& enabledY,const bool& enabledZ);

		/**
		*	Set the slice properties to render.
		*/
		void setSlices(const int& sliceX, const int& sliceY, const int& sliceZ,
					   const bool& enabledX,const bool& enabledY,const bool& enabledZ);

	protected:

		/**
		*	Destructor.
		*/
		virtual ~WGlyphRender();

	private:

		/**
		*	Callback used to update the data thread-safe without removing and readding the glyph render
		*	object from the scene graph.
		*/
		class DataChangeCallback: public UpdateCallback
		{
			public:

				DataChangeCallback();

				virtual void update(osg::NodeVisitor*,osg::Drawable* drawable);

				bool m_changed;
				bool m_dataChanged;

				int m_order;
				int m_slices[3];
				int m_sliceEnabled[3];

				boost::shared_ptr<WDataSetSingle> m_tensorData;
		};

		/**
		*	Callback used to update the CL objects.
		*/
		class ReloadCallback: public CLDataChangeCallback
		{
			public:

				ReloadCallback(bool reloadFactors);

				virtual void change(CLProgramDataSet* clProgramDataSet) const;

				bool factors;
		};

		/**
		*	Derives from OpenCLRender::CLProgramDataSet.
		*/
		class CLObjects: public CLProgramDataSet
		{
			public:

				CLObjects();

				~CLObjects();

				cl_program clProgram;
				cl_kernel clScaleKernel;
				cl_kernel clRenderKernel;
				cl_mem tensorData;
				cl_mem factors;

				bool dataCreated;
				bool reloadData;
				bool reloadAuxData;
		};

		/**
		*	Overrides OpenCLRender::initProgram.
		*/
		virtual CLProgramDataSet* initProgram(const CLViewInformation& clViewInfo) const;

		/**
		*	Overrides OpenCLRender::setBuffers.
		*/
		virtual void setBuffers(const CLViewInformation& clViewInfo,
								CLProgramDataSet* clProgramDataSet) const;

		/**
		*	Overrides OpenCLRender::render.
		*/
		virtual void render(const CLViewInformation& clViewInfo,
							CLProgramDataSet* clProgramDataSet,const osg::State& state) const;

		/**
		*	Loads new data to GPU memory.
		*/
		void loadCLData(const CLViewInformation& clViewInfo,CLObjects& clObjects) const;

		/**
		*	Tensor order.
		*/
		int m_order;

		/**
		*	Number of tensors in grid's x, y and z direction.
		*/
		int m_numOfTensors[3];

		/**
		*	Slice positions.
		*/
		int m_slices[3];

		/**
		*	Show slices.
		*/
		int m_sliceEnabled[3];

		/**
		*	The tensor data set.
		*/
		boost::shared_ptr<WDataSetSingle> m_tensorData;

		/**
		*	The kernel source code.
		*/
		std::string m_kernelSource;

		/**
		*	Has the kernel source code successfully been read?
		*/
		mutable bool m_sourceRead;
};

inline osg::Object* WGlyphRender::cloneType() const
{
	return 0;
}

inline osg::Object* WGlyphRender::clone(const osg::CopyOp& copyop) const
{
	return new WGlyphRender(*this,copyop);
}

inline bool WGlyphRender::isSameKindAs(const osg::Object* obj) const
{
	return dynamic_cast<const WGlyphRender*>(obj) != 0;
}

inline const char* WGlyphRender::libraryName() const
{
	return "tensorGlyphs";
}

inline const char* WGlyphRender::className() const
{
	return "WGlyphRender";
}

inline bool WGlyphRender::isSourceRead() const
{
	return m_sourceRead;
}

#endif