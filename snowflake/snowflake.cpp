/*Name: Matthew Matze
 *Date: 1-27-15
 *Class: CSC3360
 *Location:~/csc3360/Snowflake/snowflake.cpp
 */

#include"../include/Angel.h"
#include <vector>
#include <cstdlib>
#include <stdio.h>

	int NumPoints=0;

int init(int iterations);
void display();
void keyboard(unsigned char key, int x, int y);
std::vector<vec2> koch(std::vector<vec2> snowflake,std::vector<vec2> temp, int j, int *n);

int main(int argc, char **argv)
{
	int iterations = atoi(argv[1]);
printf("fdasf");
	if(iterations<0){
		std::cerr<<"Invalid number of iterations\n";
		exit(1);
	}
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGBA);
   glutInitWindowSize(512,512);
   glutCreateWindow(" Snowflake ");

   //compatibility issues with the libraries and drivers installed
   //glewExperiental=GL_TRUE;
   glewInit();
   
   init(iterations);
   
	glutDisplayFunc(display);
   glutKeyboardFunc(keyboard);
   
	glutMainLoop();
   	return 0;
   }
   
int init(int iterations)
{
std::vector<vec2> snowflake, temp;
vec2 a;
vec2* points;

printf("sdfa");
int j=0;
int n=0;
a.x=0;
a.y=.5;
snowflake.push_back(a);
a.x=.5;
a.y=-sqrt(3)/4;
snowflake.push_back(a);
a.x=-.5;
a.y=-sqrt(3)/4;
snowflake.push_back(a);
	n=snowflake.size();
for(int i=0;i<iterations;i++){
	snowflake=koch(snowflake, temp, j, &n);
	n=snowflake.size();
}
//printf("%i", n);
NumPoints=n;
points =(vec2*)malloc(n*sizeof(vec2));
	for(int j=0;j<n;j++){
	//printf("%f %f\n", snowflake.front().x,snowflake.front().y);
	points[j]=snowflake.front();
	//printf("%f %f\n", points[j].x,points[j].y);
	snowflake.erase(snowflake.begin());
}
	//Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);


	//Create and Initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER,NumPoints*sizeof(vec2),points, GL_STATIC_DRAW);

	//Load Shaders and use the resulting shader program
	GLuint program = InitShader("../include/vshader21.glsl", "../include/fshader21.glsl");
	glUseProgram(program);

	//Initialize the vertex position attribute from the vertex shader
	GLuint loc=glGetAttribLocation(program,"vPosition");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc,2,GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glClearColor(0,0,0,0); //Black Background
	return 0;
}
void display()
{
   glClear(GL_COLOR_BUFFER_BIT);//Clear the Window
	glViewport(0,0, 512,512);
printf("%i", NumPoints);
	glDrawArrays(GL_LINE_LOOP, 0, NumPoints);//draw the points
	glFlush();
}

void keyboard(unsigned char key, int x, int y)
{
	switch(key){
   case 'q':
		exit(EXIT_SUCCESS);
      break;
   case 033:
		exit(EXIT_SUCCESS);
      break;
   case 'Q':
		exit(EXIT_SUCCESS);
      break;
	}
}
std::vector<vec2> koch(std::vector<vec2> snowflake,std::vector<vec2> temp, int j, int *n)
{
std::vector<vec2>::iterator it;
vec2 p1,p2,p3,p4,p5, mid1, mid2, mid3, mid2b, mid3b;
//printf("%f %f %f %f ", p1.x, p1.y ,p2.x, p2.y);
//printf("%f %f\n ", p3.x, p3.y);
if(j==*n-1){
	p1=snowflake.front();
	p5=temp.front();
}
if(j!=*n-1)
	{
		it=snowflake.begin();
		for(int i=0; i<2;i++)
		{
		if(i==0)
			p1=*it;
		if(i==1)
			p5=*it;
		it++;
	}
}
mid1.x=((p1.x+p5.x)/2);
mid1.y=((p1.y+p5.y)/2);
mid2.x=((mid1.x+p1.x)/2);
mid2.y=((mid1.y+p1.y)/2);
mid3.x=((mid1.x+mid2.x)/2);
mid3.y=((mid1.y+mid2.y)/2);

p2=mid3;

mid2b.x=((mid1.x+p5.x)/2);
mid2b.y=((mid1.y+p5.y)/2);
mid3b.x=((mid1.x+mid2b.x)/2);
mid3b.y=((mid1.y+mid2b.y)/2);

p4=mid3b;

p3.x=((p1.x+p5.x)/2)-(sqrt(3)/6)*(p5.y-p1.y);
p3.y=((p1.y+p5.y)/2)+(sqrt(3)/6)*(p5.x-p1.x);
temp.push_back(p1);
temp.push_back(p2);
temp.push_back(p3);
temp.push_back(p4);
snowflake.erase(snowflake.begin());

j++;
if(j<*n){
	temp=koch(snowflake,temp, j, n);
}
return temp;
}
