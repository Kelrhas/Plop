#pragma once
#include <limits>

#include <glm/detail/qualifier.hpp>
#include <glm/gtx/component_wise.hpp>
// https://gist.github.com/alexshtf/eb5128b3e3e143187794
namespace Detail
{
	double constexpr sqrtNewtonRaphson(double x, double curr, double prev)
	{
		return curr == prev
			? curr
			: sqrtNewtonRaphson(x, 0.5 * (curr + x / curr), curr);
	}
}
double constexpr Sqrt(double x)
{
	return x >= 0 && x < std::numeric_limits<double>::infinity()
		? Detail::sqrtNewtonRaphson(x, x, 0)
		: std::numeric_limits<double>::quiet_NaN();
}

namespace glm
{
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_QUALIFIER T manhattanDistance(vec<L, T, Q> const &p0, vec<L, T, Q> const &p1)
	{
		return glm::compAdd(glm::abs(p0 - p1));
	}
}
