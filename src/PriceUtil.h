#pragma once
#include <cmath>
#include <cstdint>
#include <limits>

class PriceUtil {
public:
    enum class Round { Down, Up, Nearest };
    static constexpr uint64_t kScale = 1'000'000'000;

    static uint64_t toTickPrice(double px, Round mode) {
        const double scaled  = px * static_cast<double>(kScale);
        const double nearest = std::round(scaled);

        // get threshold for wrong digits
        // epislon is the diff between 1.0 and the next rep double value
        const double tol = std::abs(scaled) * std::numeric_limits<double>::epsilon();

        double r;
        if (std::abs(scaled - nearest) <= tol) {
            r = nearest;
        } else {
            switch (mode) {
                case Round::Down:    r = std::floor(scaled); break;
                case Round::Up:      r = std::ceil(scaled);  break;
                case Round::Nearest: r = nearest;            break;
                default:             r = nearest;            break;
            }
        }

        if (std::isnan(r) || r <= 0.0) return 0; //todo:probably shd throw an error or something
        if (r >= 18446744073709551616.0)  // 2^64
            return std::numeric_limits<uint64_t>::max();//todo:throw error too
        return static_cast<uint64_t>(r);
    }
};