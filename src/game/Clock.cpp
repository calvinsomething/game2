#include "Clock.h"
#include <chrono>
#include <thread>

constexpr double yards_per_mile = 1760, nanoseconds_in_hour = 1000000000.0 * 60 * 60;

Clock::Clock()
{
    start();
}

void Clock::start()
{
    start_time = clock.now();

    start_frame();
}

long long Clock::get_ellapsed()
{
    return (clock.now() - start_time).count();
}

void Clock::start_frame()
{
    auto next_frame_time = frame_time + std::chrono::nanoseconds(min_frame_delta);

    auto now = clock.now();

    if (next_frame_time > now)
    {
        std::this_thread::sleep_for(next_frame_time - frame_time);
    }
    else
    {
        next_frame_time = now;
    }

    previous_frame_duration = (next_frame_time - frame_time).count();

    miles_per_hour_to_yards_per_frame_factor = yards_per_mile * previous_frame_duration / nanoseconds_in_hour;

    frame_time = next_frame_time;
}

long long Clock::get_frame_delta()
{
    return (clock.now() - frame_time).count();
}

void Clock::set_max_fps(uint32_t fps)
{
    min_frame_delta = 1'000'000'000 / fps;
}

uint32_t Clock::get_max_fps()
{
    return max_fps;
}

// Returns the distance that would be travelled based on mph and the previous frame duration.
float Clock::speed_to_distance(float mph)
{
    return mph * miles_per_hour_to_yards_per_frame_factor;
}
