#pragma once

#include <string_view>
#include <filesystem>
#include "SceneData.h"
#include "TArray.h"
#include "RenderInfo.h"
#include "enum.h"
#include "FAssetManager.h"
#include "FObjViewer.h"
#include "FFrustum.h"
#include "WorldType.h"

inline constexpr std::string_view kSceneDataDir = "SceneData\\";
inline constexpr std::string_view kSceneDataSuffix = ".Scene";

class FFileManager;
class FFrameTimer;
class FEditorViewportClient;
class FGraphicsManager;
class UWorld;
class URenderer;
struct FViewport;
struct FEditorLayout;
struct FEditorViewport;
class UStaticMesh;
class UActorComponent;
class FWorldContext;

class FEditorEngine
{
public:
	FEditorEngine();
	~FEditorEngine();

	void Tick(float deltaTime);
	void Render(float deltaTime, FRenderCollector& outCollector);

	UWorld* CreateNewWorld(EWorldType worldType);
	void DeleteWorld(UWorld* world);
	void DeleteWorldContext(EWorldType worldType);
	void CreateNewWorldContext(EWorldType worldType);
	FWorldContext* FindWorldContext(EWorldType worldType);

	// Todo : WorldContext 저장 체제로 변경 해야함
	void SaveScene(FCamera* Camera, const std::filesystem::path& scenePath, const FFileManager& fileManager);
	void LoadScene(FCamera* Camera, const std::filesystem::path& scenePath, const FFileManager& fileManager);

	UActorComponent* GetSelectedComponent() const { return mSelectedComponent; }
	bool IsComponentSelected() const { return mSelectedComponent != nullptr; }
	void SetSelectedComponent(UActorComponent* component);
	void ResetSelectedComponent() { mSelectedComponent = nullptr; }

	void StartPIE();
	void EndPIE();

private:
	TArray<FWorldContext*> mWorldContexts;
	UActorComponent* mSelectedComponent = nullptr;
};
