#include <algorithm>
#include <array>
#include <iostream>
#include <span>

int main()
{
    const std::array values{1, 2, 3};
    const std::span<const int> view{values};
    if (!std::ranges::equal(view.subspan(1), std::array{2, 3}))
    {
        std::cerr << "C++20 span/ranges smoke test failed\n";
        return 1;
    }
    return 0;
}
