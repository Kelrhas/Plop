#include "Plop_pch.h"

#include "IUIElement.h"

#include "Assets/TextureLoader.h"
#include "Input/Input.h"
#include "Math/Math.h"

#include <glm/gtc/type_ptr.hpp>
#include <imgui_internal.h>

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

	void IUIElement::SetPivot(const glm::vec2 _vPivot)
	{
		if (m_vPivot != _vPivot)
		{
			m_vPivot = _vPivot;
			OnPositionChanged();
			for (IUIElementWeakPtr xChild : m_vecChildren)
			{
				xChild.lock()->OnParentPositionChanged();
			}
		}
	}

	void IUIElement::SetRotation(float _fAngle)
	{
		if (m_fRotation != _fAngle)
		{
			m_fRotation = _fAngle;
			OnPositionChanged();
			for (IUIElementWeakPtr xChild : m_vecChildren)
			{
				xChild.lock()->OnParentPositionChanged();
			}
		}
	}

	bool IUIElement::AddElementTo(IUIElementWeakPtr _xElement, IUIElementWeakPtr _xParent)
	{
		ASSERTM(!_xElement.expired(), "The element is not valid");
		ASSERTM(!_xParent.expired(), "The parent is not valid");
		if (_xElement.expired() || _xParent.expired())
			return false;

		ASSERTM(_xElement.lock() != _xParent.lock(), "The element and the parent are the same");
		if (_xElement.lock() == _xParent.lock())
			return false;

		auto xParent = _xParent.lock();
		auto it		 = std::find_if(xParent->m_vecChildren.begin(), xParent->m_vecChildren.end(), [_xElement](IUIElementWeakPtr _xOther) {
			 return _xElement.lock() == _xOther.lock();
		 });

		ASSERT(it == xParent->m_vecChildren.end());
		if (it != xParent->m_vecChildren.end())
			return false;

		xParent->m_vecChildren.push_back(_xElement);
		_xElement.lock()->m_xParent = _xParent;

		return true;
	}

	bool IUIElement::RemoveElementFrom(IUIElementWeakPtr _xElement, IUIElementWeakPtr _xParent)
	{
		ASSERTM(!_xElement.expired(), "The element is not valid");
		ASSERTM(!_xParent.expired(), "The parent is not valid");
		if (_xElement.expired() || _xParent.expired())
			return false;

		ASSERTM(_xElement.lock() != _xParent.lock(), "The element and the parent are the same");
		if (_xElement.lock() == _xParent.lock())
			return false;

		auto xParent = _xParent.lock();
		auto it		 = std::find_if(xParent->m_vecChildren.begin(), xParent->m_vecChildren.end(), [_xElement](IUIElementWeakPtr _xOther) {
			 return _xElement.lock() == _xOther.lock();
		 });

		if (it == xParent->m_vecChildren.end())
			return false;

		xParent->m_vecChildren.erase(it);

		return true;
	}

	void IUIElement::OnUpdate(const TimeStep &_ts)
	{
		ImGui::PushID(this);
		ImGui::Text(m_sName.c_str());
		glm::vec2 vTemp;
		float	  fTemp;

		vTemp = m_vPosition;
		if (ImGui::SliderFloat2("Pos", glm::value_ptr(vTemp), 0.f, 1.f))
			SetPosition(vTemp);

		vTemp = m_vSize;
		if (ImGui::SliderFloat2("Size", glm::value_ptr(vTemp), 0.f, 1.f))
			SetSize(vTemp);

		vTemp = m_vPivot;
		if (ImGui::SliderFloat2("Pivot", glm::value_ptr(vTemp), 0.f, 1.f))
			SetPivot(vTemp);

		fTemp = m_fRotation;
		if (ImGui::SliderFloat("Angle(Rad)", &fTemp, 0.f, glm::two_pi<float>()))
			SetRotation(fTemp);

		ImGui::Text("Final pos: %.3f %.3f", GetGlobalScreenPos().x, GetGlobalScreenPos().y);
		ImGui::Text("Final size: %.3f %.3f", GetGlobalScreenSize().x, GetGlobalScreenSize().y);
		ImGui::Checkbox("Clip children", &m_bClipChildrenToRect);

		if (auto pImage = dynamic_cast<Image *>(this))
		{
			fTemp = pImage->m_fFixedRatio;
			if (ImGui::SliderFloat("Fixed ratio", &fTemp, 0.01f, 10.f))
			{
				pImage->SetRatio(fTemp);
			}

			auto NameFromMethod = [](Image::RatioMethod _eMethod) {
				switch (_eMethod)
				{
					case Plop::UI::Image::RatioMethod::FREE: return "Free";
					case Plop::UI::Image::RatioMethod::KEEP_WIDTH: return "Width";
					case Plop::UI::Image::RatioMethod::KEEP_HEIGHT: return "Height";
					default: break;
				}
				return "ERROR";
			};

			if (ImGui::BeginCombo("Ratio method", NameFromMethod(pImage->m_eRatioMethod)))
			{
				auto ComboMethod = [&](Image::RatioMethod _eMethod) {
					if (ImGui::Selectable(NameFromMethod(_eMethod), _eMethod == pImage->m_eRatioMethod))
					{
						pImage->SetRatioMethod(_eMethod);
					}
				};
				ComboMethod(Image::RatioMethod::FREE);
				ComboMethod(Image::RatioMethod::KEEP_WIDTH);
				ComboMethod(Image::RatioMethod::KEEP_HEIGHT);
				ImGui::EndCombo();
			}
		}

		const glm::vec3 vCol = GetDebugColor();
		ImGui::ColorButton("", glm::vec4(vCol, 1.f));
		ImGui::Separator();
		ImGui::PopID();

		for (IUIElementWeakPtr xChild : m_vecChildren)
		{
			ImGui::Indent();
			xChild.lock()->OnUpdate(_ts);
			ImGui::Unindent();
		}
	}

	glm::vec3 IUIElement::GetDebugColor() const
	{
		const float fRed   = ((size_t)this % 0xff) / 255.f;
		const float fGreen = (((size_t)this ^ 0xf5) % 0xff) / 255.f;
		const float fBlue  = 0.f;
		return glm::vec3(fRed, fGreen, fBlue);
	}

	void IUIElement::ComputeGlobalTransform()
	{
		const glm::vec2 vParentGlobalSize = m_xParent.expired() ? VEC2_1 : m_xParent.lock()->GetGlobalScreenSize();
		const glm::vec2 vTempGlobalSize	  = vParentGlobalSize * m_vSize;
		const glm::mat3 mParentTransform  = m_xParent.expired() ? glm::identity<glm::mat3>() : m_xParent.lock()->m_mGlobalTransform;

		const glm::mat3 mTranslation = glm::mat3(VEC3_X, VEC3_Y, glm::vec3(m_vPosition, 1.f));
		const glm::mat3 mRotation	 = glm::rotate2D(glm::identity<glm::mat3>(), m_fRotation);
		const glm::mat3 mScale		 = glm::mat3(glm::vec3(m_vSize.x, 0.f, 0.f), glm::vec3(0.f, m_vSize.y, 0.f), VEC3_Z);
		const glm::mat3 mPivot		 = glm::mat3(VEC3_X, VEC3_Y, glm::vec3(-m_vPivot, 1.f));

		m_mGlobalTransform = (((mParentTransform * mTranslation) * mScale) * mRotation) * mPivot;

		VisitChildren([](IUIElementWeakPtr _xChild) {
			_xChild.lock()->ComputeGlobalTransform();
			return VisitorFlow::CONTINUE;
		});
	}

	glm::vec2 IUIElement::ComputeParentGlobalSize() const
	{
		IUIElementWeakPtr xWeakElmt = m_xParent;
		glm::vec2		  vSize		= VEC2_1;
		while (!xWeakElmt.expired())
		{
			auto xElmt = xWeakElmt.lock();

			vSize *= xElmt->GetSize();

			xWeakElmt = xElmt->m_xParent;
		}

		return vSize;
	}

	const glm::mat3 &IUIElement::GetParentGlobalTransform() const
	{
		if (m_xParent.expired())
			return m_xParent.lock()->m_mGlobalTransform;

		return MAT3_1;
	}

	glm::vec2 IUIElement::GetGlobalScreenPos() const { return m_mGlobalTransform * VEC3_Z; }

	glm::vec2 IUIElement::GetGlobalScreenPivotPos() const { return m_mGlobalTransform * glm::vec3(m_vPivot, 1.f); }

	glm::vec2 IUIElement::GetGlobalScreenSize() const
	{
		const float fX = glm::sqrt(glm::pow(m_mGlobalTransform[0][0], 2.f) + glm::pow(m_mGlobalTransform[1][0], 2.f));
		const float fY = glm::sqrt(glm::pow(m_mGlobalTransform[0][1], 2.f) + glm::pow(m_mGlobalTransform[1][1], 2.f));
		return glm::vec2(fX, fY);
	}

	void IUIElement::GetGlobalScreenCorners(glm::vec2 &_vTopLeft, glm::vec2 &_vTopRight, glm::vec2 &_vBottomLeft, glm::vec2 &_vBottomRight) const
	{
		_vTopLeft	  = m_mGlobalTransform * glm::vec3(0.f, 0.f, 1.f);
		_vTopRight	  = m_mGlobalTransform * glm::vec3(1.f, 0.f, 1.f);
		_vBottomLeft  = m_mGlobalTransform * glm::vec3(0.f, 1.f, 1.f);
		_vBottomRight = m_mGlobalTransform * glm::vec3(1.f, 1.f, 1.f);
	}

	void IUIElement::GetGlobalScreenBoundingBox(glm::vec2 &_vTopLeft, glm::vec2 &_vBottomRight) const
	{
		glm::vec2 vTopLeft, vTopRight, vBottomLeft, vBottomRight;
		GetGlobalScreenCorners(vTopLeft, vTopRight, vBottomLeft, vBottomRight);
		_vTopLeft	  = glm::min(glm::min(vTopLeft, vTopRight), glm::min(vBottomLeft, vBottomRight));
		_vBottomRight = glm::max(glm::max(vTopLeft, vTopRight), glm::max(vBottomLeft, vBottomRight));
	}

	glm::vec2 IUIElement::GetImGuiScreenPos() const
	{
		const glm::vec2 vWindowSize = ImGui::GetWindowSize();
		const glm::vec2 vWindowPos	= ImGui::GetWindowPos();
		// const glm::vec2 vSize		= GetGlobalScreenSize() * vWindowSize;
		return GetGlobalScreenPos() * vWindowSize + vWindowPos;
	}

	glm::vec2 IUIElement::GetImGuiScreenPivotPos() const
	{
		const glm::vec2 vWindowSize = ImGui::GetWindowSize();
		const glm::vec2 vWindowPos	= ImGui::GetWindowPos();
		// const glm::vec2 vSize		= GetGlobalScreenSize() * vWindowSize;
		return GetGlobalScreenPivotPos() * vWindowSize + vWindowPos;
	}

	glm::vec2 IUIElement::GetImGuiScreenSize() const
	{
		const glm::vec2 vWindowSize = ImGui::GetWindowSize();
		return GetGlobalScreenSize() * vWindowSize;
	}

	void IUIElement::GetImGuiScreenCorners(glm::vec2 &vTopLeft, glm::vec2 &vTopRight, glm::vec2 &vBottomLeft, glm::vec2 &vBottomRight) const
	{
		const glm::vec2 vWindowSize = ImGui::GetWindowSize();
		const glm::vec2 vWindowPos	= ImGui::GetWindowPos();

		vTopLeft	 = glm::vec2(m_mGlobalTransform * glm::vec3(0.f, 0.f, 1.f)) * vWindowSize + vWindowPos;
		vTopRight	 = glm::vec2(m_mGlobalTransform * glm::vec3(1.f, 0.f, 1.f)) * vWindowSize + vWindowPos;
		vBottomLeft	 = glm::vec2(m_mGlobalTransform * glm::vec3(0.f, 1.f, 1.f)) * vWindowSize + vWindowPos;
		vBottomRight = glm::vec2(m_mGlobalTransform * glm::vec3(1.f, 1.f, 1.f)) * vWindowSize + vWindowPos;
	}

	void IUIElement::GetImGuiScreenBoundingBox(glm::vec2 &_vTopLeft, glm::vec2 &_vBottomRight) const
	{
		const glm::vec2 vWindowSize = ImGui::GetWindowSize();
		const glm::vec2 vWindowPos	= ImGui::GetWindowPos();

		glm::vec2 vTopLeft, vTopRight, vBottomLeft, vBottomRight;
		GetGlobalScreenCorners(vTopLeft, vTopRight, vBottomLeft, vBottomRight);
		_vTopLeft	  = glm::min(glm::min(vTopLeft, vTopRight), glm::min(vBottomLeft, vBottomRight)) * vWindowSize + vWindowPos;
		_vBottomRight = glm::max(glm::max(vTopLeft, vTopRight), glm::max(vBottomLeft, vBottomRight)) * vWindowSize + vWindowPos;
	}

	{
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

		ImDrawList *pDrawList = ImGui::GetForegroundDrawList();
		if (m_bClipChildrenToRect)
		{
			glm::vec2 vMin, vMax;
			GetImGuiScreenBoundingBox(vMin, vMax);
			pDrawList->PushClipRect(vMin, vMax, true);
		}
		{
			VisitChildren([](IUIElementWeakPtr _xChild) {
				ASSERT(!_xChild.expired());

				auto xChild = _xChild.lock();
				xChild->OnRender();

				return VisitorFlow::CONTINUE;
			});
		}
		if (m_bClipChildrenToRect)
			pDrawList->PopClipRect();
	}


	//////////////////////////////////////////////////////////////////////////
	/*=============================== Text ===============================*/

	void Text::SetText(const String &_str)
	{
		if (m_sText != _str) // strcmp(m_sText.c_str(), _str.c_str()) != 0)
		{
			m_sText = _str;
			OnTextChanged();
		}
	}

	void Text::Render() const
	{
		/* TODO:
		 * - adaptive font size depending on screen size ?
		 */
	}

	void Text::OnTextChanged()
	{
		/* TODO:
		 * - recompute the text size, text break, wrap and so on
		 */
	}

	//////////////////////////////////////////////////////////////////////////
	/*=============================== Image ===============================*/

	void Image::SetImage(const StringPath &_filePath)
	{
		if (m_imgFilePath != _filePath)
		{
			m_imgFilePath = _filePath;
			OnImageChanged();
		}
	}

	void Image::SetRatioMethod(RatioMethod _eMethod)
	{
		if (m_eRatioMethod != _eMethod)
		{
			m_eRatioMethod = _eMethod;
			OnImageSizeChanged();
		}
	}

	void Image::SetRatio(float _fRatio)
	{
		if (m_fFixedRatio != _fRatio)
		{
			m_fFixedRatio = _fRatio;
			OnImageSizeChanged();
		}
	}

	void Image::Render() const
	{
		ImDrawList *	pDrawList = ImGui::GetForegroundDrawList();
		const glm::vec2 vMin	  = GetImGuiScreenPos();
		const glm::vec2 vMax	  = vMin + GetImGuiScreenSize();

		const glm::vec3 vCol = GetDebugColor();
		ImColor			imCol(vCol.r, vCol.g, vCol.b);

		glm::vec2 vTopLeft, vTopRight, vBottomLeft, vBottomRight;
		GetImGuiScreenCorners(vTopLeft, vTopRight, vBottomLeft, vBottomRight);
		if (m_hTexture)
			pDrawList->AddImageQuad((ImTextureID)m_hTexture->GetNativeHandle(), vTopLeft, vTopRight, vBottomRight, vBottomLeft);
		else
			pDrawList->AddQuadFilled(vTopLeft, vTopRight, vBottomRight, vBottomLeft, imCol);

		pDrawList->AddCircleFilled(GetImGuiScreenPivotPos(), 6, 0xFFFFFFFF);
		pDrawList->AddCircleFilled(GetImGuiScreenPivotPos(), 3, imCol);
	}

	void Image::OnSizeChanged()
	{
		SUPER::OnSizeChanged();
		OnImageSizeChanged();
	}

	void Image::OnParentSizeChanged()
	{
		SUPER::OnParentSizeChanged();
		OnImageSizeChanged();
	}

	void Image::OnImageSizeChanged()
	{
		if (m_eRatioMethod != RatioMethod::FREE)
		{
			if (m_fFixedRatio > 0.f)
			{
				const glm::vec2	 vGlobalSize  = GetGlobalScreenSize();
				const glm::vec2 &vCurrentSize = GetSize();
				if (vGlobalSize.x / vGlobalSize.y != m_fFixedRatio)
				{
					if (m_eRatioMethod == RatioMethod::KEEP_WIDTH)
					{
						glm::vec2 vParentSize = ComputeParentGlobalSize();
						glm::vec2 vNewSize(vCurrentSize.x, (vGlobalSize.x / vParentSize.y) / m_fFixedRatio);
						SetSize(vNewSize);
					}
					else if (m_eRatioMethod == RatioMethod::KEEP_HEIGHT)
					{
						glm::vec2 vParentSize = ComputeParentGlobalSize();
						glm::vec2 vNewSize((vGlobalSize.y / vParentSize.x) / m_fFixedRatio, vCurrentSize.y);
						SetSize(vNewSize);
					}
				}
			}
			else
			{
				// TODO: reset size ?
			}
		}
	}

	void Image::OnImageChanged()
	{
		/* TODO:
		 * - if needed, load the file
		 * - if needed, upload to GPU
		 * - store the GPU handle to draw later
		 */

		float fOldRatio = m_hTexture ? m_hTexture->GetWidth() / (float)m_hTexture->GetHeight() : -1.f;
		if (m_imgFilePath.empty())
		{
			m_hTexture = TextureHandle();
		}
		else
		{
			m_hTexture		= AssetLoader::GetTexture(m_imgFilePath.string());
			float fNewRatio = m_hTexture ? m_hTexture->GetWidth() / (float)m_hTexture->GetHeight() : -1.f;
			if (fNewRatio > glm::epsilon<float>() && fNewRatio != fOldRatio)
				OnImageSizeChanged();
		}
	}
} // namespace Plop::UI
