#pragma once


namespace Plop::Math
{
	/// <summary>
	/// Returns the Catmull-Rom spline interpolation for the given control points
	/// </summary>
	/// <param name="_p0">First control point</param>
	/// <param name="_p1">Second control point</param>
	/// <param name="_p2">Third control point</param>
	/// <param name="_p3">Fourth control point</param>
	/// <param name="_t">Interpolation parameter</param>
	/// <param name="_alpha">The spline parameter [0;1], default to centripetal spline</param>
	/// <returns>The interpolated point</returns>
	template<typename T>
	T CatmullRom( const T& _p0, const T& _p1, const T& _p2, const T& _p3, float _t, float _alpha = 0.5f )
	{
		_alpha = glm::clamp( _alpha, 0.f, 1.f );


		auto GetT = [_alpha]( float t, const T& p0, const T& p1 ) -> float{
			auto d = p1 - p0;
			float a = glm::dot( d, d );
			float b = glm::pow( a, 0.5f * _alpha );
			return b + t;
		};


		float t0 = 0;
		float t1 = GetT( t0, _p0, _p1 );
		float t2 = GetT( t1, _p1, _p2 );
		float t3 = GetT( t2, _p2, _p3 );

		float t = glm::mix( t1, t2, _t );


		float t10 = t1 - t0;
		float t21 = t2 - t1;
		float t32 = t3 - t2;
		float t20 = t2 - t0;
		float t31 = t3 - t1;

		T a1 = _p0 * ((t1 - t) / t10) + _p1 * ((t - t0) / t10);
		T a2 = _p1 * ((t2 - t) / t21) + _p2 * ((t - t1) / t21);
		T a3 = _p2 * ((t3 - t) / t32) + _p3 * ((t - t2) / t32);
		T b1 = a1 * ((t2 - t) / t20) + a2 * ((t - t0) / t20);
		T b2 = a2 * ((t3 - t) / t31) + a3 * ((t - t1) / t31);
		T c  = b1 * ((t2 - t) / t21) + b2 * ((t - t1) / t21);

		return c;
	}

	/// <summary>
	/// Returns the tangent at a given Catmull-Rom spline interpolation for the given control points
	/// </summary>
	/// <param name="_p0">First control point</param>
	/// <param name="_p1">Second control point</param>
	/// <param name="_p2">Third control point</param>
	/// <param name="_p3">Fourth control point</param>
	/// <param name="_t">Interpolation parameter</param>
	/// <param name="_alpha">The spline parameter [0;1], default to centripetal spline</param>
	/// <returns>The tangent at the interpolated point</returns>
	template<typename T>
	T CatmullRomTangent( const T& _p0, const T& _p1, const T& _p2, const T& _p3, float _t, float _alpha = 0.5f ) // default to centripetal
	{
		_alpha = glm::clamp( _alpha, 0.f, 1.f );


		auto GetT = [_alpha]( float t, const T& p0, const T& p1 ) -> float {
			auto d = p1 - p0;
			float a = glm::dot( d, d );
			float b = glm::pow( a, 0.5f * _alpha );
			return b + t;
		};


		// TODO: handle the case where two control points are superposed, it leads to divisions by 0

		float t0 = 0;
		float t1 = GetT( t0, _p0, _p1 );
		float t2 = GetT( t1, _p1, _p2 );
		float t3 = GetT( t2, _p2, _p3 );

		float t = glm::mix( t1, t2, _t );


		float t10 = t1 - t0;
		float t21 = t2 - t1;
		float t32 = t3 - t2;
		float t20 = t2 - t0;
		float t31 = t3 - t1;
		float t1t = t1 - t;
		float t2t = t2 - t;
		float t3t = t3 - t;
		float tt0 = t - t0;
		float tt1 = t - t1;
		float tt2 = t - t2;
		float tt3 = t - t3;

		T a1 = _p0 * (t1t / t10) + _p1 * (tt0 / t10);
		T a2 = _p1 * (t2t / t21) + _p2 * (tt1 / t21);
		T a3 = _p2 * (t3t / t32) + _p3 * (tt2 / t32);
		T b1 = a1 * (t2t / t20) + a2 * (tt0 / t20);
		T b2 = a2 * (t3t / t31) + a3 * (tt1 / t31);

		T a1prime = (1 / t10) * (_p1 - _p0);
		T a2prime = (1 / t21) * (_p2 - _p1);
		T a3prime = (1 / t32) * (_p3 - _p2);
		T b1prime = (1 / t20) * (a2 - a1) + (t2t / t20) * a1prime + (tt0 / t20) * a2prime;
		T b2prime = (1 / t31) * (a3 - a2) + (t3t / t31) * a2prime + (tt1 / t31) * a3prime;
		T cprime =  (1 / t21) * (b2 - b1) + (t2t / t21) * b1prime + (tt1 / t21) * b2prime;

		return cprime;
	}


	struct CatmullRomCurve
	{
		/// <summary>
		/// Retrieves the interpolation at a specific point along the curve
		/// </summary>
		/// <param name="_t">The interpolation parameter, must be between 0 and the number of control points - 3</param>
		glm::vec3 Interpolate( float _t );


		glm::vec3 GetTangent( float _t );

		bool IsValid() const { return vecControlPoints.size() > 3; }

		std::vector<glm::vec3> vecControlPoints;
	};
	using CatmullRomCurvePtr = std::shared_ptr<CatmullRomCurve>;
}