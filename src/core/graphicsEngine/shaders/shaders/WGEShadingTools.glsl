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

#ifndef WGESHADINGTOOLS_GLSL
#define WGESHADINGTOOLS_GLSL

#version 120

/**
 * A struct containing the needed light and material parameters commonly used in most shaders.
 *
 * \note This is for evaluating the phong equation for 1 channel only.
 */
struct wge_LightIntensityParameter
{
    // These 4 parameters are similar to those in gl_MaterialParameters
    float materialAmbient;  //!< Material ambient intensity.
    float materialDiffuse;  //!< Material diffuse intensity.
    float materialSpecular; //!< Material Specular intensity.
    float materialShinines; //!< Material shinines factor

    // These 4 parametes are a stripped down version of gl_LightSourceParameters
    float lightDiffuse;     //!< Light diffuse intensity.
    float lightAmbient;     //!< Light ambient intensity.
    vec3  lightPosition;    //!< Light position in world-space

    vec3  viewDirection;    //!< View direction vector. Well this actually is -vec3( 0.0, 0.0, -1.0 )
};

/**
 * This variable contains the OpenWalnut default light. You should definitely use this for your lighting to obtain an identical look for all
 * rendered images.
 */
wge_LightIntensityParameter wge_DefaultLightIntensity = wge_LightIntensityParameter(
    0.04,                            // material ambient
    0.75,                            // material diffuse
    1.0,                             // material specular
    250.0,                           // material shininess
    1.0,                             // light diffuse
    1.0,                             // light ambient
    gl_LightSource[0].position.xyz,  // light position
    vec3( 0.0, 0.0, 1.0 )            // view direction
);

/**
 * This variable contains the OpenWalnut default light. You should definitely use this for your lighting to obtain an identical look for all
 * rendered images. This version looks has no ambient factor and provides full diffuse colors.
 */
wge_LightIntensityParameter wge_DefaultLightIntensityFullDiffuse = wge_LightIntensityParameter(
    0.0,                             // material ambient
    1.0,                             // material diffuse
    1.0,                             // material specular
    250.0,                           // material shininess
    1.0,                             // light diffuse
    1.0,                             // light ambient
    gl_LightSource[0].position.xyz,  // light position
    vec3( 0.0, 0.0, 1.0 )            // view direction
);

/**
 * This variable contains the OpenWalnut default light. You should definitely use this for your lighting to obtain an identical look for all
 * rendered images. This version looks a little bit more metallic.
 */
wge_LightIntensityParameter wge_DefaultLightIntensityLessDiffuse = wge_LightIntensityParameter(
    0.0,                             // material ambient
    0.35,                            // material diffuse
    1.0,                             // material specular
    250.0,                           // material shininess
    1.0,                             // light diffuse
    1.0,                             // light ambient
    gl_LightSource[0].position.xyz,  // light position
    vec3( 0.0, 0.0, 1.0 )            // view direction
);

/**
 * Function to calculate lighting based on "Real-Time Volume Graphics, p 119, chapter 5.4, Listing 5.1".
 *
 * \param ambient   materials ambient color
 * \param diffuse   materials diffuse color
 * \param specular  materials specular color
 * \param shininess material shininess
 * \param lightColor  the light color
 * \param ambientLight the ambient light color
 * \param normalDir the normal
 * \param viewDir   viewing direction
 * \param lightDir  light direction
 *
 * \return the color.
 */
vec4 blinnPhongIllumination( vec3 ambient, vec3 diffuse, vec3 specular, float shininess,
                             vec3 lightColor, vec3 ambientLight,
                             vec3 normalDir, vec3 viewDir, vec3 lightDir )
{
    normalDir *= sign( dot( normalDir, viewDir ) );

    vec3 H =  normalize( lightDir + viewDir );

    // compute ambient term
    vec3 ambientV = ambient * ambientLight;

    // compute diffuse term
    float diffuseLight = max( dot( lightDir, normalDir ), 0.0 );
    vec3 diffuseV = diffuse * diffuseLight;

    // compute specular term
    float specularLight = pow( max( dot( H, normalDir ), 0.0 ), shininess );
    if( diffuseLight <= 0.) specularLight = 0.;
    vec3 specularV = specular * specularLight;

    return vec4( ambientV + ( diffuseV + specularV ) * lightColor, 1.0 );
}

/**
 * Function to calculate lighting intensity based on "Real-Time Volume Graphics, p 119, chapter 5.4, Listing 5.1".
 * It is basically the same as blinnPhongIllumination function above. But it is faster if you just need
 * the intensity.
 *
 * \param ambient   materials ambient intensity
 * \param diffuse   materials diffuse intensity
 * \param specular  materials specular intensity
 * \param shininess material shininess
 * \param lightIntensity  the light intensity
 * \param ambientIntensity the ambient light intensity
 * \param normalDir the normal
 * \param viewDir   viewing direction
 * \param lightDir  light direction
 *
 * \return the light intensity.
 */
float blinnPhongIlluminationIntensity( float ambient, float diffuse, float specular, float shininess,
                                       float lightIntensity, float ambientIntensity,
                                       vec3 normalDir, vec3 viewDir, vec3 lightDir )
{
    normalDir *= sign( dot( normalDir, viewDir ) );

    vec3 H =  normalize( lightDir + viewDir );

    // compute ambient term
    float ambientV = ambient * ambientIntensity;

    // compute diffuse term
    float diffuseLight = max( dot( lightDir, normalDir ), 0.0 );
    float diffuseV = diffuse * diffuseLight;

    // compute specular term
    float specularLight = pow( max( dot( H, normalDir ), 0.0 ), shininess );
    if( diffuseLight <= 0.) specularLight = 0.;
    float specularV = specular * specularLight;

    return ambientV + ( diffuseV + specularV ) * lightIntensity;
}

/**
 * Function to calculate lighting intensity based on "Real-Time Volume Graphics, p 119, chapter 5.4, Listing 5.1".
 * It is basically the same as blinnPhongIllumination function above. But it is faster if you just need
 * the intensity.
 *
 * \param parameter the wge_LightIntensityParameter defining material and light
 * \param normal the normal. Needs to be normalized.
 *
 * \return lighting intensity.
 */
float blinnPhongIlluminationIntensity( in wge_LightIntensityParameter parameter, in vec3 normal )
{
    return blinnPhongIlluminationIntensity(
        parameter.materialAmbient,
        parameter.materialDiffuse,
        parameter.materialSpecular,
        parameter.materialShinines,
        parameter.lightDiffuse,
        parameter.lightAmbient,
        normal,
        parameter.viewDirection,
        parameter.lightPosition
        );
}

/**
 * Function to calculate lighting intensity based on "Real-Time Volume Graphics, p 119, chapter 5.4, Listing 5.1".
 * It is basically the same as blinnPhongIllumination function above. But it is faster if you just need
 * the intensity. This uses the wge_DefaultLightIntensity.
 *
 * \param normal the normal. Must be normalized beforehand
 *
 * \return the light intensity
 */
float blinnPhongIlluminationIntensity( in vec3 normal )
{
    return blinnPhongIlluminationIntensity( wge_DefaultLightIntensity, normal );
}

/**
 * Function to calculate lighting intensity for a matte (non-specular) Phong illumination.
 *
 * \param ambient   materials ambient color
 * \param diffuse   materials diffuse color
 * \param lightColor  the light color
 * \param ambientLight the ambient light color
 * \param normalDir the normal
 * \param viewDir   viewing direction
 * \param lightDir  light direction
 *
 * \return the color.
 */
vec4 mattePhongIllumination( vec3 ambient, vec3 diffuse,
                             vec3 lightColor, vec3 ambientLight,
                             vec3 normalDir, vec3 viewDir, vec3 lightDir )
{
    vec3 H =  normalize( lightDir + viewDir );

    // compute ambient term
    vec3 ambientV = ambient * ambientLight;

    // compute diffuse term
    float diffuseLight = max( dot( lightDir, normalDir ), 0.0 );
    vec3 diffuseV = diffuse * diffuseLight;

    return vec4( ambientV + diffuseV * lightColor, 1.0 );
}

/**
 * Function to calculate lighting intensity for a matte (non-specular) Phong illumination.
 * It is basically the same as blinnPhongIllumination function above. But it is faster if you just need
 * the intensity.
 *
 * \param ambient   materials ambient intensity
 * \param diffuse   materials diffuse intensity
 * \param specular  materials specular intensity
 * \param shininess material shininess
 * \param lightIntensity  the light intensity
 * \param ambientIntensity the ambient light intensity
 * \param normalDir the normal
 * \param viewDir   viewing direction
 * \param lightDir  light direction
 *
 * \return the light intensity.
 */
float mattePhongIlluminationIntensity( float ambient, float diffuse,
                                       float lightIntensity, float ambientIntensity,
                                       vec3 normalDir, vec3 viewDir, vec3 lightDir )
{
    vec3 H =  normalize( lightDir + viewDir );

    // compute ambient term
    float ambientV = ambient * ambientIntensity;

    // compute diffuse term
    float diffuseLight = max( dot( lightDir, normalDir ), 0.0 );
    float diffuseV = diffuse * diffuseLight;

    return ambientV + diffuseV * lightIntensity;
}

/**
 * Function to calculate lighting intensity for a matte (non-specular) Phong illumination.
 * Any specular or shininess given with the parameters will be ignored.
 * It is basically the same as blinnPhongIllumination function above. But it is faster if you just need
 * the intensity.
 *
 * \param parameter the wge_LightIntensityParameter defining material and light
 * \param normal the normal. Needs to be normalized.
 *
 * \return lighting intensity.
 */
float mattePhongIlluminationIntensity( in wge_LightIntensityParameter parameter, in vec3 normal )
{
    return mattePhongIlluminationIntensity(
        parameter.materialAmbient,
        parameter.materialDiffuse,
        parameter.lightDiffuse,
        parameter.lightAmbient,
        normal,
        parameter.viewDirection,
        parameter.lightPosition
        );
}

/**
 * Function to calculate lighting intensity for a matte (non-specular) Phong illumination.
 * It is basically the same as blinnPhongIllumination function above. But it is faster if you just need
 * the intensity. This uses the wge_DefaultLightIntensity without the specular and shininess.
 *
 * \param normal the normal. Must be normalized beforehand
 *
 * \return the light intensity
 */
float mattePhongIlluminationIntensity( in vec3 normal )
{
    wge_LightIntensityParameter noSpecular = wge_DefaultLightIntensity;
    noSpecular.materialShinines = 0.0;
    noSpecular.materialSpecular = 0.0;
    return mattePhongIlluminationIntensity( noSpecular, normal );
}

/**
 * This illumination technique is from "Jens Krüger and Rüdiger Westermann - EFFICIENT STIPPLE RENDERING". It is a non-linear illumination model
 * which only handles ambient and diffuse components. The parameter alpha determines how much the diffuse light should depend on the orientation
 * of the surface towards the light source (the camera in OpenWalnut's default case ). It is acutally quite similar to the Phong specular term.
 *
 * \param parameter the light parameter
 * \param normal the normal. Must be normalized beforehand.
 * \param alpha the non-linear influence of surface orientation
 *
 * \return light intensity
 */
float kruegerNonLinearIllumination( in wge_LightIntensityParameter parameter, in vec3 normal, in float alpha )
{
    float diffuseIntensity = pow( ( dot( parameter.lightPosition.xyz, normal ) + 1.0 ) / 2.0, alpha );
    float diffuse = parameter.lightDiffuse *  parameter.materialDiffuse * diffuseIntensity;
    float ambient = parameter.materialAmbient * parameter.lightAmbient;
    return ambient + diffuse;
}

/**
 * This illumination technique is from "Jens Krüger and Rüdiger Westermann - EFFICIENT STIPPLE RENDERING". It is a non-linear illumination model
 * which only handles ambient and diffuse components. The parameter alpha determines how much the diffuse light should depend on the orientation
 * of the surface towards the light source (the camera in OpenWalnut's default case ). This version uses wge_DefaultLightIntensityFullDiffuse as
 * parameter. It is acutally quite similar to the Phong specular term.
 *
 * \param normal the normal. Must be normalized beforehand.
 * \param alpha the non-linear influence of surface orientation
 *
 * \return light intensity
 */
float kruegerNonLinearIllumination( in vec3 normal, in float alpha )
{
    return kruegerNonLinearIllumination( wge_DefaultLightIntensityFullDiffuse, normal, alpha );
}

/**
 * Calculates the gradient inside a luminance 3D texture at the specified position.
 *
 * \param sampler the texture sampler to use
 * \param pos where in the texture
 * \param stepsize the offset used in the kernel. Should be related to the nb. of voxels.
 *
 * \return the gradient
 */
vec3 getGradient( in sampler3D sampler, in vec3 pos, in float stepsize )
{
    float valueXP = texture3D( sampler, pos + vec3( stepsize, 0.0, 0.0 ) ).r;
    float valueXM = texture3D( sampler, pos - vec3( stepsize, 0.0, 0.0 ) ).r;
    float valueYP = texture3D( sampler, pos + vec3( 0.0, stepsize, 0.0 ) ).r;
    float valueYM = texture3D( sampler, pos - vec3( 0.0, stepsize, 0.0 ) ).r;
    float valueZP = texture3D( sampler, pos + vec3( 0.0, 0.0, stepsize ) ).r;
    float valueZM = texture3D( sampler, pos - vec3( 0.0, 0.0, stepsize ) ).r;

    return vec3( valueXP - valueXM, valueYP - valueYM, valueZP - valueZM );
}

/**
 * Calculates the gradient inside a luminance 3D texture at the specified position.
 *
 * \param sampler the texture sampler to use
 * \param pos where in the texture
 *
 * \return the gradient
 */
vec3 getGradient( in sampler3D sampler, in vec3 pos )
{
    // unfortunately the ATI driver does not allow default values for function arguments
    return getGradient( sampler, pos, 0.005 );
}

/**
 * Calculates the gradient in a luminance 3D texture at the specified position. Unlike getGradient, this switches the orientation of the gradient
 * according to the viewing direction. This ensures, that the gradient always points towards the camera and therefore is useful as a normal.
 *
 * \param sampler the texture sampler to use
 * \param pos where in the texture
 * \param viewDir the direction from the camera to pos
 * \param stepsize the offset used in the kernel. Should be related to the nb. of voxels.
 *
 * \return the gradient
 */
vec3 getGradientViewAligned( in sampler3D sampler, in vec3 pos, in vec3 viewDir, in float stepsize )
{
    vec3 grad = getGradient( sampler, pos, stepsize );
    return  sign( dot( grad, -viewDir ) ) * grad;
}

/**
 * Calculates the gradient in a luminance 3D texture at the specified position. Unlike getGradient, this switches the orientation of the gradient
 * according to the viewing direction. This ensures, that the gradient always points towards the camera and therefore is useful as a normal.
 *
 * \param sampler the texture sampler to use
 * \param pos where in the texture
 * \param viewDir the direction from the camera to pos
 *
 * \return the gradient
 */
vec3 getGradientViewAligned( in sampler3D sampler, in vec3 pos, in vec3 viewDir )
{
    // unfortunately the ATI driver does not allow default values for function arguments
    return getGradientViewAligned( sampler, pos, viewDir, 0.005 );
}

/**
 * Re-orient the specified vector to point towards the camera. This should be done AFTER modelview transformation.
 *
 * \param v the vector to re-orient
 *
 * \return the new vector. Only normalized if v was normalized.
 */
vec3 viewAlign( vec3 v )
{
    return sign( dot( v, vec3( 0.0, 0.0, 1.0 ) ) ) * v;
}

#endif // WGESHADINGTOOLS_GLSL

