#include <type_traits>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <optional>
#include <iostream>

using Timestamp = std::uint64_t;

template<typename T>
concept Interpolator = requires(T t)
{
    {t.operator()(typename T::underlying(),
                 Timestamp{0u},
                 typename T::underlying(),
                 Timestamp{0u},
                 Timestamp{0u})} -> std::convertible_to<typename T::underlying>;
};

template<typename TData>
struct Lerp
{
    using underlying = TData;

    TData operator()(const TData& a, 
                     Timestamp ta,
                     const TData& b,
                     Timestamp tb,
                     Timestamp ts) const noexcept
    {
        const float dt = (ts - ta) * 1.f / (tb - ta);

         // a * dt + (1 - dt) * b; simplified
        return (a - b) * dt + b;
    }
};

// Assumes timestamps are added in ascending order
// Imitates live data stream
template<typename TData, Interpolator Interp>
class InterpolationStorage
{
public:
    InterpolationStorage(Interp&& interp)
        : _interp{interp}
    {}

    void put(Timestamp ts, TData&& data)
    {
        _timestamps.push_back(ts);
        _data.push_back(std::move(data));
    }

    std::optional<TData> get(Timestamp ts) const noexcept
    {
        if (_data.size() < 2)
        {
            return std::nullopt;
        }

        auto it = std::lower_bound(_timestamps.begin(), 
                                   _timestamps.end(), 
                                   ts);
        // ts > than everything in the _timestamps
        if (it == _timestamps.end())
        {
            return std::nullopt;
        }

        const auto tIdx = std::distance(_timestamps.begin(), it);

        // ts <= than a first timestamp
        if (tIdx == 0)
        {
            return *it == ts ? std::optional<TData>(_data.front()) 
                             : std::nullopt;
        }

        // ts in the middle and data size >= 2
        auto prevIt = std::prev(it);
        const auto prevIdx = std::distance(_timestamps.begin(), prevIt);
        return _interp(_data[prevIdx], *prevIt, _data[tIdx], *it, ts);
    }

private:
    // Assumed sorted
    std::vector<Timestamp> _timestamps;
    std::vector<TData> _data;

    Interp _interp;
};

using LerpInterpolationStorage = InterpolationStorage<float, Lerp<float>>;

int main(int, char**)
{
    LerpInterpolationStorage ldh{Lerp<float>{}};
    ldh.put(Timestamp{1u}, 1);
    ldh.put(Timestamp{3u}, 3);

    std::cout << "At timestamp 2: " << ldh.get(2u).value_or(10000.f) << std::endl;

    ldh.put(Timestamp{9u}, 6);
    std::cout << "At timestamp 6: " << ldh.get(6u).value_or(10000.f) << std::endl;
    
    return 0;
}