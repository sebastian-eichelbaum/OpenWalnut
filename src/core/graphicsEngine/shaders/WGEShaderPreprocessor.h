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

#ifndef WGESHADERPREPROCESSOR_H
#define WGESHADERPREPROCESSOR_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "../../common/WCondition.h"

#include "../WExportWGE.h"

/**
 * Base class for each preprocessing possible to shader code. This is useful to derive your own preprocessor which might be able to implement
 * some tricky "metaprogramming" for GLSL or similar. Another possibility are defines. In addition, it implements an update notification
 * mechanism which forces associated WGEShader to reload. This is useful for preprocessors that can be modified dynamically.
 */
class WGE_EXPORT WGEShaderPreprocessor  // NOLINT
{
public:
    /**
     * Shared pointer for this class.
     */
    typedef boost::shared_ptr< WGEShaderPreprocessor > SPtr;

    /**
     * A const shared pointer for this class.
     */
    typedef boost::shared_ptr< const WGEShaderPreprocessor > ConstSPtr;

    /**
     * Default constructor.
     */
    WGEShaderPreprocessor();

    /**
     * Destructor.
     */
    virtual ~WGEShaderPreprocessor();

    /**
     * Returns the condition denoting a change in this preprocessor filter. WGEShader subscribes this and rebuilds all related shaders if needed.
     *
     * \return the condition firing whenever the preprocessor updates.
     */
    virtual WCondition::SPtr getChangeCondition() const;

    /**
     * Process the whole code. It is not allowed to modify some internal state in this function because it might be called by several shaders.
     *
     * \param code the code to process
     * \param file the filename of the shader currently processed. Should be used for debugging output.
     *
     * \return the resulting new code
     */
    virtual std::string process( const std::string& file, const std::string& code ) const = 0;

    /**
     * (De-)activates the preprocessor. An inactive preprocessor does not modify the shader code.
     *
     * \param active true if preprocessor should be active
     */
    void setActive( bool active = true );

    /**
     * If the preprocessor is active, this returns true.
     *
     * \return if active: true
     */
    bool getActive() const;

protected:
    /**
     * Fires m_updateCondition which should denote an update in the preprocessor filter.
     */
    virtual void updated();

private:
    /**
     * The condition fires on every call of updated().
     */
    WCondition::SPtr m_updateCondition;

    /**
     * If true the preprocessor is active.
     */
    bool m_active;
};

#endif  // WGESHADERPREPROCESSOR_H

