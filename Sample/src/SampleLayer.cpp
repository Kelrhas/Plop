#include "SampleLayer.h"

#include <imgui.h>

#include <Constants.h>
#include <Renderer/Renderer.h>
#include <Renderer/Texture.h>

void SampleLayer::OnRegistered()
{
	m_pCamera = new Plop::OrthographicCamera();
}

void SampleLayer::OnUpdate(Plop::TimeStep& _timeStep)
{
	if (ImGui::Begin("Sample window"))
	{
		ImGui::Text("Sample text");
		ImGui::End();
	}


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
		//xMesh->m_xShader = Plop::Shader::Create("data/shaders/vertexColor.glsl");

		xTex = Plop::Texture::Create2D( "data/textures/approval.png" );
	}

	//m_pCamera->Rotate(VEC3_FORWARD, glm::radians(25.f) * _timeStep.GetGameDeltaTime());

	Plop::Renderer::Clear();

	Plop::Renderer::PrepareScene(m_pCamera);

	xTex->BindSlot( 0 );
	Plop::Renderer::SubmitDraw(xMesh);

	Plop::Renderer::EndScene();

}