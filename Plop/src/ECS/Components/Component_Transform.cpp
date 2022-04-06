#include "Plop_pch.h"
#include "Component_Transform.h"


#include <glm/gtc/type_ptr.hpp>
#include <imgui_custom.h>
#include <ImGuizmo.h>

#include "Application.h"
#include "ECS/LevelBase.h"
#include "ECS/ECSHelper.h"
#include "ECS/Components/BaseComponents.h"
#include "Utils/JsonTypes.h"

namespace Plop
{


	void Component_Transform::EditorUI()
	{
		float fResetBtnWidth = ImGui::CalcTextSize( "X" ).x + ImGui::GetStyle().FramePadding.x * 2;

		// TODO: add a switch local/world
		ImGui::Text( "Local space" );
		ImGui::Separator();

		ImGui::DragFloat3( "Pos", glm::value_ptr( vPosition ), 0.1f );
		ImGui::SameLine( ImGui::GetWindowContentRegionMax().x - fResetBtnWidth );
		if (ImGui::SmallButton( "X##Pos" ))
			SetLocalPosition( VEC3_0 );
		ImGui::Custom::Tooltip( "Reset" );

		// TODO: Use matrices in Runtime environment and 2 vec3 & 1 quat in editor environment

		static bool bIsEditing = false;
		static glm::vec3 vTemp = VEC3_0;
		glm::vec3 vRot = bIsEditing ? vTemp : glm::degrees( glm::eulerAngles( GetLocalRotation() ) );
		bool bChanged = ImGui::DragFloat3( "Rot", glm::value_ptr( vRot ) );

		if (ImGui::IsItemActivated())
		{
			vTemp = vRot;
			bIsEditing = true;
		}
		else if (!ImGui::IsItemActive())
			bIsEditing = false;

		if (bIsEditing && bChanged)
		{
			vTemp = vRot;
			SetLocalRotation( glm::quat( glm::radians( vRot ) ) );
		}

		ImGui::SameLine( ImGui::GetWindowContentRegionMax().x - fResetBtnWidth );
		if (ImGui::SmallButton( "X##Rot" ))
			SetLocalRotation( glm::identity<glm::quat>() );
		ImGui::Custom::Tooltip( "Reset" );


		ImGui::DragFloat3( "Sca", glm::value_ptr( vScale ), 0.1f, 0.01f );
		ImGui::SameLine( ImGui::GetWindowContentRegionMax().x - fResetBtnWidth );
		if (ImGui::SmallButton( "X##Sca" ))
			SetLocalScale( VEC3_1 );
		ImGui::Custom::Tooltip( "Reset" );
	}

	void Component_Transform::FromJson( const json& _j )
	{
		if (_j.contains( "Position" ))
			SetLocalPosition( _j["Position"] );
		if (_j.contains( "Rotation" ))
			if (_j["Rotation"].size() == 3)
				SetLocalRotation( glm::quat( glm::vec3( _j["Rotation"] ) ) );
			else
				SetLocalRotation( _j["Rotation"] );
		if (_j.contains( "Scale" ))
			SetLocalScale( _j["Scale"] );
	}
	
	json Component_Transform::ToJson() const
	{
		json j;
		j["Position"] = GetLocalPosition();
		j["Rotation"] = GetLocalRotation();
		j["Scale"] = GetLocalScale();
		return j;
	}

	float Component_Transform::Distance2D( const Component_Transform& _other ) const
	{
		glm::vec2 vThisPos = GetWorldPosition();
		glm::vec2 vOtherPos = _other.GetWorldPosition();

		return glm::distance( vThisPos, vOtherPos );
	}

	float Component_Transform::Distance2DSquare( const Component_Transform& _other ) const
	{
		glm::vec2 vThisPos = GetWorldPosition();
		glm::vec2 vOtherPos = _other.GetWorldPosition();

		return glm::distance2( vThisPos, vOtherPos );
	}

	// LOCAL

	void Component_Transform::SetLocalMatrix( const glm::mat4& _m )
	{
		glm::vec3 vRotation;
		ImGuizmo::DecomposeMatrixToComponents( glm::value_ptr( _m ), glm::value_ptr( vPosition ), glm::value_ptr( vRotation ), glm::value_ptr( vScale ) );
		qRotation = glm::quat( glm::radians( vRotation ) );
	}

	glm::mat4 Component_Transform::GetLocalMatrix() const
	{
		glm::mat4 mTransform = glm::translate( glm::identity<glm::mat4>(), vPosition );
		mTransform = mTransform * glm::toMat4( qRotation );
		mTransform = glm::scale( mTransform, vScale );
		return mTransform;
	}


	void Component_Transform::TranslateLocal( const glm::vec3& _vTrans )
	{
		vPosition += _vTrans;
	}


	// WORLD

	void Component_Transform::SetWorldMatrix( const glm::mat4& _m )
	{
		glm::mat4 mParentWorldMatrix = glm::identity<glm::mat4>();

		auto xLevel = Application::GetCurrentLevel().lock();
		auto owner = GetComponentOwner( xLevel, *this );
		Entity parent( owner.GetComponent<Component_GraphNode>().parent, xLevel );
		if (parent)
		{
			// TODO: cache the parent worldmatrix or our own world and local matrix
			mParentWorldMatrix = parent.GetComponent<Component_Transform>().GetWorldMatrix();
		}

		mParentWorldMatrix = glm::inverse( mParentWorldMatrix );

		glm::mat4 mLocalMatrix = mParentWorldMatrix * _m;

		SetLocalMatrix( mLocalMatrix );
	}

	glm::mat4 Component_Transform::GetWorldMatrix() const
	{
		glm::mat4 mLocalMatrix = GetLocalMatrix();

		auto xLevel = Application::GetCurrentLevel().lock();

		auto owner = GetComponentOwner( xLevel, *this );
		Entity parent( owner.GetComponent<Component_GraphNode>().parent, xLevel );
		if (parent)
		{
			// TODO: cache the parent worldmatrix or our own world and local matrix
			return parent.GetComponent<Component_Transform>().GetWorldMatrix() * mLocalMatrix;
		}

		return mLocalMatrix;
	}

	void Component_Transform::SetWorldPosition( const glm::vec3& _vPos )
	{
		glm::mat4 mParentWorldMatrix = glm::identity<glm::mat4>();

		auto xLevel = Application::GetCurrentLevel().lock();
		auto owner = GetComponentOwner( xLevel, *this );
		Entity parent( owner.GetComponent<Component_GraphNode>().parent, xLevel );
		if (parent)
		{
			// TODO: cache the parent worldmatrix or our own world and local matrix
			mParentWorldMatrix = parent.GetComponent<Component_Transform>().GetWorldMatrix();
		}

		mParentWorldMatrix = glm::inverse( mParentWorldMatrix );

		glm::vec3 vLocalPos = mParentWorldMatrix * glm::vec4( _vPos, 0.f );

		SetLocalPosition( vLocalPos );
	}

	glm::vec3 Component_Transform::GetWorldPosition() const
	{
		glm::vec3 vLocalPos = GetLocalPosition();

		auto xLevel = Application::GetCurrentLevel().lock();

		auto owner = GetComponentOwner( xLevel, *this );
		Entity parent( owner.GetComponent<Component_GraphNode>().parent, xLevel );
		if (parent)
		{
			// TODO: cache the parent worldmatrix or our own world and local matrix
			return (parent.GetComponent<Component_Transform>().GetWorldMatrix() * glm::vec4( vLocalPos, 1.f )).xyz;
		}

		return vLocalPos;
	}

	void Component_Transform::SetWorldRotation( const glm::quat& _qRot )
	{
		glm::mat4 mParentWorldMatrix = glm::identity<glm::mat4>();

		auto xLevel = Application::GetCurrentLevel().lock();
		auto owner = GetComponentOwner( xLevel, *this );
		Entity parent( owner.GetComponent<Component_GraphNode>().parent, xLevel );
		if (parent)
		{
			// TODO: cache the parent worldmatrix or our own world and local matrix
			mParentWorldMatrix = parent.GetComponent<Component_Transform>().GetWorldMatrix();
		}

		mParentWorldMatrix = glm::inverse( mParentWorldMatrix );

		glm::quat qLocalRot = mParentWorldMatrix * glm::mat4( _qRot );

		SetLocalRotation( qLocalRot );
	}

	glm::quat Component_Transform::GetWorldRotation() const
	{
		return glm::quat( GetWorldMatrix() );
	}

	void Component_Transform::TranslateWorld( const glm::vec3& _vTrans )
	{
		glm::mat4 mParentWorldMatrix = glm::identity<glm::mat4>();

		auto xLevel = Application::GetCurrentLevel().lock();
		auto owner = GetComponentOwner( xLevel, *this );
		Entity parent( owner.GetComponent<Component_GraphNode>().parent, xLevel );
		if (parent)
		{
			// TODO: cache the parent worldmatrix or our own world and local matrix
			mParentWorldMatrix = parent.GetComponent<Component_Transform>().GetWorldMatrix();
		}

		mParentWorldMatrix = glm::inverse( mParentWorldMatrix );

		glm::vec3 vTransLocal = mParentWorldMatrix * glm::vec4( _vTrans, 1.f );

		TranslateLocal( vTransLocal );
	}

}

#ifndef USE_COMPONENT_MGR
namespace MM
{
	template <>
	void ComponentEditorWidget<Plop::Component_Transform>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Plop::Component_Transform>( e );
		comp.EditorUI();
	}

	template <>
	json ComponentToJson<Plop::Component_Transform>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Plop::Component_Transform>( e );
		return comp.ToJson();
	}

	template<>
	void ComponentFromJson<Plop::Component_Transform>( entt::registry& reg, entt::registry::entity_type e, const json& _j )
	{
		auto& comp = reg.get_or_emplace<Plop::Component_Transform>( e );
		comp.FromJson( _j );
	}

}
#endif