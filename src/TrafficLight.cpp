#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <future>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _condition.wait(lock, [this]{
        return !_queue.empty();
    });
    T msg = std::move(_queue.front());
    _queue.pop_front();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _queue.emplace_back(msg);
    _condition.notify_one();
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{

    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto msg = _queue.receive();
        if(msg == green){
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    //Random generation between 4 a 6 seconds.
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(4000,6000);
    int cycle_duration = dist(gen);

    auto last_switched_time = std::chrono::system_clock::now();

    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto tmp_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - last_switched_time);
        int duration_since_switch = tmp_seconds.count();

        if(duration_since_switch >= cycle_duration){
            _currentPhase = _currentPhase == red ? green : red;

            auto sent_ftr = std::async(std::launch::async, &MessageQueue<TrafficLightPhase>::send, &_queue, std::move(_currentPhase));
            sent_ftr.wait();
            last_switched_time = std::chrono::system_clock::now();
            cycle_duration = dist(gen);
        }
    }

}