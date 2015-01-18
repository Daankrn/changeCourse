//
//  trackSegment.cpp
//  changeCourse
//
//  Created by Daan Krijnen on 14/01/15.
//
//

#include "trackSegment.h"

trackSegment::trackSegment(){
    n = 0;
    p = 0;
    
}

void trackSegment::update(float val){
    addValue(val);
}

void trackSegment::addValue(float val){
    total -= history[p];
    history[p] = val;
    total += val;
    p = ++p % NSAMPLES;
    average = getAverage();
}

float trackSegment::getPos(){
    return average;
}

float trackSegment::getTrackWidth(){
    return getStandardDeviation()/2;
}

float trackSegment::getAverage(){
    if(n < NSAMPLES) n++;
    return total / n;
}

float trackSegment::getStandardDeviation(){
    
    
    return 20;
}