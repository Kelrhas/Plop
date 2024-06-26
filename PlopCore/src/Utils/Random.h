#pragma once

#include "Types.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Plop
{
	////////////////
	/// based on xorshift*, taking only the high 32 bits
	/// https://en.wikipedia.org/wiki/Xorshift#xorshift*
	class Random
	{
	public:
		Random(U64 _uSeed = 0);

		void  SetSeed(U64 _uSeed);
		U32	  NextInt();
		float NextFloat01();
		float NextFloatNeg11();


		template<typename T>
		T Next01()
		{
			T t = NextFloat01();
			return t;
		}

		template<>
		float Next01<float>()
		{
			return NextFloat01();
		}

		template<>
		glm::vec2 Next01<glm::vec2>()
		{
			glm::vec2 v;
			v.x = NextFloat01();
			v.y = NextFloat01();

			return v;
		}

		template<>
		glm::vec3 Next01<glm::vec3>()
		{
			glm::vec3 v;
			v.x = NextFloat01();
			v.y = NextFloat01();
			v.z = NextFloat01();

			return v;
		}

		template<>
		glm::vec4 Next01<glm::vec4>()
		{
			glm::vec4 v;
			v.x = NextFloat01();
			v.y = NextFloat01();
			v.z = NextFloat01();
			v.w = NextFloat01();

			return v;
		}

		template<typename T>
		T Between(T min, T max)
		{
			return NextFloat01() * (max - min) + min;
		}


	private:
		U64 m_uSeed;
		U64 m_uState;

		static Random s_globalRandom;
	};


	void  RandomSeed(U64 _uSeed);
	U32	  RandomInt32();
	float RandomFloat01();


	template<typename RandomElementType, typename WeightType = U32>
	class RandomTable
	{
	public:
		using Weight  = WeightType;
		using Element = RandomElementType;

		struct CumulatedWeightElement
		{
			Element elmt;
			Weight	uWeight;

			CumulatedWeightElement(const Element &_elmt, Weight _uWeight) : elmt(_elmt), uWeight(_uWeight) {}
		};

		RandomTable();

		void AddElement(const Element &_elmt, Weight _uWeight)
		{
			if (m_vecTable.size() > 0)
			{
				_uWeight += m_vecTable.back().uWeight;
			}

			m_vecTable.push_back(CumulatedWeightElement(_elmt, _uWeight));
		}

		void RemoveElement(const Element &_elmt)
		{
			bool   bFound = false;
			Weight uWeightFound;
			int	   iNbElmt = m_vecTable.size();
			for (int i = 0; i < iNbElmt; ++i)
			{
				CumulatedWeightElement &weightedElmt = m_vecTable[i];
				if (bFound)
				{
					weightedElmt.uWeight -= uWeightFound;
				}
				else if (weightedElmt.elmt == _elmt)
				{
					bFound		 = true;
					uWeightFound = weightedElmt.uWeight;
					m_vecTable.erase(m_vecTable.begin() + i);
					--i;
				}
			}
		}

		const Element &NextElement()
		{
			ASSERTM(m_vecTable.size() > 0, "Nothing in the random table");

			Weight uWeight = m_Random.NextInt() % m_uMaxWeight;
			for (const CumulatedWeightElement &weightedElmt : m_vecTable)
			{
				if (uWeight < weightedElmt.uWeight)
					return weightedElmt.elmt;
			}

			return m_vecTable[0].elmt;
		}

		const Element &NextElementAndRemove()
		{
			const Element &elmt = NextElement();

			RemoveElement(elmt);

			return elmt;
		}


	private:
		Random m_Random;
		Weight m_uMaxWeight;
		// std::forward_list<CumulatedWeightElement> 	m_lTable;
		std::vector<CumulatedWeightElement> m_vecTable; // current Weight to next Weight will yield current element
	};

} // namespace Plop