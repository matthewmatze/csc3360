/*Name: Matthew Matze
 *Date: 1-21-15
 *Class: CSC3360
 *Location:~/csc3360/circle/circle.cpp
 */
#include"Angel.h"

	int NumPoints=0;

struct coor{
	float x,y;
};
std::queue<coor> circlefunc(std::queue<coor> circle, std::queue<coor> temp, std::queue<coor> work, coor p1, coor p2, int i);
void display();
void keyboard(unsigned char key, int x, int y);
int init();

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(512, 512);

	glutCreateWindow(" Circle ");

	//compatibility issues with the libraries and drivers installed
	//glewExperiental=GL_TRUE;
	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

	glutMainLoop();
	return 0;
}


int init()
{
	std::queue<coor> circle, temp, work;
	coor a,p1,p2;
	vec2 *points;

	int i=0;
	a.x=0;
	a.y=1;
	p1=a;
	circle.push(a);
	a.x=1;
	a.y=0;
	p2=a;
	circle.push(a);
	a.x=0;
	a.y=-1;
	circle.push(a);
	a.x=-1;
	a.y=0;
	circle.push(a);
	NumPoints=circle.size();
	//printf("%i", n);
	for(int j=0;j<10;j++){
		circle=circlefunc(circle, temp, work, p1, p2, i);
		NumPoints=circle.size();
	}
	printf("%i\n", NumPoints);
	points =(vec2*)malloc(NumPoints*sizeof(vec2));
	for(int j=0;j<NumPoints;j++){
		//printf("%f %f\n", circle.front().x,circle.front().y);
		points[j].x=circle.front().x/40;
		points[j].y=circle.front().y/40;
		printf("%f %f\n", points[j].x,points[j].y);
		circle.pop();
	}
	//Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);

	//Create and Initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, NumPoints*sizeof(coor), points, GL_STATIC_DRAW);

	//Load Shaders and use the resulting shader program
	GLuint program = InitShader("vshader21.glsl", "fshader21.glsl");
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

//printf("%i", NumPoints);
	glDrawArrays(GL_POINTS, 0, NumPoints);//draw the points
	glFlush();
}

void keyboard(unsigned char key, int x, int y)
{
	switch(key){
	case 033:
		exit(EXIT_SUCCESS);
		break;
	}
}

std::queue<coor> circlefunc(std::queue<coor> circle,std::queue<coor> temp,std::queue<coor> work, coor p1, coor p2, int i)
{
coor p3;
double midx, midy, len;
if(i!=NumPoints-1){
work=circle;
p1=work.front();
work.pop();
p2=work.front();
//printf("%f %f %f %f ", p1.x, p1.y ,p2.x, p2.y);
}
if(i==NumPoints-1){
p1=circle.front();
p2=temp.front();
//printf("%f %f %f %f ", p1.x, p1.y ,p2.x, p2.y);
}	
midx=p1.x-p2.x;
midy=p1.y-p2.y;
len=sqrt((midx*midx)+(midy*midy));
p3.x=((p1.x+p2.x))/len;
p3.y=((p1.y+p2.y))/len;
//printf("%f %f\n ", p3.x, p3.y);
circle.pop();
temp.push(p1);
temp.push(p3);
//printf("%f %f\n ", temp.back().x, temp.back().y);
i++;
if(i<NumPoints){
	temp=circlefunc(circle, temp, work, p2, p1, i);
}
return temp;
}
