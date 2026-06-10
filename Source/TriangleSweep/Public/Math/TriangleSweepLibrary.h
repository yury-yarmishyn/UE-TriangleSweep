#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PhysicsEngine/ConvexElem.h"
#include "TriangleSweepLibrary.generated.h"

UCLASS()
class TRIANGLESWEEP_API UTriangleSweepLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Triangle Sweep|Collision", meta = (AdvancedDisplay="TraceColor, TraceHitColor, DrawTime", bIgnoreSelf = "true", WorldContext = "WorldContextObject", AutoCreateRefTerm = "ActorsToIgnore, DrawTime, TraceColor, TraceHitColor", DisplayName = "Sweep Triangle By Channel"))
	static bool SweepTriangleByChannel(
		UObject* WorldContextObject,
		const FVector& A, 
		const FVector& B, 
		const FVector& C,
		TEnumAsByte<ECollisionChannel> TraceChannel,
		bool bTraceComplex,
		const TArray<AActor*>& ActorsToIgnore,
		EDrawDebugTrace::Type DrawDebugType,
		FHitResult& OutHit,
		bool bIgnoreSelf,
		FLinearColor TraceColor = FLinearColor::Red,
		FLinearColor TraceHitColor = FLinearColor::Green,
		float DrawTime = 5.0f);

	UFUNCTION(BlueprintCallable, Category = "Triangle Sweep|Collision", meta = (AdvancedDisplay="TraceColor, TraceHitColor, DrawTime", bIgnoreSelf = "true", WorldContext = "WorldContextObject", AutoCreateRefTerm = "ActorsToIgnore, DrawTime, TraceColor, TraceHitColor", DisplayName = "Sweep Triangle By Profile"))
	static bool SweepTriangleByProfile(
		UObject* WorldContextObject,
		const FVector& A, 
		const FVector& B, 
		const FVector& C,
		UPARAM(Meta=(GetOptions="Engine.KismetSystemLibrary.GetCollisionProfileNames")) FName ProfileName,
		bool bTraceComplex,
		const TArray<AActor*>& ActorsToIgnore,
		EDrawDebugTrace::Type DrawDebugType,
		FHitResult& OutHit,
		bool bIgnoreSelf,
		FLinearColor TraceColor = FLinearColor::Red,
		FLinearColor TraceHitColor = FLinearColor::Green,
		float DrawTime = 5.0f);

	UFUNCTION(BlueprintCallable, Category = "Triangle Sweep|Collision", meta = (AdvancedDisplay="TraceColor, TraceHitColor, DrawTime", bIgnoreSelf = "true", WorldContext = "WorldContextObject", AutoCreateRefTerm = "ActorsToIgnore, DrawTime, TraceColor, TraceHitColor", DisplayName = "Sweep Triangle For Objects"))
	static bool SweepTriangleForObjects(
		UObject* WorldContextObject,
		const FVector& A, 
		const FVector& B, 
		const FVector& C,
		const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes,
		bool bTraceComplex,
		const TArray<AActor*>& ActorsToIgnore,
		EDrawDebugTrace::Type DrawDebugType,
		FHitResult& OutHit,
		bool bIgnoreSelf,
		FLinearColor TraceColor = FLinearColor::Red,
		FLinearColor TraceHitColor = FLinearColor::Green,
		float DrawTime = 5.0f);
	
	UFUNCTION(BlueprintCallable, Category = "Triangle Sweep|Collision", meta = (AdvancedDisplay="TraceColor, TraceHitColor, DrawTime", bIgnoreSelf = "true", WorldContext = "WorldContextObject", AutoCreateRefTerm = "ActorsToIgnore, DrawTime, TraceColor, TraceHitColor", DisplayName = "Multi Sweep Triangle By Channel"))
	static bool SweepTriangleMultiByChannel(
		UObject* WorldContextObject,
		const FVector& A, 
		const FVector& B, 
		const FVector& C,
		TEnumAsByte<ECollisionChannel> TraceChannel,
		bool bTraceComplex,
		const TArray<AActor*>& ActorsToIgnore,
		EDrawDebugTrace::Type DrawDebugType,
		TArray<FHitResult>& OutHits,
		bool bIgnoreSelf,
		FLinearColor TraceColor = FLinearColor::Red,
		FLinearColor TraceHitColor = FLinearColor::Green,
		float DrawTime = 5.0f);

	UFUNCTION(BlueprintCallable, Category = "Triangle Sweep|Collision", meta = (AdvancedDisplay="TraceColor, TraceHitColor, DrawTime", bIgnoreSelf = "true", WorldContext = "WorldContextObject", AutoCreateRefTerm = "ActorsToIgnore, DrawTime, TraceColor, TraceHitColor", DisplayName = "Multi Sweep Triangle By Profile"))
	static bool SweepTriangleMultiByProfile(
		UObject* WorldContextObject,
		const FVector& A, 
		const FVector& B, 
		const FVector& C,
		UPARAM(Meta=(GetOptions="Engine.KismetSystemLibrary.GetCollisionProfileNames")) FName ProfileName,
		bool bTraceComplex,
		const TArray<AActor*>& ActorsToIgnore,
		EDrawDebugTrace::Type DrawDebugType,
		TArray<FHitResult>& OutHits,
		bool bIgnoreSelf,
		FLinearColor TraceColor = FLinearColor::Red,
		FLinearColor TraceHitColor = FLinearColor::Green,
		float DrawTime = 5.0f);

	UFUNCTION(BlueprintCallable, Category = "Triangle Sweep|Collision", meta = (AdvancedDisplay="TraceColor, TraceHitColor, DrawTime", bIgnoreSelf = "true", WorldContext = "WorldContextObject", AutoCreateRefTerm = "ActorsToIgnore, DrawTime, TraceColor, TraceHitColor", DisplayName = "Multi Sweep Triangle For Objects"))
	static bool SweepTriangleMultiForObjects(
		UObject* WorldContextObject,
		const FVector& A, 
		const FVector& B, 
		const FVector& C,
		const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes,
		bool bTraceComplex,
		const TArray<AActor*>& ActorsToIgnore,
		EDrawDebugTrace::Type DrawDebugType,
		TArray<FHitResult>& OutHits,
		bool bIgnoreSelf,
		FLinearColor TraceColor = FLinearColor::Red,
		FLinearColor TraceHitColor = FLinearColor::Green,
		float DrawTime = 5.0f);

private:
	static bool SweepTriangleGenericCore(
		UWorld* World,
		const FVector& A, 
		const FVector& B, 
		const FVector& C,
		ECollisionChannel TraceChannel,
		FName ProfileName,
		const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes,
		const FCollisionQueryParams& QueryParams,
		uint8 TraceMode,
		bool bIsMulti,
		FHitResult& OutSingleHit,
		TArray<FHitResult>& OutMultiHits);

	static bool InitSweepParams(
		UObject* WorldContextObject, 
		bool bTraceComplex, 
		const TArray<AActor*>& ActorsToIgnore, 
		bool bIgnoreSelf, 
		UWorld*& OutWorld, 
		FCollisionQueryParams& OutQueryParams);
	static void DrawDebugTriangleSweep(
		UWorld* World, 
		const FVector& A, 
		const FVector& B, 
		const FVector& C, 
		bool bHit, 
		const FHitResult& HitResult, 
		EDrawDebugTrace::Type DrawDebugType, 
		FLinearColor TraceColor, 
		FLinearColor TraceHitColor, 
		float DrawTime);

	static void DrawDebugTriangleSweepMulti(
		UWorld* World, 
		const FVector& A, 
		const FVector& B, 
		const FVector& C, 
		bool bHit, 
		const TArray<FHitResult>& HitResults, 
		EDrawDebugTrace::Type DrawDebugType, 
		FLinearColor TraceColor, 
		FLinearColor TraceHitColor, 
		float DrawTime);

	static void TestComponentAgainstTriangle(
		UPrimitiveComponent* HitComp, 
		const FVector& A, const FVector& B, const FVector& C, 
		const FPlane& TrianglePlane,
		TArray<FVector, TInlineAllocator<32>>& OutPoints);

	static bool TestBoxTriangleSAT(
		const FTransform& BoxTransform, 
		const FVector& Extent, 
		const FVector& A, const FVector& B, const FVector& C);

	static void IntersectCapsule(
		const FVector& Center, float HalfHeight, float Radius, const FVector& UpVector, 
		const FPlane& Plane, TArray<FVector, TInlineAllocator<32>>& OutPoints);

	static void IntersectConvex(
		const FTransform& ConvexTransform, const FKConvexElem& ConvexElem, 
		const FVector& A, const FVector& B, const FVector& C, 
		TArray<FVector, TInlineAllocator<32>>& OutPoints);

};
