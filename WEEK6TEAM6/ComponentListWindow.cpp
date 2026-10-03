#include "ComponentListWindow.h"
#include "ObjectFactory.h"
#include "Actor.h"
#include "SceneManager.h"
#include "Object.h"
#include "World.h"
#include "FEditorUIManager.h"
#include "FInstrumentor.h"
#include "SceneComponent.h"
#include "PrimitiveComponent.h"
#include "UStaticMeshComponent.h"
#include <algorithm>

void FComponentListWindow::Render(const FGuiReference& GuiReference)
{
	PROFILE_FUNCTION();

	ImGuiIO& io = ImGui::GetIO();
	UWorld* CurrentWorld = GuiReference.SceneManager->GetCurrentWorld();
	AActor* SelectedActor = GuiReference.SceneManager->GetSelectedActor();
	mSelectedActorComp = GuiReference.SceneManager->GetSelectedActorComp();

	ImGuiWindowFlags Flags = ImGuiWindowFlags_NoCollapse;

	ImGui::Begin("ComponentList Panel", nullptr, Flags);

	ImGui::SeparatorText("Component List");
	if (ImGui::Button("Add") && mSelectedActorComp)
	{
		if (mSelectedActorComp->IsA<USceneComponent>())
		{
			ImGui::OpenPopup("ItemListPopup");
		}
	}
	if (ImGui::BeginPopup("ItemListPopup"))
	{
		ImGui::Text("AddItemList");
		ImGui::Separator();
		if (ImGui::Button("StaticMeshComponent"))
		{
			UStaticMeshComponent* MeshComponent = FObjectFactory::ConstructObject<UStaticMeshComponent>(FVector(0, 0, 0), FRotator(0, 0, 0), FVector(1, 1, 1));
			USceneComponent* ParentComp = mSelectedActorComp->Cast<USceneComponent>();
			MeshComponent->AttachToComponent(ParentComp);
			SelectedActor->AddComponent(MeshComponent);
			//GuiReference.SceneManager->SetSelectedPrimitive(MeshComponent);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (SelectedActor)
	{
		if (ImGui::BeginChild("ComponentList", ImVec2(0, 0), ImGuiChildFlags_Borders))
		{
			USceneComponent* CurrentRootComponent = SelectedActor->GetRootComponent();
			if (CurrentRootComponent)
			{
				RenderTreeSceneComponent(CurrentRootComponent, GuiReference);
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

void FComponentListWindow::RenderTreeSceneComponent(USceneComponent* InSceneComp, const FGuiReference& GuiReference)
{
	ImGuiTreeNodeFlags NodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;
	const bool bHasNoChildren = (InSceneComp->GetAttachChildren().Num() == 0);
	// TODO:: 액터선택이 아니라 신컴포넌트 선택 기능 추가해줘야 함
	const bool bIsSelected = (mSelectedActorComp == InSceneComp);
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
		mSelectedActorComp = InSceneComp;
		GuiReference.SceneManager->SetSelectedActorComp(InSceneComp);
	}
	if (bNodeOpen && !bHasNoChildren)
	{
		for (USceneComponent* ChildComp : InSceneComp->GetAttachChildren())
		{
			if (ChildComp)
			{
				RenderTreeSceneComponent(ChildComp, GuiReference);
			}
		}
		ImGui::TreePop();
	}
}

