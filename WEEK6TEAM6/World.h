#pragma once

#include "Object.h"
#include "Actor.h"
#include "RenderInfo.h"
#include "FFrustum.h"
#include "TActiveTickList.h"
#include "Level.h"
#include "WorldType.h"

class UWorld final : public UObject
{
	REFLECT_CLASS(UWorld, UObject)

public:
	UWorld() = default;
	virtual ~UWorld();

	static UWorld* CreateWorld(EWorldType worldType)
	{
		UWorld* world = FObjectFactory::ConstructUnInitializedObject<UWorld>();
		world->InitializeWorld();
		world->mWorldType = worldType;
		return world;
	}
	void InitializeWorld();

	static UWorld* DuplicateWorldForPIE()
	{

	}

	virtual void SerializeClass(json::JSON& outJson) const override;
	virtual void DeserializeClass(const json::JSON& inJson) override;

	void AddActor(ULevel* level, AActor* actor);
	bool RemoveActor(ULevel* level, uint32 uuid);
	//Todo : MoveActor 필요함

	void RegisterActorComponents(AActor* actor);
	void UnregisterActorComponents(AActor* actor);

	void RegisterComponent(UActorComponent* Component);
	void UnregisterComponent(UActorComponent* component);

    void RefreshComponentTick(UActorComponent* Component); // 등록되었거나 Tickable 플래그가 변경된 컴포넌트만 활성 목록에 반영합니다.
	void MarkBoundsDirty(UActorComponent* component); // AAABB가 바뀌었음을 표시합니다. Tick에서 BVH를 재구성할 때 사용합니다.
	void RequestRenderUpdate(UActorComponent* component); // 렌더링 대상이 된 컴포넌트를 등록합니다. Unique 체크를 안하므로 렌더링 후 Clear()로 비워야 함.

	// 모든 메시가 공유할 LOD 기준 카메라 위치를 Tick 시작 전에 전달합니다.
	void SetLODViewOrigin(const FVector& ViewOrigin) { mLODViewOrigin = ViewOrigin; }
	const FVector& GetLODViewOrigin() const { return mLODViewOrigin; }
	void Render(float deltaTime, FRenderCollector& outCollector);

	bool IsAABBsDirty() const { return mbAABBsDirty; }
	void SetAABBsClean() { mbAABBsDirty = false; }
	const TArray<FAABB>& GetCachedEntryAABBs() const { return mCachedEntryAABBs; }

	EWorldType GetWorldType() { return mWorldType; }
	void SetWorldType(EWorldType worldType) { mWorldType = worldType; }
	ULevel* GetPersistentLevel() { return PersistentLevel; }

	void Tick(float deltaTime);

private:
	int32 getActorIndex(uint32 actorUUID) const;

private:
	enum
	{
		DEFAULT_RESERVE_MEM = 1024U
	};

	TArray<UPrimitiveComponent*> mPrimitiveComponents;
	TArray<UActorComponent*> mNonPrimitiveRenderableComponents; // Primitive는 아닌데 렌더링 기능이 있는 컴포넌트.
	TArray<UActorComponent*> mUUIDRenderableComponents;
	// UUID는 표시 옵션을 순회 전에 한 번 검사하기 위해 별도의 Tick 목록에 둡니다.
	TActiveTickList<UActorComponent> mTickableComponents;
	TActiveTickList<UActorComponent> mUUIDTickableComponents;
	// 소멸 중 가상 타입 정보가 바뀌어도 등록 당시 목록에서 제거할 수 있게 보관합니다.
	struct FComponentRegistration
	{
		UPrimitiveComponent* Primitive = nullptr;
		bool bRenderable = false;
		bool bUUID = false;
	};
	TMap<UActorComponent*, FComponentRegistration> ComponentRegistrations;

	TArray<UActorComponent*> mShouldRenderComponents; // 이번 프레임에 렌더링 대상이 된 컴포넌트. 렌더링 후 Clear()로 비워야 함.

	bool mbBVHDirty = true;
	bool mbAABBsDirty = true;
	TArray<FAABB> mCachedEntryAABBs;
	FBVH<UPrimitiveComponent*> mBVH;
	TArray<FBVHNode*> QueryStack;
	TArray<FBVHItemRange> VisibleRanges;
	FVector mLODViewOrigin;

	ULevel* PersistentLevel = nullptr;
	TArray<ULevel*> SubLevels; // 구현 X
	EWorldType mWorldType = EWorldType::Editor;
};
