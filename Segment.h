
#ifndef _SEGMENT_H
#define	_SEGMENT_H

#define CIRCLE_PTS_COUNT 8

class Segment
{
public:
        Segment()
        {
                index = -1;
        }
        
        Point3d * circlePts[CIRCLE_PTS_COUNT];
        
        /*okresla ktory punkt z tego segmentu, polaczyc z circlePts[0] poprzedniego segmentu.
         w pierwszym(korzeniu) segmencie ta wartosc rowna sie -1*/
        int index;
};


#endif