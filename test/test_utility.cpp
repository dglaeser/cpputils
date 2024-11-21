#include <cstdlib>
#include <vector>
#include <type_traits>

#include <boost/ut.hpp>

#include <cpputils/utility.hpp>

int main() {
    using boost::ut::operator""_test;
    using boost::ut::expect;
    using boost::ut::eq;

    "value_or_reference_by_temporary"_test = [] () {
        cpputils::value_or_reference storage{std::vector<int>{42}};
        static_assert(!decltype(storage)::holds_reference);
        static_assert(std::is_same_v<typename decltype(storage)::stored_t, std::vector<int>>);
    };

    "value_or_reference_by_reference"_test = [] () {
        std::vector<int> v{42};
        cpputils::value_or_reference storage{v};
        static_assert(decltype(storage)::holds_reference);
        static_assert(std::is_same_v<typename decltype(storage)::stored_t, std::vector<int>&>);
        expect(eq(&v, &storage.get()));
    };

    "value_or_reference_by_const_reference"_test = [] () {
        const std::vector<int> v{42};
        cpputils::value_or_reference storage{v};
        static_assert(decltype(storage)::holds_reference);
        static_assert(std::is_same_v<typename decltype(storage)::stored_t, const std::vector<int>&>);
        expect(eq(&v, &storage.get()));
    };

    "indexed"_test = [] () {
        cpputils::indexed<int, char, double> indexed;
        static_assert(indexed.index_of(int{}).value == 0);
        static_assert(indexed.template index_of<int>().value == 0);

        static_assert(indexed.index_of(char{}).value == 1);
        static_assert(indexed.template index_of<char>().value == 1);

        static_assert(indexed.index_of(double{}).value == 2);
        static_assert(indexed.template index_of<double>().value == 2);

        static_assert(std::is_same_v<
            decltype(indexed.get(indexed.template index_of<double>())),
            double
        >);
    };

    "variadic_accessor"_test = [] () {
        cpputils::variadic_accessor accessor{int{42}, char{'K'}, double{12.0}};
        static_assert(accessor.index_of(int{}).value == 0);
        static_assert(accessor.template index_of<int>().value == 0);
        expect(eq(accessor.get(accessor.template index_of<int>()), 42));

        static_assert(accessor.index_of(char{}).value == 1);
        static_assert(accessor.template index_of<char>().value == 1);
        expect(eq(accessor.get(accessor.template index_of<char>()), 'K'));

        static_assert(accessor.index_of(double{}).value == 2);
        static_assert(accessor.template index_of<double>().value == 2);
        expect(eq(accessor.get(accessor.template index_of<double>()), 12.0));

    };

    return EXIT_SUCCESS;
}
