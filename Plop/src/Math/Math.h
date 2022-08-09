#pragma once
#include <glm/detail/qualifier.hpp>
#include <glm/gtx/component_wise.hpp>
#include <limits>

// https://gist.github.com/alexshtf/eb5128b3e3e143187794
namespace Detail
{
	double constexpr sqrtNewtonRaphson(double x, double curr, double prev) { return curr == prev ? curr : sqrtNewtonRaphson(x, 0.5 * (curr + x / curr), curr); }
} // namespace Detail

double constexpr Sqrt(double x)
{
	return x >= 0 && x < std::numeric_limits<double>::infinity() ? Detail::sqrtNewtonRaphson(x, x, 0) : std::numeric_limits<double>::quiet_NaN();
}

namespace glm
{
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_QUALIFIER T manhattanDistance(vec<L, T, Q> const &p0, vec<L, T, Q> const &p1)
	{
		return glm::compAdd(glm::abs(p0 - p1));
	}

	template<typename T, glm::qualifier Q>
	GLM_FUNC_QUALIFIER glm::mat<3, 3, T, Q> rotate2D(const glm::mat<3, 3, T, Q> &_from, float _fAngle)
	{
		const float fCos = glm::cos(_fAngle);
		const float fSin = glm::sin(_fAngle);

		glm::mat<3, 3, T, Q> mRotation = glm::identity<glm::mat<3, 3, T, Q>>();
		mRotation[0][0]				   = fCos;
		mRotation[1][1]				   = fCos;
		mRotation[0][1]				   = -fSin;
		mRotation[1][0]				   = fSin;

		return _from * mRotation;
	}
} // namespace glm
