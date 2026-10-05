#pragma once

#include "Object.h"

class FClassInfo;
class UWorld;

class ULevel final : public UObject
{
	REFLECT_CLASS(ULevel, UObject)

public:
	ULevel() = default;
	~ULevel();

	void AddActor(AActor* actor);
	bool RemoveActor(uint32 uuid);
	bool FindActor(uint32 uuid, AActor*& outActor) const;

	void PostInitProperties() override;

	void SerializeClass(json::JSON& outJson) const override;
	void DeserializeClass(const json::JSON& inJson) override;

	TArray<AActor*>& GetActors() { return mActors; }
	void SetOwningWorld(UWorld* world) { OwningWorld = world; }
	UWorld* GetOwningWorld() { return OwningWorld; }

private:
	UWorld* OwningWorld = nullptr;
	TArray<AActor*> mActors;
};