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

#ifndef WGESHADERDEFINEOPTIONS_H
#define WGESHADERDEFINEOPTIONS_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "WGEShaderPreprocessor.h"

#include "../WExportWGE.h"

/**
 * This GLSL preprocessor is able to set one define from a list of defines depending on the active option. You should prefer this class instead
 * of WGEShaderDefine if many mutual exclusive options should be handled comfortably.
 *
 * \note: operations on the option list are not thread-safe.
 */
class WGE_EXPORT WGEShaderDefineOptions: public WGEShaderPreprocessor
{
public:
    /**
     * Shared pointer for this class.
     */
    typedef boost::shared_ptr< WGEShaderDefineOptions > SPtr;

    /**
     * A const shared pointer for this class.
     */
    typedef boost::shared_ptr< const WGEShaderDefineOptions > ConstSPtr;

    /**
     * The type of the index list
     */
    typedef std::vector< size_t > IdxList;

    /**
     * Create a new instance of this class. The first option is mandatory and is set as default.
     *
     * \param first fist option. Is default.
     * \param option2 another option
     * \param option3 another option
     * \param option4 another option
     * \param option5 another option
     * \param option6 another option
     * \param option7 another option
     * \param option8 another option
     * \param option9 another option
     * \param option10 another option
     */
    WGEShaderDefineOptions( std::string first,
                            std::string option2 = "", std::string option3 = "", std::string option4 = "", std::string option5 = "",
                            std::string option6 = "", std::string option7 = "", std::string option8 = "", std::string option9 = "",
                            std::string option10 = "" );

    /**
     * Create a new instance of this class. The first option is mandatory and is set as default.
     *
     * \param options the list of options. Must have a size greater 0.
     */
    explicit WGEShaderDefineOptions( std::vector< std::string > options );

    /**
     * Destructor.
     */
    virtual ~WGEShaderDefineOptions();

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
     * Returns the currently active option as index.
     *
     * \return the index of the active option
     */
    const IdxList& getActiveOptions() const;

    /**
     * Returns the name of the specified option.
     *
     * \param idx the index
     *
     * \return the name
     */
    std::string getOptionName( size_t idx ) const;

    /**
     * Activates the option specified.
     *
     * \param idx the option index to activate
     * \param exclusive if true, all active options get deactivated and the specified one will be the only active one afterwards
     */
    void activateOption( size_t idx, bool exclusive = true );

    /**
     * De-activates the specified option. If it is not activated, nothing happens.
     *
     * \param idx the option to deactivate
     */
    void deactivateOption( size_t idx );

    /**
     * Activates all the options.
     */
    void activateAllOptions();

    /**
     * De-activates all the options.
     */
    void deactivateAllOptions();

    /**
     * Adds the specified string as option which is inserted to the code as "#define NAME" if active. Must be a unique name. If it already exists
     * in the list, nothing happens.
     *
     * \param opt the option name.
     */
    void addOption( std::string opt );

protected:
    /**
     * Sets the specified index list as the new activation list. Triggers an update.
     *
     * \param newList the ne list getting copied to the internal activation list.
     */
    void setActivationList( const IdxList& newList );

private:
    /**
     * The list of options.
     */
    std::vector< std::string > m_options;

    /**
     * The currently selected options.
     */
    IdxList m_idx;
};

#endif  // WGESHADERDEFINEOPTIONS_H

