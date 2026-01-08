#define AGTB_ENABLE_DEBUG

#include <AGTB/Utils/Concept.hpp>

struct Container
{
    using value_type = int;
    // using ValueType = double;
};

template <typename T1, typename T2>
void SameAs()
{
    static_assert(std::same_as<T1, T2>);
}

int main()
{
    SameAs<int, AGTB::ExtractTypeName::ValueType<Container>>();
}