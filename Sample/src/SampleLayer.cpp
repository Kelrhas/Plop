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


	static Plop::ShaderPtr xShader = nullptr;
	static Plop::VertexArrayPtr xVAO = nullptr;
	if (xVAO == nullptr)
	{
		float vertices[] = {
			-1.0f, -1.0f, 0.0f, 1.f, 0.f, 0.f, 1.f,
			1.0f, -1.0f, 0.0f, 1.f, 0.f, 1.f, 1.f,
			0.0f, 1.0f, 0.0f, 0.f, 1.f, 0.f, 1.f
		};


		xVAO.reset(Plop::VertexArray::Create());
		xVAO->Bind();

		Plop::VertexBufferPtr xVertBuff;
		xVertBuff.reset(Plop::VertexBuffer::Create((uint32_t)sizeof(vertices) * 3, (float*)&vertices));

		Plop::BufferLayout layout = {
			{ "position", Plop::BufferLayout::ElementType::FLOAT3},
			{ "color", Plop::BufferLayout::ElementType::FLOAT4},
			//{ "normal", BufferLayout::ElementType::FLOAT3, true}
		};

		xVertBuff->SetLayout(layout);
		xVAO->AddVertexBuffer(xVertBuff);


		uint32_t indices[3] = { 0, 1, 2 };
		Plop::IndexBufferPtr xIndBuff;
		xIndBuff.reset(Plop::IndexBuffer::Create((uint32_t)sizeof(indices), indices));
		xVAO->SetIndexBuffer(xIndBuff);

		xShader.reset(Plop::Shader::Create("data/shaders/vertexColor.glsl"));

	}

	m_pCamera->Rotate(VEC3_FORWARD, glm::radians(25.f) * _timeStep.GetGameDeltaTime());

	Plop::Renderer::Clear();

	Plop::Renderer::PrepareScene(m_pCamera);

	Plop::Renderer::SubmitDraw(xShader, xVAO);

	Plop::Renderer::EndScene();

}