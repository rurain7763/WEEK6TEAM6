#pragma once

#include "Core.h"
#include "TArray.h"
#include "ImGui/imgui.h"
#include "Actor.h"
#include "ActorComponent.h"

struct FGuiReference;
class FSceneManager;
class UObject;

class FComponentListWindow
{
public:
	void Render(const FGuiReference& GuiReference);

private:
	//AActor* CurrentActor = nullptr;
	void RenderTreeSceneComponent(USceneComponent* InSceneComponent);
	UActorComponent* SelectedComponent;
};
