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
    vec3 diag    = vec3( gl_TexCoord[1].x, gl_TexCoord[1].z, gl_TexCoord[2].z );
    vec3 offdiag = vec3( gl_TexCoord[1].y, gl_TexCoord[2].x, gl_TexCoord[2].y );
    //vec3 diag    = vec3( gl_TexCoord[1].x, gl_TexCoord[1].y, gl_TexCoord[1].z );
    //vec3 offdiag = vec3( gl_TexCoord[2].x, gl_TexCoord[2].y, gl_TexCoord[2].z );
    
    // calculate eigenvectors, and rotation matrix
    vec3 evals = getEigenvalues( diag, offdiag );
    evals = vec3( 1.0, 0.5, 0.5 );

    // first eigenvector
    vec3 ABCx = diag - evals.x;
    vec3 ev0 = getEigenvector( ABCx, offdiag );
    ev0 = vec3( 0.0, 1.0, 0.0 );

    // second eigenvector
    vec3 ABCy = diag - evals.y;
    vec3 ev1 = getEigenvector( ABCy, offdiag );
    ev1 = vec3( 0.0, 0.0, 1.0 );

    // third eigenvector
    //vec3 ABCz = diag - evals.z;
    //vec3 ev2 = getEigenvector( ABCz, offdiag );
    vec3 ev2 = cross( ev0.xyz, ev1.xyz ); // as they are orthogonal

    // glyphs color and anisotropy
    float FA = getFA( evals );
    FA = clamp( FA, 0.0, 1.000 ); // filter out invalid FA values later
    gl_FrontColor = getColor( ev0.xyz, FA );

    // throw away glyphs whose FA is below threshold and whose eigenvalues are below threshold
    v_alphaBeta.w = 0.0;
    if ( ( FA < u_faThreshold ) || ( evals.z  < u_evThreshold ) )
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
        v_cs = 3.0 * ( evals.z )           / evalSum;

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

    /////////////////////////////////////////////////////////////////////////////////////////////
    // 4: bounding box calculations
    /////////////////////////////////////////////////////////////////////////////////////////////


    // some scaling
    float Sscaling = u_scaling;
    if ( u_unifyEV )
        Sscaling*=evals.x;

    // also scale them to let the maximum value be 1
    // HINT: eigenvalues are allready sorted, the largest eigenvalue is in evals.x
    evals.z = ( evals.x / evals.z ) / Sscaling;
    evals.y = ( evals.x / evals.y ) / Sscaling;
    evals.x = 1.0 / Sscaling;

    // and some precalculations for the scaling values
    float dimX = 1.0 / evals.z;
    float dimY = 1.0 / evals.y;
    float dimZ = 1.0 / evals.x;

    // an untransformed superquadric is bounded by a unit cube. The Eigenvalues are the radii of the ellipse/quadric and therefore deform the
    // unit cube. This new bounding cube is the defined by its eight vertices:
    mat4 rightVertices = mat4( vec4( dimX,  dimY,  dimZ, 1.0 ),
                               vec4( dimX,  dimY, -dimZ, 1.0 ),
                               vec4( dimX, -dimY,  dimZ, 1.0 ), 
                               vec4( dimX, -dimY, -dimZ, 1.0 ) );

    mat4 leftVertices = mat4(  vec4(-dimX,  dimY,  dimZ, 1.0 ),
                               vec4(-dimX,  dimY, -dimZ, 1.0 ),
                               vec4(-dimX, -dimY,  dimZ, 1.0 ), 
                               vec4(-dimX, -dimY, -dimZ, 1.0 ) );

    // rotationmatrix describing the coordinate system rotation corresponding to the eigenvectors
    mat4 glyphSystem = mat4( ( ev0.xyz ), 0.0,
                             ( ev1.xyz ), 0.0,
                             ( ev2.xyz ), 0.0,
                             0.0, 0.0, 0.0, 1.0 ); 
    mat4 glyphTransform = gl_ModelViewMatrix * glyphSystem;

    // transform the unit cube with modelview matrix (and glyph rotation matrix)
    rightVertices = glyphTransform * rightVertices;
    leftVertices  = glyphTransform * leftVertices;

    float bboxMaxX = max(
                        max( abs( rightVertices[0].x ), 
                        max( abs( rightVertices[1].x ),
                        max( abs( rightVertices[2].x ),
                             abs( rightVertices[3].x ) ) ) )
                    ,
                        max( abs( leftVertices[0].x ),
                        max( abs( leftVertices[1].x ),
                        max( abs( leftVertices[2].x ),
                             abs( leftVertices[3].x ) ) ) )
                    );

    float bboxMaxY = max(
                        max( abs( rightVertices[0].y ), 
                        max( abs( rightVertices[1].y ),
                        max( abs( rightVertices[2].y ),
                             abs( rightVertices[3].y ) ) ) )
                    ,
                        max( abs( leftVertices[0].y ),
                        max( abs( leftVertices[1].y ),
                        max( abs( leftVertices[2].y ),
                             abs( leftVertices[3].y ) ) ) )
                    );

       v_bboxMaxZ = max(
                        max( abs( rightVertices[0].z ), 
                        max( abs( rightVertices[1].z ),
                        max( abs( rightVertices[2].z ),
                             abs( rightVertices[3].z ) ) ) )
                    ,
                        max( abs( leftVertices[0].z ),
                        max( abs( leftVertices[1].z ),
                        max( abs( leftVertices[2].z ),
                             abs( leftVertices[3].z ) ) ) )
                    );

    // the original center translated
    bboxMaxX -= abs( glyphTransform[3].x );
    bboxMaxY -= abs( glyphTransform[3].y );
    v_bboxMaxZ -= abs( glyphTransform[3].z );
    
    /////////////////////////////////////////////////////////////////////////////////////////////
    // 4: now span the quad according to bounding box, so that the fragment shader can render
    //    the correctly transformed superquadric without clipped edges etc.
    /////////////////////////////////////////////////////////////////////////////////////////////

    // calculate propper bounding volume for this quadric (scale)
    vec4 p1 = vec4( bboxMaxX * gl_MultiTexCoord0.x,
                    bboxMaxY * gl_MultiTexCoord0.y, 0.0, 0.0 );
    gl_Position = ( gl_ModelViewProjectionMatrix * gl_Vertex ) + ( gl_ProjectionMatrix * p1 );
  
    // just to be sure to have propper clipping - we only clip complete glyphs at their center
    gl_ClipVertex = ( gl_ModelViewMatrix * gl_Vertex );// + p1;


    /////////////////////////////////////////////////////////////////////////////////////////////
    // 5: Transform light and plane as well as ray back to glyph space
    /////////////////////////////////////////////////////////////////////////////////////////////

    // NOTE: as each glyph has ben transformed using the modelview matrix "mv" and the glyphSystem matrix "g": the inverse transformation
    // ( transfer a vector from camera space back to glyph space) is (mv * g)^-1 = g^-1 * mv^-1

    mat4 inverseWorldTransform = transpose( glyphSystem ) * gl_ModelViewMatrixInverse;

    // calculate light direction once per quadric
    v_lightDir.xyz = normalize( ( inverseWorldTransform * gl_LightSource[0].position ).xyz );

    // the viewing direction for this vertex:
    v_viewDir.xyz = ( inverseWorldTransform * vec4( 0.0, 0.0, 1.0, 0.0 ) ).xyz;







    // scale texture coordinates to maintain size
    v_planePoint = vec4( gl_TexCoord[0].x * bboxMaxX, gl_TexCoord[0].y * bboxMaxY, 0.0, 1.0 );

    // apply transformation to quadric -> apply inverse transformation to ray
    v_planePoint = transpose(  1.0/determinant( glyphTransform ) * glyphTransform ) * v_planePoint;

    // apply scaling to the ray (eigenvalues used)
    v_planePoint.x *= evals.z;
    v_planePoint.y *= evals.y;
    v_planePoint.z *= evals.x;

    // this is the magic value, that describes the length of viewdir in the depth-buffer space
    // as it is aligned to the z-axis at the moment, it is the same everywhere
    v_scaleT = .50333333333333333333;






    // the plane point on the projection plane of the raytracer
    vec4 v_planePoint2 = vec4( gl_TexCoord[0].x, gl_TexCoord[0].y, 0.0, 1.0 );
    v_planePoint2 = transpose( glyphSystem ) * v_planePoint;
    
    //v_planePoint = v_planePoint2;
}

