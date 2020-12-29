#include "SampleLayer.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <Assets/TextureLoader.h>
#include <Debug/Debug.h>
#include <Constants.h>
#include <Events/EventDispatcher.h>
#include <Input/Input.h>
#include <Renderer/Renderer.h>
#include <Renderer/Texture.h>
#include <ECS/BaseComponents.h>

//////////////////////////////////////////////////////////////////////////
// SampleLayer

void SampleLayer::OnRegistered()
{
}

void SampleLayer::OnUpdate(Plop::TimeStep& _timeStep)
{
	PROFILING_FUNCTION();

	static Plop::MeshPtr xMesh = nullptr;
	if (xMesh == nullptr)
	{
		xMesh = std::make_shared<Plop::Mesh>();
		float vertices[] = {
			-0.6f, -0.6f, 0.0f,		/*1.f, 0.f, 0.f, 1.f,*/		0.f, 0.f,
			-0.6f,  0.6f, 0.0f,		/*1.f, 0.f, 1.f, 1.f,*/		0.f, 1.f,
			 0.6f,  0.6f, 0.0f,		/*0.f, 1.f, 0.f, 1.f,*/		1.f, 1.f,
			 0.6f, -0.6f, 0.0f,		/*1.f, 1.f, 0.f, 1.f,*/		1.f, 0.f
		};


		xMesh->m_xVertexArray = Plop::VertexArray::Create();
		xMesh->m_xVertexArray->Bind();

		Plop::BufferLayout layout = {
			{ "position", Plop::BufferLayout::ElementType::FLOAT3},
			//{ "color", Plop::BufferLayout::ElementType::FLOAT4},
			{ "uv", Plop::BufferLayout::ElementType::FLOAT2}
		};

		Plop::VertexBufferPtr xVertBuff = Plop::VertexBuffer::Create(sizeof(vertices), (void*)&vertices);
		xVertBuff->SetLayout(layout);
		xMesh->m_xVertexArray->AddVertexBuffer(xVertBuff);


		uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };
		Plop::IndexBufferPtr xIndBuff = Plop::IndexBuffer::Create(sizeof(indices) / sizeof(uint32_t), indices);
		xMesh->m_xVertexArray->SetIndexBuffer(xIndBuff);

		xMesh->m_xShader = Plop::Renderer::LoadShader("data/shaders/textured.glsl");
		xMesh->m_xShader->Bind();
		xMesh->m_xShader->SetUniformVec4("u_color", glm::vec4(1.f));

		xMesh->m_hTex = Plop::AssetLoader::GetTexture( "assets/textures/grass.png" );
	}

	Plop::Renderer::Clear();

}

//////////////////////////////////////////////////////////////////////////
// SampleLayer2D

void SampleLayer2D::OnRegistered()
{
	
}

void SampleLayer2D::OnUnregistered()
{

}

void SampleLayer2D::OnUpdate(Plop::TimeStep& _timeStep)
{
	PROFILING_FUNCTION();

	if (Plop::Input::IsMouseLeftPressed())
	{
		static Plop::ParticleData data;
		data.fLifeTimeBase = 2.f;
		data.vPositionBase = glm::vec3( 0.f );
		data.vPositionVariationMin = glm::vec3( -0.5f );
		data.vPositionVariationMax = glm::vec3( 0.5f );
		data.vSizeStart = glm::vec2( 0.15f );
		data.vSizeEnd = glm::vec2( 0.02f );
		data.vSpeedStart = glm::vec3( 0.f, 1.f, 0.f );
		data.vSpeedEnd = glm::vec3( 0.f, -1.f, 0.f );
		data.vColorStart = glm::vec4( 1.f );
		data.vColorEnd = glm::vec4( 1.f, 0.f, 0.f, 1.f );
		m_particles.Spawn( data, 50 );
	}
	m_particles.Update( _timeStep );
}
