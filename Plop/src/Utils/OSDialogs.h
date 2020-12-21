#pragma once


namespace Plop
{
	struct Dialog
	{
		static const char* IMAGE_FILTER;
		static const char* LEVEL_FILTER;


		static bool OpenFile( StringPath& _sFilePath, const char* _pFilter );
		static bool SaveFile( StringPath& _sFilePath, const char* _pFilter );
	};
}
