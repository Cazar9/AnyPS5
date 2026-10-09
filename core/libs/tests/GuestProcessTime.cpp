#include "SceTypes.hpp"
#include <cstdint>
#include <cstdlib>

extern "C" {
std::uint64_t APS5_VABI sceKernelGetProcessTime();
std::uint64_t APS5_VABI sceKernelGetProcessTimeCounter();
std::uint64_t APS5_VABI sceKernelGetProcessTimeCounterFrequency();
int APS5_VABI sceKernelUsleep_nid_postfix(KernelUseconds microseconds);
}

static constexpr int SCE_OK = 0;
static constexpr std::uint64_t MICROSECONDS_PER_SECOND = 1000000ULL;
static constexpr std::uint64_t NANOSECONDS_PER_SECOND = 1000000000ULL;

static void Require(bool value) { if (!value) std::abort(); }

int main() {
    std::uint64_t previousTime = sceKernelGetProcessTime();
    std::uint64_t previousCounter = sceKernelGetProcessTimeCounter();
    for (int i = 0; i < 10000; ++i) {
        const std::uint64_t currentTime = sceKernelGetProcessTime();
        const std::uint64_t currentCounter = sceKernelGetProcessTimeCounter();
        Require(currentTime >= previousTime);
        Require(currentCounter >= previousCounter);
        previousTime = currentTime;
        previousCounter = currentCounter;
    }

    const std::uint64_t frequency = sceKernelGetProcessTimeCounterFrequency();
    Require(frequency == NANOSECONDS_PER_SECOND);

    const std::uint64_t startTime = sceKernelGetProcessTime();
    const std::uint64_t startCounter = sceKernelGetProcessTimeCounter();
    Require(sceKernelUsleep_nid_postfix(20000) == SCE_OK);
    const std::uint64_t endTime = sceKernelGetProcessTime();
    const std::uint64_t endCounter = sceKernelGetProcessTimeCounter();

    Require(endTime >= startTime);
    Require(endCounter >= startCounter);
    const std::uint64_t elapsedTime = endTime - startTime;
    const std::uint64_t elapsedCounter = endCounter - startCounter;
    Require(elapsedCounter > 0);
    const std::uint64_t scaledCounterUs = elapsedCounter / 1000ULL;
    const std::uint64_t toleranceUs = 2000;
    Require(scaledCounterUs + toleranceUs >= elapsedTime);
    Require(elapsedTime + toleranceUs >= scaledCounterUs);
}
