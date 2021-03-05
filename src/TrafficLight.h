#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;


template <class T>
class MessageQueue
{
public:
void send(T &&msg);
T receive();

private:
std::deque<T> _queue;
std::condition_variable _condition;
std::mutex _mutex;
    
};


enum TrafficLightPhase{
    red,
    green
};

class TrafficLight:TrafficObject
{
public:
    // constructor / desctructor

    TrafficLight();

    // getters / setters

    // typical behaviour method
    void waitForGreen();
    void simulate();
    TrafficLightPhase getCurrentPhase();

private:
    // typical behaviour methods

    void cycleThroughPhases();

    TrafficLightPhase _currentPhase;

    std::condition_variable _condition;
    std::mutex _mutex;
    MessageQueue<TrafficLightPhase> _queue;
};

#endif