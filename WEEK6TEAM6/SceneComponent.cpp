#include "SceneComponent.h"

#include <format>

#include "Transform.h"
#include "JsonUtil.h"

void USceneComponent::Initialize(FVector location, FRotator rotation, FVector scale3D)
{
	UActorComponent::Initialize();

	mRelativeTransform.SetLocation(location);
	mRelativeTransform.SetRotation(rotation);
	mRelativeTransform.SetScale(scale3D);
}

USceneComponent::~USceneComponent()
{
}

void USceneComponent::SerializeClass(json::JSON& outJson) const
{
	UActorComponent::SerializeClass(outJson);
	outJson["Properties"]["mRelativeLocation"] = JsonUtils::ToJson(mRelativeTransform.GetLocation());
	outJson["Properties"]["mRelativeRotation"] = JsonUtils::ToJson(mRelativeTransform.GetRotation());
	outJson["Properties"]["mRelativeScale3D"] = JsonUtils::ToJson(mRelativeTransform.GetScale());
}

void USceneComponent::DeserializeClass(const json::JSON& inJson)
{
	UActorComponent::DeserializeClass(inJson);

	const json::JSON& propertiesJson = inJson.at("Properties");

	if (!propertiesJson.hasKey("mRelativeLocation")
		|| propertiesJson.at("mRelativeLocation").JSONType() != json::JSON::Class::Array
		|| propertiesJson.at("mRelativeLocation").length() != 3)
	{
		throw std::runtime_error(std::format("{}: mRelativeLocation property requires an array of length 3", GetClass()->Name));
	}

	if (!propertiesJson.hasKey("mRelativeRotation")
		|| propertiesJson.at("mRelativeRotation").JSONType() != json::JSON::Class::Array
		|| propertiesJson.at("mRelativeRotation").length() != 3)
	{
		throw std::runtime_error(std::format("{}: mRelativeRotation property requires an array of length 3", GetClass()->Name));
	}

	if (!propertiesJson.hasKey("mRelativeScale3D")
		|| propertiesJson.at("mRelativeScale3D").JSONType() != json::JSON::Class::Array
		|| propertiesJson.at("mRelativeScale3D").length() != 3)
	{
		throw std::runtime_error(std::format("{}: mRelativeScale3D property requires an array of length 3", GetClass()->Name));
	}

	mRelativeTransform.SetLocation(JsonUtils::FromJson<FVector>(propertiesJson.at("mRelativeLocation")));
	mRelativeTransform.SetRotation(JsonUtils::FromJson<FRotator>(propertiesJson.at("mRelativeRotation")));
	mRelativeTransform.SetScale(JsonUtils::FromJson<FVector>(propertiesJson.at("mRelativeScale3D")));
}

FVector USceneComponent::GetRelativeLocation() const
{
	return mRelativeTransform.GetLocation();
}

void USceneComponent::SetRelativeLocation(FVector location)
{
	mRelativeTransform.SetLocation(location);
	UpdateComponentToWorld();
}

FRotator USceneComponent::GetRelativeRotation() const
{
	return mRelativeTransform.GetRotation();
}

void USceneComponent::SetRelativeRotation(FRotator rotation)
{
	mRelativeTransform.SetRotation(rotation);
	UpdateComponentToWorld();
}

FVector USceneComponent::GetRelativeScale3D() const
{
	return mRelativeTransform.GetScale();
}

void USceneComponent::SetRelativeScale3D(FVector scale)
{
	mRelativeTransform.SetScale(scale);
	UpdateComponentToWorld();
}

const FTransform& USceneComponent::GetTransform() const
{
	return mRelativeTransform;
}

FVector USceneComponent::GetWorldLocation() const
{
	return mWorldTransform.GetLocation();
}

void USceneComponent::SetWorldLocation(FVector location)
{
	mWorldTransform.SetLocation(location);
	UpdateComponentToWorld();
}

FRotator USceneComponent::GetWorldRotation() const
{
	return mWorldTransform.GetRotation();
}

void USceneComponent::SetWorldRotation(FRotator rotation)
{
	mWorldTransform.SetRotation(rotation);
	UpdateComponentToWorld();
}

FVector USceneComponent::GetWorldScale3D() const
{
	return mWorldTransform.GetScale();
}

void USceneComponent::SetWorldScale3D(FVector scale)
{
	mWorldTransform.SetScale(scale);
	UpdateComponentToWorld();
}

const FTransform& USceneComponent::GetWorldTransform() const
{
	return mWorldTransform;
}

// 자식 컴포넌트에서 부모 컴포넌트 포인터 추가하고 부모 컴포넌트 배열에 현재 컴포넌트 추가하기, 이미 부모 컴포넌트가 있으면 정리하기
bool USceneComponent::AttachToComponent(USceneComponent* InParentComp)
{
	if (!InParentComp)
	{
		return false;
	}
	if (InParentComp == this)
	{
		return false;
	}
	if (InParentComp->IsAttachedTo(this))
	{
		return false;
	}

	// 기존 부모에서 자신 제거
	if (mAttachParent)
	{
		// TODO:: TArray에 RemoveSingle 추가하기
		TArray<USceneComponent*> OldParentAttachChildren = mAttachParent->GetAttachChildren();
		int32 idx = OldParentAttachChildren.Find(this);
		if (idx != -1)
		{
			OldParentAttachChildren.RemoveAt(idx, 1);
			mAttachParent = nullptr;
		}
	}
	mAttachParent = InParentComp;
	InParentComp->AddChildComp(this);
}

bool USceneComponent::IsAttachedTo(const USceneComponent* TestComp)
{
	if (!mAttachParent)
	{
		return false;
	}
	if (mAttachParent == TestComp)
	{
		return true;
	}
	return mAttachParent->IsAttachedTo(TestComp);
}

void USceneComponent::AddChildComp(USceneComponent* InSceneComp)
{
	if (!InSceneComp)
		return;
	mAttachChildren.Add(InSceneComp);
}

void USceneComponent::UpdateComponentToWorld()
{
	if (mAttachParent)
	{
		//mWorldTransform.SetScale(mAttachParent->GetWorldScale3D() * mRelativeTransform.GetScale());
		//mWorldTransform.SetRotation(mAttachParent->GetWorldRotation() * mRelativeTransform.GetRotation());
		//mWorldTransform.SetLocation(mAttachParent->GetWorldLocation() * mRelativeTransform.GetLocation());
	}
	else
	{
		mWorldTransform = mRelativeTransform;
	}
	for (USceneComponent* Child : mAttachChildren)
	{
		UpdateComponentToWorld();
	}
	OnTransformChanged();
}
