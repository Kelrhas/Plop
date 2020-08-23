#include "SampleLayer.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

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
	static Plop::TexturePtr xTex = nullptr;
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

		xMesh->m_xTex = Plop::Texture::Create2D( "assets/textures/grass.png" );
	}

	Plop::Renderer::Clear();

	/*
	Plop::Renderer::PrepareScene(*m_pCamera);
	{
		PROFILING_SCOPE( "DrawScene" );
		xMesh->m_mTransform = glm::identity<glm::mat4>();
		Plop::Renderer::SubmitDraw( xMesh );
		xMesh->m_mTransform = glm::translate( xMesh->m_mTransform, VEC3_RIGHT );
		Plop::Renderer::SubmitDraw( xMesh );
		xMesh->m_mTransform = glm::translate( xMesh->m_mTransform, VEC3_RIGHT );
		Plop::Renderer::SubmitDraw( xMesh );
		xMesh->m_mTransform = glm::translate( xMesh->m_mTransform, VEC3_RIGHT );
		Plop::Renderer::SubmitDraw( xMesh );
	}
	Plop::Renderer::EndScene();
	*/
}

//////////////////////////////////////////////////////////////////////////
// SampleLayer2D

void SampleLayer2D::OnRegistered()
{
	if (m_xLevel == nullptr)
	{
		m_xLevel = std::make_shared<Plop::LevelBase>();
		m_xLevel->Init();
	}


	if (m_xSpritesheet == nullptr)
	{
		m_xSpritesheet = Plop::Texture::Create2D( "assets/textures/tilesheet.png" );
	}

	Plop::Entity cameraEntity = m_xLevel->CreateEntity( "Camera" );
	Plop::CameraComponent& cameraComp = cameraEntity.AddComponent<Plop::CameraComponent>();
	cameraComp.xCamera = std::make_shared<Plop::Camera>();
	cameraComp.xCamera->Init();

	if (!m_PlayerEntity)
	{
		m_PlayerEntity = m_xLevel->CreateEntity( "Player" );
		ASSERT( m_PlayerEntity, "Invalid entity" );

		Plop::SpriteRendererComponent& renderer = m_PlayerEntity.AddComponent<Plop::SpriteRendererComponent>();

		renderer.xSprite = std::make_shared<Plop::Sprite>();
		renderer.xSprite->SetTexture( m_xSpritesheet );
		renderer.xSprite->SetSpriteIndex( { 18, 1 }, { 23, 13 } );
		renderer.xSprite->SetSpriteIndex( { 0, 0 }, { 23, 13 }, { 9, 1 } );

		static float fAngle = 0.f;
	}

	if (m_xTowerMesh == nullptr)
	{
		m_xTowerMesh = std::make_shared<Plop::Mesh>();
		float vertices[] = {
			-0.6f, -0.6f, 0.0f,		/*1.f, 0.f, 0.f, 1.f,	*/	0.f, 0.f,
			-0.6f,  0.6f, 0.0f,		/*1.f, 0.f, 1.f, 1.f,	*/	0.f, 1.f,
			 0.6f,  0.6f, 0.0f,		/*0.f, 1.f, 0.f, 1.f,	*/	1.f, 1.f,
			 0.6f, -0.6f, 0.0f,		/*1.f, 1.f, 0.f, 1.f,	*/	1.f, 0.f
		};


		m_xTowerMesh->m_xVertexArray = Plop::VertexArray::Create();
		m_xTowerMesh->m_xVertexArray->Bind();

		Plop::BufferLayout layout = {
			{ "position", Plop::BufferLayout::ElementType::FLOAT3},
			//{ "color", Plop::BufferLayout::ElementType::FLOAT4},
			{ "uv", Plop::BufferLayout::ElementType::FLOAT2}
		};

		Plop::VertexBufferPtr xVertBuff = Plop::VertexBuffer::Create(sizeof(vertices), (float*)&vertices);
		xVertBuff->SetLayout(layout);
		m_xTowerMesh->m_xVertexArray->AddVertexBuffer(xVertBuff);


		uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };
		Plop::IndexBufferPtr xIndBuff = Plop::IndexBuffer::Create(sizeof(indices) / sizeof(uint32_t), indices);
		m_xTowerMesh->m_xVertexArray->SetIndexBuffer(xIndBuff);

		m_xTowerMesh->m_xShader = Plop::Renderer::LoadShader("data/shaders/textured.glsl");
		m_xTowerMesh->m_xShader->Bind();
		m_xTowerMesh->m_xShader->SetUniformVec4("u_color", glm::vec4(1.f));

		m_xTowerMesh->m_xTex = Plop::Texture::Create2D("assets/textures/tower.png");
		m_xTowerMesh->m_xTex->BindSlot( 1 );
	}
}

void SampleLayer2D::OnUnregistered()
{
	if (m_PlayerEntity)
	{
		m_PlayerEntity.RemoveComponent<glm::mat4>();
	}
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

	if (m_xLevel->BeforeUpdate())
	{
		m_xLevel->Update( _timeStep );

		m_particles.Update( _timeStep );

		m_xLevel->AfterUpdate();
	}
}
