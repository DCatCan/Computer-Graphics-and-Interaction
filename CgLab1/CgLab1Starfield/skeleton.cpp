// Introduction lab that covers:
// * C++
// * SDL
// * 2D graphics
// * Plotting pixels
// * Video memory
// * Color representation
// * Linear interpolation
// * glm::vec3 and std::vector

#include "SDL.h"
#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include "SDLauxiliary.h"

using namespace std;
using glm::vec3;

// --------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
SDL_Surface* screen;
vector<vec3> stars(1000);
float r = float(rand()) / float(RAND_MAX);
float focalLength = SCREEN_HEIGHT/2;
float velocity = 0.001;
int t;
// --------------------------------------------------------
// FUNCTION DECLARATIONS

void Draw();
void Update();
float random(float max, float min);

// --------------------------------------------------------
// FUNCTION DEFINITIONS

int main( int argc, char* argv[] )
{
	screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
	t = SDL_GetTicks();
	for(int i = 0; i< stars.size(); i++){
		stars[i].x = random(1,-1);
		stars[i].y = random(1,-1);
		stars[i].z = random(1,0);
	}



	while( NoQuitMessageSDL() )

	{
		Draw();
		Update();

	}
	SDL_SaveBMP( screen, "screenshot.bmp" );
	return 0;
}

void Draw()
{
	SDL_FillRect( screen, 0, 0 );
	if( SDL_MUSTLOCK(screen) )
		SDL_LockSurface(screen);
	for( size_t s=0; s<stars.size(); ++s ){

		float x = stars[s].x;
		float y = stars[s].y;
		float z = stars[s].z;

		float u = focalLength *(x/z) + SCREEN_WIDTH/2;
		float v = focalLength *(y/z) + SCREEN_HEIGHT/2;

		vec3 color = 0.2f * vec3(1,1,1) / (stars[s].z*stars[s].z);
		PutPixelSDL(screen, u, v, color);
	}
	if( SDL_MUSTLOCK(screen) )
		SDL_UnlockSurface(screen);
	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}
float random(float max, float min){
	return ((float(rand()) / float(RAND_MAX)) * (max - min) + min);
}
void Update(){
	int t2 = SDL_GetTicks();
	float dt = float(t2-t);
	t = t2;
	for( int s=0; s<stars.size(); ++s ){
		stars[s].z = stars[s].z - dt*velocity;

		if( stars[s].z <= 0 )
			stars[s].z += 1;
		if( stars[s].z > 1 )
			stars[s].z -= 1;
	}

}
