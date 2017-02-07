attribute vec4 vPosition;

//set up to pass color to fshader.
attribute vec4 vColor;
varying vec4 fColor;

uniform mat4 p;    //projection view matrix
uniform mat4 mv;    //model view matrix


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
   gl_Position =  p * mv * vPosition;

   fColor = vColor;

}
