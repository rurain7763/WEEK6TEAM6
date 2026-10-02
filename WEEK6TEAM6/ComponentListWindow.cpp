#include "ComponentListWindow.h"
#include "ObjectFactory.h"
#include "Actor.h"
#include "SceneManager.h"
#include "Object.h"
#include "World.h"
#include "FEditorUIManager.h"
#include "FInstrumentor.h"
#include "SceneComponent.h"
#include <algorithm>

void FComponentListWindow::Render(const FGuiReference& GuiReference)
{
	PROFILE_FUNCTION();

	ImGuiIO& io = ImGui::GetIO();
	UWorld* CurrentWorld = GuiReference.SceneManager->GetCurrentWorld();
	AActor* SelectedActor = GuiReference.SceneManager->GetSelectedActor();

	ImGuiWindowFlags Flags = ImGuiWindowFlags_NoCollapse;

	ImGui::Begin("ComponentList Panel", nullptr, Flags);

	ImGui::SeparatorText("Component List");
	if (SelectedActor)
	{
		if (ImGui::BeginChild("ComponentList", ImVec2(0, 0), ImGuiChildFlags_Borders))
		{
			USceneComponent* CurrentRootComponent = SelectedActor->GetRootComponent();
			if (CurrentRootComponent)
			{
				RenderTreeSceneComponent(CurrentRootComponent);
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

void FComponentListWindow::RenderTreeSceneComponent(USceneComponent* InSceneComp)
{
	ImGuiTreeNodeFlags NodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;
	const bool bHasNoChildren = (InSceneComp->GetAttachChildren().Num() == 0);
	const bool bIsSelected = false;
	if (bHasNoChildren)
	{
		NodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}
	// TODO:: 액터선택이 아니라 신컴포넌트 선택 기능 추가해줘야 함
	if (bIsSelected)
	{
		NodeFlags |= ImGuiTreeNodeFlags_Selected;
	}
	bool bNodeOpen = ImGui::TreeNodeEx((void*)InSceneComp, NodeFlags, "%s (UUID: %d)",
		InSceneComp->GetClass()->Name.CStr(), InSceneComp->UUID);
	if (ImGui::IsItemClicked())
	{
		SelectedComponent = InSceneComp;
	}
	if (bNodeOpen && !bHasNoChildren)
	{
		for (USceneComponent* ChildComp : InSceneComp->GetAttachChildren())
		{
			if (ChildComp)
			{
				RenderTreeSceneComponent(ChildComp);
			}
		}
		ImGui::TreePop();
	}
}

