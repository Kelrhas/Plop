#include "SampleLayer.h"

#include <imgui.h>

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
			-0.6f, -0.6f, 0.0f,		1.f, 0.f, 0.f, 1.f,		0.f, 0.f,
			-0.6f,  0.6f, 0.0f,		1.f, 0.f, 1.f, 1.f,		0.f, 1.f,
			 0.6f,  0.6f, 0.0f,		0.f, 1.f, 0.f, 1.f,		1.f, 1.f,
			 0.6f, -0.6f, 0.0f,		1.f, 1.f, 0.f, 1.f,		1.f, 0.f
		};


		xMesh->m_xVertexArray = Plop::VertexArray::Create();
		xMesh->m_xVertexArray->Bind();

		Plop::BufferLayout layout = {
			{ "position", Plop::BufferLayout::ElementType::FLOAT3},
			{ "color", Plop::BufferLayout::ElementType::FLOAT4},
			{ "uv", Plop::BufferLayout::ElementType::FLOAT2}
		};

		Plop::VertexBufferPtr xVertBuff = Plop::VertexBuffer::Create((uint32_t)sizeof(vertices) * 3, (float*)&vertices);
		xVertBuff->SetLayout(layout);
		xMesh->m_xVertexArray->AddVertexBuffer(xVertBuff);


		uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };
		Plop::IndexBufferPtr xIndBuff = Plop::IndexBuffer::Create((uint32_t)sizeof(indices) / sizeof(uint32_t), indices);
		xMesh->m_xVertexArray->SetIndexBuffer(xIndBuff);

		xMesh->m_xShader = Plop::Renderer::LoadShader("data/shaders/textured.glsl");

		xMesh->m_xTex = Plop::Texture::Create2D( "assets/textures/approval.png" );
	}

	Plop::Renderer::Clear();

	Plop::Renderer::PrepareScene(*m_pCamera);

	xMesh->m_mTransform = glm::identity<glm::mat4>();
	Plop::Renderer::SubmitDraw(xMesh);
	xMesh->m_mTransform = glm::translate(xMesh->m_mTransform, VEC3_RIGHT);
	Plop::Renderer::SubmitDraw(xMesh);
	xMesh->m_mTransform = glm::translate(xMesh->m_mTransform, VEC3_RIGHT);
	Plop::Renderer::SubmitDraw(xMesh);
	xMesh->m_mTransform = glm::translate(xMesh->m_mTransform, VEC3_RIGHT);
	Plop::Renderer::SubmitDraw(xMesh);

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
			-0.6f, -0.6f, 0.0f,		1.f, 0.f, 0.f, 1.f,		0.f, 0.f,
			-0.6f,  0.6f, 0.0f,		1.f, 0.f, 1.f, 1.f,		0.f, 1.f,
			 0.6f,  0.6f, 0.0f,		0.f, 1.f, 0.f, 1.f,		1.f, 1.f,
			 0.6f, -0.6f, 0.0f,		1.f, 1.f, 0.f, 1.f,		1.f, 0.f
		};


		m_xTowerMesh->m_xVertexArray = Plop::VertexArray::Create();
		m_xTowerMesh->m_xVertexArray->Bind();

		Plop::BufferLayout layout = {
			{ "position", Plop::BufferLayout::ElementType::FLOAT3},
			{ "color", Plop::BufferLayout::ElementType::FLOAT4},
			{ "uv", Plop::BufferLayout::ElementType::FLOAT2}
		};

		Plop::VertexBufferPtr xVertBuff = Plop::VertexBuffer::Create((uint32_t)sizeof(vertices) * 3, (float*)&vertices);
		xVertBuff->SetLayout(layout);
		m_xTowerMesh->m_xVertexArray->AddVertexBuffer(xVertBuff);


		uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };
		Plop::IndexBufferPtr xIndBuff = Plop::IndexBuffer::Create((uint32_t)sizeof(indices) / sizeof(uint32_t), indices);
		m_xTowerMesh->m_xVertexArray->SetIndexBuffer(xIndBuff);

		m_xTowerMesh->m_xShader = Plop::Renderer::LoadShader("data/shaders/textured.glsl");

		m_xTowerMesh->m_xTex = Plop::Texture::Create2D("assets/textures/tower.png");
	}
}

void SampleLayer2D::OnUnregistered()
{
	Plop::EventDispatcher::UnregisterListener(&m_CameraController);
}

void SampleLayer2D::OnUpdate(Plop::TimeStep& _timeStep)
{
	m_CameraController.OnUpdate(_timeStep);

	Plop::Renderer::PrepareScene(m_CameraController.GetCamera());

	m_xTowerMesh->m_mTransform = glm::scale(glm::identity<glm::mat4>(), glm::vec3(0.1f));
	Plop::Renderer::SubmitDraw(m_xTowerMesh);
	m_xTowerMesh->m_mTransform = glm::translate(m_xTowerMesh->m_mTransform, -VEC3_RIGHT);
	Plop::Renderer::SubmitDraw(m_xTowerMesh);
	m_xTowerMesh->m_mTransform = glm::translate(m_xTowerMesh->m_mTransform, -VEC3_RIGHT);
	Plop::Renderer::SubmitDraw(m_xTowerMesh);
	m_xTowerMesh->m_mTransform = glm::translate(m_xTowerMesh->m_mTransform, -VEC3_RIGHT);
	Plop::Renderer::SubmitDraw(m_xTowerMesh);

	Plop::Renderer::EndScene();
}
