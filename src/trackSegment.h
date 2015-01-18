//
//  trackSegment.h
//  changeCourse
//  class for segment of a track
//  consists of track width and position calculation
//  Created by Daan Krijnen on 14/01/15.
//
//

#ifndef __changeCourse__trackSegment__
#define __changeCourse__trackSegment__

#include <stdio.h>
#include "ofMain.h"
#define NSAMPLES 5

class trackSegment {
public:
    //constructor
    trackSegment();
    
    //functions
    void update(float val);
    
    //variables
    float getPos(); //the resulting position
    float getTrackWidth(); //track width, based on standard deviation
    
private:
    float getAverage();
    float getStandardDeviation();
    void addValue(float val);
    float history[NSAMPLES];            //array of history
    float total;                        //sum of history
    float average;                      //average of history
    int n;                              //number of sample data points gathered
    int p;                              //position counter/looper
};

//


#endif /* defined(__changeCourse__trackSegment__) */
