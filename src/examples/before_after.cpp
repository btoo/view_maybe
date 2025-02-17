#include <smd/views/maybe.h>
#include <smd/views/nullable.h>

#include <cassert>
#include <vector>
#include <algorithm>

namespace ranges = std::ranges;
using namespace smd;

std::optional<int> possible_value() { return {7}; }

void use(int t) { assert(t == t); }

void before0() {
    {
        auto&& opt = possible_value();
        if (opt) {
            // a few dozen lines ...
            use(*opt); // is *opt OK ?
        }
    }
}

void after0() {
    for (auto&& opt : views::nullable(possible_value())) {
        // a few dozen lines ...
        use(opt); // opt is OK
    }
}

void before1() {
    std::optional o{7};
    if (o) {
        *o = 9;
        std::cout << "o=" << *o << " prints 9\n";
    }
    std::cout << "o=" << *o << " prints 9\n";
}

void after1() {
    std::optional o{7};
    for (auto&& i : views::nullable(std::ref(o))) {
        i = 9;
        std::cout << "i=" << i << " prints 9\n";
    }
    std::cout << "o=" << *o << " prints 9\n";

    // if range for is too much magic
    if (auto v = views::nullable(std::ref(o)); std::begin(v) != std::end(v)) {
        auto itr = std::begin(v);
        *itr     = 10;
        std::cout << "*itr=" << *itr << " prints 10\n";
    }
    std::cout << "o=" << *o << " prints 10\n";
}

void before2() {
    std::vector<int> v{2, 3, 4, 5, 6, 7, 8, 9, 1};
    auto             test = [](int i) -> std::optional<int> {
        switch (i) {
        case 1:
        case 3:
        case 7:
        case 9:
            return i;
        default:
            return {};
        }
    };

    auto&& r = v | ranges::views::transform(test) |
               ranges::views::filter([](auto x) { return bool(x); }) |
               ranges::views::transform([](auto x) { return *x; }) |
               ranges::views::transform([](int i) {
                   std::cout << i;
                   return i;
               });
    for (auto&& i : r) {
    };
}

void after2() {
    std::vector<int> v{2, 3, 4, 5, 6, 7, 8, 9, 1};
    auto             test = [](int i) -> std::optional<int> {
        switch (i) {
        case 1:
        case 3:
        case 7:
        case 9:
            return i;
        default:
            return {};
        }
    };

    auto&& r = v | ranges::views::transform(test) |
               ranges::views::transform(views::nullable) |
               ranges::views::join | ranges::views::transform([](int i) {
                   std::cout << i;
                   return i;
               });
    for (auto&& i : r) {
    };
    std::cout << std::endl;
}

void after3() {
    std::vector<int> v{2, 3, 4, 5, 6, 7, 8, 9, 1};
    auto             test = [](int i) -> smd::views::maybe_view<int> {
        switch (i) {
        case 1:
        case 3:
        case 7:
        case 9:
            return smd::views::maybe_view{i};
        default:
            return smd::views::maybe_view<int>{};
        }
    };

    auto&& r = v | ranges::views::transform(test) | ranges::views::join |
               ranges::views::transform([](int i) {
                   std::cout << i;
                   return i;
               });
    for (auto&& i : r) {
    };
    std::cout << std::endl;
}

int main() {
    before0();
    after0();

    before1();
    after1();

    before2();
    after2();
    after3();
}
