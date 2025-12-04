

#include <stdio.h>
#include <stdlib.h>
#include "gfx.h"
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <limits.h>
//physics engine
//

//basic vector implementation to make program easier to write
typedef struct{
	float x;
	float y;
} vec2;

typedef enum{
	ADD,
	SUB,
	SCALAR,
	MAG,
	UNIT,
	DOT,
} operator;

vec2 vectoroper_vf(vec2 vector1, float lambda, operator OP){
	if(OP == SCALAR){
		vec2 vectorresult = {
			.x = vector1.x * lambda,
			.y = vector1.y * lambda,	
		};
		return vectorresult;
	}
	else{
		printf("VECTOROPER ERROR (1 VECTOR AND SCALAR)");
		exit(1);
	}
}
vec2 vectoroper_vv(vec2 vector1, vec2 vector2,operator OP){
	if(OP == ADD){
		vec2 vectorresult = {
			.x = vector1.x + vector2.x,
			.y = vector1.y + vector2.y,
		};
		return vectorresult;
	}
	else if(OP == SUB){
		vec2 vectorresult = {
			.x = vector1.x - vector2.x,
			.y = vector1.x - vector2.y,
		};
		return vectorresult;
	}
	else if(OP == DOT){
		vec2 vectorresult = {
			.x = vector1.x * vector2.x,
			.y = vector1.y * vector2.y,
		};
		return vectorresult;
	}
	else{
		printf("VECTOROPER ERROR (2 VECTORS)");
		exit(1);
	}
}
float vectoroper_v_f(vec2 vector1, operator OP){
	if(OP == MAG){
		float magnitude = sqrt(pow(vector1.x, 2) + pow(vector1.y, 2));
		return magnitude;
	}
	else{
		printf("VECTOROPER ERROR (1 VECTOR RETURN FLOAT)");
		exit(1);
	}
}
vec2 vectoroper_v(vec2 vector1, operator OP){
	if(OP == UNIT){
		float invmag = pow(vectoroper_v_f(vector1, MAG), 2);
		vec2 vectorresult = vectoroper_vf(vector1, invmag, SCALAR);
		return vectorresult;
	}
	else{
		printf("VECTOROPER ERROR (1 VECTOR RETURN VEC2");
		exit(1);
	}
}


typedef struct {
	vec2 pos;
	vec2 vel;
	float mass;
} particle;

typedef struct {
	particle* data;
	size_t size;
	size_t occupied;
} array_p;

array_p arr_init(size_t size){
	array_p array = {
		.occupied = 0,
		.size = size,
		.data = malloc(sizeof(particle)*size),
	};
	return array;
}
void arr_push(array_p* array, particle* newpart){
	if (array->size == array->occupied){
		array->size *= 2;
		array->data = realloc(array->data, array->size * sizeof(particle));
		if(array->data == NULL) {
			printf("NULL POINTER ON ARRAY PUSH"); exit(1);
		}	
	}
	(array->data)[array->occupied] = *newpart;
	array->occupied++;
}

void arr_free(array_p* array){free(array->data);}

array_p particle_list;
float dt = 0.01;
float pullk = 0.001;

vec2 calcforces(particle part1, size_t current){
	//here we can put whatever forces we like onto the particle, but we will make separate function for acceleration
	//just for debuggings sake/may come in handy later
	vec2 totalforce = {0,0};
	for(size_t i = 0;i < particle_list.occupied;i++){
		//gravity type force
		vec2 pullforce;
		if(i == current) pullforce = (vec2){0,0};
		else{
			particle part2 = particle_list.data[i];
			vec2 dispvec = vectoroper_vv(part2.pos,part1.pos,SUB);
			pullforce = vectoroper_vf(vectoroper_v(dispvec,UNIT),pullk*part1.mass*part2.mass*pow(vectoroper_v_f(dispvec,MAG),-2),SCALAR);
		}
		totalforce = vectoroper_vv(totalforce, pullforce, ADD);
	}
	return totalforce;
}

void marchphys() {
	for(size_t i = 0;i < particle_list.occupied;i++){
		vec2 acc = vectoroper_vf(calcforces(particle_list.data[i], i),pow(particle_list.data[i].mass,-1),SCALAR);
		particle_list.data[i].vel = vectoroper_vv(vectoroper_vf(acc, dt, SCALAR), particle_list.data[i].vel, ADD);
		vec2 vel = particle_list.data[i].vel;
		vec2 pos = particle_list.data[i].pos;
		particle_list.data[i].pos = vectoroper_vv(vectoroper_vf(vel, dt, SCALAR), pos, ADD);
	}
}

//graphics side
//
//

void drawbox(int vertorigin[2], int size);

#define xsize = 1024;
#define ysize = 512;
float scale = 0.05;
void draw_particle(particle part){
	int xscreen_cam = xsize/2;
	int yscreen_cam = ysize/2;
	
	int xscreen_part = xscreen_cam + (int)round(scale*part.pos.x);
	int yscreen_part = yscreen_cam + (int)round(scale*part.pos.y);
	int temparray[2] = {xscreen_part,yscreen_part};
	drawbox(temparray, 1);
	//gfx_point(xscreen_part,yscreen_part);
}
void draw_world(){
	for(size_t i = 0;i < particle_list.occupied;i++){draw_particle(particle_list.data[i]);}

}

void drawbox(int vertorigin[2], int size){
	gfx_line(vertorigin[0], vertorigin[1], vertorigin[0] + size, vertorigin[1]);
	gfx_line(vertorigin[0], vertorigin[1], vertorigin[0], vertorigin[1] + size);
	gfx_line(vertorigin[0] + size, vertorigin[1], vertorigin[0] + size, vertorigin[1] + size);
	gfx_line(vertorigin[0], vertorigin[1] + size, vertorigin[0] + size, vertorigin[1] + size);

}

int main()
{
	char c;

	gfx_open(xsize,ysize,"Particle Sim");

	gfx_color(0,200,100);
	//initialise the particles

	particle part1 = {
		.pos = {6000.4,0},
		.vel = {0,4000},
		.mass = 50,
	};
	
	particle part2 = {
		.pos = {0,0},
		.vel = {0,0},
		.mass = 1000,	
	};
	
	particle part3 = {
		.pos = {-5000,0},
		.vel = {0,4000},
		.mass = 2,	
	};
	
	particle_list = arr_init(2);
	arr_push(&particle_list, &part1);
	arr_push(&particle_list, &part2);
	arr_push(&particle_list, &part3);
	printf("%d",particle_list.occupied);	
	while(1) {
		gfx_clear();

		c = gfx_getKey();
		if(c != 0) {
			if(c=='q') break;
			if(c=='p'){
				particle temppart = {
					.pos = {0,0},
					.vel = {0,0},
					.mass = 1,
				};
				arr_push(&particle_list, &temppart);
			}
			if(c=='['){scale *= 2;}
			if(c==']'){scale *= 0.5;}
		}
		draw_world();	
		marchphys();	
		gfx_flush();
		usleep(10000);
			
	}

	return 0;
}
