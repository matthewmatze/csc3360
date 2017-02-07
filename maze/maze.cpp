/*Name: Matthew Matze
 *Date: 3-6-15
 *Class: CSC3360
 *Location:~/csc3360/maze/maze.cpp
 */

#include"../include/Angel.h"
#include <vector>
#include <cstdlib>
#include <stdio.h>
#include <stack>

	int NumPoints;
	int NumBread;

void display();
void keyboard(unsigned char key, int x, int y);
int init(int *iterations);
struct wall{
	bool outside;
	vec2 a,b;
	bool display;
	int cell[2];
};
struct breadcrumb{
	bool display;
	vec2 p[4];
};
void generateMaze(std::vector<wall> &maze, int *width, int *height, int *entrance, int *exit);
//void generateBreadCrumbs(std::vector<breadcrumb> &breadcrumbv, int *width, int *height);

int main(int argc, char **argv)
{
	int *iterations;
	iterations = (int*)malloc(2*sizeof(int));
	iterations[0] = atoi(argv[1]);
	iterations[1] = atoi(argv[2]);
	if(argc!=3){
		std::cerr<<"Invalid number of iterations\n";
		exit(1);
	}
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGBA);
   glutInitWindowSize(512,512);

   glutCreateWindow(" Maze ");

   //compatibility issues with the libraries and drivers installed
   //glewExperiental=GL_TRUE;
   glewInit();
   
   init(iterations);
   
	glutDisplayFunc(display);
   glutKeyboardFunc(keyboard);
   
	glutMainLoop();
   	return 0;
}
   
int init(int *iterations)
{
	std::vector<wall> maze;
//	std::vector<breadcrumb> breadcrumb;
	int n=iterations[0];
	int m=iterations[1];
	int entrance,exit;
	int* stack;
	int top=-1;
	stack = (int*)malloc(n*m*sizeof(int));
	vec2* points;
	vec2* breadcrumbs;
	//printf("%d",n);
	generateMaze(maze, &n,&m,&entrance,&exit);
//	generateBreadCrumbs(breadcrumb, &n, &m);
//	int breadcrumbsize=breadcrumb.size();
//	NumBread=breadcrumbsize;
	int mazesize=maze.size();
//	NumPoints=mazesize;
	//std::cout<<NumPoints;
	points = (vec2*)malloc(NumPoints*2*sizeof(vec2));
//	breadcrumbs = (vec2*)malloc(NumBread*4*sizeof(vec2));
	int r=0;
	for(int i=0;i<NumPoints*2;i+=2){
	//if(maze.front().outside==false)
		//printf("%d %d %d\n", i/2, maze.front().cell[0],maze.front().cell[1]);
		if(maze.front().display!=false){
			points[r]=maze.front().a;
			points[r+1]=maze.front().b;
			r+=2;
			//printf("%d %f %f %f %f\n", i, points[i].x, points[i].y, points[i+1].x, points[i+1].y);
		}
		maze.erase(maze.begin());
	}
	int q=0;
	//for(int i=0;i<NumBread*4;i+=4){
/*	for(int i=0;i<NumBread*4;i+=4){
		//if(breadcrumb.front().display!=false){
			breadcrumbs[q]=breadcrumb.front().p[0];
			q++;
			breadcrumbs[q]=breadcrumb.front().p[1];
			q++;
			breadcrumbs[q]=breadcrumb.front().p[2];
			q++;
			breadcrumbs[q]=breadcrumb.front().p[3];
			q++;
			printf("%d %f %f %f %f %f %f %f %f\n", q, points[q-4].x, points[q-4].y, points[q-3].x, points[q-3].y, points[q-2].x, points[q-2].y, points[q-1].x, points[q-1].y);
*/			//printf("%d %f %f %f %f %f %f %f %f\n", q, breadcrumbs[q-4].x, breadcrumbs[q-4].y, breadcrumbs[q-3].x, breadcrumbs[q-3].y, breadcrumbs[q-2].x, breadcrumbs[q-2].y, breadcrumbs[q-1].x, breadcrumbs[q-1].y);
		//}
		breadcrumb.erase(breadcrumb.begin());
	}
	//NumPoints=r);
	//q=q-(NumPoints*2);
	NumPoints=r;
	/*GLushort *data;
	data = (GLushort*)malloc(NumBread*4*sizeof(vec2));
	stack[0]=entrance;
	GLushort tmp = stack[0]*4+NumPoints*2;
*/
	//Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);
	//printf("looool");
	//Create and Initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER,NumPoints*sizeof(vec2),points, GL_STATIC_DRAW);
	//glBufferSubData(GL_ARRAY_BUFFER,0,NumPoints*16,points);
	//glBufferSubData(GL_ARRAY_BUFFER,NumPoints*16,NumBread*32,breadcrumbs);
	//data=(GLushort*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER,GL_READ_WRITE);
	
	

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
	//glViewport(0,0, 512,512);
	//printf("%i", NumPoints);
	glDrawArrays(GL_LINES, 0, NumPoints);//draw the points
	//glDrawElements(GL_QUADS,(NumBread+1)*4,GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
	//glutSwapBuffers();
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
void generateMaze(std::vector<wall> &maze, int *width, int *height,int *entrance, int *exit)
{
	srand(time(NULL));
	wall a;
	int k=0;
	int p=0;
	a.a.x=-.5;
	a.a.y=.5;
	a.display=true;
	int i,j;
	for(i=0;i<=*width;i++)
	{
		for(j=0;j<*height;j++)
		{
			maze.push_back(a);
			a.a.x=a.a.x+((float)1 / *height);
			maze.back().b=a.a;
			//printf("%f %f %f %f\n", maze.back().a.x, maze.back().a.y, maze.back().b.x, maze.back().b.y);
			if(i==*width||i==0){
				maze.back().outside=true;
				//printf("%d %d\n",i ,*height);
			}else{
				maze.back().cell[0]=k;
				maze.back().cell[1]=k+*height;
				//printf("%d %d %d\n", i*(*width)+j, maze.back().cell[0], maze.back().cell[1]);
				k++;
				maze.back().outside=false;
			}
		}	
		a.a.y=a.a.y-((float)1 / *width);
		a.a.x=-.5;
	}
	//std::cout<<"\n";
	a.a.x=-.5;
	a.a.y=.5;
	a.display=true;
	for(i=0;i<=*height;i++)
	{
		for(j=0;j<*width;j++)
		{
			maze.push_back(a);
			a.a.y=a.a.y-((float)1 / *width);
			//printf("%f %f\n", maze.back().a.x, maze.back().a.y);
			maze.back().b=a.a;
			//printf("%f %f %f %f\n", maze.back().a.x, maze.back().a.y, maze.back().b.x, maze.back().b.y);
			if(i==*height||i==0){
				maze.back().outside=true;
				//printf("%d %d\n",i ,*width);
			}else{
				maze.back().cell[0]=p;
				maze.back().cell[1]=p+1;
				//printf("%d  %d\n", maze.back().cell[0], maze.back().cell[1]);
				p+=*height;
				maze.back().outside=false;
			}
		}
		a.a.y=.5;
		a.a.x=a.a.x+((float)1 / *height);
	if(i!=0)
		p=(p-((*width)*(*height))+1);
	}
	//int mazesize=maze.size();
	std::vector<wall> inner,outer;
	while(!maze.empty())
	{
		//std::cout<<"Whole:\n"<<maze.back().a.x<<" "<<maze.back().a.y<<"\n";
		//std::cout<<maze.back().b.x<<" "<<maze.back().b.y<<"\n\n";
		if(maze.back().outside==false)
		{
			inner.push_back(maze.back());
			maze.erase(maze.end());
		}
		if(maze.back().outside==true)
		{
			outer.push_back(maze.back());
			maze.erase(maze.end());
		}
	}
	int innersize=inner.size();
	int outersize=outer.size();
	int rands = rand()%4;
	if(rands==0)
	{
		*entrance=rand()%(outersize-*width-*width-*height);
	}
	if(rands==1)
	{
		*entrance=(rand()%(outersize-*width-*width-*height))+*width;
	}
	if(rands==2)
	{
		*entrance=(rand()%(outersize-*width-*width-*height))+*width+*height;
	}
	if(rands==3)
	{
		*entrance=(rand()%(outersize-*width-*width-*height))+*width+*height+*width;
	}
	if(rands==0)
		*exit=*entrance+(*height);
	if(rands==1)
		*exit=*entrance-*height;
	if(rands==2)
		*exit=*entrance+*width;
	if(rands==3)
		*exit=*entrance-*width;
	printf("%d %d\n",rands,*exit);
	for(int i=outersize;i<innersize+outersize;i++)
	{
		maze.push_back(inner.back());
		inner.erase(inner.end());
		//std::cout<<"Inner: \n"<<maze.back().a.x<<" "<<maze.back().a.y<<"\n";
		//std::cout<<maze.back().b.x<<" "<<maze.back().b.y<<"\n\n";
	}
	for(int i=0;i<outersize;i++)
	{
		maze.push_back(outer.back());
		outer.erase(outer.end());
		if(i==*entrance)
		{
			maze.back().display=false;
			printf("%d\n",i);
		}
		if(i==*exit)
		{
			maze.back().display=false;
			printf("%d\n",i);
		}
		//std::cout<<"Outer: \n"<<maze.back().a.x<<" "<<maze.back().a.y<<"\n";
		//std::cout<<maze.back().b.x<<" "<<maze.back().b.y<<"\n\n";
	}
	int* cellused;
	cellused = (int*)malloc(((*width)*(*height))*sizeof(int));
		//printf("%d %d\n",innersize, outersize);
	for(int i=0; i<(*width)*(*height); i++)
		cellused[i]=-1;
	i=0;
	int x;
	int f,g;
	while(i<(((*height)*(*width))-1)){
		x=rand()%innersize; 
		f=maze.at(x).cell[0];
		g=maze.at(x).cell[1];
		while(cellused[g]!=(-1)){
			g=cellused[g];
		}
		while(cellused[f]!=(-1)){
			f=cellused[f];
		}
		//printf("\n %d %d %d \n", x, f, g);
		if(f!=g){
			cellused[f]=g;
			maze.at(x).display=false;
			//printf("%d %d\n", x, cellused[f]);
			i++;
		}
	}
	//maze.at(10).display=false;
}
/*void generateBreadCrumbs( std::vector<breadcrumb> &breadcrumbv, int *width, int *height)
{
	breadcrumb tmp,tmp2;
	tmp.p[0].x=-.5;
	tmp.p[0].y=.5;
	tmp.p[1].x=-.5;
	tmp.p[1].y=.5;
	tmp.p[2].x=-.5;
	tmp.p[2].y=.5;
	tmp.p[3].x=-.5;
	tmp.p[3].y=.5;
	tmp2=tmp;
	for(int j=0;j<(*height);j++)
	{
		for(int i=0;i<(*width);i++)
		{
			breadcrumbv.push_back(tmp);
			breadcrumbv.back().p[0].x+=((float)1 / *width / (float)2);
			breadcrumbv.back().p[1].y-=((float)1 / *height / (float)2);
			breadcrumbv.back().p[2].y-=((float)1 / *height);
			breadcrumbv.back().p[2].x+=((float)1 / *width / (float)2);
			breadcrumbv.back().p[3].y-=((float)1 / *height / (float)2);
			breadcrumbv.back().p[3].x+=((float)1 / *width);
			tmp.p[0].x+=((float)1 / *width);
			tmp.p[1].x+=((float)1 / *width);
			tmp.p[2].x+=((float)1 / *width);
			tmp.p[3].x+=((float)1 / *width);
		}
		tmp2.p[0].y-=((float)1 / *height);
		tmp2.p[1].y-=((float)1 / *height);
		tmp2.p[2].y-=((float)1 / *height);
		tmp2.p[3].y-=((float)1 / *height);
		tmp=tmp2;	
	}
}*/
