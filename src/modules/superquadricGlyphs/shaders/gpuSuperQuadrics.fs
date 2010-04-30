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

#include "shadingTools.fs"

// commonly used variables
#include "gpuSuperQuadrics.varyings"

// tollerance value for float comparisons
float zeroTollerance=0.01;

#define RenderMode_Box
//#define RenderMode_BoundedBox
//#define RenderMode_Superquadric
//#define RenderMode_Ellipsoid

/////////////////////////////////////////////////////////////////////////////////////////////
// GPU Super Quadrics -- fragment shader -- sPow
//
// This function extends the "pow" function with features to handle negative base values.
/////////////////////////////////////////////////////////////////////////////////////////////
float sPow(float x, float y)
{
  if (y==0.0)
    return 1.0;
  else
    return pow(abs(x), y);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// GPU Super Quadrics -- fragment shader -- superquadric
//
// This function calculates allmost every value at a given point (the value of the
// superquadric function, gradient and derivate)
//
// Parameters:
// viewDir:     viewing direction vector
// planePoint:  point on projection plane
// t:           current point on ray
// ray:         returns position on ray
// gradient:    the gradient vector at current ray point
// sqd:         value of derived function at current ray point
//
// Returns:
//              The function value of the superquadrics function at current point
/////////////////////////////////////////////////////////////////////////////////////////////
float superquadric(vec3 viewDir, vec3 planePoint, float t, out vec3 ray, out vec3 gradient, out float sqd)
{
  ray = planePoint.xyz + t*viewDir.xyz;

  // those values will be needed multiple times ...
  float rayXYPow=sPow(ray.x, v_alphaBeta.x) + sPow(ray.y, v_alphaBeta.x);
  float rayZPow=sPow(ray.z, v_alphaBeta.y);

  // the value at given position
  float sq=sPow(rayXYPow, v_alphaBeta.z) + rayZPow  - 1.0;

  // calculate squadric value for current t

  // if we get a hit we need those values for calculating the gradient at the hit point
  // if we do not get a hit we use those values for calculating the derivation at the current point
  // for doing the next newton step

  /////////////////// HINT ///////////////////
  // If we derive sign(x)*pow(abs(x),y) we use product rule to get:
  //                    sign' * pow + sign * pow'
  // Well the derived sign function is nothing else than the unit impulse delta.
  // It is              delta<>0 iff x=0 
  // And also           x=0 -> pow(x, y)=0
  // so delta(x)*pow(x) will allways be 0 ... we can drop it
  //
  // ==> y * sign(x)* sPow(x, y-1.0);

   
  float a =sign(rayXYPow) * sPow(rayXYPow, v_alphaBeta.z-1.0);
  float b1=sign(ray.x) * sPow(ray.x, v_alphaBeta.x - 1.0);

  float b2=sign(ray.y) * sPow(ray.y, v_alphaBeta.x - 1.0);
  float c =v_alphaBeta.y  * sign(ray.z) * sPow(ray.z, v_alphaBeta.y  - 1.0);

  // now we can reuse some values to calculate the gradient vector at the hit point
  gradient=vec3(v_alphaBeta.y * a * b1, v_alphaBeta.y * a * b2, c );

  // calculate the derived function, reuse as much previously calculated values as possible
  sqd=dot(gradient, viewDir);

  // thats it, return value at current point
  return sq;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// GPU Super Quadrics -- fragment shader -- superquadric
//
// This function is the "light" version of the above one. It just calculates the result of
// the superquadrics function.
//
// Parameters:
// viewDir:     viewing direction vector
// planePoint:  point on projection plane
// t:           current point on ray
//
// Returns:
//              The function value of the superquadrics function at current point
/////////////////////////////////////////////////////////////////////////////////////////////
float superquadric(vec3 viewDir, vec3 planePoint, float t)
{
  vec3 ray = planePoint.xyz + t*viewDir.xyz;

  // those values will be needed multiple times ...
  float rayXYPow=sPow(ray.x, v_alphaBeta.x) + sPow(ray.y, v_alphaBeta.x);
  float rayZPow=sPow(ray.z, v_alphaBeta.y);

  // the value at given position
  float sq=sPow(rayXYPow, v_alphaBeta.z) + rayZPow  - 1.0;

  // thats it, return value at current point
  return sq;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// GPU Super Quadrics -- fragment shader -- findBBoxIntersection
//
// This function tries to find an intersection from the given ray with the bounding box
// of the glyph.
//
// Parameters:
// viewDir:     viewing direction vector
// planePoint:  point on projection plane
// bboxT:       the ray parameter if intersection was found
//
// Returns:
//              true if there has been an intersection
/////////////////////////////////////////////////////////////////////////////////////////////
bool findBBoxIntersection(vec3 viewDir, vec3 planePoint, out float bboxT, out vec3 normal
#ifdef RenderMode_BoundedBox  // just add this parameter if we want a bounding box silhouette
, out float silhouette
#endif
    )
{

  bboxT=0.0;

  float bbPlaneDistance;
  vec3 bbPlanePoint;
  
  // initialize to avoid compiler warning
  normal=vec3(1.0, 0.0, 0.0);

  // x plane
  if (viewDir.x>0.0)
  {
    // find the t on which the ray has the value x=1
    bbPlaneDistance=(1.0 - planePoint.x)/viewDir.x;
    // get the point coordinates
    bbPlanePoint=(planePoint + (bbPlaneDistance*viewDir)).xyz;
    bboxT=bbPlaneDistance;

    // normal for this plane
    normal=vec3(1.0, 0.0, 0.0);

    // check if the point is also in the y and z borders
    if ((abs(bbPlanePoint.y)<=1.0) && (abs(bbPlanePoint.z)<=1.0))
    {
#ifdef RenderMode_BoundedBox
      // the most nearest border is used for silhouette rendering
      silhouette=max(abs(bbPlanePoint.y), abs(bbPlanePoint.z));
#endif
      return true;
    }
  }
  else if (viewDir.x<0.0)
  {
    bbPlaneDistance=(-1.0 - planePoint.x)/viewDir.x;
    bbPlanePoint=(planePoint + (bbPlaneDistance*viewDir)).xyz;
    bboxT=bbPlaneDistance;
    normal=vec3(-1.0, 0.0, 0.0);
    
    if ((abs(bbPlanePoint.y)<=1.0) && (abs(bbPlanePoint.z)<=1.0))
    {
#ifdef RenderMode_BoundedBox
      silhouette=max(abs(bbPlanePoint.y), abs(bbPlanePoint.z));
#endif
      return true;
    }
  }

  // y plane
  if (viewDir.y>0.0)
  {
    bbPlaneDistance=(1.0 - planePoint.y)/viewDir.y;
    bbPlanePoint=(planePoint + (bbPlaneDistance*viewDir)).xyz;
    bboxT=bbPlaneDistance;
    normal=vec3(0.0, 1.0, 0.0);

    if ((abs(bbPlanePoint.x)<=1.0) && (abs(bbPlanePoint.z)<=1.0))
    {
#ifdef RenderMode_BoundedBox
      silhouette=max(abs(bbPlanePoint.x), abs(bbPlanePoint.z));
#endif
      return true;
    }
  }
  else if (viewDir.y<0.0)
  {
    bbPlaneDistance=(-1.0 - planePoint.y)/viewDir.y;
    bbPlanePoint=(planePoint + (bbPlaneDistance*viewDir)).xyz;
    bboxT=bbPlaneDistance;
    normal=vec3(0.0, -1.0, 0.0);

    if ((abs(bbPlanePoint.x)<=1.0) && (abs(bbPlanePoint.z)<=1.0))
    {
#ifdef RenderMode_BoundedBox
      silhouette=max(abs(bbPlanePoint.x), abs(bbPlanePoint.z));
#endif
      return true;
    }
  }

  // z plane
  if (viewDir.z>0.0)
  {
    bbPlaneDistance=(1.0 - planePoint.z)/viewDir.z;
    bbPlanePoint=(planePoint + (bbPlaneDistance*viewDir)).xyz;
    bboxT=bbPlaneDistance;
    normal=vec3(0.0, 0.0, 1.0);

    if ((abs(bbPlanePoint.x)<=1.0) && (abs(bbPlanePoint.y)<=1.0))
    {
#ifdef RenderMode_BoundedBox
      silhouette=max(abs(bbPlanePoint.x), abs(bbPlanePoint.y));
#endif
      return true;
    }
  }
  else if (viewDir.z<0.0)
  {
    bbPlaneDistance=(-1.0 - planePoint.z)/viewDir.z;
    bbPlanePoint=(planePoint + (bbPlaneDistance*viewDir)).xyz;
    bboxT=bbPlaneDistance;
    normal=vec3(0.0, 0.0, -1.0);

    if ((abs(bbPlanePoint.x)<=1.0) && (abs(bbPlanePoint.y)<=1.0))
    {
#ifdef RenderMode_BoundedBox
      silhouette=max(abs(bbPlanePoint.x), abs(bbPlanePoint.y));
#endif
      return true;
    }
  }

  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// GPU Super Quadrics -- fragment shader -- main
/////////////////////////////////////////////////////////////////////////////////////////////
void main(void)
{

gl_FragColor = vec4( abs( v_planePoint.xyz ), 1.0 );
//return;

  // filter out glyphs whose anisotropy is smaller than the threshold or where the eigenvalues
  // are below the threshold (alphaBeta.w is != if this is the case)
  if( v_alphaBeta.w > 0.0 )
  {
    //discard;
  }
 
  /////////////////////////////////////////////////////////////////////////////////////////////
  // 1: try to find a goot start value for newton iteration
  /////////////////////////////////////////////////////////////////////////////////////////////

  // well the following value is quite empiric but it works well with squadrics whose beta<=alpha<=1.0
  // HINT: if the ray hits the surface of the glyph lastT will allways be negative!
  float lastT=0.0;
  int numNewtonLoops=10;

  // this vector stores the gradient if there has been a hit
  vec3 grad=vec3(0.0, 0.0, 1.0);

  // some vars that will be needed later
  // got a hit?
  bool hit=false;
  vec3 hitPoint=vec3(0.0, 0.0, 0.0);

#ifdef RenderMode_BoundedBox // Render Box with silhouette

  // the bounded box needs the "distance to the border" value
  float silhouette=0.0;

  // use bounding box intersection to find good start value
  if (!findBBoxIntersection(v_viewDir.xyz, v_planePoint.xyz, lastT, grad, silhouette))
    discard;

  // findBBoxIntersection allready did the job
  hit=true;

#else
 
  // the following glyphs will not need the silhouette

  // use bounding box intersection to find good start value
  if (!findBBoxIntersection(v_viewDir.xyz, v_planePoint.xyz, lastT, grad))
  {
      gl_FragColor = vec4( 1.0, 1.0, 1.0, 1.0 );
    return;
    discard;
  }

#ifdef RenderMode_Superquadric // Superquadric based rendermode

  /////////////////////////////////////////////////////////////////////////////////////////////
  // 2: newton iteration to determine roots of the superquadric-ray intersection
  /////////////////////////////////////////////////////////////////////////////////////////////

  // now try to calculate the intersection of the given ray with the superquadric.
  // here we are using newtons iterative method for finding roots

  // the iteration loop (NOTE: due to the fact that the shaders do not support an at compile time unknown loop count we set it to
  // this quite empiric value (which works well (for at least the squadrics with beta<=alpha<=1.0))
  for (int i=0; i<numNewtonLoops; i++)
  {
    // calculate all needed values
    float sqd=0.0;
    float sq=superquadric(v_viewDir.xyz, v_planePoint.xyz, lastT, hitPoint, grad, sqd);

    // new ray parameter
    float newT=lastT-(sq / sqd);

    // near enough?
    // or has t not changed much since last iteration?
    if (!hit && ( (abs(sq)<=zeroTollerance) || (abs(newT - lastT) < zeroTollerance)))
    {
      hit=true;
      break;
    }

    // if the ray parameter starts to jump around (it should get smaller step by step (because lastT is negative))
    // NOTE: this speeds up rendering at rays that will miss the glyph (round about 50% faster!)
    if (newT > lastT)
      break;

    // not near enough -> another newton step
    lastT=newT;
  }

#endif

#ifdef RenderMode_Ellipsoid // Render ellipsoids

  /////////////////////////////////////////////////////////////////////////////////////////////
  // 2: solve ellipsoid equation to determine roots of the intersection
  /////////////////////////////////////////////////////////////////////////////////////////////

  float A = dot(v_viewDir.xyz, v_viewDir.xyz);
  float B = 2.0 * dot(v_viewDir.xyz, v_planePoint.xyz);
  float C = dot(v_planePoint.xyz, v_planePoint.xyz) - 1.0;

  // try to solve At^2 + Bt + C = 0
  float discriminant = (B * B) - (4.0 * A * C);
  
  // no solution
  if(discriminant <= -0.0)
  {
    discard;
  }

  // there will be a solution
  hit=true; 

  // use solution formula
  float twoAinv=1./(2.*A);
  float root=sqrt( discriminant );
  float t1=(-B + root)*twoAinv;
  float t2=(-B - root)*twoAinv;

  // get the nearer solution
  // both t will be < 0 so take the larger t
  float firsthit;
  if(t1 > t2)
    firsthit = t1;
  else
    firsthit = t2;
  
  // on a sphere surface the normal is allways the vector from the middle point (in our case (0,0,0))
  // to the surface point
  grad = v_planePoint.xyz + v_viewDir.xyz * firsthit;

#endif

#ifdef RenderMode_Box // Render a box 

  /////////////////////////////////////////////////////////////////////////////////////////////
  // 2: solve ray - plane intersections to determine intersections with the cube
  /////////////////////////////////////////////////////////////////////////////////////////////

  // findBBoxIntersection allready did the job
  hit=true;

#endif

#endif  // #ifdef RenderMode_BoundedBox


  /////////////////////////////////////////////////////////////////////////////////////////////
  // 3: draw or discard the pixel
  /////////////////////////////////////////////////////////////////////////////////////////////

  if (hit)
  {
    //gl_FragDepth = gl_FragCoord.z - 1.0/3.5 * lastT;

    // draw shaded pixel
        gl_FragColor = blinnPhongIllumination(
           // material properties
           gl_Color.rgb * 0.2,                    // ambient color
           gl_Color.rgb * 2.0,                    // diffuse color
           gl_Color.rgb,                          // specular color
           30.0,                                  // shininess

           // light color properties
	  	   gl_LightSource[0].diffuse.rgb,         // light color
           gl_LightSource[0].ambient.rgb,         // ambient light

           // directions
           normalize(grad),                       // normal
           v_viewDir.xyz,                           // viewdir
           v_lightDir.xyz);                         // light direction

#ifdef RenderMode_BoundedBox

    // add the silhouette
    if (silhouette>0.9)
      gl_FragColor=vec4(0.0, 0.0, 0.0, 0.0);

#endif

  }
  else // no hit: discard
    // want to see the bounding box? uncomment this line
    gl_FragColor=vec4(0.5, 0.5, 0.5, 1.0);
    //discard;
}

