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

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
SDL_Surface* screen;
vec3 topLeft(1,0,0); // red
vec3 topRight(1,0,1); // blue
vec3 bottomLeft(0,1,1); // green
vec3 bottomRight(1,0.3,0.1); // yellow
vector<vec3> leftSide( SCREEN_HEIGHT );
vector<vec3> rightSide( SCREEN_HEIGHT );

vector<vec3> left2right(SCREEN_WIDTH);


// --------------------------------------------------------
// FUNCTION DECLARATIONS

void Draw();
float step(float a, float b, float points);
void InterpolateTal(float a, float b, vector<float>& result);
void Interpolate(vec3 a, vec3 b, vector<vec3>& result);

// --------------------------------------------------------
// FUNCTION DEFINITIONS

int main( int argc, char* argv[] )
{
	screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
	/*vector<float> result( 10 ); // Create a vector width 10 floats
	Interpolate( 5, 14, result ); // Fill it with interpolated values
	for( int i=0; i<result.size(); ++i )
		cout << result[i] << " ";*/

	/*vector<vec3> result( 4 );
	vec3 a(1,4,9.2);
	vec3 b(4,1,9.8);
	Interpolate( a, b, result );

	for( int i=0; i<result.size(); ++i ){
		cout << "( "
		<< result[i].x << ", "
		<< result[i].y << ", "
		<< result[i].z << " ) ";
	}*/



	while( NoQuitMessageSDL() )

	{
		Draw();
	}
	SDL_SaveBMP( screen, "screenshot.bmp" );
	return 0;
}

void Draw()
{
	Interpolate(topLeft, bottomLeft, leftSide);
	Interpolate(topRight, bottomRight, rightSide);


	for( int y=0; y<SCREEN_HEIGHT; ++y )
	{
		Interpolate(leftSide[y],rightSide[y], left2right);

		for( int x=0; x<SCREEN_WIDTH; ++x )
		{



			vec3 color(left2right[x].x,left2right[x].y,left2right[x].z);
			//vec3 color(0,0,1);
			PutPixelSDL( screen, x, y, color );
		}
	}

	if( SDL_MUSTLOCK(screen) )
		SDL_UnlockSurface(screen);

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}

void InterpolateTal(float a, float b, vector<float>& result){
	float N = result.size();
	float step = (b-a)/(N-1);
	for(int i = 0; i < N; i++){
		result[i] = a;
		a += step;
	}
}
float step(float a, float b, float points){
	float c = (b-a)/(points-1);
	return c;
}
void Interpolate(vec3 a, vec3 b, vector<vec3>& result){
float N = result.size();
float xStep = step(a.x, b.x, N);
float yStep = step(a.y, b.y, N);
float zStep = step(a.z, b.z, N);

for( int i = 0; i < N; i++){
	result[i].x = a.x;
	result[i].y = a.y;
	result[i].z = a.z;

	a.x = a.x + xStep;
	a.y = a.y + yStep;
	a.z = a.z + zStep;
	}
}
