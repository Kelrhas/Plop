#include "Config.h"

#include "SpritesheetLoader.h"

#include "Application.h"
#include "Renderer/RendererConfig.h"
#include "TextureLoader.h"
#include "Utils/OSDialogs.h"

#include <entt/entt.hpp>

namespace Plop::AssetLoader
{
	struct SpritesheetLoader final
	{
		using result_type = SpritesheetPtr;

		SpritesheetPtr operator()(const StringPath &_path) const
		{
			SpritesheetPtr xSpritesheet;

			std::ifstream assetFile(_path, std::ios::in);
			ASSERTM(assetFile.is_open(), "Spritesheet file not found: {}", std::filesystem::absolute(_path).string().c_str());
			if (assetFile.is_open())
			{
				json jSpritesheet;

				assetFile >> jSpritesheet;

				ASSERTM(jSpritesheet.contains("TexturePath"), "SpritesheetLoader: No TexturePath in file %s", _path.string().c_str());

				if (!jSpritesheet.contains("TexturePath"))
					return {};

				xSpritesheet = std::make_shared<Spritesheet>();
				xSpritesheet->LoadFromFile(std::filesystem::canonical(_path));

				StringPath sTexPath = _path.parent_path() / jSpritesheet["TexturePath"];

				xSpritesheet->m_hTexture = GetTexture(sTexPath);
				if (jSpritesheet.contains("Columns"))
					xSpritesheet->m_uNbColumn = jSpritesheet["Columns"];
				if (jSpritesheet.contains("Rows"))
					xSpritesheet->m_uNbRow = jSpritesheet["Rows"];
				if (jSpritesheet.contains("Aliases"))
					jSpritesheet["Aliases"].get_to(xSpritesheet->m_mapAliases);
			}

			return xSpritesheet;
		}
	};

	entt::resource_cache<Spritesheet, SpritesheetLoader> s_CacheSpritesheet {};

	SpritesheetHandle GetSpritesheet(const StringPath &_sFilepath)
	{
		String sName = _sFilepath.stem().string();
		return s_CacheSpritesheet.load(entt::hashed_string(sName.c_str()), _sFilepath).first->second;
	}

	void ClearSpritesheetCache()
	{
		s_CacheSpritesheet.clear();
	}

	SpritesheetHandle PickSpritesheetFromCache()
	{
		SpritesheetHandle hNewSpritesheet;

		bool bOpened = true;
		if (ImGui::BeginPopupModal("Pick a loaded spritesheet###PickSpritesheetFromCache", &bOpened))
		{
			static ImGuiTextFilter texFilter;
			texFilter.Draw();

			const float fRegionHeight = ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing();

			static int iNbImagePerRow = 3;
			if (ImGui::BeginChild("Spritesheet", ImVec2(0, fRegionHeight)))
			{
				// ImGuiStyle& style = ImGui::GetStyle();
				// float fContentWidth = ImGui::GetContentRegionAvail().x - style.ItemSpacing.x;
				// float fSpacing = style.ItemSpacing.x + 2 * style.FramePadding.x;
				// float fWidth = (fContentWidth - (iNbImagePerRow - 1) * (fSpacing)) / iNbImagePerRow;

				ImGui::Columns(iNbImagePerRow, nullptr, false);

				for (auto [id, _hSpritesheet] : s_CacheSpritesheet)
				{
					if (_hSpritesheet)
					{
						if (!texFilter.PassFilter(_hSpritesheet->GetFilePathStr().c_str()))
							continue;

						const float fImageRatio = (float)_hSpritesheet->GetTextureHandle()->GetWidth() / (float)_hSpritesheet->GetTextureHandle()->GetHeight();

						const ImVec2 vUvMin = Renderer::USE_INVERTED_Y_UV ? ImVec2(0, 1) : ImVec2(0, 0);
						const ImVec2 vUvMax = Renderer::USE_INVERTED_Y_UV ? ImVec2(1, 0) : ImVec2(1, 1);
						if (ImGui::ImageButton((ImTextureID)_hSpritesheet->GetTextureHandle()->GetNativeHandle(),
											   ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / fImageRatio),
											   vUvMin,
											   vUvMax))
						{
							hNewSpritesheet = _hSpritesheet;
							continue;
						}


						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::Text(_hSpritesheet->GetFilePath().stem().string().c_str());
							ImGui::Text(_hSpritesheet->GetFilePath().parent_path().string().c_str());
							ImGui::EndTooltip();
						}
						ImGui::NextColumn();
					}
				}
				ImGui::Columns(1);
			}
			ImGui::EndChild();

			if (ImGui::Button("Load from file"))
			{
				StringPath filePath;
				if (Dialog::OpenFile(filePath, Dialog::SPRITESHEET_FILTER))
				{
					hNewSpritesheet = GetSpritesheet(filePath);
				}
			}

			ImGui::SameLine();
			ImGui::SliderInt("# column", &iNbImagePerRow, 1, 10);

			if (hNewSpritesheet)
			{
				ImGuiIO &io = ImGui::GetIO();
				if (!io.KeyCtrl)
					ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}


		return hNewSpritesheet;
	}
} // namespace Plop::AssetLoader