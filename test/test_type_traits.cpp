#include <cstdlib>
#include <vector>
#include <type_traits>

#include <cpputils/type_traits.hpp>

struct complete {};
struct incomplete;

int main() {

    {
        static_assert(cpputils::is_equal<1, 1>::value);
        static_assert(!cpputils::is_equal<0, 1>::value);

        static_assert(!cpputils::is_less<1, 1>::value);
        static_assert(!cpputils::is_less<1, 0>::value);
        static_assert(cpputils::is_less<0, 1>::value);
    }
    {
        using unique = cpputils::unique_t<int, char, int, double, int, double>;
        static_assert(unique::size == 3);
        static_assert(cpputils::contains_decayed_v<int, unique>);
        static_assert(cpputils::contains_decayed_v<char, unique>);
        static_assert(cpputils::contains_decayed_v<double, unique>);
        static_assert(cpputils::contains_decayed_v<double, int, char, double>);
    }
    {
        static_assert(cpputils::are_unique_v<int, char>);
        static_assert(cpputils::are_unique_v<int, char, int&>);
        static_assert(!cpputils::are_unique_v<int, char, int>);

        using types = cpputils::type_list<int, char, int, double, int, double>;
        static_assert(!cpputils::are_unique_v<types>);

        using unique = cpputils::unique_t<types>;
        static_assert(cpputils::are_unique_v<unique>);
        static_assert(unique::size == 3);
        static_assert(cpputils::contains_decayed_v<int, unique>);
        static_assert(cpputils::contains_decayed_v<char, unique>);
        static_assert(cpputils::contains_decayed_v<double, unique>);
    }
    {
        using merged = cpputils::merged_t<cpputils::type_list<int>, cpputils::type_list<char, double>>;
        static_assert(merged::size == 3);
        static_assert(cpputils::contains_decayed_v<int, merged>);
        static_assert(cpputils::contains_decayed_v<char, merged>);
        static_assert(cpputils::contains_decayed_v<double, merged>);
    }
    {
        using unique_merged = cpputils::unique_t<
            cpputils::merged_t<cpputils::type_list<int, char, double>, cpputils::type_list<char, double>>
        >;
        static_assert(unique_merged::size == 3);
        static_assert(cpputils::contains_decayed_v<int, unique_merged>);
        static_assert(cpputils::contains_decayed_v<char, unique_merged>);
        static_assert(cpputils::contains_decayed_v<double, unique_merged>);
    }
    {
        using filtered = cpputils::filtered_t<
            std::is_lvalue_reference,
            int, char, int&, const double&, std::vector<int>&&, std::vector<double>
        >;
        static_assert(filtered::size == 2);
        static_assert(cpputils::is_any_of_v<int&, filtered>);
        static_assert(cpputils::is_any_of_v<const double&, filtered>);
    }
    {
        using never_reference = cpputils::decayed_trait<std::is_lvalue_reference>;
        static_assert(!never_reference::template type<int&>::value);
    }
    {
        static_assert(cpputils::is_complete_v<complete>);
        static_assert(!cpputils::is_complete_v<incomplete>);
    }
    {
        using list = cpputils::type_list<char, int, double>;
        static_assert(std::is_same_v<cpputils::first_t<list>, char>);
        static_assert(std::is_same_v<cpputils::first_t<char, int, double>, char>);
    }

    return EXIT_SUCCESS;
}
