//////////////////////////////////////////////////////////////////////////////
// 
// Charlie Valentino 
// Spring 2012
// Stony Brook University
// CSE328 - Fundamentals of Computer Graphics
// Dr. Hong Qin
// 
//////////////////////////////////////////////////////////////////////////////
// This project will demonstrate the basics of fluid dynamics
// as based on the paper by Jos Stam.  This will represent a 3d
// adaptation of the procedures as described in the paper.
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define IX(i,j) ((i)+(N+2)*(j))	//convert a 2dim index to a single dim
#define SWAP(x0,x) {float * tmp=x0;x0=x;x=tmp;}	//facilitate swapping arrays

#include "windows.h"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <iomanip>

#include "SolverFunctions.h"

#include <gl/glut.h>

using namespace std;

/* GLOBALS */
GLint winHeight = 512, winWidth = 512;
GLint winPosX = 450, winPosY = 150;
int menu_id, mouse_mode;
int paused;
static int N;
static float dt, diff, visc;
static float force, source;
static int dvel;

static float *u, *v, *u_prev, *v_prev;

static float *dens, *dens_prev;
static int mouse_down[3];
static int omx, omy, mx, my;

//FUNCTION DECLARATIONS
void printUsage(void);

////////////////////////////////////////////////////////
// Begin Fluid Simulation
////////////////////////////////////////////////////////



/*
*	init - initializes various properties of the simulation. 
*		Also opens a file for matrix output.
*
*/
static int init ( void )
{
	N = 128;				//	4	64	128	
	dt = 0.1f;			//0.1
	diff = 0.0f;		//0.0 - 0.000001
	visc = 0.0f;		//0.0 - 0.005
	force = 30.0f;		//5.0
	source = 150.0f;		//100.0
	int size = (N+2)*(N+2);












	u			= (float *) malloc ( size*sizeof(float) );
	v			= (float *) malloc ( size*sizeof(float) );
	u_prev		= (float *) malloc ( size*sizeof(float) );
	v_prev		= (float *) malloc ( size*sizeof(float) );
	dens		= (float *) malloc ( size*sizeof(float) );	
	dens_prev	= (float *) malloc ( size*sizeof(float) );

	if ( !u || !v || !u_prev || !v_prev || !dens || !dens_prev ) {
		fprintf ( stderr, "cannot allocate data\n" );
		return ( 0 );
	}


	// CLEAR THE SCREEN AND SET A BACKGROUND COLOR
	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();

	paused = 0;

	printUsage();

	return ( 1 );
}


/*
*	clear_data - clears the data in the solver as well as the screen.
*
*/
static void clear_data ( void )
{
	int i, size=(N+2)*(N+2);

	for ( i=0 ; i<size ; i++ ) {
		u[i] = v[i] = u_prev[i] = v_prev[i] = dens[i] = dens_prev[i] = 0.0f;
	}
}



/*
*	draw_density - visualizes the density to the screen.
*
*/
static void draw_density ( void )
{
	int i, j;
	float x, y, h, d00, d01, d10, d11;

	h = 1.0f/N;

	glBegin ( GL_QUADS );
	
	for ( i=0 ; i<=N ; i++ ) {
		x = (i-0.5f)*h;
		for ( j=0 ; j<=N ; j++ ) {
			y = (j-0.5f)*h;

			d00 = dens[IX(i,j)];
			d01 = dens[IX(i,j+1)];
			d10 = dens[IX(i+1,j)];
			d11 = dens[IX(i+1,j+1)];

			glColor3f ( d00, d00, d00 ); glVertex2f ( x, y );
			glColor3f ( d10, d10, d10 ); glVertex2f ( x+h, y );
			glColor3f ( d11, d11, d11 ); glVertex2f ( x+h, y+h );
			glColor3f ( d01, d01, d01 ); glVertex2f ( x, y+h );
			//fprintf(stderr,"%f  %f  %f  %f   \t", d00, d10, d11, d01);
		}
	}

	glEnd ();
}

/*
*	printDensityMatrix - prints out the density matrix to a file in the same 
*		directory as the executable or the project source "Density_Output.txt"
*
*/
void printDensityMatrix(){
	
	//OPEN FILE FOR PRINTING THE MATRIX
	fstream densOutFile, velXOutFile, velYOutFile;

	densOutFile.open("Density_Output.txt", fstream::out);
	velXOutFile.open("Velocity_X_Output.txt", fstream::out);
	velYOutFile.open("Velocity_Y_Output.txt", fstream::out);

	densOutFile.precision(6);
	velXOutFile.precision(6);
	velYOutFile.precision(6);

	//OUTPUT FORMATTED TEXT OF THE DENSITY AND VELOCITY MATRIX
	for ( int i=N ; i>=1 ; i-- ) {
		for ( int j=1; j<=N ; j++ ) {
			densOutFile << fixed << std::right << setfill(' ') << setw(10) << dens[IX(j,i)] << "\t";
			velXOutFile << fixed << std::right << setfill(' ') << setw(10) << u[IX(j,i)] << "\t";
			velYOutFile << fixed << std::right << setfill(' ') << setw(10) << v[IX(j,i)] << "\t";
		}
		densOutFile << endl;
		velXOutFile << endl;
		velYOutFile << endl;
	}

	densOutFile.close();
}


/*
*	get_from_UI - gets input from the mouse and user.  
*		Adds velocity(LEFT_BUTTON) or density(RIGHT_BUTTON) to the 
*		corresponding matrix.
*
*/
static void get_from_UI ( float * d, float * u, float * v )
{
	int i, j, size = (N+2)*(N+2);

	// CLEAR PREVIOUS DENSITY TO ALLOW MOVEMENT
	for ( i=0 ; i<size ; i++ ) {
		u[i] = v[i] = d[i] = 0.0f;
	}

	if ( !mouse_down[0] && !mouse_down[2] ) return;

	i = (int)((mx /(float)winWidth)*N+1);
	j = (int)(((winHeight-my)/(float)winHeight)*N+1);

	if ( i<1 || i>N || j<1 || j>N ) return;

	if ( mouse_down[0] ) {
		u[IX(i,j)] = force * (mx-omx);
		v[IX(i,j)] = force * (omy-my);
		u[IX(i,N/2)] = force * 200;		//<------------------------
	}

	if ( mouse_down[2] ) {
		d[IX(i,j)] = source;
	}

	omx = mx;
	omy = my;

	return;
}


/*
*	idleFunc - solves the simulation and gets input while simulation is running.
*
*/
static void idleFunc ( void )
{
	if(!paused){
		get_from_UI ( dens_prev, u_prev, v_prev );
		vel_step ( N, u, v, u_prev, v_prev, visc, dt );
		dens_step ( N, dens, dens_prev, u, v, diff, dt );
	}
	//glutSetWindow ( win_id );
	glutPostRedisplay ();
}


/*
*	KEYBOARD FUNCTION CALLED BY glutMainLoop
*
*/
void keyboard( unsigned char key, int x, int y){
	switch (key){
	case 'c':
		//CLEAR ALL SIMULATION INFO
		cout << "Data cleared!" <<endl << endl;
		clear_data();
		paused = false;
		break;
	case 'p':
		//PAUSES AND PRINTS DENSITY MATRIX
		cout << "Data printed to Density_Output.txt, "
			"Velocity_X_Output.txt, Velocity_Y_Output.txt " << endl;
		cout << "Press SPACE to continue simulation" << endl << endl;
		printDensityMatrix();
		paused = true;
		break;
	case ' ':
		//PAUSE THE SIMULATION
		cout << "Press SPACE to continue simulation" << endl <<endl;
		paused = !paused;
		break;
	default:
		printUsage();
	}
}


/*
*	printUsage - print the instructions for usage
*
*/
void printUsage(){
	cout << "Welcome!" <<endl;
	cout << "Right Mouse button to add source smoke" <<endl;
	cout << "Left Mouse button to add velocity" <<endl;
	cout << "Press 'c' to clear the simulation and all data." <<endl;
	cout << "Press 'SPACE' to pause the simulation" <<endl;
	cout << "Press 'p' to print out the density, and velocity matrices to txt files" 
		" relative to the exe or source directory."<<endl << endl;

}


/*
*	MOTION FUNCTION CALLED BY glutMainLoop
*
*/
void mouseMotion(GLint xMouse, GLint yMouse){
	mx = xMouse;
	my = yMouse;
}


/* 
*	MOUSECLICK FUNCTION CALLED BY glutMainLoop
*
*/ 
void mouseClick(GLint button, GLint action, GLint xMouse, GLint yMouse){

	omx = mx = xMouse;
	omy = my = yMouse;

	mouse_down[button] = action == GLUT_DOWN;
}


/* 
* menu + menuCreator
*
*/
void menu(int value) //Menu function;
{
	if(value == 1)
		exit(-1);  // quit
	else if(value == 2){
		clear_data();
	}
}

void menuCreator(void)
{
	menu_id = glutCreateMenu(menu);

	glutAddMenuEntry("CLEAR",2);
	glutAddMenuEntry("Quit",1);

	glutAttachMenu(GLUT_KEY_F1);
}


/*
* DISPLAY FUNCTION CALLED BY glutMainLoop 
*
*/
void display(void){ 

	glViewport ( 0, 0, winWidth, winHeight);
	glMatrixMode ( GL_PROJECTION );
	glLoadIdentity ();
	gluOrtho2D ( 0.0, 1.0, 0.0, 1.0 );
	glClearColor ( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear ( GL_COLOR_BUFFER_BIT );

	draw_density ();

	glutSwapBuffers ();
}


/* 
*	WINDOW RESIZE FUNCTION CALLED BY glutMainLoop
*
*/
void winResize(GLint newWidth, GLint newHeight){
	glViewport (0, 0, newWidth, newHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-(newWidth/2), (newWidth/2), -(newHeight/2), (newHeight/2));

	winWidth = newWidth;
	winHeight = newHeight;

	glFlush();
	glutPostRedisplay();
}

//////////////////////----MAIN----////////////////////////////////////////////

int main(int argc, char **argv){

	// INITIALIZE GLUT
	glutInit(&argc, argv);

	// NOTE THE VALUES(WINDOW ATTRIBUTES) THAT CAN BE PASSED HERE 
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA );

	// INITIALIZE GLUT WINDOW
	glutInitWindowSize(winWidth, winHeight);
	glutInitWindowPosition(winPosX, winPosY); 
	glutCreateWindow("Final Project");


	glutSetCursor(GLUT_CURSOR_CROSSHAIR);
	glDisable(GL_DEPTH_TEST);

	// REGISTER	THE	display,
	//				mouseClick,
	//				winResize,
	// FUNCTIONS WITH GLUT
	glutDisplayFunc(display);
	glutMouseFunc(mouseClick);
	glutReshapeFunc(winResize); 
	glutKeyboardFunc(keyboard);
	//glutSpecialFunc(keyboard2GL);
	glutIdleFunc(idleFunc);
	glutMotionFunc(mouseMotion);
	menuCreator();

	if (!init()){
		fprintf(stderr, "Fatal Error in allocating memory space!!" );
		exit(1);
	}

	clear_data();

	// GLUT'S MAIN LOOP THAT IS CALLED FOREVER. NOTE THAT 
	// THIS FUNCTION NEVER RETURNS
	glutMainLoop();
}

