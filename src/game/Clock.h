#pragma once

#include <chrono>

class Clock
{
  public:
    Clock();

    void start();

    // Returns the ellapsed duration in nanoseconds since start as called.
    long long get_ellapsed();

    // Sets the frame timepoint and sleeps for the remainder of the minimum frame duration.
    void start_frame();

    long long get_frame_delta();

    void set_max_fps(uint32_t fps);

    float speed_to_distance(float mph);

  private:
    std::chrono::steady_clock clock;

    std::chrono::time_point<std::chrono::steady_clock> start_time;

    std::chrono::time_point<std::chrono::steady_clock> frame_time;

    long long min_frame_delta = 0, previous_frame_duration = 0;
    double miles_per_hour_to_yards_per_frame_factor = 0.0f;
};
