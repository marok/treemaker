
#ifndef _SEGMENT_H
#define	_SEGMENT_H

#define MAX_CIRCLE_PTS_COUNT 20
//#define CIRCLE_PTS_COUNT MAX_CIRCLE_PTS_COUNT

class Segment
{
public:
	Segment()
	{
		index = -1;
		for(int i=0; i<MAX_CIRCLE_PTS_COUNT; i++)
		{
			circlePts[i] = NULL;
		}
	}
	
	~Segment()
	{
		for(int i=0; i<MAX_CIRCLE_PTS_COUNT; i++)
		{
			if(circlePts[i]!=NULL)
			{
				delete circlePts[i];
			}
		}
	}
	
	/*Współrzędne segmentu względem jego środka*/
	Point3d * circlePts[MAX_CIRCLE_PTS_COUNT];

	/*okresla ktory punkt z tego segmentu, polaczyc z circlePts[0] poprzedniego segmentu.
	 w pierwszym(korzeniu) segmencie ta wartosc rowna sie -1*/
	int index;
};
//#undef MAX_CIRCLE_PTS_COUNT


#endif