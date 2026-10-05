#include "Level.h"
#include "Actor.h"

ULevel::~ULevel()
{
	TArray<AActor*> ActorsToDestroy = mActors;

	for (AActor* actor : ActorsToDestroy)
	{
		if (actor)
		{
			FObjectFactory::DestroyObject(actor);
		}
	}
	mActors.Empty();
}

void ULevel::PostInitProperties()
{
	UObject::PostInitProperties();
}

void ULevel::SerializeClass(json::JSON& outJson) const
{

}

void ULevel::DeserializeClass(const json::JSON& inJson)
{

}

void ULevel::AddActor(AActor* actor)
{
	mActors.Add(actor);
	actor->SetLevel(this);
}

bool ULevel::RemoveActor(uint32 uuid)
{
	for (int index = mActors.Num() - 1; index >= 0; index--)
	{
		if (mActors[index]->UUID == uuid)
		{
			mActors.RemoveAtSwap(index);
			return true;
		}
	}
	return false;
}

bool ULevel::FindActor(uint32 uuid, AActor*& outActor) const
{
	for (AActor* actor : mActors)
	{
		if (actor->UUID == uuid)
		{
			outActor = actor;
			return true;
		}
	}
	return false;
}