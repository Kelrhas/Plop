#include "Plop_pch.h"
#include "BaseComponents.h"

namespace Plop
{
	Component_Name::Component_Name(const String &_sName /*= "Entity"*/)
		: sName(_sName)
	{

	}

	Component_Name::Component_Name(const GUID &_guid)
		: guid(_guid)
	{

	}


	Component_Name &Component_Name::operator=(const Component_Name &_o)
	{
		// don't change the GUID, it should not be copied, it is unique after all
		sName = _o.sName;

		return *this;
	}
}
