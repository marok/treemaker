
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
        }
        
        Point3d * circlePts[MAX_CIRCLE_PTS_COUNT];
        
        /*okresla ktory punkt z tego segmentu, polaczyc z circlePts[0] poprzedniego segmentu.
         w pierwszym(korzeniu) segmencie ta wartosc rowna sie -1*/
        int index;
};
//#undef MAX_CIRCLE_PTS_COUNT


#endif