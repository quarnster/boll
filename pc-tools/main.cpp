#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
// #include <karma.h>


int quit=0;

void refreshGL() {
	SDL_GL_SwapBuffers();
}
void resizeGLScene(unsigned int width, unsigned int height)
{
	    if (height == 0)    /* Prevent A Divide By Zero If The Window Is Too Small */
		            height = 1;
	        /* Reset The Current Viewport And Perspective Transformation */
	        glViewport(0, 0, width, height);
		    glMatrixMode(GL_PROJECTION);
		        glLoadIdentity();
			    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
			        glMatrixMode(GL_MODELVIEW);
}
void initGL()
{
	    glEnable(GL_TEXTURE_2D);       /* Enable Texture Mapping */
	        glShadeModel(GL_SMOOTH);
		    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		        glClearDepth(1.0f);
			    glEnable(GL_DEPTH_TEST);
			        glDepthFunc(GL_LEQUAL);
				    glEnable(GL_LIGHT0);
				        glEnable(GL_LIGHTING);
					    glEnable(GL_COLOR_MATERIAL);
					        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
						    /* we use resizeGLScene once to set up our initial perspective */
						    resizeGLScene(640,480);
						        glFlush();
}


int main(int argc,char *argv[])
{
	int flags=SDL_OPENGL;
	SDL_Event e;

	SDL_Init(SDL_INIT_VIDEO/*|SDL_INIT_AUDIO*/|SDL_INIT_TIMER);

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_SetVideoMode(640,480,32,flags);

	SDL_WM_SetCaption("otbintro",0);
	SDL_ShowCursor(0);


	initGL();

	float agl = 0;
	while (!quit) {
		SDL_GL_SwapBuffers();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		glTranslatef(0,0, -7);
		glRotatef(agl, 1, 0, 0);
		glRotatef(agl*1.1, 0, 1, 0);
		glRotatef(agl*0.9, 0, 0, 1);
		agl += 5.f;

		glColor3f(1,1,1);
		glNormal3f(0,0, 1);
		glBegin(GL_TRIANGLE_STRIP);

		/* disc-generator */
		/*
			int NUM = 20;
			for (int i = 0; i < NUM; i++) {
				float per = (i / ((float) NUM)) * 2 * 3.141592f;
				glVertex3f(sin(per), cos(per), 0);
				glVertex3f(0,0,0);
			}
			glVertex3f(0, 1, 0);
			glVertex3f(0,0,0);
		*/
		glEnd();


		while(SDL_PollEvent(&e) > 0) {
			if(e.type==SDL_MOUSEBUTTONDOWN)
				quit=1;
			if(e.type==SDL_KEYDOWN)
				quit=1;
		}
		SDL_Delay(1);
	}

	SDL_Quit();
	return 0;
}
