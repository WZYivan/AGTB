#ifndef __AGTB_AGTB_HPP__
#define __AGTB_AGTB_HPP__

#include "Utils/Assert.hpp"
#include "Utils/Error.hpp"
#include "Utils/Timer.hpp"
#include "Utils/TypePack.hpp"
#include "Utils/Assert.hpp"

#include "Adjustment.hpp"
#include "Geodesy.hpp"
#include "Photogrammetry.hpp"
#include "IO.hpp"

AGTB_BEGIN

namespace Filtered
{

    namespace Angles = ::AGTB::Utils::Angles;
    namespace Assert = ::AGTB::Assert;
    using ::AGTB::Utils::Timer;
    using ::AGTB::Utils::TypePack;
    using ::AGTB::Utils::Angles::Angle;

    using ::AGTB::Container::PropPath;
    using ::AGTB::Container::PropPathAliasMap;
    using ::AGTB::Container::PropTree;
    using ::AGTB::Container::PTree;
    namespace PTreeExt = ::AGTB::Container::PTreeExt;

    using ::AGTB::Linalg::Axis;
    using ::AGTB::Linalg::CorrectionOlsSolve;
    using ::AGTB::Linalg::CsRotateForward;
    using ::AGTB::Linalg::CsRotateInverse;
    using ::AGTB::Linalg::CsScale;
    using ::AGTB::Linalg::CsTranslate;
    using ::AGTB::Linalg::LinalgOption;
    using ::AGTB::Linalg::Matrix;
    using ::AGTB::Linalg::NormalEquationMatrixInverse;
}

AGTB_END

#endif