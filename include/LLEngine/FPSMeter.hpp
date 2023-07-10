#pragma once

#include <thread>
#include <condition_variable>
#include <cstdint>

namespace llengine {
class FPSMeter {
public:
    /// @param interval means interval between the fps updates.
    explicit FPSMeter(float interval);
    FPSMeter(const FPSMeter& other) = delete;
    FPSMeter(FPSMeter&& other) = delete;
    ~FPSMeter();

    FPSMeter& operator=(const FPSMeter& other) = delete;
    FPSMeter& operator=(FPSMeter&& other) = delete;

    /// Call this function once on every frame.
    void frame() noexcept;
    inline float get_fps() const noexcept {
        return cur_fps;
    }

private:
    uint32_t frames_count = 0;
    float interval;
    std::unique_ptr<std::thread> thread;
    std::mutex mutex;
    std::condition_variable cv;
    bool working = true;

    float cur_fps = 0.0f;

    void measure_loop();
};
}