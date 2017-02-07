/* Name: Matthew Matze
   Date: 4-14-2015

   Code which creates a 3 level version of the fish game.

   A simple circleType class is established.

   This code uses one collection of points for all circles needed.

   The scale (radius) and location of the circle is fed into 
   the vshader.  At that time, the original points are handed to 
   the vshader for displaying.  The vshader does all of the work
   of scalling and translating.  The best part, the points only have to
   be loaded one time into the GL_ARRAY_BUFFER.

*/

#include "../../include/Angel.h"

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
int level=4;//Holds the Starting level(4 is actually level 3)
int NumCircle=4*level;//Defines the number of circles(level 3 is defined as 4)
int score=0;
vec2 mouseLoc;
bool plr=true;//Decides whether or not the player circle is displayed
int* ct = new int [NumCircle];//Array of values which decide whether each circle is displayed
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
		void inLoc(vec2 in_loc)
		{
			loc = in_loc;
		}
		void indir(vec2 in_dir)
		{
			dir=in_dir;
		}
		void inScale(GLfloat inscale)
		{
			radius= sqrt((inscale*inscale)+(radius*radius));
		}
		void inradius(GLfloat inscale)
		{
			radius = inscale;
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
		vec2 getDir(void)
		{
			return dir;
		}
      vec2 takeAstep()
      {
         //circle location is updated every time this method is called.
         loc+=dir;
         return loc;
      }
};
//Defines the player's radius
double plrsize=.1/level;//plrsize defines the radius of the circle for each level
//Initializes the circles
circleType* circle =(circleType*)malloc(NumCircle*sizeof(circleType));//defines the array of circles
circleType plrcircle(vec2(0.1,0.1),vec2(0.004,-0.006),plrsize);//defines the player circle

//----------------new stuff above this line--------------
int winwidth=512, winheight=512;

/*
 *Purpose: Function finds the distance between two points in one easy to use function
 *Inputs: Two vec2 Points to find the distance between
 *Outputs: The Distance between the two points in GLfloat form
 */
GLfloat distancetest(vec2 pt1, vec2 pt2)
{
	GLfloat result=sqrt((pow((pt2.x-pt1.x),2))+(pow((pt2.y-pt1.y),2)));
	return result;
}
/*
 *Purpose: Updates each variable in the circle in one easy to use function
 *Inputs: The index of the circle as int i, the radius as GLfloat r,
 *and two vec2's v for the direction and Location
 *Ouputs: Nothing but updates the global variable circle with the new info
 */
void updatecircle(int i, GLfloat r, vec2 v, vec2 l){
		circle[i].inLoc(l);
		circle[i].indir(v);
		circle[i].inradius(r);
}
//Purpose:Initialize all of the circles in level one
//Inputs:None
//Outputs:None
//Globals:The circle values are actually changed in the updatecircle function
void initlvl1(){
	double* r = (double*)malloc(NumCircle*sizeof(double));
	vec2* l = (vec2*)malloc(NumCircle*sizeof(vec2));
	vec2* v = (vec2*)malloc(NumCircle*sizeof(vec2));
	for(int i=0; i<NumCircle/4;i++){
		r[i] =   plrcircle.getScale()-((double) rand() / (RAND_MAX))/40;
		l[i].x = ((double) rand() / (RAND_MAX))+.45;	
		l[i].y = ((double) rand() / (RAND_MAX))+.45;
		v[i].x = (((double) rand() / (RAND_MAX))/200*(-1))-.005;	
		v[i].y = (((double) rand() / (RAND_MAX))/200*(-1))-.005;	
		updatecircle(i,r[i],v[i],l[i]);
	}
	for(int i=NumCircle/4; i<NumCircle/2;i++){
		r[i] = plrcircle.getScale();
		l[i].x = ((double) rand() / (RAND_MAX))+.25;	
		l[i].y = (((double) rand() / (RAND_MAX))*-1)-.25;
		v[i].x = (((double) rand() / (RAND_MAX))/200*(-1))-.005;	
		v[i].y = (((double) rand() / (RAND_MAX))/200)+.005;	
		updatecircle(i,r[i],v[i],l[i]);
	}
	for(int i=NumCircle/2; i<(NumCircle-(NumCircle/4));i++){
		r[i] = plrcircle.getScale()+((double) rand() / (RAND_MAX))/30;
		l[i].x = (((double) rand() / (RAND_MAX))*-1)-.2;
		l[i].y = (((double) rand() / (RAND_MAX))*-1)-.2;
		v[i].x = (((double) rand() / (RAND_MAX))/200)+.005;	
		v[i].y = (((double) rand() / (RAND_MAX))/200)+.005;	
		updatecircle(i,r[i],v[i],l[i]);
	}
	for(int i=NumCircle-(NumCircle/4); i<NumCircle;i++){
		r[i] = plrcircle.getScale()+((double) rand() / (RAND_MAX))/20;
		l[i].x = (((double) rand() / (RAND_MAX))*-1)-.1;	
		l[i].y = ((double) rand() / (RAND_MAX))+.1;
		v[i].x = (((double) rand() / (RAND_MAX))/200)+.005;	
		v[i].y = (((double) rand() / (RAND_MAX))/200*(-1))-.005;	
		updatecircle(i,r[i],v[i],l[i]);
	}
}
/*Purpose:Initializes the first group of circles in Levels 2 and 3
 * and checks if two start out in the same spot
 *Inputs: the index for which circle in int i, the radius in double
 * r, and the location and direction in the vec2's l and v
 *Outputs:None
 *Globals:The circle values are actually changed in the updatecircle function
 */
void startcollisiontest1(int i, double r, vec2 l, vec2 v){
		bool collide;
		do{
			collide=false;
			r = plrcircle.getScale()-((double) rand() / (RAND_MAX)/80);
			l.x = ((double) rand() / (RAND_MAX))+.3*(i+2);	
			l.y = ((double) rand() / (RAND_MAX))+.3*(i+2);	
			v.x = (((double) rand() / (RAND_MAX))/200*(-1))-.005;	
			v.y = (((double) rand() / (RAND_MAX))/200*(-1))-.005;	
			updatecircle(i,r,v,l);
			//Test for collision of randomly generated circles
			if(i>0)
			{
				for(int j=i-1;j>0;j--)
				{
					collide=collide||distancetest(circle[i].getLoc(),circle[j].getLoc())<=(circle[i].getScale()+circle[j].getScale());
				}
				//printf("Collide1: %d\n",collide);
			}
		}while(collide);
}
/*Purpose:Initializes the Second group of circles in Levels 2 and 3
 * and checks if two start out in the same spot
 *Inputs: the index for which circle in int i, the radius in double
 * r, and the location and direction in the vec2's l and v
 *Outputs:None
 *Globals:The circle values are actually changed in the updatecircle function
*/
void startcollisiontest2(int i, double r, vec2 l, vec2 v){
		bool collide;
		do{
			collide=false;
			r = plrcircle.getScale();
			l.x = ((double) rand() / (RAND_MAX))+(.25*(i-(NumCircle/4)));	
			l.y = (((double) rand() / (RAND_MAX))*-1)-(.25*(i-(NumCircle/4)));
			v.x = (((double) rand() / (RAND_MAX))/200*(-1))-.005;	
			v.y = (((double) rand() / (RAND_MAX))/200)+.005;	
			updatecircle(i,r,v,l);
			//Test for collision of randomly generated circles
			if(i>(NumCircle/4))
			{
				for(int j=i-1;j>(NumCircle/4);j--)
				{
					collide=collide||distancetest(circle[i].getLoc(),circle[j].getLoc())<=(circle[i].getScale()+circle[j].getScale());
				}
				//printf("Collide2: %d\n",collide);
			}
		}while(collide);
}
/*Purpose:Initializes the Third group of circles in Levels 2 and 3
 * and checks if two start out in the same spot
 *Inputs: the index for which circle in int i, the radius in double
 * r, and the location and direction in the vec2's l and v
 *Outputs:None
 *Globals:The circle values are actually changed in the updatecircle function
*/
void startcollisiontest3(int i, double r, vec2 l, vec2 v){
		bool collide;
		do{
			collide=false;
		r = plrcircle.getScale()+((double) rand() / (RAND_MAX))/30;
		l.x = (((double) rand() / (RAND_MAX))*-1);//-(.3*(i-(NumCircle/2)));
		l.y = (((double) rand() / (RAND_MAX))*-1);//-(.3*(i-(NumCircle/2)));
		v.x = (((double) rand() / (RAND_MAX))/200)+.005;	
		v.y = (((double) rand() / (RAND_MAX))/200)+.005;	
		updatecircle(i,r,v,l);
			//Test for collision of randomly generated circles
			if(i>(NumCircle/2))
			{
				for(int j=i-1;j>(NumCircle/2);j--)
				{
					collide=collide||distancetest(circle[i].getLoc(),circle[j].getLoc())<=(circle[i].getScale()+circle[j].getScale());
				}
				//printf("Collide3: %d\n",collide);
			}
		}while(collide);
}
/*Purpose:Initializes the fourth group of circles in Levels 2 and 3
 * and checks if two start out in the same spot
 *Inputs: the index for which circle in int i, the radius in double
 * r, and the location and direction in the vec2's l and v
 *Outputs:None
 *Globals:The circle values are actually changed in the updatecircle function
*/
void startcollisiontest4(int i, double r, vec2 l, vec2 v){
		bool collide;
		do{
			collide=false;
			r = plrcircle.getScale()+((double) rand() / (RAND_MAX))/20;
			l.x = (((double) rand() / (RAND_MAX))*-1);
			l.y = ((double) rand() / (RAND_MAX));
			v.x = (((double) rand() / (RAND_MAX))/200)+.005;	
			v.y = (((double) rand() / (RAND_MAX))/200*(-1))-.005;	
			updatecircle(i,r,v,l);
			//Test for collision of randomly generated circles
			if(i>NumCircle-(NumCircle/4))
			{
				for(int j=i-1;j>NumCircle-(NumCircle/4);j--)
				{
					collide=collide||distancetest(circle[i].getLoc(),circle[j].getLoc())<=(circle[i].getScale()+circle[j].getScale());
				}
				//printf("Collide4: %d\n",collide);
			}
		}while(collide);
}
/*Purpose:Initializes the circles in Levels 2 and 3
 *Inputs:None
 *Outputs:None
 *Globals:The circle values are actually changed
 *in the updatecircle function in each startcollisiontest
*/
void initlvl2(){
	double* r = (double*)malloc(NumCircle*sizeof(double));
	vec2* l = (vec2*)malloc(NumCircle*sizeof(vec2));
	vec2* v = (vec2*)malloc(NumCircle*sizeof(vec2));
	//printf("NumCircle: %i\n",NumCircle);
	for(int i=0; i<NumCircle/4;i++){
		startcollisiontest1(i, r[i], l[i], v[i]);
	}
for(int i=NumCircle/4; i<NumCircle/2;i++){
		startcollisiontest2(i, r[i], l[i], v[i]);
	}
	for(int i=NumCircle/2; i<(NumCircle-(NumCircle/4));i++){
		startcollisiontest3(i, r[i], l[i], v[i]);
	}
	for(int i=NumCircle-(NumCircle/4); i<NumCircle;i++){
		startcollisiontest4(i, r[i], l[i], v[i]);
	}
}
/*Purpose:Evaluates the input of each thing key inputed
 *Inputs:The inputed key
 *Outputs:None
 *Globals:None
*/
void keyboard( unsigned char key, int x, int y )
{
   switch ( key ) {
      case 'Q':
      case 'q':
          exit( EXIT_SUCCESS );
          break;
   }
   switch ( key ) {
      case 'N':
      case 'n':
			initlvl2();
			plrcircle.inLoc(vec2(0.1,0.1));
			plrcircle.inradius(plrsize);
			plr=true;
			for(int i=0;i<NumCircle;i++){
				ct[i]=true;	
			}
         break;
   }
}

/*Purpose:Set the global Mouse coordinates to the 
 * location of the mouse one the screen
 *Inputs:The GLint x and y coordinates for the location of the player circle 
 *Outputs:None
 *Globals:The mouseLoc global is changed in order to redefine the circle location
*/
void mouse(GLint x, GLint y)
{
	mouseLoc.x=x;
	mouseLoc.y=y;
	mouseLoc.x= (mouseLoc.x-(winwidth/2))/(winwidth/2);
	mouseLoc.y= -(mouseLoc.y-(winheight/2))/(winwidth/2);
//	printf("%f %f\n", mouseLoc.x, mouseLoc.y);
}

/*Purpose:Tests if two balls hit each other
 *Inputs:The two circles that you are testing if they are hitting each other
 * are circlet1 and circlet2
 *Outputs:The boolean variable outputs whether 
 *there was a collision or not and if there was then it changes the newdir1 and newdir2
*/
bool collisiontest(circleType circlet1, circleType circlet2, vec2 *newdir1, vec2 *newdir2)
{
	GLfloat m1 = circlet1.getScale()*circlet1.getScale()*3.14159265359;
	GLfloat m2 = circlet2.getScale()*circlet2.getScale()*3.14159265359;
	GLfloat v1n, v1t, v2n, v2t, v1N, v2N, v1T, v2T;
	vec2 curloc1 = circlet1.getLoc();
	vec2 curloc2 = circlet2.getLoc();
	vec2 nxtloc1 = circlet1.getDir()+circlet1.getLoc();
	vec2 nxtloc2 = circlet2.getDir()+circlet2.getLoc();
	vec2 mn, mt, V1N, V1T, V2N, V2T;
	if(distancetest(nxtloc1,nxtloc2)<=(circlet1.getScale()+circlet2.getScale()))
	{
		mn=normalize(curloc1-curloc2);
		mt.x= -mn.y;
		mt.y=mn.x;
		v1n=((mn.x*circlet1.getDir().x)+(mn.y*circlet1.getDir().y));
		v1t=((mt.x*circlet1.getDir().x)+(mt.y*circlet1.getDir().y));
		v2n=((mn.x*circlet2.getDir().x)+(mn.y*circlet2.getDir().y));
		v2t=((mt.x*circlet2.getDir().x)+(mt.y*circlet2.getDir().y));
		v1T=v1t;
		v2T=v2t;
		v1N=((v1n*(m1-m2))+(2*m2*v2n))/(m1+m2);
		v2N=((v2n*(m2-m1))+(2*m1*v1n))/(m1+m2);
		V1N=v1N*mn;
		V1T=v1T*mt;
		V2N=v2N*mn;
		V2T=v2T*mt;
		*newdir1=V1N+V1T;
		*newdir2=V2N+V2T;
		return true;
	} else {
		return false;
	}
}
/*Purpose:Tests and redefines each circles direction if they collide
 *Inputs:The two circles directions that you are testing and if they collide
 * then each circles direction is redefined
 *Outputs:Nothing 
 *Globals: If there was a collision changes the circles direction
*/
void circlecollisions(vec2 &newdir1,vec2 &newdir2){
	for(int i=0;i<NumCircle-1;i++){
		for(int j=i+1;j<NumCircle;j++)
		{
			if(collisiontest(circle[i],circle[j],&newdir1,&newdir2)&&ct[i]&&ct[j]) 
			{
				circle[i].indir(newdir1);
				circle[j].indir(newdir2);
			}
		}
	}
}
/*Purpose:Tests and redefines a circle's direction if it hits the wall
 *Inputs:The circle's direction and the circle itself if it hits
 * then the circle's direction is redefined
 *Outputs: A boolean of whether the wall was hit
 *Globals: None
*/
bool walltest(circleType circlet1, vec2 *newdir1)
{
	vec2 nxtloc1 = circlet1.getDir()+circlet1.getLoc();
	if(nxtloc1.x+circlet1.getScale()>=1)
	{
		newdir1->x= -abs(circlet1.getDir().x);
		newdir1->y= circlet1.getDir().y;
		return true;
	}
	if(nxtloc1.x-circlet1.getScale()<=-1)
	{
		newdir1->x= abs(circlet1.getDir().x);
		newdir1->y= circlet1.getDir().y;
		return true;
	}
	if(nxtloc1.y+circlet1.getScale()>=1)
	{
		newdir1->x= circlet1.getDir().x;
		newdir1->y= -abs(circlet1.getDir().y);
		return true;
	}
	if(nxtloc1.y-circlet1.getScale()<=-1)
	{
		newdir1->x= circlet1.getDir().x;
		newdir1->y= abs(circlet1.getDir().y);
		return true;
	}
		return false;
}
/*Purpose:Tests each individual wall and redefines a circle's direction 
 * if it hits the wall using the wall test function
 *Inputs:The New direction vec2 that will be used to redefine the circle
 *Outputs: None
 *Globals: The Circle at index i is being redefined
*/
void allwalls(vec2 &newdir1){
	for(int i=0;i<NumCircle;i++){
		if(walltest(circle[i],&newdir1))
			circle[i].indir(newdir1);	
	}
}
//Purpose:Decides which Game will be played and resets 
//the variables to start the new game
//Inputs:The item which defines which game you are playing
//Globals: The Player circle location, the level, each ct value, and the plr value 
void myMenu(int item)
{
	switch(item){
		case 1:
			level=1;
			NumCircle=4;
			for(int i=0;i<NumCircle;i++)
				ct[i]=true;	
			for(int i=NumCircle;i<16;i++)
				ct[i]=false;	
			plrcircle.inLoc(vec2(0.1,0.1));
			plrcircle.inradius(.1/level);
			plr=true;
			initlvl1();
			break;
		case 2:
			level=2;
			NumCircle=8;
			for(int i=0;i<NumCircle;i++)
				ct[i]=true;	
			plrcircle.inLoc(vec2(0.1,0.1));
			plrcircle.inradius(.1/level);
			plr=true;
			initlvl2();
			break;
		case 3:
			level=4;
			NumCircle=16;
			for(int i=0;i<NumCircle;i++)
				ct[i]=true;	
			plrcircle.inLoc(vec2(0.1,0.1));
			plrcircle.inradius(.1/level);
			plr=true;
			initlvl2();
			break;
	}
}
/*Purpose:Creates the Menu, adds the choice of easy, 
 * medium or hard, and attaches the menu to the right click 
 *Inputs: None
 *Outputs: None
 *Globals: Calls My Menu which has globals 
*/
void set_up_menu()
{
	glutCreateMenu(myMenu);
	glutAddMenuEntry("Easy",1);
	glutAddMenuEntry("Medium", 2);
	glutAddMenuEntry("Hard",3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
/*Purpose: Finds if the player has hit the ball on the screen
 * if they did then it will return a 0 if the player is bigger,
 * 1 if the other is bigger, and 2 if no collision
 *Inputs:The player circle and the opponents circle are the inputs 
 *Outputs: Integer value which indicates whether the player has hit
 *a circle or not and if they did which is bigger 
*/
int playercollsion(circleType plrcirclet, circleType circlet)
{
	if(distancetest(mouseLoc,circlet.getLoc())<=(plrcirclet.getScale()+circlet.getScale()))
	{
		if(plrcirclet.getScale()>circlet.getScale())
		{
			return 0;
		} 
		if(plrcirclet.getScale()<=circlet.getScale()){
			return 1;
		}
	}
	return 2;
}

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
	//printf("%d\n", plr);
	if(plr){
	glUniform1f(scaleLoc,plrcircle.getScale());
   glUniform2fv(positionLoc,1,mouseLoc);
   glDrawArrays( GL_POLYGON, 0, NumPoints );    // draw the points
	}
	for(int i=0;i<NumCircle;i++)
	{
		if(ct[i])
		{
			glUniform1f(scaleLoc,circle[i].getScale());
	   	glUniform2fv(positionLoc,1,circle[i].getLoc());
   		glDrawArrays( GL_POLYGON, 0, NumPoints );    // draw the points
		}
	}
	glutSwapBuffers();
}

//----------------------------------------------------------------------------
//
/*Purpose: Runs the playercollision function and subsequently
 * decides whether or not the player should be shown based on
 * whether the circle has been hit by the player or if they have
 * been hit previous to this iteration 
 *Inputs: None
 *Outputs: None
 *Globals: Uses ct and plr variable to tell whether either balls have
 *been hit previously and if both are still on the screen and they have it
 *and player is bigger it will resize the player circle appropriately
*/
void playercollisions(){
	int plrt;
	for(int i=0;i<NumCircle;i++){
		plrt=playercollsion(plrcircle,circle[i]);
		if(plrt==0&&ct[i]&&plr)
		{
			plrcircle.inScale(circle[i].getScale());
			ct[i]=false;
		}
		if(plrt==1&&ct[i]){
			plr=false;
		}
	}
}
/*Purpose: Decides if you win and adds the result to your score
 *Inputs: None
 *Outputs: None
 *Globals: Uses ct array and plr variable to tell whether you won
 * and updates your score and level
*/
void levelwin1(){
	if(!ct[0]&&!ct[1]&&!ct[2]&&!ct[3]){
		score++;
		printf("You Beat Level 1\n");
		printf("You have a Score of %d\n", score);
		level=0;
	}
	if(!plr){
		score-=1;
		printf("You Lost Level 1\n");
		printf("You have a Score of %d\n", score);
		level=0;
	}
}
/*Purpose: Decides if you win and adds the result to your score
 *Inputs: None
 *Outputs: None
 *Globals: Uses ct array and plr variable to tell whether you won
 * and updates your score and level
*/
void levelwin2(){
	if(!ct[0]&&!ct[1]&&!ct[2]&&!ct[3]&&!ct[4]&&!ct[5]&&!ct[6]&&!ct[7]){
		score+=2;
		printf("You Beat Level 2\n");
		printf("You have a Score of %d\n", score);
		level=0;
	}
	if(!plr){
		score-=2;
		printf("You Lost Level 2\n");
		printf("You have a Score of %d\n", score);
		level=0;
	}
}
/*Purpose: Decides if you win and adds the result to your score
 *Inputs: None
 *Outputs: None
 *Globals: Uses ct array and plr variable to tell whether you won
 * and updates your score and level
*/
void levelwin3(){
	if(!ct[0]&&!ct[1]&&!ct[2]&&!ct[3]&&!ct[4]&&!ct[5]&&!ct[6]&&!ct[7]&&!ct[8]&&!ct[9]&&!ct[10]&&!ct[11]&&!ct[12]&&!ct[13]&&!ct[14]&&!ct[15]){
		score+=4;
		printf("You Beat Level 3\n");
		printf("You have a Score of %d\n", score);
		level=0;
	}
	if(!plr){
		score-=4;
		printf("You Lost Level 3\n");
		printf("You have a Score of %d\n", score);
		level=0;
	}
}
//update the location of a circle and refresh the display
//net effect, circles will move across the screen by taking 
//one step based on the direction vector within the circleType class.
/*Purpose: Updates all Positions,Checks all collisions, and decides who wins
 *Inputs: None
 *Outputs: None
 *Globals: Updates each circle location if they are still in the game decides
 *who wins based on the level they are on and updates the mouse to the current
 *location and redisplays everything
*/
void myIdle()
{
	vec2 newdir1, newdir2;
	circlecollisions(newdir1,newdir2);
	allwalls(newdir1);
	playercollisions();
for(int i=0;i<NumCircle;i++)
	if(ct[i])
 	  circle[i].takeAstep(); 
	if(level==1){
		levelwin1();
	}
	if(level==2){
		levelwin2();
	}
	if(level==4){
		levelwin3();
	}

	plrcircle.indir(mouseLoc);
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
   glClearColor(0,0,0,0.0);  //clear color is white
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

   glutCreateWindow( "Four Circle Game movement - shader version" );

#ifndef __APPLE__
   glewExperimental = GL_TRUE;
   glewInit();
#endif

   circle_pts = generateCirclePoints(3);
   initGLBuffer_and_shaders(circle_pts, NumPoints);
   
   myinit();
	srand(time(NULL));
	if(level==1)
		initlvl1();
	if(level==2||level==4)
		initlvl2();
	for(int i=0;i<NumCircle; i++){
		ct[i]=true;
	}
	glutSetCursor(GLUT_CURSOR_NONE);
   glutDisplayFunc(display);
   glutReshapeFunc(myReshape);
	glutPassiveMotionFunc(mouse);
   glutKeyboardFunc(keyboard);
   glutIdleFunc(myIdle);

   cout << endl;
   cout << "===========================" << endl;
   cout << "Use Right Click to Choose Level"<<endl;
	cout << "Q|q to quit the program" << endl;
   cout << "===========================" << endl;
	set_up_menu();
   glutMainLoop();
   return 0;
}
