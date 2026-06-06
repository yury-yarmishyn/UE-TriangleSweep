#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PhysicsEngine/ConvexElem.h"
#include "TriangleSweepLibrary.generated.h"

/**
 * Library for geometric sweep queries and plane intersections.
 */
UCLASS()
class TRIANGLESWEEP_API UTriangleSweepLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/**
	 * Sweeps a triangle against the world, using a specific collision channel.
	 * @param TraceChannel The collision channel to trace against.
	 * @param bTraceComplex True to test against complex collision (per poly).
	 * @param bIgnoreSelf True to automatically ignore the actor executing this.
	 * @param DrawDebugType Determines if and how the debug representation should be drawn.
	 */
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

	/**
	 * Sweeps a triangle against the world, using a specific collision profile.
	 * @param ProfileName The collision profile name to trace against.
	 * @param bTraceComplex True to test against complex collision (per poly).
	 * @param bIgnoreSelf True to automatically ignore the actor executing this.
	 * @param DrawDebugType Determines if and how the debug representation should be drawn.
	 */
	UFUNCTION(BlueprintCallable, Category = "Triangle Sweep|Collision", meta = (AdvancedDisplay="TraceColor, TraceHitColor, DrawTime", bIgnoreSelf = "true", WorldContext = "WorldContextObject", AutoCreateRefTerm = "ActorsToIgnore, DrawTime, TraceColor, TraceHitColor", DisplayName = "Sweep Triangle By Profile"))
	static bool SweepTriangleByProfile(
		UObject* WorldContextObject,
		const FVector& A, 
		const FVector& B, 
		const FVector& C,
		FName ProfileName,
		bool bTraceComplex,
		const TArray<AActor*>& ActorsToIgnore,
		EDrawDebugTrace::Type DrawDebugType,
		FHitResult& OutHit,
		bool bIgnoreSelf,
		FLinearColor TraceColor = FLinearColor::Red,
		FLinearColor TraceHitColor = FLinearColor::Green,
		float DrawTime = 5.0f);

	/**
	 * Sweeps a triangle against the world, returning overlaps with specified object types.
	 * @param ObjectTypes Array of Object Types to trace for.
	 * @param bTraceComplex True to test against complex collision (per poly).
	 * @param bIgnoreSelf True to automatically ignore the actor executing this.
	 * @param DrawDebugType Determines if and how the debug representation should be drawn.
	 */
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
	
	/**
	 * Sweeps a triangle against the world, using a specific collision channel, and returns all hits.
	 */
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

	/**
	 * Sweeps a triangle against the world, using a specific collision profile, and returns all hits.
	 */
	UFUNCTION(BlueprintCallable, Category = "Triangle Sweep|Collision", meta = (AdvancedDisplay="TraceColor, TraceHitColor, DrawTime", bIgnoreSelf = "true", WorldContext = "WorldContextObject", AutoCreateRefTerm = "ActorsToIgnore, DrawTime, TraceColor, TraceHitColor", DisplayName = "Multi Sweep Triangle By Profile"))
	static bool SweepTriangleMultiByProfile(
		UObject* WorldContextObject,
		const FVector& A, 
		const FVector& B, 
		const FVector& C,
		FName ProfileName,
		bool bTraceComplex,
		const TArray<AActor*>& ActorsToIgnore,
		EDrawDebugTrace::Type DrawDebugType,
		TArray<FHitResult>& OutHits,
		bool bIgnoreSelf,
		FLinearColor TraceColor = FLinearColor::Red,
		FLinearColor TraceHitColor = FLinearColor::Green,
		float DrawTime = 5.0f);

	/**
	 * Sweeps a triangle against the world, returning all overlaps with specified object types.
	 */
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
	// Core math execution path used by all 3 public endpoints
	static bool SweepTriangleCore(
		UWorld* World,
		const FVector& A, 
		const FVector& B, 
		const FVector& C,
		ECollisionChannel TraceChannel,
		FName ProfileName,
		const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes,
		const FCollisionQueryParams& QueryParams,
		uint8 TraceMode, // 0 = Channel, 1 = Profile, 2 = Objects
		FHitResult& OutHit);

	static bool SweepTriangleMultiCore(
		UWorld* World,
		const FVector& A, 
		const FVector& B, 
		const FVector& C,
		ECollisionChannel TraceChannel,
		FName ProfileName,
		const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes,
		const FCollisionQueryParams& QueryParams,
		uint8 TraceMode,
		TArray<FHitResult>& OutHits);
		
	// Debug rendering utility
	static void DrawDebugTriangleSweep(
		UWorld* World, 
		const FVector& A, const FVector& B, const FVector& C, 
		bool bHit, const FHitResult& HitResult, 
		EDrawDebugTrace::Type DrawDebugType, 
		FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime);

	static void DrawDebugTriangleSweepMulti(
		UWorld* World, 
		const FVector& A, const FVector& B, const FVector& C, 
		bool bHit, const TArray<FHitResult>& HitResults, 
		EDrawDebugTrace::Type DrawDebugType, 
		FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime);
	
private:
	/**
	 * Calculates points where a plane intersects a primitive component's collision geometry.
	 * @param HitComp The primitive component to test against the plane.
	 * @param Plane The infinite plane.
	 * @param IntersectionPoints Array to store the found intersection points.
	 */
	static void FindPlaneIntersections(
		UPrimitiveComponent* HitComp, 
		const FPlane& Plane, 
		TArray<FVector, TInlineAllocator<32>>& IntersectionPoints);
	
	/**
	 * Computes the barycentric coordinates of a given point relative to a triangle (A, B, C).
	 * @param Point The 3D point to test (assumes the point already lies on the triangle's plane).
	 * @param A First vertex of the reference triangle.
	 * @param B Second vertex of the reference triangle.
	 * @param C Third vertex of the reference triangle.
	 * @param OutAlpha Weight of vertex A.
	 * @param OutBeta Weight of vertex B.
	 * @param OutGamma Weight of vertex C.
	 * @note If OutAlpha >= 0, OutBeta >= 0, and OutGamma >= 0, the point lies inside the triangle.
	 */
	static void CalculateBarycentric(
		const FVector& Point, 
		const FVector& A, 
		const FVector& B, 
		const FVector& C, 
		float& OutAlpha, 
		float& OutBeta, 
		float& OutGamma
	);
	/**
	 * Calculates the intersection points of a sphere with a plane.
	 * @param Center Center of the sphere in world space.
	 * @param Radius Radius of the sphere.
	 * @param Plane The infinite plane to test against.
	 * @param OutPoints Array to store the found intersection points.
	 */
	static void IntersectSphere(
		const FVector& Center, 
		float Radius, 
		const FPlane& Plane, 
		TArray<FVector, TInlineAllocator<32>>& OutPoints);

	/**
	 * Calculates the intersection points of a capsule with a plane.
	 * @param Center Center of the capsule in world space.
	 * @param HalfHeight The half-height of the capsule (from center to cap).
	 * @param Radius The radius of the capsule.
	 * @param UpVector The normalized up-axis vector of the capsule.
	 * @param Plane The infinite plane to test against.
	 * @param OutPoints Array to store the found intersection points.
	 */
	static void IntersectCapsule(
		const FVector& Center, 
		float HalfHeight, 
		float Radius, 
		const FVector& UpVector, 
		const FPlane& Plane, 
		TArray<FVector, TInlineAllocator<32>>& OutPoints);

	/**
	 * Calculates the intersection points of an oriented bounding box (OBB) with a plane.
	 * @param BoxTransform The world space transform of the box.
	 * @param Extent The half-sizes of the box on each local axis.
	 * @param Plane The infinite plane to test against.
	 * @param OutPoints Array to store the found intersection points.
	 */
	static void IntersectBox(
		const FTransform& BoxTransform, 
		const FVector& Extent, 
		const FPlane& Plane, 
		TArray<FVector, TInlineAllocator<32>>& OutPoints);
	
	/**
	 * Calculates the intersection points of a convex hull with a plane.
	 * @param ConvexTransform The world space transform of the convex mesh.
	 * @param ConvexElem The underlying Chaos physics convex element data.
	 * @param Plane The infinite plane to test against.
	 * @param OutPoints Array to store the found intersection points.
	 */
	static void IntersectConvex(
		const FTransform& ConvexTransform, 
		const FKConvexElem& ConvexElem,
		const FPlane& Plane,
		TArray<FVector, TInlineAllocator<32>>& OutPoints);
};
