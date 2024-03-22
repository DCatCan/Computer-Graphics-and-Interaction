#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"

using namespace std;
using glm::vec3;
using glm::mat3;

// ----------------------------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH = 300;
const int SCREEN_HEIGHT = 300;
vec3 lightPos( 0, -0.5, -0.7 ); //lightsource position
vec3 lightColor = 10.f * vec3( 1, 1, 1 ); //color tone of the light
vec3 indirectLight = 0.5f*vec3( 1, 1, 1 );
vec3 black(0,0,0);
mat3 C; // Camera Rotation
mat3 R; //Item rotation
float yaw; // angel
float yaw2;
float focalLength = SCREEN_HEIGHT;
vec3 cameraPos( 0, 0, -(3.001));

SDL_Surface* screen;
int t;

vector<Triangle> triangles;




// ----------------------------------------------------------------------------
// FUNCTIONS
void LoadTestModel( vector<Triangle>& triangles );
void Update();
void Draw();

struct Intersection{
	vec3 position;
	float distance;
	int triangleIndex;
};

bool ClosestIntersection(
	vec3 start,
	vec3 dir,
	const vector<Triangle>& triangles,
	Intersection& closestIntersection
);
vec3 DirectLight( const Intersection& i);




int main( int argc, char* argv[] )
{
	screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
	t = SDL_GetTicks();	// Set start value for timer.
	LoadTestModel( triangles);

	while( NoQuitMessageSDL() )
	{

		Update();
		Draw();
	}

	SDL_SaveBMP( screen, "screenshot.bmp" );
	return 0;
}

void Update()
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2-t);
	t = t2;
	cout << "Render time: " << dt << " ms." << endl;
	Uint8* keystate = SDL_GetKeyState( 0 );

	float move = 0.1;

if( keystate[SDLK_UP] )
{
// Move camera forward

cameraPos.z += move;

}
if( keystate[SDLK_DOWN] )
{
// Move camera backward
cameraPos.z -= move;
}
if( keystate[SDLK_LEFT] )
{
// Move camera to the left
yaw+= move;
}
if( keystate[SDLK_RIGHT] )
{
// Move camera to the right
yaw -= move;
}
//ROTATION AROUND THE CAMERA Y-Axis
if( keystate[SDLK_z] )
{
	yaw2 += move;
}
if( keystate[SDLK_c] )
{
	yaw2 -= move;
}
R = mat3(cos(yaw), 0, sin(yaw),0,		 1,			0, -sin(yaw), 0,			cos(yaw));
C = mat3(cos(yaw2), 0, sin(yaw2),0,		 1,			0, -sin(yaw2), 0,			cos(yaw2));


//light source: move keys
if( keystate[SDLK_w] ) // S: Backwards
	lightPos.z += move;
if( keystate[SDLK_s] ) // S: Backwards
	lightPos.z -= move;
if( keystate[SDLK_d] ) // D: Right
	lightPos.x += move;
if( keystate[SDLK_a] ) // A: Left
	lightPos.x -= move;
if( keystate[SDLK_q] ) // Q: Down
	lightPos.y += move;
if( keystate[SDLK_e] ) // E: Up
	lightPos.y -= move;



}

void Draw()
{
	if( SDL_MUSTLOCK(screen) )
		SDL_LockSurface(screen);

	vec3 currentDirectLight;

	Intersection closestIntersection;


	for( int y=0; y<SCREEN_HEIGHT; ++y )
	{
		for( int x=0; x<SCREEN_WIDTH; ++x )
		{

			vec3 direc(x-(SCREEN_WIDTH/2), y-(SCREEN_HEIGHT/2), focalLength);
			direc = C * direc;

			if(ClosestIntersection(cameraPos, direc, triangles, closestIntersection )){
				currentDirectLight = DirectLight(closestIntersection);
				vec3 color = triangles[closestIntersection.triangleIndex].color;

				PutPixelSDL(screen,x,y, color*(currentDirectLight+indirectLight));


			}else{

				PutPixelSDL(screen,x,y,black);
			}

			//vec3 color(1,0.5,0.5);
			//PutPixelSDL( screen, x, y, color );
		}
	}

	if( SDL_MUSTLOCK(screen) )
		SDL_UnlockSurface(screen);

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}

bool ClosestIntersection(vec3 start, vec3 direction, const vector<Triangle>& triangles, Intersection& closestIntersection){
 	bool value = false;

	//Defining out vectors
	vec3 s = start;
	vec3 dir = direction;
	vec3 b,x,v0,v1,v2,e1,e2;
	mat3 A;
	closestIntersection.distance = numeric_limits<float>::max();
	for(int i = 0; i < triangles.size(); i++){
		v0 = triangles[i].v0*R;// To make the object rotate around its own y-axis.
		v1 = triangles[i].v1*R;
		v2 = triangles[i].v2*R;

		e1 = v1-v0;
		e2 = v2-v0;

		b = s - v0;
		A = mat3(-dir, e1,e2);
		x = glm::inverse(A) * b;

		float t = x.x;
		float u = x.y;
		float v = x.z;

		vec3 line = s+dir*t;

		if(u >= 0 && v >= 0 && u+v <= 1 && t>= 0){
			value = true;
			if(t < closestIntersection.distance){
				closestIntersection.distance = t;
				closestIntersection.position = line;
				closestIntersection.triangleIndex = i;
			}

		}

	}
	return value;
}
vec3 DirectLight( const Intersection& i){
	float r = glm::length(lightPos - i.position);

	vec3 n_normal = triangles[i.triangleIndex].normal;
	vec3 r_direction = glm::normalize(lightPos - i.position ); // direction of the lightray from light source
	vec3 r_direction_shadow = glm::normalize(i.position - lightPos); //direction from shadowpoint back to the light

	Intersection intersection_shadow;

	if(ClosestIntersection(lightPos, r_direction_shadow, triangles, intersection_shadow)){
		float distance_2nd = glm::distance(i.position, intersection_shadow.position);
		if(intersection_shadow.distance + 0.0001f< r)
		 return black;
	}

	float crooss = glm::dot(r_direction, n_normal); // determinant
	if(crooss<0)
		crooss = 0;
	float pi = 3.14159265359f;
	vec3 D = (lightColor*crooss)/(4*pi*(r*r));





	return D;



}
