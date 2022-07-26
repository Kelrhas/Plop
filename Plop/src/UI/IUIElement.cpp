#include "Plop_pch.h"

#include "IUIElement.h"

namespace Plop::UI
{
	//////////////////////////////////////////////////////////////////////////
	/*============================= IUIElement =============================*/

	IUIElement::IUIElement(IUIElementWeakPtr _xParent) { m_xParent = _xParent; }

	void IUIElement::SetPosition(const glm::vec2 _vPos)
	{
		if (m_vPosition != _vPos)
		{
			m_vPosition = _vPos;
			OnPositionChanged();
			for (IUIElementWeakPtr xChild : m_vecChildren)
			{
				xChild.lock()->OnParentPositionChanged();
			}
		}
	}

	void IUIElement::SetSize(const glm::vec2 _vSize)
	{
		if (m_vSize != _vSize)
		{
			m_vSize = _vSize;
			OnSizeChanged();
			for (IUIElementWeakPtr xChild : m_vecChildren)
			{
				xChild.lock()->OnParentSizeChanged();
			}
		}
	}

	void IUIElement::OnUpdate()
	{
		for (IUIElementWeakPtr xChild : m_vecChildren)
		{
			xChild.lock()->OnUpdate();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	/*=========================== IVisibleElement ==========================*/

	void IVisibleElement::Show()
	{
		if (!m_bVisible)
		{
			m_bVisible = true;
			OnShow();
		}
	}

	void IVisibleElement::Hide()
	{
		if (m_bVisible)
		{
			m_bVisible = true;
			OnHide();
		}
	}

	void IVisibleElement::SetVisibility(bool _bVisible)
	{
		if (_bVisible)
		{
			Show();
		}
		else
		{
			Hide();
		}
	}

	void IVisibleElement::OnRender()
	{
		if (m_bVisible)
		{
			Render();
		}

		VisitChildren([](IUIElementWeakPtr xChild) {
			xChild.lock()->OnRender();
			return VisitorFlow::CONTINUE;
		});
	}


	//////////////////////////////////////////////////////////////////////////
	/*=============================== Text ===============================*/

	void Text::Render() {}


	//////////////////////////////////////////////////////////////////////////
	/*=============================== Image ===============================*/

	void Image::Render() {}
} // namespace Plop::UI
