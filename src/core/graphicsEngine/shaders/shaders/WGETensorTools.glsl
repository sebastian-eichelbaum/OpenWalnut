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

#ifndef WGETENSORTOOLS_GLSL
#define WGETENSORTOOLS_GLSL

#version 120

// (c) 2007 by Mario Hlawitschka

// tensors have to be stored in the following way:
// diag.x = Dxx     Diagonal elements are stored in one vector
// diag.y = Dyy
// diag.z = Dzz
// offdiag.x = Dyz  Off diagonal elements are stored in another vector
// offdiag.y = Dxz      where all components are stored in that location
// offdiag.z = Dxy      that is indexed by the coordinate that is not in the tensor index


// compute the eigenvalues of a tensor
// (Hasan et al. 2001)
vec3 getEigenvaluesHasan( vec3 diag, vec3 offdiag )
{
  const float PiThird = 3.14159265358979323846264 / 3.;
  float I1 = diag.x + diag.y + diag.z;
  float I2 = diag.x * diag.y + diag.x * diag.z + diag.y * diag.z - dot( offdiag, offdiag );
  float I3 = diag.x * diag.y * diag.z + 2. * offdiag.x * offdiag.y * offdiag.z
      - ( diag.x * offdiag.x * offdiag.x + diag.y * offdiag.y * offdiag.y + diag.z * offdiag.z * offdiag.z );

  const float third = 1. / 3.;
  float I1third = I1 * third;
  float I1thirdsqr = I1third * I1third;
  float I2third = I2 * third;
  float v  = I1thirdsqr - I2third;

  vec3 lambda;


  // debug
  //if( v < 0.000001 ) lambda = vec3(1.,0.,0.);
  //else
  {
      float s  = I1thirdsqr * I1third - I1 * I2/6. + 0.5 * I3;

      // for real eigenvalues: v>0, s^2 < v^3
      float sqrtv = sqrt( v );
      float phi= acos( s / ( v * sqrtv ) ) * third;

      float sqrtv2 = 2. * sqrtv;

      // due to the cosine function and the fact, that 0 <= phi <= pi/3
      // it is obvious that the eigenvalues need no further sorting
      lambda.x = I1third + sqrtv2 * cos( phi );
      lambda.y = I1third - sqrtv2 * cos( PiThird + phi );
      lambda.z = I1third - sqrtv2 * cos( PiThird - phi );
  }
  return lambda;
}


// Method similar to the above by Hasan proposed by Cardano et al.
// implementation is more stable than the above one, so use this as
// default in all applications
float sqr( float a )
{
    return a * a;
}

vec3 getEigenvaluesCardano( in vec3 diag, in vec3 offdiag )
{
  const float M_SQRT3 = 1.73205080756887729352744634151;
  float de = offdiag.z * offdiag.x;
  float dd = sqr( offdiag.z );
  float ee = sqr( offdiag.x );
  float ff = sqr( offdiag.y );
  float m  = diag.x + diag.y + diag.z;
  float c1 = diag.x * diag.y + diag.x * diag.z + diag.y * diag.z
             - ( dd + ee + ff );
  float c0 = diag.z * dd + diag.x * ee + diag.y * ff - diag.x * diag.y * diag.z - 2. * offdiag.y * de;

  float p, sqrt_p, q, c, s, phi;
  p = sqr( m ) - 3. * c1;
  q = m * ( p - ( 3. / 2. ) * c1 ) - ( 27. / 2. ) * c0;
  sqrt_p = sqrt( abs( p ) );

  phi = 27. * ( 0.25 * sqr( c1 ) * ( p - c1 ) + c0 * ( q + 27. / 4. * c0 ) );
  phi = ( 1. / 3. ) * atan( sqrt( abs( phi ) ), q );

  c = sqrt_p * cos( phi );
  s = ( 1. / M_SQRT3 ) * sqrt_p * sin( phi );

  vec3 w;
  w[2] = ( 1. / 3. ) * ( m - c );
  w[1] = w[2] + s;
  w[0] = w[2] + c;
  w[2] -= s;
  return w;
}

vec3 getEigenvalues( in vec3 diag, in vec3 offdiag )
{
  return getEigenvaluesCardano( diag, offdiag );
}

float getMajorEigenvalue( vec3 diag, vec3 offdiag )
{
  return getEigenvalues( diag, offdiag )[0];
}

/**
 * use the above if more than one eigenvector is required and only use this if you need exactly one value
 */
float getMajorEigenvalueOld( vec3 diag, vec3 offdiag )
{
  const float PiThird = 3.14159265358979323846264/3.;
  float I1 = diag.x + diag.y + diag.z;
  float I2 = diag.x * diag.y + diag.x * diag.z + diag.y * diag.z - dot( offdiag, offdiag );
  float I3 = diag.x * diag.y * diag.z + 2. * offdiag.x * offdiag.y * offdiag.z
      - ( diag.x * offdiag.x * offdiag.x + diag.y * offdiag.y * offdiag.y + diag.z * offdiag.z * offdiag.z );

  const float third = 1. / 3.;
  float I1third = I1 * third;
  float I1thirdsqr = I1third * I1third;
  float I2third = I2 * third;
  float v  = I1thirdsqr - I2third;

  vec3 lambda;

  float s  = I1thirdsqr * I1third - I1 * I2 / 6. + 0.5 * I3;

  // for real eigenvalues: v>0, s^2 < v^3
  float sqrtv = sqrt( v );
  float phi= acos( s / ( v * sqrtv ) ) * third;

  float sqrtv2 = 2. * sqrtv;

  // due to the cosine function and the fact, that 0 <= phi <= pi/3
  // it is obvious that the eigenvalues need no further sorting
  return I1third + sqrtv2 * cos( phi );
}

// compute vector direction depending on information computed by getEigenvalues
// before (Hasan et al. 2001)
vec3 getEigenvector( vec3 ABC /*diag without eigenalue i*/, vec3 offdiag )
{
  vec3 vec;
  vec.x = ( offdiag.z * offdiag.x - ABC.y * offdiag.y ) * ( offdiag.y * offdiag.x - ABC.z * offdiag.z ); // FIXME
  //< last component is missing in the paper! there is only a Dx?
  vec.y = ( offdiag.y * offdiag.x - ABC.z * offdiag.z ) * ( offdiag.y * offdiag.z - ABC.x * offdiag.x );
  vec.z = ( offdiag.z * offdiag.x - ABC.y * offdiag.y ) * ( offdiag.y * offdiag.z - ABC.x * offdiag.x );

  return normalize( vec );
}

// FA as used everywhere (Kondratieva et al 2005)
// computes FA from eigenvalues
float getFA( vec3 evalues )
{
  float sqrt32 = sqrt( 3. / 2. ); // would make this const, if compiler let me
  float I1 = evalues.x + evalues.y + evalues.z;
  float mu = I1 / 3.;
  vec3 deriv;
  deriv.x = ( evalues.x - mu );
  deriv.y = ( evalues.y - mu );
  deriv.z = ( evalues.z - mu );
  float FA = sqrt32 * length( deriv ) / length( evalues );
  return FA;
}

// Compute FA without doing eigenvalue decomposition
float getFA( vec3 diag, vec3 offdiag )
{
  float cross = dot( offdiag, offdiag );
  float j2 = diag.x * diag.y + diag.y * diag.z + diag.z * diag.x - cross;
  float j4 = dot( diag, diag ) + 2. * cross;
  return sqrt( ( j4 - j2 ) / j4 );
}


// simple default color map, but with precomputed FA values
vec4 getColor( vec3 dir, float FA )
{
  vec4 color;
  //dir = normalize( dir );
  //dir = clamp( dir, 1., 1.);
  color = vec4( abs( dir ) * FA, 1. );
  return color;
}

// compute the default RGB color scheme
vec4 getColor( vec3 evec, vec3 evalues )
{
  float fa = getFA( evalues );
  return getColor( evec, fa );
}

// make things transparent, proportional to FA
vec4 getColorAlpha( vec3 evec, vec3 evalues )
{
  float fa = getFA( evalues );
  //evec = clamp( evec, 1., 1.);
  return vec4( abs( evec ), 1. ) * fa;
}

#endif // WGETENSORTOOLS_GLSL

