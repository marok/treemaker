#ifndef _PARTICLEMETHOD_H
#define _PARTICLEMETHOD_H

#include "ColonizationMethod.h"


class Particle: public Node
{
	void configure() {
		energy=1.0;
		isAlive=true;
		distance=10000;
	}
public:
	float energy;
	int nearest; //index of nearest particle
	float distance;//distance to nearest particle
	bool isAlive;//if false, particle will be deleted
	int id;

	Particle():Node() {
		configure();
	}
	Particle(Point3d *p):Node(p) {
		configure();
	}

};

class ParticleMethod:public GenerationMethod
{
	vector < Particle > particles;
	vector < Particle > particlesCopy;
	vector <Node *> nodes;
	int seed;
	int points;//starting points
	float D;
	float e;
	float crownRadius;

	void setDefault() {
		points=60;
		seed=42;
		D=0.3;
		e=1;
		crownRadius=5.0;
	}
	void createParticles (Point3d * crownCenter)
	{
		int pts=points;
		particles.clear();
		while (pts) {
#define MRAND (((random() % 2000) - 1000) / (float) 1000) * crownRadius
			float rx = MRAND + crownCenter->x;

			float ry = MRAND + crownCenter->y;

			float rz = MRAND + crownCenter->z;

#undef MRAND
			Point3d a (rx, ry, rz);

			if (a.getDistance (crownCenter) <= crownRadius) {
				Particle p(&a);
				Node *n=new Node(&a);
				p.id=particles.size();
				particles.push_back(p);
				nodes.push_back(n);
				pts--;
			}
		}

	}
	void colonize() {
		const float combine_dist=0.5;
		const float target_dist=0.1;

		Point3d target(0,0,0);

		while(particles.size()>1) {

			//find nearest neighbour
			for(unsigned int i=0; i<particles.size(); i++) {
				Particle *a=&particles[i];
				for(unsigned int j=0; j<particles.size(); j++) {
					if(i==j) continue;
					Particle *b=&particles[j];
					float newDistance=a->point.getDistance(&b->point);
					if(newDistance<a->distance) {
						a->nearest=j;
						a->distance=newDistance;
					}
				}
			}
			//dla kazdej czastki s
			for(unsigned int i=0; i<particles.size(); i++) {
				Particle *s=&particles[i];
				Particle *n=&particles[s->nearest];
				if(!s->isAlive) continue;
				if(!n->isAlive) continue;
				bool merged=false;
				if(n->isAlive&&s->point.getDistance(&n->point)<=combine_dist) {
					//scal dwie czastki
					merged=true;
					n->isAlive=false;
					float x=(s->point.x+n->point.x)/2;
					float y=(s->point.y+n->point.y)/2;
					float z=(s->point.z+n->point.z)/2;
					s->energy+=n->energy;
					s->point=Point3d(x,y,z);
					s->id=nodes.size();
					Node *node=new Node(x,y,z);
					node->addChildren(nodes[n->id]);
					node->addChildren(nodes[s->id]);
					nodes.push_back(node);
				}
				//sprawdz czy p osiagnal cel
				if(s->point.getDistance(&target)<=target_dist) {
					s->isAlive=false;
				} else { //move S
					s->isAlive=false;
					Vector3d p(&s->point);
					Vector3d q(&s->point);
					Vector3d nb(&n->point); //neighbour
					Vector3d tv(&target);
					p.sub(&nb);
					q.sub(&tv);
					p.normalize();
					q.normalize();
					p.add(&q);
					p.mul(D);
					Point3d newPoint(p.d[0],p.d[1],p.d[2]);
					Particle newParticle(&newPoint);
					newParticle.id=nodes.size();

					Node *newNode=new Node(&newPoint);
					nodes.push_back(newNode);
					particles.push_back(newParticle);
				}
			}
			//usun czastki oznaczone jako ~isAlive
			for(unsigned int i=0; i<particles.size(); i++)
				if(particles[i].isAlive)
					particlesCopy.push_back(particles[i]);
			particles.clear();
			for(unsigned int i=0; i<particles.size(); i++)
				particles.push_back(particlesCopy[i]);
			particlesCopy.clear();
		}
	}
	void createTrunk() {
		for(unsigned int i=0; i<nodes.size(); i++) {


		}

	}

public:

	void init() {
		setDefault();
		srand(seed);
	}
	void generate() {
		Point3d crownCenter(0,9,0);
		createParticles(&crownCenter);
		colonize ();
	}

};

#endif