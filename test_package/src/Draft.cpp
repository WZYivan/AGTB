#include <type_traits>

class C
{
public:
    template <typename __ref>
    class Inner
    {
    public:
        __ref ref;

        Inner(__ref src) : ref(src) {}
    };

    template <typename __self>
    struct InnerGen
    {
        using InnerRef = std::conditional_t<std::is_const_v<std::remove_reference_t<__self>>, const int &, int &>;
        using InnerType = Inner<InnerRef>;
    };

    template <typename __self>
    using InnerGenT = InnerGen<__self>::InnerType;

    template <typename __self>
    void GenInner(this __self &&self)
    {
        const int x = 1;
        InnerGenT<__self> inner(x);
    }
};

int main()
{
    const C c{};
    c.GenInner();
}