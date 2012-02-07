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

#ifndef WGESHADERCODEINJECTOR_H
#define WGESHADERCODEINJECTOR_H

#include <string>

#include "WGEShaderPreprocessor.h"



/**
 * This preprocessor is able to inject code into a shader. It therefore replaces a specified keyword with code.
 */
class WGEShaderCodeInjector: public WGEShaderPreprocessor
{
public:
    /**
     * Shortcut for a shared_ptr.
     */
    typedef boost::shared_ptr< WGEShaderCodeInjector > SPtr;

    /**
     * Shortcut for a const shared_ptr.
     */
    typedef boost::shared_ptr< WGEShaderCodeInjector > ConstSPtr;

    /**
     * Default constructor.
     *
     * \param keyword this is replaced by the custom code if existing.
     */
    explicit WGEShaderCodeInjector( std::string keyword );

    /**
     * Destructor.
     */
    virtual ~WGEShaderCodeInjector();

    /**
     * Process the whole code. It is not allowed to modify some internal state in this function because it might be called by several shaders.
     *
     * \param code the code to process
     * \param file the filename of the shader currently processed. Should be used for debugging output.
     *
     * \return the resulting new code
     */
    virtual std::string process( const std::string& file, const std::string& code ) const;

    /**
     * Define the code that replaces the keyword.
     *
     * \param code the code.
     */
    void setCode( std::string code );
protected:
private:
    /**
     * the custom code.
     */
    std::string m_code;

    /**
     * What to replace.
     */
    std::string m_keyword;
};

#endif  // WGESHADERCODEINJECTOR_H

