#pragma once

#include <TimeStep.h>

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
		friend class IVisibleElement;
		friend class Dialog;

	public:
		IUIElement() = default;
		IUIElement(IUIElementWeakPtr _xParent);
		virtual ~IUIElement() = default;

		void SetPosition(const glm::vec2 _vPos);
		void SetSize(const glm::vec2 _vSize);
		void SetPivot(const glm::vec2 _vPivot);
		void SetRotation(float _fAngle);

		static bool AddElementTo(IUIElementWeakPtr _xElement, IUIElementWeakPtr _xParent);
		static bool RemoveElementFrom(IUIElementWeakPtr _xElement, IUIElementWeakPtr _xParent);

		template<typename Visitor>
		void VisitChildren(Visitor &&_v)
		{
			for (IUIElementWeakPtr xChild : m_vecChildren)
			{
				if (_v(xChild) == VisitorFlow::BREAK)
					break;
			}
		}

	protected:
		virtual void OnPositionChanged() {}
		virtual void OnSizeChanged() {}
		virtual void OnParentPositionChanged() {}
		virtual void OnParentSizeChanged() {}
		virtual void OnUpdate(const TimeStep &_ts);
		virtual void OnRender() = 0; // ask for render if any to the hierarchy

		glm::vec3 GetDebugColor() const;

		void			 ComputeGlobalTransform();
		glm::vec2		 GetGlobalScreenPos() const;
		glm::vec2		 GetGlobalScreenPivotPos() const;
		glm::vec2		 GetGlobalScreenSize() const;
		void			 GetGlobalScreenCorners(glm::vec2 &_vTopLeft, glm::vec2 &_vTopRight, glm::vec2 &_vBottomLeft, glm::vec2 &_vBottomRight) const;
		void			 GetGlobalScreenBoundingBox(glm::vec2 &_vTopLeft, glm::vec2 &_vBottomRight) const;
		const glm::vec2 &GetPivot() const { return m_vPivot; }

		glm::vec2 GetImGuiScreenPos() const;
		glm::vec2 GetImGuiScreenPivotPos() const;
		glm::vec2 GetImGuiScreenSize() const;
		void	  GetImGuiScreenCorners(glm::vec2 &vTopLeft, glm::vec2 &vTopRight, glm::vec2 &vBottomLeft, glm::vec2 &vBottomRight) const;
		void	  GetImGuiScreenBoundingBox(glm::vec2 &_vTopLeft, glm::vec2 &_vBottomRight) const;

		std::vector<IUIElementWeakPtr> m_vecChildren;

	private:
		// TODO: depth as float (relative to parent)
		glm::vec2		  m_vPosition = VEC2_1 * 0.5f; // relative to parent, move the pivot of [0,1] * the absolute size of the parent
		glm::vec2		  m_vSize	  = VEC2_1;		   // relative to parent, depending on the absolute size of the parent
		glm::vec2		  m_vPivot	  = VEC2_1 * 0.5f; // relative to self, [0,1] value, 0.5 being centered
		float			  m_fRotation = 0.f;		   // relative to parent
		glm::mat3		  m_mGlobalTransform;		   // relative to screen
		bool			  m_bClipChildrenToRect = true;
		IUIElementWeakPtr m_xParent;

		String m_sName;
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

		bool IsVisible() const { return m_bVisible; }

	protected:
		virtual void OnShow() {}
		virtual void OnHide() {}
		virtual void OnRender() final;
		virtual void Render() const = 0;

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
	protected:
	private:
	};

	/// <summary>
	/// Handles the player input, does not handle the visuals
	/// </summary>
	class Button : public IUIElement
	{
		using SUPER = IUIElement;

	public:
	protected:
	private:
	};

	/// <summary>
	/// Font rendering
	/// </summary>
	class Text : public IVisibleElement
	{
		using SUPER = IVisibleElement;

	public:
		void SetText(const String &_str);

	protected:
		virtual void Render() const override;
		void		 OnTextChanged();

	private:
		String m_sText;
	};

	/// <summary>
	/// Texture rendering
	/// </summary>
	class Image : public IVisibleElement
	{
		using SUPER = IVisibleElement;

	public:
		void SetImage(const StringPath &_filePath);

	protected:
		virtual void Render() const override;
		void		 OnImageChanged();

	private:
		StringPath m_imgFilePath;
	};

} // namespace Plop::UI
