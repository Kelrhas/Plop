#pragma once


namespace Plop
{
	class Asset
	{
	public:
		Asset() = default;
		virtual ~Asset() = 0 {};

		const StringPath& GetFilePath() const { return m_sFilePath; }
		const String GetFilePathStr() const { return m_sFilePath.string(); }

	protected:
		StringPath	m_sFilePath;
	};
}
