#include "Plop_pch.h"

#include "Dialog.h"

#include "UI/IUIElement.h"


namespace Plop::UI
{
	void Dialog::OnUpdate(const TimeStep &_ts)
	{
		ComputeGlobalTransform();
		SUPER::OnUpdate(_ts);
	}

	void Dialog::OnRender()
	{
		ImDrawList *	pDrawList = ImGui::GetForegroundDrawList();
		const glm::vec2 vMin	  = GetImGuiScreenPos();
		const glm::vec2 vMax	  = vMin + GetImGuiScreenSize();

		const glm::vec3 vCol = GetDebugColor();
		pDrawList->AddRectFilled(vMin, vMax, ImColor(vCol.r, vCol.g, vCol.b));
		if (m_bClipChildrenToRect)
			pDrawList->PushClipRect(vMin, vMax, true);
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

	void Dialog::RenderDialog()
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_Always);
		ImGui::Begin("TestDialogRender", nullptr/*, ImGuiWindowFlags_NoBackground*/);
		OnRender();
		ImGui::End();
	}

} // namespace Plop::UI