attribute vec4 vPosition;
uniform mat4 p;    //projection view matrix

uniform float scale;  //the scale 
uniform vec2 pos;  //the location of circle 


void main()
{
   //these have to be in column major order.
   mat4 S = mat4(scale, 0.0, 0.0, 0.0,
                 0.0, scale, 0.0, 0.0, 
                 0.0, 0.0, 1.0, 0.0, 
                 0.0, 0.0, 0.0, 1.0);
   mat4 T = mat4(1.0, 0.0, 0.0, 0.0,
                 0.0, 1.0, 0.0, 0.0, 
                 0.0, 0.0, 1.0, 0.0, 
                 pos.x, pos.y, 0.0, 1.0);
   gl_Position =  p * T * S * vPosition;

}


/* another way to accomplish the same thing,
   set up a modelView matrix which is identity*T*S in the CPU then
   feed it into mv as defined on the next line.

   uniform mat4 mv;    //model view matrix
 
   Then changed the statmenet gl_Position =  p * T * S * vPosition;
   becomes gl_Position =  p * mv * vPosition;
*/
