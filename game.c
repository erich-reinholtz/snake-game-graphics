// My program is yet another version of the classical Snake game, this time with a more user-friendly interface

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>

// the global variables which are needed
enum {up, down, left, right};
int dir = right, max = 5;


//------------------------------------------------------------------------
// Section for functions related to graphics -> source: Modules lecture

struct display {
    int width, height, imageWidth, imageHeight;
    SDL_Window *window;
    SDL_Surface *surface;
    SDL_Surface *images[128];
    SDL_Renderer *renderer;
};

typedef struct display display;

// printing out error message
static void SDL_Fail(char *s) {
    if (s != NULL) fprintf(stderr, "%s\n", s);
    else fprintf(stderr, "%s\n", SDL_GetError());
    SDL_Quit();
    exit(1);
}

void *notNull(void *p) { if (p == NULL) SDL_Fail(NULL); return p; }

int notNeg(int n) { if (n < 0) SDL_Fail(NULL); return n; }

// Loading an image
static void loadImage(display *d, int k, char *filename) {
    char path[100];
    strcpy(path, "images/");
    strcat(path, filename);
    SDL_Surface *image = notNull(SDL_LoadBMP(path));
    d->images[k] = image;
}

// Loading all the images.
static void loadAllImages(display *d) {
  
  // loading blank
    loadImage(d, '.', "blank.bmp");
  
  // loading heads
  loadImage(d, '^', "head_up.bmp");
  loadImage(d, 'v', "head_down.bmp");
  loadImage(d, '<', "head_left.bmp");
  loadImage(d, '>', "head_right.bmp");
  
  // loading tails
   loadImage(d, 'w', "tail_up.bmp");
   loadImage(d, 's', "tail_down.bmp");
   loadImage(d, 'a', "tail_left.bmp");
   loadImage(d, 'd', "tail_right.bmp");
  
  // loading body - straight
   loadImage(d, '-', "horizontal.bmp");
   loadImage(d, '|', "vertical.bmp");
  
  // loading body - curved
   loadImage(d, 'e', "up+right.bmp");
   loadImage(d, 'q', "up+left.bmp");
   loadImage(d, 'x', "down+right.bmp");
   loadImage(d, 'z', "down+left.bmp");
  
  // loading food
   loadImage(d, 'f', "food.bmp");
  
  // loading numbers
  loadImage(d, '3', "3.bmp");
  loadImage(d, '2', "2.bmp");
  loadImage(d, '1', "1.bmp");
  
  // loading game_over
  loadImage(d, 'g', "over.bmp"); 
}

// generating a new display
display *newDisplay(int width, int height) {
    display *d = malloc(sizeof(struct display));
    d->width = width;
    d->height = height;
    d->imageHeight = d->imageWidth = 20;
    notNeg(SDL_Init(SDL_INIT_VIDEO));
    int w = d->width * d->imageWidth, h = d->height * d->imageHeight;
    d->window = notNull(SDL_CreateWindow("Snake", 100, 100, w, h, 0));
    d->surface = notNull(SDL_GetWindowSurface(d->window));
    loadAllImages(d);
    return d;
}

// drawing the entity onto the surface, according to given coordinates
void drawEntity(display *d, int k, int x, int y) {
    int px = x * d->imageWidth;
    int py = y * d->imageHeight;
    SDL_Surface *image = d->images[k];
    if (image == NULL) SDL_Fail("No image");
    SDL_Rect box_structure = { px, py, d->imageWidth, d->imageHeight };
    SDL_Rect *box = &box_structure;
    notNeg(SDL_BlitSurface(image, NULL, d->surface, box));
}

void drawFrame(display *d) {
    notNeg(SDL_UpdateWindowSurface(d->window));
    SDL_Delay(20);
}

void pause(display *d, int msecs) {
    SDL_Delay(msecs);
}


void clear(display *d) {
    SDL_SetRenderDrawColor(d->renderer, 255, 255, 255, 255);
    SDL_RenderClear(d->renderer);
    SDL_UpdateWindowSurface(d->window);
}

//--------------------------------------------------------------------
// logic


// checks if the conditions associated with the end of the game are verified
bool checkfail(int sx[800], int sy[800])
{
  if(sx[0] == 0 || sx[0] == 39 || sy[0] == 0 || sy[0] == 19)
    return 1;
  
  int i;
  
  for(i=1; i<max; i++)
    if(sx[0] == sx[i] && sy[0] == sy[i])
      return 1;
  
  return 0;
}

// expands the length of the snake
void add1(int sx[800], int sy[800], char *way)
{
  sx[max] = sx[max-1];
  sy[max] = sy[max-1];
  way[max] = way[max-1];
  
  if(way[max-1] == 'a' || way[max-1] == 'd')
    way[max-1] = '-';
  else 
    way[max-1] = '|';
    
    max++;
}


// redefines the direction in which the snake is heading, based on the user input
void newdir(display *d)
{
  SDL_PumpEvents();
  
  int *numk = malloc(sizeof(int));
  
  const Uint8 *state;  
    state = SDL_GetKeyboardState(numk);
 
  if(state[SDL_SCANCODE_LEFT] && (dir != right))
  {
     dir = left;
     return;
  }
  if(state[SDL_SCANCODE_RIGHT] && (dir != left))
  {
     dir = right; 
    return;
  }
  
  if(state[SDL_SCANCODE_UP] && (dir != down))
   {
     dir = up;
     return;
   }
   if(state[SDL_SCANCODE_DOWN] && (dir != up))
   {
     dir = down;
     return;
   }
  
}

// generates new coordinates for the location of the food
void newfood(int *fx, int *fy, int sx[800], int sy[800])
{
  bool ok = 0;
  int i, x, y;
  
  while(ok == 0)
  {
      ok = 1;
    
     srand(time(NULL));
     x = rand() % 18;
     y = rand() % 38;
    
    for(i=0; i<max && ok == 1 ; i++)
      if((sx[i] == x) && (sy[i] == y))
        ok = 0;
    
    if(x==0 || y==0)
      ok = 0;
  }
  
  *fx = x;
  *fy = y;
  
}

void newpos(int *sx, int *sy, int *fx, int *fy, char *way)
{
  int x, y;
  x = *fx;
  y = *fy;
  
  if(sx[0]==y && sy[0]==x)
  {
    newfood(&x, &y, sx, sy);
    add1(sx, sy, way);
  }
  
  *fx = x;
  *fy = y;
  
  for(int i=max-1; i>0; i--)
  {
    sx[i] = sx[i-1];
    sy[i] = sy[i-1];
  }
  
  if(dir == up)
    sy[0]--;
  if(dir == down)
    sy[0]++;
  if(dir == left)
    sx[0]--;
  if(dir == right)
    sx[0]++;
}

//-----------------------------------------------------------------------------------------------
// The flip functions assign the appropriate images for each component which forms up a snake

void fliphead(char *way)
{
  if(dir == up)
   way[0] = '^';
   if(dir == down)
   way[0] = 'v';
   if(dir == left)
    way[0] = '<';
   if(dir == right)
    way[0] = '>';
}

void flipbody(char *way)
{
  for(int i=max-2; i>=2; i--)
    way[i] = way[i-1];
}

void flipneck(char *way, int pdir)
{
  if(way[1] == '-')
  {
    if(dir == up && pdir == left)
      way[1] = 'e';
    
    if(dir == up && pdir == right)
      way[1] = 'q';
    
    if(dir == down && pdir == left)
      way[1] = 'x';
    
    if(dir == down && pdir == right)
      way[1] = 'x';
  }
  
  if(way[1] == '|')
  {
    if(pdir == up && dir == left)
      way[1] = 'z';
    
    if(pdir == up && dir == right)
      way[1] = 'x';
    
    if(pdir == down && dir == left)
      way[1] = 'q';
    
    if(pdir == down && dir == right)
      way[1] = 'e';
  }
  // first oblique case
  if(way[1] == 'x')
  {
    if(pdir == down)
    {
      if(dir == down)
        way[1] = '|';
      if(dir == left)
        way[1] = 'q';
      if(dir == right)
        way[1] = 'e';
    }
    
    if(pdir == right)
    {
      if(dir == up)
        way[1] = 'q';
      if(dir == right)
        way[1] = '-';
      if(dir == down)
        way[1] = 'z';
    }
  }
  
  // second oblique case
  if(way[1] == 'z')
  {
    if(pdir == down)
    {
      if(dir == down)
        way[1] = '|';
      if(dir == left)
        way[1] = 'q';
      if(dir == right)
        way[1] = 'e';
    }
    
    if(pdir == left)
    {
      if(dir == up)
        way[1] = 'e';
      if(dir == left)
        way[1] = '-';
      if(dir == down)
        way[1] = 'x';
    }
  }
  
  //third oblique case
  if(way[1] == 'e')
  {
    if(pdir == up)
    {
      if(dir == up)
        way[1] = '|';
      if(dir == left)
        way[1] = 'z';
      if(dir == right)
        way[1] = 'x';
    }
    
    if(pdir == right)
    {
      if(dir == up)
        way[1] = 'q';
      if(dir == right)
        way[1] = '-';
      if(dir == down)
        way[1] = 'z';
    }
  }
  
  //fourth oblique case
  if(way[1] == 'q')
  {
    if(pdir == up)
    {
      if(dir == up)
        way[1] = '|';
      if(dir == left)
        way[1] = 'z';
      if(dir == right)
        way[1] = 'x';
    }
    
    if(pdir == left)
    {
      if(dir == up)
        way[1] = 'e';
      if(dir == left)
        way[1] = '-';
      if(dir == down)
        way[1] = 'x';
    }
  }
  
}

void fliptail(char *way)
{
  if(way[max-1] == 'd')
  {
    if(way[max-2] == '-')
      way[max-1] = 'd'; 
    if(way[max-2] == 'q')
      way[max-1] = 'w';
    if(way[max-2] == 'z')
      way[max-1] = 's';
  }
  
  if(way[max-1] == 's')
  {
    if(way[max-2] == '|')
      way[max-1] = 's';
    if(way[max-2] == 'q')
      way[max-1] = 'a';
    if(way[max-2] == 'e')
      way[max-1] = 'd';
  }
  
  if(way[max-1] == 'a')
  {
    if(way[max-2] == '-')
      way[max-1] = 'a';
    if(way[max-2] == 'x')
      way[max-1] = 's';
    if(way[max-2] == 'e')
      way[max-1] = 'w';
  }
  
  if(way[max-1] == 'w')
  {
    if(way[max-2] == '|')
      way[max-1] = 'w';
    if(way[max-2] == 'z')
      way[max-1] = 'a';
    if(way[max-2] == 'x')
      way[max-1] = 'd';
  }
  
}

void flip(char *way, int pdir)
{
  fliptail(way);
  flipbody(way);
  
   fliphead(way);
  
 
  flipneck(way, pdir);
}

// draws the whole snake onto the display
void draw(display *d, int *sx, int *sy, char *way, int fx, int fy)
{
   drawEntity(d, 'f', fy, fx);
  
  for(int i=0; i<max; i++)
   drawEntity(d, way[i], sx[i], sy[i]);
}

// once conditions of checkfail are fulfilled, a game over sign is drawn onto the display
void over(display *d)
{
  pause(d, 100);
  clear(d);
  for (int i=0; i<40; i++) {
          for (int j=0; j<20; j++) {
              drawEntity(d, '.', i, j);
          }
      }
    
  drawEntity(d, 'g', 12, 4);
  drawFrame(d);
  pause(d, 1700);
  
  return;
}

// calls all previous functions, thus generating the snake's movement
void move(display *d, int *sx, int *sy, char *way)
{
  int fx, fy;
  int pdir;
  fx = fy = -1;
  newfood(&fx, &fy, sx, sy);
  
  while(1)
  {
    clear(d);
    pdir = dir;
    newdir(d);
    newpos(sx, sy, &fx, &fy, way);
  
    for (int i=0; i<40; i++) {
          for (int j=0; j<20; j++) {
              drawEntity(d, '.', i, j);
          }
      }
    
      flip(way, pdir);
      draw(d, sx, sy, way, fx, fy);
 
      drawFrame(d);
  
      pause(d, 69);
  
    if(checkfail(sx, sy))
    {    
      over(d);
      exit(1);   
    }
  }
}

// initialises the integer arrays containing coordinates for each component that forms up the snake
void init_s(int s[800])
{
    int i;
    for(i=0; i<800; i++)
        s[i] = -1;
}

// initialises the character array which remembers which appropriate picture is to be shown for each
// component that forms up the snake
void init_c(char way[800])
{
  way[0] = '>';
  way[1] = '-';
  way[2] = '-';
  way[3] = '-';
  way[4] = 'd';
}

// a classical 3-2-1 counter before the game begins
void start(display *d, char x)
{
  for (int i=0; i<40; i++) {
        for (int j=0; j<20; j++) {
            drawEntity(d, '.', i, j);
        }
    }
    
    drawEntity(d, x, 19, 8);
    drawFrame(d);
  pause(d, 1000);
}

//--------------------------------------------------------------------------
// Automated testing 

void testcheckfail()
{
  int sx[1000] = {5, 5, 6, 6, 5};
  int sy[1000] = {6, 5, 5, 6, 6};
  max = 5;
  assert(checkfail(sx, sy)==1);
  
  sx[4] = 7;
  assert(checkfail(sx, sy)==0);
  
  sy[0] = 0;
  sy[1] = sy[2] = 1;
  sy[3] = sy[4] = 2;
  assert(checkfail(sx, sy)==1);
}

bool checknewpos(int sx[800], int sy[800], int cx[800], int cy[800], char way[800], char trace[800], int max)
{
  for(int i=0; i<max; i++)
     if((sx[i] != cx[i]) || (sy[i]!=cy[i]) || (way[i] != trace[i]))
      return 0;
   return 1;
}

void testnewpos()
{
   int sx[800] = {4, 3, 3, 2, 1, 1, 1, 1};
   int sy[800] = {5, 5, 4, 4, 4, 3, 2, 1};
   char way[800];
   strcpy(way, "vze|z--d");
  
   max = 8;
  
   dir = down;
   int pdir = dir;
  
   int fx, fy;
   fx = fy = 5;
  
  int cx[800] = {5, 4, 3, 3, 2, 1, 1, 1}; 
  int cy[800] = {5, 5, 5, 4, 4, 4, 3, 2};
  char trace[800];
  strcpy(trace, "v|ze|z-d");
  
  newpos(sx, sy, &fx, &fy, way);
  flip(way, pdir);
  
  assert(checknewpos(sx, sy, cx, cy, way, trace, max));
  
  int dx[800] = {5, 5, 4, 3, 3, 2, 1, 1, 1};
  int dy[800] = {4, 5, 5, 5, 4, 4, 4, 3, 2};
  strcpy(trace, "<q|ze|z-d");
  
  newpos(sx, sy, &fx, &fy, way);
  flip(way, pdir);
  
  assert(checknewpos(sx, sy, dx, dy, way, trace, max));
}

void test()
{
  testcheckfail();
  testnewpos();
  printf("All tests pass\n");
}


//--------------------------------------------------------------------------
// The main function 

int main() {
    display *d = newDisplay(40, 20);
  
  int sx[800], sy[800];
  char way[800];
  
  init_s(sx);
  init_s(sy);
  init_c(way);
  
    sx[0] = 5;
    sy[0] = 5;
  
  for(int i=1; i<5; i++)
  {
    sx[i] = sx[i-1]-1;
    sy[i] = sy[i-1]; 
  }
  
  //test(); 
  
  //---------
  
  start(d, '3');
  start(d, '2');
  start(d, '1');
  
  //---------

  move(d, sx, sy, way);
  
  
    return 0;
}