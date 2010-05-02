///////////////////////////////////////////////////////////////////////////////
//
///// /////  ////       Project  :   FAnToM
//   //  // //  //      Module   :   Fge (Rendering and Viewer Components)
//   //  // /////       File     :   $RCSfile: $
//   //  // //          Language :   C++
//    /////  ////       Date     :   $Date: $
         //             Author   :   $Author: ebaum $
//////////              Revision :   $Revision: 9259 $

#version 120

// include some utility functions
#include "tensorTools.fs"

// commonly used variables
#include "gpuSuperQuadrics.varyings"

/////////////////////////////////////////////////////////////////////////////////////////////
// GPU Super Quadrics -- vertex shader -- main
//
/////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
    /////////////////////////////////////////////////////////////////////////////////////////////
    // 1: standart shader stuff
    /////////////////////////////////////////////////////////////////////////////////////////////

    // aquire texture data
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_TexCoord[1] = gl_MultiTexCoord1;
    gl_TexCoord[2] = gl_MultiTexCoord2;

    /////////////////////////////////////////////////////////////////////////////////////////////
    // 2: aquire tensor data, calculate eigen system
    /////////////////////////////////////////////////////////////////////////////////////////////

    // get tensor data
    vec3 diag    = vec3( gl_TexCoord[1].x, gl_TexCoord[1].y, gl_TexCoord[1].z );
    vec3 offdiag = vec3( gl_TexCoord[2].x, gl_TexCoord[2].y, gl_TexCoord[2].z );
    
    // calculate eigenvectors, and rotation matrix
    vec3 evals = getEigenvalues( diag, offdiag );
    evals = vec3( 1.0, 1.0, 1.0 );

    // first eigenvector
    vec3 ABCx = diag - evals.x;
    vec3 ev0 = getEigenvector( ABCx, offdiag );
    ev0 = normalize( vec3( 1.0, 0.0, 0.0 ) );

    // second eigenvector
    vec3 ABCy = diag - evals.y;
    vec3 ev1 = getEigenvector( ABCy, offdiag );
    ev1 = normalize( vec3( 0.0, 1.0, 0.0 ) );

/*ev1 -= dot( ev0, ev1 ) * ev0;
ev1 = normalize( ev1 );
*/

    // third eigenvector
    vec3 ABCz = diag - evals.z;
    //vec3 ev2 = getEigenvector( ABCz, offdiag );
    vec3 ev2 = cross( ev0.xyz, ev1.xyz ); // as they are orthogonal

    // glyphs color and anisotropy
    float FA = getFA( evals );
    FA = clamp( FA, 0.0, 1.000 ); // filter out invalid FA values later
    gl_FrontColor = getColor( ev0.xyz, FA );

    // throw away glyphs whose FA is below threshold and whose eigenvalues are below threshold
    v_alphaBeta.w = 0.0;
    if ( FA < u_faThreshold )
    {
        v_alphaBeta.w = 1.0;
    }
    if ( evals.z < u_evThreshold ) 
    {
        v_alphaBeta.w = 1.0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // 3: precalculate some glyph characteristics 
    /////////////////////////////////////////////////////////////////////////////////////////////

    // calculate alpha and beta corresponding to eigenvalues
    float evalSum =    evals.x + evals.y + evals.z;
    float cl =       ( evals.x - evals.y ) / evalSum;
    float cp = 2.0 * ( evals.y - evals.z ) / evalSum;
    float cs = 3.0 * ( evals.z )           / evalSum;

    // by default we use ellipsoids
    float kmAlpha=1.0;
    float kmBeta=1.0;

    // the above values define the shape of the glyph
    if (cl>=cp)
    {
        kmAlpha= pow( 1.0 - cp, u_gamma );
        kmBeta = pow( 1.0 - cl, u_gamma );
    }
    else
    {
        kmAlpha= pow( 1.0 - cl, u_gamma );
        kmBeta = pow( 1.0 - cp, u_gamma );
    }

    // limit the alpha/beta range to [0.2,1.0]
    // HINT: we do this to avoid the exploding number of needed iterations for smaller alpha/beta to find the hit point
    kmAlpha = clamp( kmAlpha, 0.2, 1.0 );
    kmBeta  = clamp( kmBeta, 0.2, 1.0 );

    // finally do some precalculations on alpha and beta
    v_alphaBeta.x = 2.0 / kmAlpha;
    v_alphaBeta.y = 2.0 / kmBeta;
    v_alphaBeta.z = kmAlpha / kmBeta;


    evals.z = ( evals.x / evals.z );
    evals.y = ( evals.x / evals.y );
    evals.x = 1.0;

    /////////////////////////////////////////////////////////////////////////////////////////////
    // 4: now span the quad according to bounding box, so that the fragment shader can render
    //    the correctly transformed superquadric without clipped edges etc.
    /////////////////////////////////////////////////////////////////////////////////////////////

    // rotation-matrix describing the coordinate system rotation corresponding to the eigenvectors
    mat4 glyphSystem = mat4( ( ev0.xyz ), 0.0,
                             ( ev1.xyz ), 0.0,
                             ( ev2.xyz ), 0.0,
                             0.0, 0.0, 0.0, 1.0 ); 

    // calculate propper bounding volume for this quadric (scale)
    vec4 coord = vec4( evals.x * gl_MultiTexCoord0.x,
                       evals.y * gl_MultiTexCoord0.y, 
                       evals.z * gl_MultiTexCoord0.z,
                       0.0 ); 

    gl_Position = gl_ModelViewProjectionMatrix * glyphSystem * ( gl_Vertex + coord );
  
    // just to be sure to have propper clipping
    gl_ClipVertex = gl_ModelViewMatrix * coord;

    /////////////////////////////////////////////////////////////////////////////////////////////
    // 5: Transform light and plane as well as ray back to glyph space
    /////////////////////////////////////////////////////////////////////////////////////////////

    // calculate light direction once per quadric
    v_lightDir.xyz = normalize( ( gl_ModelViewMatrixInverse * gl_LightSource[0].position ).xyz );

    // the viewing direction for this vertex:
    v_viewDir.xyz  = ( gl_ModelViewMatrixInverse * vec4( 0.0, 0.0, 1.0, 0.0 ) ).xyz; 

    v_planePoint = vec4( gl_MultiTexCoord0.x,
                         gl_MultiTexCoord0.y,
                         gl_MultiTexCoord0.z,
                         0.0 );
    //v_planePoint = coord;
}

