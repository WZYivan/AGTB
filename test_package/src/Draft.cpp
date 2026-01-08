#define AGTB_ENABLE_DEBUG

#include <AGTB/AGTB.hpp>

struct Container
{
    using value_type = int;
};

int main()
{
    AGTB::Assert::SameAs<int, AGTB::ExtractTypeName::ValueType<Container>>();
}