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
//--------------------------------------------------------------------------

#version 120

mat4 mv;
	
void main ()
{   
    //gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	//gl_Position = ftransform();

	gl_FrontColor = gl_Color;

    //remove translation
    //matrixCompMult(rt, gl_ModelViewProjectionMatrix)
    mat4 rt = mat4( 1.0, 1.0, 1.0, 1.0,
                    1.0, 1.0, 1.0, 1.0,
                    1.0, 1.0, 1.0, 1.0,
                    0.0, 0.0, 0.0, 1.0
                    );
    //mat4 mv = matrixCompMult(rt, gl_ModelViewProjectionMatrix);
    mat4 mv = gl_ModelViewProjectionMatrix;

    vec4 e1 = vec4(1.0, 0.0, 0.0, 0.0);
    vec4 e2 = vec4(0.0, 1.0, 0.0, 0.0);
    vec4 e3 = vec4(0.0, 0.0, 1.0, 0.0);
    float l1 = 1/length( e1*mv);
    float l2 = 1/length( e2*mv);
    float l3 = 1/length( e3*mv); 
/*    float l1 = 1/length( mv*e1);
    float l2 = 1/length( mv*e2);
    float l3 = 1/length( mv*e3);
*/
    

    mat4 temp = mat4( l1, 0.0, 0.0, 0.0,
                      0.0, l2, 0.0, 0.0,
                      0.0, 0.0, l3, 0.0,
                      0.0, 0.0, 0.0, 1.0);
    
    vec4 v = temp * gl_Vertex;
 
    mv = matrixCompMult(rt, gl_ModelViewProjectionMatrix);

    //mv = matrixCompMult(mv, temp);
    //mv *= temp;

    gl_Position =  matrixCompMult(rt, gl_ModelViewProjectionMatrix) * gl_Vertex;
    //gl_Position = mv * v;
}


