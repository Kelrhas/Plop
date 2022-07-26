#pragma once

#include "UI/IUIElement.h"

namespace Plop::UI
{
	/// <summary>
	/// Root of a visible UI
	/// </summary>
	class Dialog : public IVisibleElement
	{
		using SUPER = IVisibleElement;
	public:
		Dialog() = default;

		virtual void Render() override;

		template<class T, class... Args>
		std::weak_ptr<T> CreateElement(const String &_sName, Args... _args)
		{
			ASSERT(m_allocated.find(_sName) == m_allocated.end());

			std::shared_ptr<T> xElem	= std::make_shared<T>(_args...);
			std::weak_ptr<T>   xCreated = xElem;
			m_allocated[_sName]			= std::move(xElem);

			return xCreated;
		}

	private:
		std::unordered_map<String, IUIElementPtr> m_allocated;
	};
} // namespace Plop::UI
