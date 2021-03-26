#include "Plop_pch.h"
#include "Curves.h"


namespace Plop::Math
{
	glm::vec3 CatmullRomCurve::Interpolate( float _t )
	{
		_t = glm::clamp( _t, 0.f, (float)vecControlPoints.size() - 3 );

		const float fLocalT = glm::fract( _t );

		if (fLocalT == 0.f) // control points
		{
			size_t cacheIndex = (size_t)_t;
			return vecControlPoints[cacheIndex + 1];
		}
		//else if (_t < vecCache.size()) // is it cached
		//{
		//	size_t cacheIndex = (size_t)0;
		//}
		else
		{
			const int index1 = (int)floorf( _t ) + 1;
			const int index2 = index1 + 1;
			const int index0 = index1 - 1;
			const int index3 = glm::min( index1 + 2, (int)vecControlPoints.size() - 1 );


			return CatmullRom<glm::vec3>( vecControlPoints[index0], vecControlPoints[index1], vecControlPoints[index2], vecControlPoints[index3], fLocalT );
		}
	}

	glm::vec3 CatmullRomCurve::GetTangent( float _t )
	{

		_t = glm::clamp( _t, 0.f, (float)vecControlPoints.size() - 3 );

		// TODO optimize for control points

		bool bEnd = (_t == (float)vecControlPoints.size() - 3);
		const float fLocalT = bEnd ? 1.f : glm::fract( _t );

		const int index1 = bEnd ? (int)vecControlPoints.size() - 3 : (int)floorf( _t ) + 1;
		const int index2 = index1 + 1;
		const int index0 = index1 - 1;
		const int index3 = glm::min( index1 + 2, (int)vecControlPoints.size() - 1 );

		return CatmullRomTangent<glm::vec3>( vecControlPoints[index0], vecControlPoints[index1], vecControlPoints[index2], vecControlPoints[index3], fLocalT );
	}
}