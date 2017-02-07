/* Name: Matthew Matze
   Date: 5-2-2015
   Class: CSC-3360
	Location: csc3360/map/spherePerspectiveColor_v3
*/

#include<stdio.h>
#include "../../include/Angel.h"

using namespace std;

//shader stuff
GLint projLoc;
GLint mvLoc;
GLint scaleLoc;
GLint positionLoc; 

typedef vec4 pt_type; 
typedef vec4 color4; 
typedef vec3 color3;

//holds the points for one map at the origin with radius 1
//each point needs a color so map_colors are insync with map_pts
int NumPoints;  //number of iterations to generate the points
pt_type *map_pts;
pt_type *map_normals;
vec4 *map_colors;
int numdpoints;
pt_type *diamond_pts;//define the diamond points
int W,R;

//world coordinates
const GLfloat wld_bot=-10.0, wld_top=10.0;
const GLfloat wld_left=-10.0, wld_right=10.0;
const GLfloat wld_near=0.01, wld_far=1000.0;
GLfloat cwld_bot=-10.0, cwld_top=10.0;
GLfloat cwld_left=-10.0, cwld_right=10.0;
GLfloat cwld_near=.01, cwld_far=1000.0;

// Viewing transformation parameters

GLfloat radius = 3.0;
GLfloat theta = 0.0;
GLfloat phi = 0.0;

const GLfloat  dr = 5.0 * DegreesToRadians;

mat4 p, mv;
vec3 eye = vec3(1.8, -2.7, 1.325);
vec3 at = vec3(0.5, -1.4, 0.4);
vec3 up = vec3(0.0, 0.0, 4.0);

//----------------new stuff above this line--------------
int winwidth=512, winheight=512;

//----------------------------------------------------------------------------
pt_type *generateMapPoints(int iterations)
{
   //determine the size of the array based on the number of iterations of 
   //the recursive function.  NOTE: for each triangle segment that is cut,
   //you will end up 4 smaller triangles segments with 3 pts per triangle.
   //NumPoints = 3*(int)(pow(4,iterations+1));
  FILE *raw;
   raw=fopen("../honolulu.raw","r");//Open File
   fscanf(raw,"%i %i", &W, &R);
   NumPoints = W*R*6;
	float max=1500; 
   pt_type *tripoints = new pt_type [NumPoints];
	float fw = float(W);
	float fr = float(R);
   map_pts = new pt_type [NumPoints];

   float **points = (float **)malloc(R * sizeof(float *));
	for(int i = 0; i < R; i++)
   	points[i] = (float *)malloc(W * sizeof(float));
//	printf("TEST");
	for(int i=0;i<R;i++){
      for(int j=0;j<W;j++){
         fscanf(raw, "%f", &points[i][j]); //Set the points array from file
 		}
	}
	int k=0;
	int multiplier=2;
	for(int i=0;i<R-1;i++){
      for(int j=0;j<W-1;j++){
			tripoints[k]=vec4((multiplier*(i/fw-.5)),multiplier*(-j)/fr,points[i][j]/max,1.0);
			k++;
			tripoints[k]=vec4((multiplier*((i+1)/fw-.5)),multiplier*(-j)/fr,points[i+1][j]/max,1.0);
			k++;
			tripoints[k]=vec4((multiplier*((i)/fw-.5)),multiplier*(-(j+1))/fr,points[i][j+1]/max,1.0);
			k++;
			tripoints[k]=vec4((multiplier*((i+1)/fw-.5)),multiplier*(-j/fr),points[i+1][j]/max,1.0);
			k++;
			tripoints[k]=vec4((multiplier*((i+1)/fw-.5)),multiplier*(-(j+1))/fr,points[i+1][j+1]/max,1.0);
			k++;
			tripoints[k]=vec4((multiplier*(i/fw-.5)),multiplier*(-(j+1))/fr,points[i][j+1]/max,1.0);
			k++;
		}
	}
	for(int i=0;i<R;i++)
		free(points[i]);
	free(points);
	//int n=iterations;
   return tripoints;
}


pt_type* generatediamond(){
	
numdpoints=12;
pt_type *diamond_pts = new pt_type[numdpoints];
int z=1;
//Set each diamond point maunally
diamond_pts[0]=vec4(.5,.5,.5,z);
diamond_pts[1]=vec4(.5,.4,.5,z);
diamond_pts[2]=vec4(0.4,.5,.5,z);
diamond_pts[3]=vec4(.5,.5,.5,z);
diamond_pts[4]=vec4(.5,.4,.5,z);
diamond_pts[5]=vec4(.45,.45,.6,z);
diamond_pts[6]=vec4(.5,.5,.5,z);
diamond_pts[7]=vec4(0.4,.5,.5,z);
diamond_pts[8]=vec4(.45,.45,.6,z);
diamond_pts[9]=vec4(.5,.4,.5,z);
diamond_pts[10]=vec4(0.4,.5,.5,z);
diamond_pts[11]=vec4(.45,.45,.6,z);

	return diamond_pts;
}

//----------------------------------------------------------------------------
//projLoc, scaleLoc, positionLoc and vLoc are define as global 
//variables since multiple callback functions will need to access them.
void initGLBuffer_and_shaders(pt_type points[],pt_type diamond_pts[], vec4 colors[], vec4 normals[], int NumPoints)
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
   unsigned int diamondBytes = numdpoints * sizeof(pt_type);
   unsigned int colorBytes = (NumPoints+numdpoints)*sizeof(vec4);
   unsigned int normalBytes = (NumPoints+numdpoints)*sizeof(vec4);
   unsigned int buffSize = vertsBytes + normalBytes + colorBytes + diamondBytes; 
   glGenBuffers( 1, &buffer );
   glBindBuffer( GL_ARRAY_BUFFER, buffer );
   glBufferData( GL_ARRAY_BUFFER, buffSize, NULL , GL_STATIC_DRAW );
   glBufferSubData( GL_ARRAY_BUFFER, 0, vertsBytes, points );
   glBufferSubData( GL_ARRAY_BUFFER, vertsBytes, diamondBytes, diamond_pts );
   glBufferSubData( GL_ARRAY_BUFFER, vertsBytes+diamondBytes, colorBytes, colors );
   glBufferSubData( GL_ARRAY_BUFFER, vertsBytes+colorBytes+diamondBytes, normalBytes, normals );

   // Load shaders and use the resulting shader program
   program = InitShader( "vshader.glsl", "fshader.glsl" );
   glUseProgram( program );

   //Get the locations of transformation matrices from the vshader
   projLoc = glGetUniformLocation(program, "p");
   mvLoc = glGetUniformLocation(program, "mv");
   //Get the locations of the scale and position from the vshader
   //These will be filled as the program runs, the vshader will
   //use these to values to correctly size and position the map.
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
   pt_type light_position( 0.0, 0.0, 8.0, 1.0 );
   color4 light_ambient( 0.1, 0.1, 0.1, 1.0 );
   color4 light_diffuse(0.4, 0.4, 0.4, 1.0 );
   color4 light_specular( 0.4, 0.4, 0.4, 1.0 );
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

   glCullFace(GL_FRONT);
   glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the window
   for(int i=0; i<NumPoints+numdpoints; i+=3){	
   	if(i>=NumPoints){
	 	 	glDrawArrays( GL_POLYGON, i, 3 );    // draw the Diamondpoints
		} else {
	  	glDrawArrays( GL_POLYGON, i, 3 );    // draw the Map
		}
	}
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
	case 'e': at.x += .1; break;
	case 'c': at.x -= .1; break;
	case 'a': at.y += .1; break;
	case 'd': at.y -= .1; break;
	case 'w': at.z	+= .05; break; 
	case 's': at.z	-= .05; break;
	case 'u': eye.x += .1; break;
	case 'm': eye.x -= .1; break;	
	case 'j': eye.y += .1; break;	
	case 'l': eye.y -= .1; break;	
	case 'i': eye.z += .025; break;	
	case 'k': eye.z -= .025; break;	
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
		 eye = vec3(1.8, -2.7, 1.325);
   	 at = vec3(0.5, -1.4, 0.4);
   	 up = vec3(0.0, 0.0, 4.0);

       radius = 3.0;
       theta  = 0.0;
       phi    = 0.0;
       break;
    }

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

//update the location of a map and refresh the display
//net effect, maps will move across the screen by taking 
//one step based on the direction vector within the mapType class.
void myIdle()
{
//Attempt to spin the pyramid
//	mat4 S=mat4(vec4(scale,0.0,0.0,0.0),	
//					vec4(0.0,scale,0.0,0.0),	
//					vec4(0.0,0.0,1.0,0.0),	
//					vec4(0.0,0.0,0.0,1.0));
//	mat4 t=mat4(vec4(1.0,0.0,0.0,0.0),	
//					vec4(0.0,1.0,0.0,0.0),	
//					vec4(0.0,0.0,1.0,0.0),	
//					vec4(0.0,0.0,0.0,1.0));
//	glUniformMatrix4fv(SLoc,1,GL_TRUE,S);	
//	sLoc = glGetAttribLocation(program, S
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
   //vec3 eye=vec3(radius*sin(theta)*cos(phi),
   //              radius*sin(theta)*sin(phi),
   //              radius*cos(theta));
	vec3 eye = vec3(1.8, -2.7, 1.325);
   vec3 at = vec3(0.5, -1.4, 0.4);
   vec3 up = vec3(0.0, 0.0, 4.0);

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
   //gets the front faces of the map
   glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);
}

vec4 * generatePointColors(int NumPoints)
{
   vec4 *colors = new vec4[NumPoints+numdpoints];
   for(int i=0;i<NumPoints;i++){//coloring for the scene
     	if(map_pts[i].z<.0000001)
			colors[i] = vec4(0.0/255,37.0/255,125.0/255,.5);
     	if(map_pts[i].z>=.0000001&&map_pts[i].z<.00135)
			colors[i] = vec4(19.0/255,189.0/255,186.0/255,.5);
     	if(map_pts[i].z>=.00135&&map_pts[i].z<.003)
			colors[i] = vec4(245.0/255,245.0/255,228.0/255,1.0);
     	if(map_pts[i].z>=.003&&map_pts[i].z<.1)
			colors[i] = vec4(5.0/255,105.0/255,18.0/255,1.0);
		if(map_pts[i].z>=.100&&map_pts[i].z<.300)
			colors[i] = vec4(2.0/255,87.0/255,13.0/255,1.0);
		if(map_pts[i].z>=.300&&map_pts[i].z<.600)
			colors[i] = vec4(0.0/255,79.0/255,11.0/255,1.0);
		if(map_pts[i].z>=.600&&map_pts[i].z<.900)
			colors[i] = vec4(0.0/255,130.0/255,17.0/255,1.0);
		if(map_pts[i].z>=.900&&map_pts[i].z<.1000)
			colors[i] = vec4(194.0/255,50.0/255,50.0/255,1.0);
	}
   for(int i=NumPoints;i<NumPoints+numdpoints;i++)
			colors[i] = vec4(100.0,0.0,0.0,1.0);
   return colors;
//vec4(100.0,0.0,0.0,1.0);
//vec4(rand()%100/100.0,rand()%100/100.0,rand()%100/100.0,1.0);
}

pt_type * generateSphereNormals(pt_type * points, pt_type * diamondpoints, int NumPoints)
{
   vec4 *normals = new pt_type[NumPoints+numdpoints];
   vec4 *surnorm = new pt_type[NumPoints];
	vec4 v0;
	vec4 v1;
   for(int i=0;i<NumPoints;i+=3){
		v0 = vec4(points[i+1].x-points[i].x,points[i+1].y-points[i].y,points[i+1].z-points[i].z,1);
		v1 = vec4(points[i+2].x-points[i+1].x,points[i+2].y-points[i+1].y,points[i+2].z-points[i+1].z,1);
		surnorm[i] = cross(v0,v1);
		surnorm[i+1] = cross(v0,v1);
		surnorm[i+2] = cross(v0,v1);
	}
	vec4 test;
	int numtest=1;
	for(int i=0;i<NumPoints;i++){
		/*test=points[i];
		numtest=1;
		for(int j=0;j<NumPoints;j++){
			if(points[i]==points[j]&&j!=i){
				test+=surnorm[j];
				numtest++;
			}
		}
	normals[i]=Normalize(test);//numtest;
	}*/
		//normals[i]=points[i];
		//normals[i+1]=points[i+1];
		//normals[i+2]=points[i+2];
	normals[i]=points[i];
	}
	for(int i=NumPoints;i<NumPoints+numdpoints;i++){
		normals[i]=diamondpoints[i-NumPoints];
	}
   return normals;
}

int main( int argc, char **argv )
{
   int iterations;
   if(argc != 2) {
      fprintf(stderr,"usage: map <iterations>\n");
      exit(1);
   } else {
      iterations = atoi(argv[1]); 
   }
   glutInit( &argc, argv );
   glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
   glutInitWindowSize( winwidth, winheight );

   glutCreateWindow( "Honolulu - Elevation Map" );

#ifndef __APPLE__
   glewExperimental = GL_TRUE;
   glewInit();
#endif

   map_pts = generateMapPoints(iterations);
	diamond_pts = generatediamond();	
   map_normals = generateSphereNormals(map_pts, diamond_pts, NumPoints);
   map_colors = generatePointColors(NumPoints);
   //for(int k=0;k<NumPoints;k++)
      //cout << map_pts[k] << endl;
   initGLBuffer_and_shaders(map_pts,diamond_pts, map_colors, map_normals, NumPoints);

   
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
   cout << "==== Eye control ===="<< endl;
   cout << "i|k Move Camera Up/Down"<< endl;
   cout << "j|l Move Camera Left/Right *From Original Position"<< endl;
   cout << "u|m Move Camera Toward/Away from the scene*From Original Position"<< endl;
   cout << "==== At control ===="<< endl;
   cout << "e|c Angle Camera Up/Down"<< endl;
   cout << "a|d Angle Camera Left/Right *From Original Poisition"<< endl;
   cout << "e|c Angle Camera Closer/Further From Scene Center *From Original Position"<< endl;
   cout << "space bar resets back to original values"<< endl;
   cout << "Use Q|q to quit the program" << endl;
   cout << "===========================" << endl;
   glutMainLoop();
   return 0;
}
