/* Name: R. SHore
   Date: 3-20-2015
   Class: CSC-3360

   Sample code to draw two circles on the screen and have them move.

   A simple circleType class is established.

   This code uses one collection of points for both circles.

   The scale (radius) and location of the circle is fed into 
   the vshader.  At that time, the original points are handed to 
   the vshader for displaying.  The vshader does all of the work
   of scalling and translating.  The best part, the points only have to
   be loaded one time into the GL_ARRAY_BUFFER.

*/

#include "../include/Angel.h"

using namespace std;

//shader stuff
GLint projLoc;
GLint scaleLoc;
GLint positionLoc; 
GLint vLoc;


typedef vec2 pt_type;

//holds the points for one circle at the origin with radius 1
int NumPoints;  //number of iterations to generate the points
pt_type *circle_pts;


//world coordinates
const GLfloat wld_bot=-1.0, wld_top=1.0;
const GLfloat wld_left=-1.0, wld_right=1.0;
const GLfloat wld_near=-1.0, wld_far=1.0;
GLfloat cwld_bot=-1.0, cwld_top=1.0;
GLfloat cwld_left=-1.0, cwld_right=1.0;
GLfloat cwld_near=-1.0, cwld_far=1.0;

//A simple class to hold data and methods for the circle
class circleType {
   private: 
      pt_type loc;
      vec2 dir;  //direction, length is speed.
      GLfloat radius;
   public:
      circleType(vec2 in_loc, vec2 in_dir, GLfloat in_rad)
      {
         loc = in_loc; 
         dir = in_dir;
         radius = in_rad;     
      }
      GLfloat getScale(void)
      {
         //using the radius as the scale.
         return radius; 
      }
      vec2 getLoc(void)
      {
         return loc;
      }
      vec2 takeAstep()
      {
         //circle location is updated every time this method is called.
         loc+=dir;
         return loc;
      }
};

//Circles to be drawn, myinit() sets up the two circles
circleType circle1(vec2(0.5,0.5),vec2(-0.006,-0.006),0.2);
circleType circle2(vec2(-0.8,-0.4),vec2(0.004,0.006),0.15);

//----------------new stuff above this line--------------
int winwidth=512, winheight=512;


//----------------------------------------------------------------------------
//A recursive function to find the points in the UNIT circle
void makeCirclePoints(pt_type a, pt_type c, pt_type points[], int &n, int iterations)
{
   if(iterations == 0) {
      points[n++] = a;
   } else {
      pt_type b; 
      //vector overloaded opeartions in next two lines
      b = (a+c)/2; 
      b = normalize(b);

      makeCirclePoints(a, b, points, n, iterations-1); 
      makeCirclePoints(b, c, points, n, iterations-1); 
   }
}

//----------------------------------------------------------------------------
pt_type *generateCirclePoints(int iterations)
{
   //determine the size of the array based on the number of iterations of 
   //the recursive function.  NOTE: for each line segment that is cut,
   //you will end up 4 smaller line segments.
   NumPoints = (int)(pow(2,iterations+2));

   pt_type *points = new pt_type[NumPoints];
   int n=0;

   // Specifiy the vertices for a triangle
   pt_type vertices[4] = {
      pt_type( 0.0, 1.0), 
      pt_type( -1.0, 0.0 ), 
      pt_type( 0.0, -1.0 ), 
      pt_type( 1.0, 0.0)
   };

   makeCirclePoints(vertices[0], vertices[1], points, n, iterations); 
   makeCirclePoints(vertices[1], vertices[2], points, n, iterations); 
   makeCirclePoints(vertices[2], vertices[3], points, n, iterations); 
   makeCirclePoints(vertices[3], vertices[0], points, n, iterations); 

   return points;
}

//----------------------------------------------------------------------------
//projLoc, scaleLoc, positionLoc and vLoc are define as global 
//variables since multiple callback functions will need to access them.
void initGLBuffer_and_shaders(pt_type points[], int NumPoints)
{
   GLuint buffer;
   GLuint vao;
   GLuint program;
   mat4 p;
   // Create a vertex array object
#ifdef __APPLE__
   glGenVertexArraysAPPLE( 1, &vao );
   glBindVertexArrayAPPLE( vao );
#else
   glGenVertexArrays( 1, &vao );
   glBindVertexArray( vao );
#endif


   // Create and initialize a buffer object
   glGenBuffers( 1, &buffer );
   glBindBuffer( GL_ARRAY_BUFFER, buffer );
   glBufferData( GL_ARRAY_BUFFER, NumPoints * sizeof(pt_type), points , GL_STATIC_DRAW );

   // Load shaders and use the resulting shader program
   program = InitShader( "vshader.glsl", "fshader.glsl" );
   glUseProgram( program );

   //Get the locations of transformation matrices from the vshader
   projLoc = glGetUniformLocation(program, "p");
   //Get the locations of the scale and position from the vshader
   //These will be filled as the program runs, the vshader will
   //use these to values to correctly size and position the circle.
   scaleLoc = glGetUniformLocation(program, "scale");
   positionLoc = glGetUniformLocation(program, "pos");


   // Initialize the vertex position attribute from the vertex shader
   vLoc = glGetAttribLocation( program, "vPosition" );
   glEnableVertexAttribArray( vLoc );
   glVertexAttribPointer( vLoc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

   glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
}

//----------------------------------------------------------------------------
/*NOTE: As noted in the vshader.glsl comments, you could
  1) have two methods in the circleType class to give you 
     appropriate scale and translate matrices.  These 
     are really simple methods.
  2) establish a model view matrix, mv, which is identity*S*T
     and feed it into the shader instead of feeding the scale and
     location to let the vshader.  
  Not sure if there is much difference between this and what 
  the code currently does.
*/
void display( void )
{
   glClear( GL_COLOR_BUFFER_BIT );     // clear the window
   glUniform1f(scaleLoc,circle1.getScale());
   glUniform2fv(positionLoc,1,circle1.getLoc());
   glDrawArrays( GL_LINE_LOOP, 0, NumPoints );    // draw the points

   glUniform1f(scaleLoc,circle2.getScale());
   glUniform2fv(positionLoc,1,circle2.getLoc());
   glDrawArrays( GL_LINE_LOOP, 0, NumPoints );    // draw the points

   glutSwapBuffers();
}

//----------------------------------------------------------------------------

void keyboard( unsigned char key, int x, int y )
{
   switch ( key ) {
      case 'Q':
      case 'q':
          exit( EXIT_SUCCESS );
          break;
   }
}

//update the location of a circle and refresh the display
//net effect, circles will move across the screen by taking 
//one step based on the direction vector within the circleType class.
void myIdle()
{
   circle1.takeAstep(); 
   circle2.takeAstep(); 
   glutPostRedisplay();           
}
//----------------------------------------------------------------------------

/* myReshape is called when the window is resized.
 * w and h are the new pixel width and height of the window
 */
void myReshape(GLint w, GLint h)
{
   mat4 p;
   glViewport(0,0,w,h);
   winwidth = w;
   winheight = h;

   if(w<=h) {
      cwld_bot = wld_bot*(GLfloat)h/w;
      cwld_top = wld_top*(GLfloat)h/w;
   } else {
      cwld_left = wld_left*(GLfloat)w/h;
      cwld_right = wld_right*(GLfloat)w/h;
   }
   p = Ortho(cwld_left, cwld_right, cwld_bot, cwld_top, cwld_near, cwld_far);
   glUniformMatrix4fv(projLoc, 1, GL_TRUE, p);

}

void myinit()
{
   mat4 p;
   glClearColor(1.0,1.0,1.0,0.0);  //clear color is white
   glColor3f(0.0,0.0,0.0);         //fill color set to black

   /* set up standard orthogonal view with clipping box
      as cube of side 2 centered at origin
      This is default view and these statements could be removed. */
   p = Ortho(wld_left, wld_right, wld_bot, wld_top, wld_near, wld_far);
   glUniformMatrix4fv(projLoc, 1, GL_TRUE, p);
}


int main( int argc, char **argv )
{
   glutInit( &argc, argv );
   glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE);
   glutInitWindowSize( winwidth, winheight );

   glutCreateWindow( "Two circle movement - shader version" );

#ifndef __APPLE__
   glewExperimental = GL_TRUE;
   glewInit();
#endif

   circle_pts = generateCirclePoints(3);
   initGLBuffer_and_shaders(circle_pts, NumPoints);
   
   myinit();

   glutDisplayFunc(display);
   glutReshapeFunc(myReshape);
   glutKeyboardFunc(keyboard);
   glutIdleFunc(myIdle);

   cout << endl;
   cout << "===========================" << endl;
   cout << "Use Q|q to quit the program" << endl;
   cout << "===========================" << endl;
   glutMainLoop();
   return 0;
}
