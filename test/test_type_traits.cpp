#include <cstdlib>
#include <type_traits>

#include <cpputils/type_traits.hpp>

int main() {

    {
        using unique = cpputils::unique_types_t<int, char, int, double, int, double>;
        static_assert(cpputils::type_list_size_v<unique> == 3);
        static_assert(cpputils::contains_decayed_v<int, unique>);
        static_assert(cpputils::contains_decayed_v<char, unique>);
        static_assert(cpputils::contains_decayed_v<double, unique>);
    }
    {
        using unique = cpputils::unique_types_t<
            cpputils::type_list<int, char, int, double, int, double>
        >;
        static_assert(cpputils::type_list_size_v<unique> == 3);
        static_assert(cpputils::contains_decayed_v<int, unique>);
        static_assert(cpputils::contains_decayed_v<char, unique>);
        static_assert(cpputils::contains_decayed_v<double, unique>);
    }
    {
        using merged = cpputils::merged_types_t<cpputils::type_list<int>, cpputils::type_list<char, double>>;
        static_assert(cpputils::type_list_size_v<merged> == 3);
        static_assert(cpputils::contains_decayed_v<int, merged>);
        static_assert(cpputils::contains_decayed_v<char, merged>);
        static_assert(cpputils::contains_decayed_v<double, merged>);
    }

    {
        using unique_merged = cpputils::unique_types_t<
            cpputils::merged_types_t<cpputils::type_list<int, char, double>, cpputils::type_list<char, double>>
        >;
        static_assert(cpputils::type_list_size_v<unique_merged> == 3);
        static_assert(cpputils::contains_decayed_v<int, unique_merged>);
        static_assert(cpputils::contains_decayed_v<char, unique_merged>);
        static_assert(cpputils::contains_decayed_v<double, unique_merged>);
    }

    return EXIT_SUCCESS;
}
