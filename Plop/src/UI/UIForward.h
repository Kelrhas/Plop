#pragma once

namespace Plop::UI
{
	// interfaces
	class IUIElement;
	class IVisibleElement;
	class IContainer;

	// actual classes
	class Dialog;
	class Button;
	class Text;
	class Image;

	// typedefs
	using DialogPtr = std::shared_ptr<Dialog>;
}