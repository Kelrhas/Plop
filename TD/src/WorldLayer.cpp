#include "TD_pch.h"
#include "WorldLayer.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <Debug/Debug.h>
#include <Constants.h>
#include <ECS/Components/BaseComponents.h>
#include <ECS/Components/Component_ParticleSystem.h>
#include <Events/EventDispatcher.h>
#include <Input/Input.h>
#include <Renderer/Renderer.h>
#include <Renderer/Texture.h>
#include <Renderer/Sprite.h>

void WorldLayer::OnRegistered()
{

}

void WorldLayer::OnUnregistered()
{
}

void WorldLayer::OnUpdate(Plop::TimeStep& _ts)
{
	PROFILING_FUNCTION();
}
