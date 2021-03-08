#pragma once


namespace Plop::Math
{
	template<typename T>
	T CatmullRom( const T& _p0, const T& _p1, const T& _p2, const T& _p3, float _t, float _alpha = 0.5f ) // default to centripetal
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

		float t = glm::lerp( t1, t2, _t );


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
}