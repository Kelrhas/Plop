#include "SampleLayer.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <Debug/Debug.h>
#include <Constants.h>
#include <Events/EventDispatcher.h>
#include <Input/Input.h>
#include <Renderer/Renderer.h>
#include <Renderer/Texture.h>

//////////////////////////////////////////////////////////////////////////
// SampleLayer

void SampleLayer::OnRegistered()
{
	m_pCamera = new Plop::OrthographicCamera();
}

void SampleLayer::OnUpdate(Plop::TimeStep& _timeStep)
{
	PROFILING_FUNCTION();

	if (ImGui::Begin("Sample window"))
	{
		ImGui::Text("Sample text");
	}
	ImGui::End();


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

	Plop::Renderer::PrepareScene(*m_pCamera);
	{
		PROFILING_SCOPE( "DrawScene" );
		//xMesh->m_mTransform = glm::identity<glm::mat4>();
		//Plop::Renderer::SubmitDraw( xMesh );
		//xMesh->m_mTransform = glm::translate( xMesh->m_mTransform, VEC3_RIGHT );
		//Plop::Renderer::SubmitDraw( xMesh );
		//xMesh->m_mTransform = glm::translate( xMesh->m_mTransform, VEC3_RIGHT );
		//Plop::Renderer::SubmitDraw( xMesh );
		//xMesh->m_mTransform = glm::translate( xMesh->m_mTransform, VEC3_RIGHT );
		//Plop::Renderer::SubmitDraw( xMesh );
	}
	Plop::Renderer::EndScene();

}

//////////////////////////////////////////////////////////////////////////
// SampleLayer2D

void SampleLayer2D::OnRegistered()
{
	Plop::EventDispatcher::RegisterListener(&m_CameraController);

	m_CameraController.Init();

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

	if (m_xSpritesheet == nullptr)
	{
		m_xSpritesheet = Plop::Texture::Create2D( "assets/textures/tilesheet.png" );
	}
}

void SampleLayer2D::OnUnregistered()
{
	Plop::EventDispatcher::UnregisterListener(&m_CameraController);
}

void SampleLayer2D::OnUpdate(Plop::TimeStep& _timeStep)
{
	PROFILING_FUNCTION();

	m_CameraController.OnUpdate(_timeStep);

	Plop::Renderer2D::PrepareScene(m_CameraController.GetCamera());

	static glm::vec2 vSize( 1.f );
	ImGui::DragFloat2( "Grass Size", glm::value_ptr( vSize ), 0.1f, 0.f, 10.f );
	static Plop::TexturePtr xGrassTex = Plop::Texture::Create2D( "assets/textures/grass.png" );
	static Plop::TexturePtr xApprovalTex = Plop::Texture::Create2D( "assets/textures/approval.png" );
	static Plop::TexturePtr xTowerTex = Plop::Texture::Create2D( "assets/textures/tower.png" );
	{
		PROFILING_SCOPE("Drawing grass");
		static int nb = 5;
		ImGui::DragInt( "Grid", &nb, 0.1f, 0, 50 );
		for (int i = 0; i < nb; ++i)
		{
			for (int j = 0; j < nb; ++j)
			{
				Plop::Renderer2D::DrawQuadColorRotated( glm::vec3( i, j, 0.f ), vSize, i / (float)nb * 3.14f, glm::vec4(i/ (float)nb, j/ (float)nb, 0.f, 1.f) );
				Plop::Renderer2D::DrawQuadTexture( glm::vec3( i+nb, j, 0.f ), vSize, xGrassTex );
				Plop::Renderer2D::DrawQuadTexture( glm::vec3( i, j + nb, 0.f ), vSize, xApprovalTex );
				Plop::Renderer2D::DrawQuadTextureRotated( glm::vec3( i+nb, j + nb, 0.f ), vSize, i / (float)nb * 3.14f, xTowerTex );
			}
		}
	}
	static float fPlayerSpeed = 0.1f;
	ImGui::DragFloat( "Speed", &fPlayerSpeed, 0.1f, 0.f, 0.3f );
	if (ImGui::Button( "Reset" ))
	{
		m_vPlayerSpeed = glm::vec2( 0.f );
		m_vPlayerPos = glm::vec2( 0.f );
	}
	m_vPlayerSpeed *= 0.5f;
	if (Plop::Input::IsKeyDown( Plop::KeyCode::KEY_Q ))
		m_vPlayerSpeed.x = -1.f;
	if (Plop::Input::IsKeyDown( Plop::KeyCode::KEY_D ))
		m_vPlayerSpeed.x = 1.f;
	if (Plop::Input::IsKeyDown( Plop::KeyCode::KEY_Z ))
		m_vPlayerSpeed.y = 1.f;
	if (Plop::Input::IsKeyDown( Plop::KeyCode::KEY_S ))
		m_vPlayerSpeed.y = -1.f;
	
	float fSpeedSq = glm::dot( m_vPlayerSpeed, m_vPlayerSpeed );
	if(fSpeedSq > fPlayerSpeed)
		m_vPlayerSpeed = glm::normalize( m_vPlayerSpeed ) * fPlayerSpeed;

	if (glm::any( glm::isnan( m_vPlayerSpeed ) ))
		m_vPlayerSpeed = glm::vec2( 0.f );

	m_vPlayerPos += m_vPlayerSpeed;

	static Plop::Sprite playerSprite;
	if (playerSprite.GetTexture() == nullptr)
	{
		playerSprite.SetTexture( m_xSpritesheet );
		playerSprite.SetSpriteIndex( 18, 1, 23, 13 );
	}
	static float fAngle = 0.f;
	ImGui::DragFloat( "Angle", &fAngle, 0.1f, -3.14f, 3.14f );
	Plop::Renderer2D::DrawSprite( m_vPlayerPos, glm::vec2(1.f), playerSprite, fAngle);

	if (Plop::Input::IsMouseLeftPressed())
	{
		static Plop::ParticleData data;
		data.fLifeTimeBase = 2.f;
		data.vPositionBase = glm::vec3( 0.f );
		data.vPositionVariationMin = glm::vec3( -0.5f );
		data.vPositionVariationMax = glm::vec3( 0.5f );
		data.vSizeStart = glm::vec2( 0.3f );
		data.vSizeEnd = glm::vec2( 0.1f );
		data.vSpeedStart = glm::vec3( 0.f, 1.f, 0.f );
		data.vSpeedEnd = glm::vec3( 0.f, -1.f, 0.f );
		data.vColorStart = glm::vec4( 1.f );
		data.vColorEnd = glm::vec4( 1.f, 0.f, 0.f, 1.f );
		m_particles.Spawn( data, 10 );
	}

	m_particles.Update( _timeStep );

	Plop::Renderer2D::EndScene();
}
