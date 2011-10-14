
#ifndef _SEGMENT_H
#define	_SEGMENT_H

#define CIRCLE_PTS_COUNT 8

class Segment
{
public:
        Segment(Node *node)
        {
                this->node = node;
                index = -1;
        }
        
        Segment *next;
        Node *node;
        Point3d * circlePts[CIRCLE_PTS_COUNT];
        
        /*okresla z ktorym punktem na nastepnym segmencie polaczyc circlePts[0] z tego segmentu
         w ostatnim segmencie ta wartosc rowna sie -1*/
        int index;
};


#endif