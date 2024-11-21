#include <cstdlib>
#include <vector>
#include <type_traits>

#include <boost/ut.hpp>

#include <cpputils/utility.hpp>

int main() {
    using boost::ut::operator""_test;
    using boost::ut::expect;
    using boost::ut::eq;
    using cpputils::ic;

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

    "indexed_tuple"_test = [] () {
        cpputils::indexed_tuple tuple{int{42}, char{'K'}, double{12.0}};
        static_assert(tuple.index_of(int{}).value == 0);
        static_assert(tuple.template index_of<int>().value == 0);
        expect(eq(tuple.get(tuple.template index_of<int>()), 42));

        static_assert(tuple.index_of(char{}).value == 1);
        static_assert(tuple.template index_of<char>().value == 1);
        expect(eq(tuple.get(tuple.template index_of<char>()), 'K'));

        static_assert(tuple.index_of(double{}).value == 2);
        static_assert(tuple.template index_of<double>().value == 2);
        expect(eq(tuple.get(tuple.template index_of<double>()), 12.0));
    };

    "value_list_access"_test = [] () {
        constexpr cpputils::values<0, 1, 2> values;
        static_assert(values.at(ic<0>) == 0);
        static_assert(values.at(ic<1>) == 1);
        static_assert(values.at(ic<2>) == 2);

        static_assert(values.first() == 0);
        static_assert(values.last() == 2);
    };

    "value_list_drop_n"_test = [] () {
        constexpr cpputils::values<0, 1, 2> values;
        constexpr auto dropped = values.template drop<1>();
        static_assert(dropped.size == 2);
        static_assert(dropped.at(ic<0>) == 1);
        static_assert(dropped.at(ic<1>) == 2);

        constexpr auto empty = values.template drop<3>();
        static_assert(empty.size == 0);
    };

    "value_list_crop_n"_test = [] () {
        constexpr cpputils::values<0, 1, 2> values;
        constexpr auto cropped = values.template crop<1>();
        static_assert(cropped.size == 2);
        static_assert(cropped.at(ic<0>) == 0);
        static_assert(cropped.at(ic<1>) == 1);

        constexpr auto empty = values.template crop<3>();
        static_assert(empty.size == 0);
    };

    "value_list_take_n"_test = [] () {
        constexpr cpputils::values<0, 1, 2> values;
        constexpr auto firsts = values.template take<2>();
        static_assert(firsts.size == 2);
        static_assert(firsts.at(ic<0>) == 0);
        static_assert(firsts.at(ic<1>) == 1);
    };

    "value_list_equality"_test = [] () {
        constexpr cpputils::values<0, 1, 2> values;
        static_assert(values == cpputils::values<0, 1, 2>{});
    };

    "value_list_stream"_test = [] () {
        std::ostringstream s;
        s << cpputils::values<0, 1, 2>{};
        expect(eq(s.str(), std::string{"0, 1, 2"}));
    };

    "value_list_reduce"_test = [] () {
        constexpr cpputils::values<0, 1, 2> values;
        static_assert(values.reduce_with(std::plus{}, 0) == 3);
        static_assert(values.reduce_with(std::multiplies{}, 1) == 0);
    };

    "value_list_concat"_test = [] () {
        constexpr cpputils::values<0, 1, 2> values0;
        constexpr cpputils::values<3, 4> values1;
        constexpr auto sum = values0 + values1;
        static_assert(std::is_same_v<
            std::remove_cvref_t<decltype(sum)>,
            cpputils::values<0, 1, 2, 3, 4>
        >);
    };

    return EXIT_SUCCESS;
}
