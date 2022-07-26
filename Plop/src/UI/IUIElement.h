#pragma once

namespace Plop::UI
{
	/// <summary>
	/// Interface of all UI elements.
	/// Handles the positionning and size
	/// </summary>
	class IUIElement;
	using IUIElementPtr		= std::shared_ptr<IUIElement>;
	using IUIElementWeakPtr = std::weak_ptr<IUIElement>;

	class IUIElement
	{
	public:
		IUIElement() = default;
		IUIElement(IUIElementWeakPtr _xParent);
		virtual ~IUIElement() = default;

		void SetPosition(const glm::vec2 _vPos);
		void SetSize(const glm::vec2 _vSize);

		virtual void OnPositionChanged() {}
		virtual void OnSizeChanged() {}
		virtual void OnParentPositionChanged() {}
		virtual void OnParentSizeChanged() {}
		virtual void OnUpdate();
		virtual void OnRender() = 0; // ask for render if any to the hierarchy

		template<typename Visitor>
		void VisitChildren(Visitor &&_v)
		{
			for (IUIElementWeakPtr xChild : m_vecChildren)
			{
				if (_v(xChild) == VisitorFlow::BREAK)
					break;
			}
		}

	private:
		glm::vec2					   m_vPosition = VEC2_0; // relative to parent
		glm::vec2					   m_vSize	   = VEC2_1; // relative to parent
		IUIElementWeakPtr			   m_xParent;
		std::vector<IUIElementWeakPtr> m_vecChildren;
	};

	/// <summary>
	/// Interface of all visible (and therefore renderable) elements
	/// Handles the visibility
	/// </summary>
	class IVisibleElement : public IUIElement
	{
		using SUPER = IUIElement;
	public:
		void Show();
		void Hide();
		void SetVisibility(bool _bVisible);

		virtual void OnShow() {}
		virtual void OnHide() {}
		virtual void OnRender() override;
		virtual void Render() = 0;

	private:
		bool m_bVisible = true;
	};

	/// <summary>
	/// Interface for a UIElement container that manage their children properties (position, size ...)
	/// Compute its children position/size
	/// Can be a list, a grid ...
	/// </summary>
	class IContainer : public IUIElement
	{
		using SUPER = IUIElement;
	public:
	private:
	};

	/// <summary>
	/// Handles the player input, does not handle the visuals
	/// </summary>
	class Button : public IUIElement
	{
		using SUPER = IUIElement;
	public:
	private:
	};

	/// <summary>
	/// Font rendering
	/// </summary>
	class Text : public IVisibleElement
	{
		using SUPER = IVisibleElement;
	public:
		virtual void Render() override;

	private:
	};

	/// <summary>
	/// Texture rendering
	/// </summary>
	class Image : public IVisibleElement
	{
		using SUPER = IVisibleElement;
	public:
		virtual void Render() override;

	private:
	};

} // namespace Plop::UI
