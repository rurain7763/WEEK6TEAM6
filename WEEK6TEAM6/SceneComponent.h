#pragma once

#include "ActorComponent.h"
#include "GraphicsManager.h"

#include "Vector.h"

class FTransform;

class USceneComponent : public UActorComponent
{
	REFLECT_CLASS(USceneComponent, UActorComponent)
public:
	USceneComponent() = default;
	virtual ~USceneComponent();

	void Initialize(FVector location, FRotator rotation, FVector scale3D);

	virtual void SerializeClass(json::JSON& outJson) const override;
	virtual void DeserializeClass(const json::JSON& inJson) override;

	FVector GetRelativeLocation() const;
	void SetRelativeLocation(FVector location);

	FRotator GetRelativeRotation() const;
	void SetRelativeRotation(FRotator rotation);

	FVector GetRelativeScale3D() const;
	void SetRelativeScale3D(FVector scale);

	const FTransform& GetTransform() const;

	bool AttachToComponent(USceneComponent* InParentComp);
	bool IsAttachedTo(const USceneComponent* TestComp);

	const USceneComponent* GetAttachParent() { return mAttachParent; }
	TArray<USceneComponent*>& GetAttachChildren() { return mAttachChildren; }
	void AddChildComp(USceneComponent* InSceneComp);
protected:
	virtual void OnTransformChanged() {}

private:
	FTransform mRelativeTransform;
	TArray<USceneComponent*> mAttachChildren;
	USceneComponent* mAttachParent = nullptr;
};

