#pragma once

#include "SceneComponent.h"
#include "RenderInfo.h"

class UExponentialHeightFogComponent : public USceneComponent
{
	REFLECT_CLASS(UExponentialHeightFogComponent, USceneComponent);

public:

	UExponentialHeightFogComponent()
	{
		SetRenderable(true);
	}
	~UExponentialHeightFogComponent() = default;

	void SetFogDensity(float fogdensity) { FogDensity = fogdensity; }
	void SetFogHeightFalloff(float fogheightfalloff) { FogHeightFalloff = fogheightfalloff; }
	void SetStartDistance(float startdistance) { StartDistance = startdistance; }
	void SetFogCutoffDistance(float fogcuttofdistance) { FogCutoffDistance = fogcuttofdistance; }
	void SetFogMaxOpacity(float fogmaxopacity) { FogMaxOpacity = fogmaxopacity; }
	void SetFogInscatteringColor(FVector4 foginscatteringcolor) { FogInscatteringColor = foginscatteringcolor; }

	float GetFogDensity() { return FogDensity; }
	float GetFogHeightFalloff() { return FogHeightFalloff; }
	float GetStartDistance() { return StartDistance; }
	float GetFogCutoffDistance() { return FogCutoffDistance; }
	float GetFogMaxOpacity() { return FogMaxOpacity; }
	FVector4 GetFogInscatteringColor() { return FogInscatteringColor; }

	void Render(FRenderCollector& Collector)
	{
		FExponentialHeightFogInfo Info{};
		Info.FogDensity = FogDensity;
		Info.FogHeightFalloff = FogHeightFalloff;
		Info.StartDistance = StartDistance;
		Info.FogCutoffDistance = FogCutoffDistance;
		Info.FogMaxOpacity = FogMaxOpacity;
		Info.FogInscatteringColor= FogInscatteringColor;
		Info.FogZ = GetWorldLocation().z;
		
		Collector.SetEHFogInfo(Info);
		UActorComponent::Render(Collector);
	}

private:

	float FogDensity = 1.0f;
	float FogHeightFalloff = 1.0f;
	float StartDistance = 0.0f;
	float FogCutoffDistance = 1.0f;
	float FogMaxOpacity = 1.0f;

	FVector4 FogInscatteringColor = {0.0f, 0.0f, 0.0f, 1.0f};

};