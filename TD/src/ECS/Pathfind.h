#pragma once


template<typename CellType>
struct GridPathFind
{
	bool bValid = false;
	float fTotalCost = 0.f;
	std::vector<CellType> vecPath;
};

template <class CellType, class CostFunc, class NeighborFunc>
GridPathFind<CellType> A_Star(const CellType &start, const CellType &end, CostFunc EstimateCostFunc, NeighborFunc GetNeighborFunc)
{
	GridPathFind<CellType> pf;

	struct CellInfo
	{
		CellType previous;
		float fCurrentCost = FLT_MAX;
		float fEstimateTotalCost = FLT_MAX;
	};

	static std::vector<CellType> vecOpen;
	static std::unordered_map<CellType, CellInfo> mapCellInfo;

	vecOpen.clear();
	vecOpen.push_back(start);
	mapCellInfo.clear();

	mapCellInfo.insert_or_assign(start, CellInfo {start, 0, EstimateCostFunc(start, end)});

	while (!vecOpen.empty())
	{
		CellType current = vecOpen.front();
		const auto &currentInfo = mapCellInfo[current];
		float fLowestCost = currentInfo.fEstimateTotalCost;
		for (auto &cell : vecOpen)
		{
			if (float fCost = currentInfo.fEstimateTotalCost; fCost < fLowestCost)
			{
				current = cell;
				fLowestCost = fCost;
			}
		}

		if (current == end)
		{
			pf.bValid = true;
			pf.fTotalCost = currentInfo.fCurrentCost;
			// reconstruct path
			pf.vecPath.push_back(current);
			CellType previous = currentInfo.previous;
			while (previous != start)
			{
				pf.vecPath.push_back(previous);
				const auto &previousInfo = mapCellInfo[previous];
				previous = previousInfo.previous;
			}

			std::reverse(pf.vecPath.begin(), pf.vecPath.end());

			return pf;
		}

		vecOpen.erase(std::remove(vecOpen.begin(), vecOpen.end(), current), vecOpen.end());

		static std::vector<CellType> vecNeighbors;
		vecNeighbors.clear();
		GetNeighborFunc(current, vecNeighbors);


		for (const CellType &vNeighbor : vecNeighbors)
		{
			float fNeighborCost = vNeighbor.GetPathFindCost();
			if (fNeighborCost < 0)
				continue;

			float fCost = currentInfo.fCurrentCost + fNeighborCost;
			auto &neighborInfo = mapCellInfo[vNeighbor];

			if (fCost < neighborInfo.fCurrentCost)
			{
				neighborInfo.previous = current;
				neighborInfo.fCurrentCost = fCost;
				neighborInfo.fEstimateTotalCost = fCost + EstimateCostFunc(vNeighbor, end);
				if (std::find(vecOpen.begin(), vecOpen.end(), vNeighbor) == vecOpen.end())
					vecOpen.push_back(vNeighbor);
			}

		}

	}

	return pf;
}