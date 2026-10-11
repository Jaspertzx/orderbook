#include "OrderBook.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <vector>

using Clock = std::chrono::steady_clock;

namespace {

struct LatencyStats {
    double p50Ns, p99Ns, maxNs;
};

// Times `op` once per iteration and returns percentile stats in nanoseconds.
template <typename Op>
LatencyStats measure(int iterations, Op op) {
    std::vector<double> samples;
    samples.reserve(iterations);
    for (int i = 0; i < iterations; ++i) {
        auto start = Clock::now();
        op(i);
        auto end = Clock::now();
        samples.push_back(std::chrono::duration<double, std::nano>(end - start).count());
    }
    std::sort(samples.begin(), samples.end());
    return {samples[iterations / 2], samples[iterations * 99 / 100], samples.back()};
}

void report(const char* name, const LatencyStats& s) {
    std::printf("[latency] %-12s p50=%.0fns p99=%.0fns max=%.0fns\n",
                name, s.p50Ns, s.p99Ns, s.maxNs);
}

}  // namespace

// Thresholds are deliberately loose so sanitizer / Debug builds still pass.
// Tighten them for Release builds as your book gets faster.
constexpr double kP99BudgetNs = 50'000;

TEST(Latency, AddOrder) {
    OrderBook book;
    auto stats = measure(100'000, [&](int i) {
        Side side = (i % 2) ? Side::Buy : Side::Sell;
        book.addOrder(side, 100.0 + (i % 50) * 0.01, 10);
    });
    report("addOrder", stats);
    EXPECT_LT(stats.p99Ns, kP99BudgetNs);
}

TEST(Latency, CancelOrder) {
    OrderBook book;
    const int n = 100'000;
    std::vector<uint64_t> ids;
    ids.reserve(n);
    for (int i = 0; i < n; ++i) ids.push_back(book.addOrder(Side::Buy, 100.0, 10));

    auto stats = measure(n, [&](int i) { book.cancelOrder(ids[i]); });
    report("cancelOrder", stats);
    EXPECT_LT(stats.p99Ns, kP99BudgetNs);
}
