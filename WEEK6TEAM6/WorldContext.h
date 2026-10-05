#pragma once

#include "WorldType.h"

class UWorld;

class FWorldContext
{
public:
	FWorldContext() = default;
	~FWorldContext() = default;

	UWorld* GetWorld() const { return World; }
	void SetWorld(UWorld* InWorld) { World = InWorld; }

	void SetWorldType(EWorldType worldtype) { WorldType = worldtype; }
	EWorldType GetWorldType() { return WorldType; }

private:
	UWorld* World = nullptr;
	EWorldType WorldType = EWorldType::Editor;
};