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

#ifndef WMOVERLAYATLAS_H
#define WMOVERLAYATLAS_H

#include <string>
#include <vector>

#include <osg/Geode>

#include "../../graphicsEngine/WGEManagedGroupNode.h"
#include "../../graphicsEngine/WROISphere.h"

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

#include "WAtlasSlice.h"

/**
 *  This module loads pictures from usual image file (png, jpeg) and displays them on slices
 *
 * \ingroup modules
 */
class WMOverlayAtlas: public WModule
{
public:

    /**
     * constructor
     */
    WMOverlayAtlas();

    /**
     * destructor
     */
    virtual ~WMOverlayAtlas();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const;

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     */
    virtual const char** getXPMIcon() const;

protected:

    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();

    /**
     * updates the plane with the current settings
     */
    void updatePlane();

    /**
     *  updates textures and shader parameters
     */
    void updateTextures();

    /**
     * creates and initializes the uniform parameters for the shader
     * \param rootState The uniforms will be applied to this state.
     */
    void initUniforms( osg::StateSet* rootState );

    /**
     * Used as callback which simply sets m_textureChanged to true. Called by WSubject whenever the datasets change.
     */
    void notifyTextureChange();

    /**
     * setter for dirty flag
     */
    void setDirty();

    /**
     * getter
     * \return dirty flag
     */
    bool isDirty();

private:
    /**
     * helper function to read a text file
     * \param fileName
     * \return string containing the file
     */
    std::vector< std::string > readFile( const std::string fileName );

    /**
     * loads and parses the meta file
     * \param path to the meta file
     * \return true if a meta file was succesfully loaded, false otherwise
     */
    bool loadAtlas( boost::filesystem::path path );

    /**
     * updates the texture on a coronal oriented slice depending on hte module's settings
     */
    void updateCoronalSlice();

    /**
     * listener function for property changes
     *
     * \param property - the property which fired the event
     */
    void propertyChanged( boost::shared_ptr< WPropertyBase > property );

    /**
     * initializes the drawable elements of the module
     */
    void init();

    /**
     * called when texture manipulators are moved int he scene
     */
    void manipulatorMoved();

    /**
     * hides/unhides the manipulators
     */
    void toggleManipulators();

    /**
     * update callback for osg node traversal
     */
    void updateCallback();

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     *  helper function converts a string into a float
     *  \param s the string
     *  \return float
     */
    float s2f( std::string s );

    WPropTrigger  m_propReadTrigger; //!< This property triggers the actual reading,
    WPropFilename m_propMetaFile; //!< The tree will be read from this file, i hope we will get a real load button some time

    WPropBool m_showManipulators; //!< property controlling the display of the manipulator widgets

    WPropInt m_propCoronalSlicePos; //!< property controlling which slice shopuld be shown

    std::vector< WAtlasSlice >m_axialSlices; //!< vector containing image objects with axial orientation

    std::vector< WAtlasSlice >m_coronalSlices; //!< vector containing image objects with coronal orientation

    std::vector< WAtlasSlice >m_sagittalSlices; //!< vector containing image objects with sagittal orientation

    WPosition m_p0; //!< stores the last position of the center manipulator
    WPosition m_p1; //!< stores the last position of  manipulator 1
    WPosition m_p2; //!< stores the last position of  manipulator 2
    WPosition m_p3; //!< stores the last position of  manipulator 3
    WPosition m_p4; //!< stores the last position of  manipulator 4

    boost::shared_ptr<WROISphere> m_s0; //!< stores pointer to the center manipulator
    boost::shared_ptr<WROISphere> m_s1; //!< stores pointer to manipulator 1
    boost::shared_ptr<WROISphere> m_s2; //!< stores pointer to manipulator 2
    boost::shared_ptr<WROISphere> m_s3; //!< stores pointer to manipulator 3
    boost::shared_ptr<WROISphere> m_s4; //!< stores pointer to manipulator 4

    bool m_dirty; //!< dirty flag, used when manipulator positions change

    /**
     * The root node used for this modules graphics.
     */
    osg::ref_ptr< WGEManagedGroupNode > m_rootNode;

    /**
     * The geometry rendered by this module.
     */
    osg::ref_ptr< osg::Geode > m_geode;

    /**
     * True when textures have changed.
     */
    bool m_textureChanged;

    /**
     * the shader object for this module
     */
    osg::ref_ptr< WGEShader > m_shader;

    /**
     * True if the shader shouldn't discard a fragment when the value is zero
     */
    WPropBool m_showComplete;

    /**
     * vector of uniforms for type of texture
     */
    std::vector< osg::ref_ptr<osg::Uniform> > m_typeUniforms;

    /**
     * vector of alpha values per texture
     */
    std::vector< osg::ref_ptr<osg::Uniform> > m_alphaUniforms;

    /**
     * vector of thresholds per texture
     */
    std::vector< osg::ref_ptr<osg::Uniform> > m_thresholdUniforms;

    /**
     * vector of color maps per texture
     */
    std::vector< osg::ref_ptr<osg::Uniform> > m_cmapUniforms;

    /**
     * vector of samplers
     */
    std::vector< osg::ref_ptr<osg::Uniform> > m_samplerUniforms;

    osg::ref_ptr<osg::Uniform> m_showCompleteUniform; //!< Determines whether the slice should be drawn completely
    boost::shared_ptr< boost::function< void() > > m_changeRoiSignal; //!< Signal that can be used to update the manipulator
};

inline float WMOverlayAtlas::s2f( std::string s )
{
    return boost::lexical_cast<float>( s );
}

bool WMOverlayAtlas::isDirty()
{
    return m_dirty;
}

void WMOverlayAtlas::setDirty()
{
    m_dirty = true;
}


#endif  // WMOVERLAYATLAS_H
