/*
 * @Author Kalan
 * @Date 2017-2-9
 * @Description 圖學作業 1 - 產生迷宮
 */

#include "Angel.h"
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <vector>

/* Screen Width and Height */
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

/* Maze Width and height */
const int MAZE_WIDTH = 15;
const int MAZE_HEIGHT = 15;

enum { N=1, E=4, S=2, W=8 };

int DX[9];
int DY[9];
int OPPOSITE[9];

int shuffle_array(int *array, int size);


GLuint init_shader( void ) {
    GLuint program = InitShader( "vshader21.glsl", "fshader21.glsl" );
    glUseProgram( program );
    
    return program;

}

int shuffle_array(int *array, int size) {
  int i;
  
  for( i=0; i<(size - 1); i++) {
    int r = i + (rand() % (size - i));
    int temp = array[i];
    array[i] = array[r];
    array[r] = temp;
  }
  
  return *array;
}

void gen_maze(int cx, int cy, int grid[][MAZE_HEIGHT]) {
    int dx, dy, nx, ny;
    int directions[4] = {N, E, S, W};

    shuffle_array(directions, 4);
    
    int i;
    
    for(i = 0; i < 4; i++) {
        dx = DX[directions[i]];
        dy = DY[directions[i]];
        
        nx = cx + dx;
        ny = cy + dy;

        if ( ((nx < MAZE_WIDTH) & (nx >= 0)) & ((ny < MAZE_HEIGHT) & (ny >= 0)) ) {
            if (grid[nx][ny] == 0) {
                grid[cx][cy] = (int)((int)grid[cx][cy] | (int)directions[i]);
                grid[nx][ny] = (int)((int)grid[nx][ny] | (int)OPPOSITE[directions[i]]);
                gen_maze(nx, ny, grid);
            }
        }
        
    }}

/**
 * draw maze wall with given x, y
 * @params x, y
 * @params wall int wall direction (0001: N, 0010: S, 0100: E, 1000: W)
 */
void draw_wall(int x, int y, int wall, std::vector<vec2> *vect) {
  float offsetX = (float) 1 / MAZE_WIDTH;
  float offsetY = (float) 1 / MAZE_HEIGHT;
  
  
  /*======= draw Top ========*/
  if ((wall & N) == 0) {
    vect->push_back(vec2(
      -1 + (2 * x * offsetX),
      1 - ( 2* y * offsetY)
     ));
    
    vect->push_back(vec2(
      -1 + ( 2* (x + 1) * offsetX),
      1 - ( 2* y * offsetY)
    ));
  }
  
  /*======= draw Bottom ========*/
  if ((wall & S) == 0) {
    vect->push_back(vec2(
      -1 + ( 2 * x * offsetX),
      1 - ( 2 * (y + 1) * offsetY)
    ));
    
    vect->push_back(vec2(
      -1 + ( 2 * (x + 1) * offsetX),
      1 - ( 2 * (y + 1) * offsetY)
    ));
    
  }
  
  /*======= draw Right ========*/
  if ((wall & E) == 0) {
    vect->push_back(vec2(
                         -1 + ( 2 * (x + 1) * offsetX),
                         1 - ( 2 * y * offsetY)
                         ));
    
    vect->push_back(vec2(
                         -1 + ( 2 * (x + 1) * offsetX),
                         1 - ( 2 * (y + 1) * offsetY)
                         ));
  }
  
  /*======= draw Left ========*/
  if ((wall & W) == 0) {
    vect->push_back(vec2(
                         -1 + ( 2 * x * offsetX),
                         1 - ( 2 * y * offsetY)
                         ));
    
    vect->push_back(vec2(
                         -1 + ( 2 * x * offsetX),
                         1 - ( 2 * (y + 1) * offsetY)
                         ));
  }
}

void
init( void )
{
    GLuint program = init_shader();
    int mazes[MAZE_WIDTH][MAZE_HEIGHT];
    
    std::vector<vec2> vect;
  
  
    srand((unsigned int)time((time_t *)NULL)); /* random seed */
    memset(&mazes[0], 0, sizeof(mazes)); /* set all Array value to 0 */
    
    OPPOSITE[N] = S;
  
    OPPOSITE[E] = W;
  
    OPPOSITE[S] = N;
  
    OPPOSITE[W] = E;
  
    DX[N] = 0;
    DX[E] = 1;
    DX[S] = 0;
    DX[W] = -1;
    
    DY[N] = -1;
    DY[E] = 0;
    DY[S] = 1;
    DY[W] = 0;
    
    gen_maze(0, 0, mazes);
    
    
    for (int x = 0; x < MAZE_WIDTH; x++) {
        for(int y = 0; y < MAZE_HEIGHT; y++) {
#ifdef DEBUG
            std::printf("%d ", mazes[x][y]);
#endif
            draw_wall(y, x, mazes[y][x], &vect);
        }
    }
    
    GLuint vao;
    glGenVertexArraysAPPLE( 1, &vao );
    glBindVertexArrayAPPLE( vao );

    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
  
    /* Convert vector to array */
    vec2 vects[MAZE_WIDTH * MAZE_HEIGHT * 10];
    std::copy(vect.begin(), vect.end(), vects);
  
    glBufferData( GL_ARRAY_BUFFER, sizeof(vects), vects, GL_STATIC_DRAW );

    GLuint loc = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( loc );
    glVertexAttribPointer(
                          loc,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          BUFFER_OFFSET(0)
    );

    glClearColor( 0.0, 0.0, 0.0, 1.0 ); // with black background
  
}

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT );
    glPointSize(10.0);
    glDrawArrays( GL_LINES, 0, MAZE_WIDTH * MAZE_HEIGHT * 10 );
    glFlush();
}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
    case 033:
        exit( EXIT_SUCCESS );
        break;
    }
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_RGBA );
    glutInitWindowSize( SCREEN_WIDTH, SCREEN_HEIGHT );

    // If you are using freeglut, the next two lines will check if 
    // the code is truly 3.2. Otherwise, comment them out
    
    glutCreateWindow( "Maze Generator" );

    
    init();
    
    
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

    glutMainLoop();
    return 0;
}
