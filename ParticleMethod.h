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
	vector <int> reached;
	vector <Node *> nodes;

	//int seed;
	//int points;//starting points
	//float D;
	float e;
	float crownRadius;

	void setDefault() {
		//	points=1000;
		//	seed=42;//spr 40
		//	D=0.2;
		e=1;
		crownRadius=5.0;
	}
	void createParticles (Point3d * crownCenter)
	{
		int pts=params->points;
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
#define _MAX(a,b) ((a)>(b)?(a):(b))
		const float combine_dist=_MAX(params->cd,params->D+0.1);//0.5;
		const float target_dist=0.3;

		Point3d target(0,0,0);
		int round=0;

		reached.clear();
		while(particles.size()>1) {
			fprintf(stderr,"round=%d\n",round++);
			//getchar();
			//find nearest neighbour
			for(unsigned int i=0; i<particles.size(); i++) {
				Particle *a=&particles[i];
				a->distance=10000;
				for(unsigned int j=0; j<particles.size(); j++) {
					if(i==j) continue;
					Particle *b=&particles[j];
					float newDistance=a->point.getDistance(&b->point);
					//printf("newDistance(%d,%d) = %f\n",i,j,newDistance);
					if(newDistance<a->distance) {
						a->nearest=j;
						a->distance=newDistance;
					}
				}
			}
			//fprintf(stderr,"particle.size()=%d\n",particles.size());
			//for(unsigned int i=0; i<particles.size(); i++)
			//{
			//	fprintf(stderr,"%d - %d %f\n",i,particles[i].nearest,particles[i].distance);
			//}
			//return;
			//dla kazdej czastki s
			unsigned int oldsize=particles.size();
			for(unsigned int i=0; i<oldsize; i++) {
				Particle *s=&particles[i];
				Particle *n=&particles[s->nearest];
				//
				//fprintf(stderr,"\n\ti=%d, id=%d, nearest=%d, nearest_id=%d",i,s->id,s->nearest,n->id);

				if(!s->isAlive) continue;
				if(!n->isAlive) continue;
				bool merged=false;
				if(s->point.getDistance(&n->point)<=combine_dist) {
					//scal dwie czastki
					//	fprintf(stderr,"merge (%d,%d)",i,s->nearest);
					merged=true;
					n->isAlive=false;
					float x=(s->point.x+n->point.x)/2;
					float y=(s->point.y+n->point.y)/2;
					float z=(s->point.z+n->point.z)/2;
					Node *node=new Node(x,y,z);
					//	fprintf(stderr," id's = %d,%d\n",n->id,s->id);
					//	fprintf(stderr,"nodes size = %d\n",nodes.size());
					node->addChildren(nodes[n->id]);
					node->addChildren(nodes[s->id]);
					s->energy+=n->energy;
					s->point=Point3d(x,y,z);
					s->id=nodes.size();

					nodes.push_back(node);
				}
				//sprawdz czy p osiagnal cel
				if(s->point.getDistance(&target)<=target_dist) {
					s->isAlive=false;
					//	fprintf(stderr,"target ");
					reached.push_back(s->id);

				} else { //move S
					//	fprintf(stderr,"move ");

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
					p.mul(params->D);
					Point3d newPoint(p.d[0],p.d[1],p.d[2]);
					//	printf("z: \n");
					//	s->point.print();
					//	n->point.print();
					//puts("robie");
					//newPoint.print();
					Particle newParticle(&newPoint);
					newParticle.id=nodes.size();
//add child
					Node *newNode=new Node(&newPoint);
					newNode->addChildren(nodes[s->id]);
					nodes.push_back(newNode);
					particles.push_back(newParticle);
				}
			}
			particlesCopy.clear();
			//usun czastki oznaczone jako ~isAlive
			for(unsigned int i=0; i<particles.size(); i++)
				if(particles[i].isAlive)
					particlesCopy.push_back(particles[i]);
			particles.clear();
			for(unsigned int i=0; i<particlesCopy.size(); i++)
				particles.push_back(particlesCopy[i]);
			//fprintf(stderr,"\nparticles size=%d\n",particles.size());
		}
		//podlacz wszystkie nody ktore osiagnely cen do jednego zrodla
		Node *root=new Node(0,0,0);
		for(unsigned int i=0; i<reached.size(); i++) {
			int id=reached[i];
			root->addChildren(nodes[id]);
		}
		nodes.push_back(root);
	}


public:
	MethodParameters *params;

	ParticleMethod( MethodParameters *params) {
		this->params=params;
	}
	void init() {
		setDefault();
		srand(params->seed);
		nodes.clear();
		particles.clear();
		particlesCopy.clear();
	}
	void generate() {
		fprintf(stderr,"particle generate\n");
		Point3d crownCenter(0,0,9);
		createParticles(&crownCenter);
		colonize ();
		fprintf(stderr,"generate()");
		fprintf(stderr,"nodes =%d \n",nodes.size());
		for(unsigned int i=0; i<nodes.size(); i++)
			nodes[i]->print();
	}
	Node *getRoot() {
		if(nodes.size()==0)
			return NULL;
		int last=nodes.size()-1;
		return nodes[last];
	}
};

#endif