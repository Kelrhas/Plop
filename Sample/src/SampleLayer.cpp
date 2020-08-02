#include "SampleLayer.h"

#include <imgui.h>

#include <Constants.h>
#include <Renderer/Renderer.h>

void SampleLayer::OnUpdate(Plop::TimeStep& _timeStep)
{
	if (ImGui::Begin("Sample window"))
	{
		ImGui::Text("Sample text");
		ImGui::End();
	}


	static Plop::MeshPtr xMesh = nullptr;
	if (xMesh == nullptr)
	{
		xMesh = std::make_shared<Plop::Mesh>();
		float vertices[] = {
			-1.0f, -1.0f, 0.0f, 1.f, 0.f, 0.f, 1.f,
			1.0f, -1.0f, 0.0f, 1.f, 0.f, 1.f, 1.f,
			0.0f, 1.0f, 0.0f, 0.f, 1.f, 0.f, 1.f
		};


		xMesh->m_xVertexArray.reset(Plop::VertexArray::Create());
		xMesh->m_xVertexArray->Bind();

		Plop::VertexBufferPtr xVertBuff;
		xVertBuff.reset(Plop::VertexBuffer::Create((uint32_t)sizeof(vertices) * 3, (float*)&vertices));

		Plop::BufferLayout layout = {
			{ "position", Plop::BufferLayout::ElementType::FLOAT3},
			{ "color", Plop::BufferLayout::ElementType::FLOAT4},
			//{ "normal", BufferLayout::ElementType::FLOAT3, true}
		};

		xVertBuff->SetLayout(layout);
		xMesh->m_xVertexArray->AddVertexBuffer(xVertBuff);


		uint32_t indices[3] = { 0, 1, 2 };
		Plop::IndexBufferPtr xIndBuff;
		xIndBuff.reset(Plop::IndexBuffer::Create((uint32_t)sizeof(indices), indices));
		xMesh->m_xVertexArray->SetIndexBuffer(xIndBuff);

		xMesh->m_xShader.reset(Plop::Shader::Create("data/shaders/vertexColor.glsl"));
	}

	m_pCamera->Rotate(VEC3_FORWARD, glm::radians(25.f) * _timeStep.GetGameDeltaTime());

	Plop::Renderer::Clear();

	Plop::Renderer::PrepareScene(m_pCamera);

	Plop::Renderer::SubmitDraw(xMesh);

	Plop::Renderer::EndScene();

}