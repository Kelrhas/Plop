#pragma once


namespace Plop
{
	class Asset
	{
	public:
		Asset() = default;
		virtual ~Asset() = 0 {};

		void LoadFromFile( const StringPath& _path ) { m_sFilePath = _path; }

		const StringPath& GetFilePath() const { return m_sFilePath; }
		const String GetFilePathStr() const { return m_sFilePath.string(); }

	protected:
		StringPath	m_sFilePath;
	};
}
