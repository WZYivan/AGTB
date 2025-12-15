#include <type_traits>
#include <tuple>

template <typename... __args>
struct TypePack
{
    using Pack = std::tuple<__args...>;

    template <std::size_t __idx>
    using At = std::tuple_element_t<__idx, Pack>;
};

template <typename __src, typename __tar>
void Assert()
{
    static_assert(std::is_same_v<__src, __tar>);
}

int main()
{
    using pack = TypePack<int, double>;
    using t0 = pack::At<0>;
    using t1 = pack::At<1>;

    Assert<int, t0>;
    Assert<double, t1>;
}