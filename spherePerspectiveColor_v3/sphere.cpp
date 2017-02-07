/* Name: R. SHore
   Date: 3-20-2015
   Class: CSC-3360

*/

#include<stdio.h>
#include "../include/Angel.h"

using namespace std;

//shader stuff
GLint projLoc;
GLint mvLoc;
GLint scaleLoc;
GLint positionLoc; 

typedef vec4 pt_type; 
typedef vec4 color4; 
typedef vec3 color3;

//holds the points for one sphere at the origin with radius 1
//each point needs a color so sphere_colors are insync with sphere_pts
int NumPoints;  //number of iterations to generate the points
pt_type *sphere_pts;
pt_type *sphere_normals;
vec4 *sphere_colors;


//world coordinates
const GLfloat wld_bot=-1.0, wld_top=1.0;
const GLfloat wld_left=-1.0, wld_right=1.0;
const GLfloat wld_near=0.5, wld_far=5.0;
GLfloat cwld_bot=-1.0, cwld_top=1.0;
GLfloat cwld_left=-1.0, cwld_right=1.0;
GLfloat cwld_near=0.5, cwld_far=5.0;

// Viewing transformation parameters

GLfloat radius = 3.0;
GLfloat theta = 0.0;
GLfloat phi = 0.0;

const GLfloat  dr = 5.0 * DegreesToRadians;

mat4 p, mv;
vec3 eye;
vec3 at = vec3(0.0, 0.0, 0.0);
vec3 up = vec3(0.0, 1.0, 0.0);

//----------------new stuff above this line--------------
int winwidth=512, winheight=512;

//----------------------------------------------------------------------------
void makeTrianglePoints(pt_type a, pt_type b, pt_type c, pt_type points[])
{
   static int k=0;
   points[k++] = a;
   points[k++] = b;
   points[k++] = c;
}

pt_type unit( const pt_type& p )
{
   float len = p.x*p.x + p.y*p.y + p.z*p.z;
   pt_type t;
   if ( len > DivideByZeroTolerance ) {
      t = p / sqrt(len);
      t.w = 1.0;
   }
   return t;
}

//A recursive function to find the points in the UNIT sphere
void makeSpherePoints(pt_type a, pt_type b, pt_type c, pt_type points[], int iterations)
{
   pt_type v1, v2, v3;
   if (iterations == 0)
      makeTrianglePoints(a, b, c, points);
   else {
      v1 = unit((a + b)/2);
      v2 = unit((a + c)/2);
      v3 = unit((b + c)/2);
      makeSpherePoints(a ,v2, v1, points, iterations-1);
      makeSpherePoints(c ,v3, v2, points, iterations-1);
      makeSpherePoints(b ,v1, v3, points, iterations-1);
      makeSpherePoints(v1 ,v2, v3, points, iterations-1);
   }
}

//----------------------------------------------------------------------------
pt_type *generateSpherePoints(int iterations)
{
   //determine the size of the array based on the number of iterations of 
   //the recursive function.  NOTE: for each triangle segment that is cut,
   //you will end up 4 smaller triangles segments with 3 pts per triangle.
   NumPoints = 3*(int)(pow(4,iterations+1));

   pt_type *points = new pt_type[NumPoints];
   int n=iterations;

   // Specifiy the vertices for a triangle
   pt_type vertices[4] = {
      pt_type( 0.0, 0.0, 1.0, 1.0), 
      pt_type( 0.0, 2*sqrt(2)/3.0, -1/3.0, 1.0 ), 
      pt_type( -sqrt(6)/3.0, -sqrt(2)/3.0, -1/3.0, 1.0 ), 
      pt_type( sqrt(6)/3.0, -sqrt(2)/3.0, -1/3.0, 1.0)
   };

   makeSpherePoints(vertices[0], vertices[1], vertices[2], points, n);
   makeSpherePoints(vertices[3], vertices[2], vertices[1], points, n);
   makeSpherePoints(vertices[0], vertices[3], vertices[1], points, n);
   makeSpherePoints(vertices[0], vertices[2], vertices[3], points, n);

   return points;
}

//----------------------------------------------------------------------------
//projLoc, scaleLoc, positionLoc and vLoc are define as global 
//variables since multiple callback functions will need to access them.
void initGLBuffer_and_shaders(pt_type points[], vec4 colors[], vec4 normals[], int NumPoints)
{
   GLuint buffer;
   GLuint vao;
   GLuint program;
   GLint vLoc;
   GLint vNormal;
   GLint vColor;
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
   unsigned int vertsBytes = NumPoints * sizeof(pt_type);
   unsigned int colorBytes = NumPoints*sizeof(vec4);
   unsigned int normalBytes = NumPoints*sizeof(vec4);
   unsigned int buffSize = vertsBytes + normalBytes + colorBytes; 
   glGenBuffers( 1, &buffer );
   glBindBuffer( GL_ARRAY_BUFFER, buffer );
   glBufferData( GL_ARRAY_BUFFER, buffSize, NULL , GL_STATIC_DRAW );
   glBufferSubData( GL_ARRAY_BUFFER, 0, vertsBytes, points );
   glBufferSubData( GL_ARRAY_BUFFER, vertsBytes, colorBytes, colors );
   glBufferSubData( GL_ARRAY_BUFFER, vertsBytes+colorBytes, normalBytes, normals );

   // Load shaders and use the resulting shader program
   program = InitShader( "vshader.glsl", "fshader.glsl" );
   glUseProgram( program );

   //Get the locations of transformation matrices from the vshader
   projLoc = glGetUniformLocation(program, "p");
   mvLoc = glGetUniformLocation(program, "mv");
   //Get the locations of the scale and position from the vshader
   //These will be filled as the program runs, the vshader will
   //use these to values to correctly size and position the sphere.
   scaleLoc = glGetUniformLocation(program, "scale");
   positionLoc = glGetUniformLocation(program, "pos");


   // Initialize the vertex position attribute from the vertex shader
   vLoc = glGetAttribLocation( program, "vPosition" );
   glEnableVertexAttribArray( vLoc );
   glVertexAttribPointer( vLoc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

   // Initialize the colors of each vertex
   vColor = glGetAttribLocation( program, "vColor" );
   glEnableVertexAttribArray( vColor );
   glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(NumPoints*sizeof(pt_type)));

   // Initialize the normal position of each vertex
   vNormal = glGetAttribLocation( program, "vNormal" );
   glEnableVertexAttribArray( vNormal );
   glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(2*NumPoints*sizeof(pt_type)));


   // Initialize shader lighting parameters
   pt_type light_position( 4.0, 4.0, 4.0, 1.0 );
   color4 light_ambient( 0.2, 0.2, 0.2, 1.0 );
   color4 light_diffuse(0.8, 0.8, 0.8, 1.0 );
   color4 light_specular( 0.8, 0.8, 0.8, 1.0 );
   color4 material_ambient( 1.0, 0.0, 1.0, 1.0 );
   color4 material_diffuse( 1.0, 0.8, 0.0, 1.0 );
   color4 material_specular( 1.0, 0.0, 1.0, 1.0 );
   float material_shininess = 5.0;
   color4 ambient_product = light_ambient * material_ambient;
   color4 diffuse_product = light_diffuse * material_diffuse;
   color4 specular_product = light_specular * material_specular;
   glUniform4fv( glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product );
   glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product );
   glUniform4fv( glGetUniformLocation(program, "SpecularProduct"), 1, specular_product );
   glUniform4fv( glGetUniformLocation(program, "LightPosition"), 1, light_position );
   glUniform1f( glGetUniformLocation(program, "Shininess"), material_shininess );


   glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
}

//----------------------------------------------------------------------------
void display( void )
{

   glEnable(GL_CULL_FACE);
   if(sqrt(eye[0]*eye[0]+eye[1]*eye[1]+eye[2]*eye[2])>1) 
      glCullFace(GL_BACK);
   else
      glCullFace(GL_FRONT);
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the window
   for(int i=0; i<NumPoints; i+=3)
      glDrawArrays( GL_TRIANGLES, i, 3 );    // draw the points

   glutSwapBuffers();
}

//----------------------------------------------------------------------------

void keyboard( unsigned char key, int x, int y )
{
   GLfloat aspect,w,h;
   switch( key ) {
   case 033: // Escape Key
   case 'q': case 'Q':
       exit( EXIT_SUCCESS );
       break;

   case 'x': cwld_left *= 1.1; cwld_right *= 1.1; break;
   case 'X': cwld_left *= 0.9; cwld_right *= 0.9; break;
   case 'y': cwld_bot *= 1.1; cwld_top *= 1.1; break;
   case 'Y': cwld_bot *= 0.9; cwld_top *= 0.9; break;
   case 'z': cwld_near  *= 1.1; cwld_far *= 1.1; break;
   case 'Z': cwld_near *= 0.9; cwld_far *= 0.9; break;
   case 'r': radius += 0.2; break;
   case 'R': radius -= 0.2; break;
   case 'o': theta += dr; break;
   case 'O': theta -= dr; break;
   case 'p': phi += dr; break;
   case 'P': phi -= dr; break;

   case ' ':  // reset values to their defaults
       cwld_left = wld_left; 
       cwld_right = wld_right;
       cwld_bot = wld_bot;
       cwld_top = wld_top;
       cwld_near = wld_near;
       cwld_far = wld_far;

       radius = 3.0;
       theta  = 0.0;
       phi    = 0.0;
       break;
    }

   eye[0] = radius*sin(theta)*cos(phi);
   eye[1] = radius*sin(theta)*sin(phi);
   eye[2] = radius*cos(theta);
   //eye[3] = 1.0;
   cout << "eye: " << eye << endl;
   cout << "at: " << at << endl;
   cout << "up: " << up << endl;
   cout << "==========================" << endl;
   //set the aspect ration of width to height
   w=cwld_right-cwld_left;
   h=cwld_top-cwld_bot;
   if(w<=h)
      aspect = h/w;
   else
      aspect = w/h;
   p = Perspective(45.0,aspect,cwld_near, cwld_far);
   glUniformMatrix4fv(projLoc, 1, GL_TRUE, p);

   mv = LookAt(eye, at, up);
   glUniformMatrix4fv(mvLoc, 1, GL_TRUE, mv);

   glutPostRedisplay();
}

//update the location of a sphere and refresh the display
//net effect, spheres will move across the screen by taking 
//one step based on the direction vector within the sphereType class.
void myIdle()
{
   glutPostRedisplay();           
}
//----------------------------------------------------------------------------

/* myReshape is called when the window is resized.
 * w and h are the new pixel width and height of the window
 */
void myReshape(GLint w, GLint h)
{
   GLfloat aspect;
   mat4 p;
   glViewport(0,0,w,h);
   winwidth = w;
   winheight = h;

   if(w<=h) {
      cwld_bot = wld_bot*(GLfloat)h/w;
      cwld_top = wld_top*(GLfloat)h/w;
      aspect = (GLfloat)h/w;
   } else {
      cwld_left = wld_left*(GLfloat)w/h;
      cwld_right = wld_right*(GLfloat)w/h;
      aspect = (GLfloat)w/h;
   }
   p = Perspective(45.0,aspect,cwld_near, cwld_far);
   glUniformMatrix4fv(projLoc, 1, GL_TRUE, p);

}

void myinit()
{
   GLfloat aspect,h,w;
   mat4 p, mv;
   //vec4 eye = vec4(0.0, 0.0, 2.0, 1.0);
   vec3 eye=vec3(radius*sin(theta)*cos(phi),
                 radius*sin(theta)*sin(phi),
                 radius*cos(theta));
   vec3 at = vec3(0.0, 0.0, 0.0);
   vec3 up = vec3(0.0, 1.0, 0.0);

   glClearColor(1.0,1.0,1.0,0.0);  //clear color is white
   glColor3f(0.0,0.0,0.0);         //fill color set to black

   //set the aspect ration of width to height
   w=cwld_right-cwld_left;
   h=cwld_top-cwld_bot;
   if(w<=h)
      aspect = h/w;
   else
      aspect = w/h;

   p = Perspective(45.0,aspect,cwld_near, cwld_far);
   glUniformMatrix4fv(projLoc, 1, GL_TRUE, p);

   mv = LookAt(eye, at, up);
   glUniformMatrix4fv(mvLoc, 1, GL_TRUE, mv);

   //inside or outside, need to deal with what you can see
   //The next two lines setups up so the fragment shader only 
   //gets the front faces of the sphere
   glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);
}

vec4 * generatePointColors(int NumPoints)
{
   vec4 *colors = new vec4[NumPoints];
   for(int i=0;i<NumPoints;i++)
      colors[i] = vec4(rand()%100/100.0,rand()%100/100.0,rand()%100/100.0,1.0);
   return colors;
}

pt_type * generateSphereNormals(pt_type * points, int NumPoints)
{
   vec4 *normals = new pt_type[NumPoints];
   for(int i=0;i<NumPoints;i++)
      normals[i] = points[i];
   return normals;
}

int main( int argc, char **argv )
{
   int iterations;
   if(argc != 2) {
      fprintf(stderr,"usage: sphere <iterations>\n");
      exit(1);
   } else {
      iterations = atoi(argv[1]); 
   }
   glutInit( &argc, argv );
   glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
   glutInitWindowSize( winwidth, winheight );

   glutCreateWindow( "Sphere - shader version" );

#ifndef __APPLE__
   glewExperimental = GL_TRUE;
   glewInit();
#endif

   sphere_pts = generateSpherePoints(iterations);
   sphere_normals = generateSphereNormals(sphere_pts,NumPoints);
   sphere_colors = generatePointColors(NumPoints);
   //for(int k=0;k<NumPoints;k++)
      //cout << sphere_pts[k] << endl;
   initGLBuffer_and_shaders(sphere_pts, sphere_colors, sphere_normals, NumPoints);

   
   myinit();

   glutDisplayFunc(display);
   glutReshapeFunc(myReshape);
   glutKeyboardFunc(keyboard);
   glutIdleFunc(myIdle);

   cout << endl;
   cout << "===========================" << endl;
   cout << "==== Frustum control ===="<< endl;
   cout << "x|X increase/decrease the X-axis"<< endl;
   cout << "y|Y increase/decrease the Y-axis"<< endl;
   cout << "z|Z increase/decrease the near/far on Z-axis"<< endl;
   cout << "==== LookAt control ===="<< endl;
   cout << "r|R increase/decrease the radius"<< endl;
   cout << "o|O increase/decrease theta "<< endl;
   cout << "p|P increase/decrease phi "<< endl;
   cout << "space bar resets back to original values"<< endl;
   cout << "Use Q|q to quit the program" << endl;
   cout << "===========================" << endl;
   glutMainLoop();
   return 0;
}
