#pragma once

#include <memory>

namespace Plop
{
	class RenderContext;
	using RenderContextPtr = std::shared_ptr<RenderContext>;

	class RenderContext
	{
	public:
		RenderContext() {}
		virtual ~RenderContext() = default;

		virtual void Init() = 0;
		virtual void Flush() = 0;
		virtual void Destroy() = 0;

		virtual void SetCurrent() = 0;
		virtual void SetVSync(bool _bEnabled) = 0;

		virtual void* GetNativeContext() const = 0;

		static RenderContextPtr Create();

	protected:
	};
}
