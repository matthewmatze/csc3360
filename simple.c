/*Name:Matthew Matze
 *Date:1-14-15
 *Class:CSC3360
 *Location:~/csc3360/simple.c
 */

/*
 *a) The display function gets executed only when you resize the image and not when you click the mouse in the window, move your mouse across the window, or when you move the image around the screen.

b) When you resize the window the white area becomes a rectangle.
 */
#include<stdio.h>
#include<GL/glut.h>
void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POLYGON);
		glVertex2f(-0.5,-0.5);
		glVertex2f(-0.5,0.5);
		glVertex2f(0.5,0.5);
		glVertex2f(0.5,-0.5);
	glEnd();
	glFlush();
}
int main(int argc,char **argv)
{
	glutInit(&argc, argv);
	glutCreateWindow("simple");
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}
