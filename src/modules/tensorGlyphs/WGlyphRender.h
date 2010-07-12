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

#include "../../dataHandler/WDataSetSingle.h"

#include "OpenCLRender.h"

/**
*	An osg::Drawable for rendering high order tensor glyphs with OpenCL.
*/
class WGlyphRender: public OpenCLRender
{
	public:

		/**
		*	Standard constructor.
		*/
		WGlyphRender();

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
		*	Has the kernel source code successfully been read?
		*/
		bool isSourceRead() const;

		/**
		*	Set the new tensor data to render. This method assumes data validity.
		*/
		void setTensorData(WDataSetSingle* data);

	protected:

		/**
		*	Destructor.
		*/
		virtual ~WGlyphRender();

	private:

		/**
		*	Derives from OpenCLRender::CLProgramDataSet.
		*/
		class CLObjects: public CLProgramDataSet
		{
			public:

				CLObjects();

				~CLObjects();

				cl_program clProgram;
				cl_kernel clKernel;
				cl_mem tensorData;

				bool dataCreated;
		};

		/**
		*	Overrides OpenCLRender::initProgram.
		*/
		virtual CLProgramDataSet* initProgram(const CLViewInformation& clViewInfo) const;

		/**
		*	Overrides OpenCLRender::setBuffers.
		*/
		virtual void setBuffers(const CLViewInformation& clViewInfo,CLProgramDataSet* clProgramDataSet) const;

		/**
		*	Overrides OpenCLRender::render.
		*/
		virtual void render(const CLViewInformation& clViewInfo,CLProgramDataSet* clProgramDataSet,const osg::State& state) const;

		/**
		*	Loads new data to GPU memory.
		*/
		void loadCLData(const CLViewInformation& clViewInfo,CLObjects& clObjects) const;

		unsigned int m_numOfCoeffs;
		unsigned int m_numOfTensors;

		float* m_tensorData;

		bool m_sourceRead;
		bool m_dataInitialized;
		bool m_dataChanged;

		std::string m_kernelSource;
};

inline osg::Object* WGlyphRender::cloneType() const
{
	return new WGlyphRender();
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

#endif