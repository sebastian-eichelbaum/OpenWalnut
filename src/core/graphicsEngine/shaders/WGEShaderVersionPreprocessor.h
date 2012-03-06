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

#ifndef WGESHADERVERSIONPREPROCESSOR_H
#define WGESHADERVERSIONPREPROCESSOR_H

#include <string>

#include "WGEShaderPreprocessor.h"



/**
 * This preprocessor removes the version-statement from the code and puts it to the beginning of the code. This is requires for some GLSL
 * compiler.
 */
class WGEShaderVersionPreprocessor: public WGEShaderPreprocessor
{
public:
    /**
     * Default constructor.
     */
    WGEShaderVersionPreprocessor();

    /**
     * Destructor.
     */
    virtual ~WGEShaderVersionPreprocessor();

    /**
     * Process the whole code. It is not allowed to modify some internal state in this function because it might be called by several shaders.
     *
     * \param code the code to process
     * \param file the filename of the shader currently processed. Should be used for debugging output.
     *
     * \return the resulting new code
     */
    virtual std::string process( const std::string& file, const std::string& code ) const;

protected:
private:
};

#endif  // WGESHADERVERSIONPREPROCESSOR_H

