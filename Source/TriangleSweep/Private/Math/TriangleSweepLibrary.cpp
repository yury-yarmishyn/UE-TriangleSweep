#include "Math/TriangleSweepLibrary.h"
#include "Engine/OverlapResult.h"
#include "PhysicsEngine/BodySetup.h"

#if UE_BUILD_SHIPPING
#define ENABLE_SWEEP_TIMERS 0
#else
#ifndef ENABLE_SWEEP_TIMERS
#define ENABLE_SWEEP_TIMERS 0
#endif
#endif

bool UTriangleSweepLibrary::SweepTriangleByChannel(
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
	FLinearColor TraceColor, 
	FLinearColor TraceHitColor, 
	float DrawTime)
{
	if (!WorldContextObject) return false;
	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return false;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SweepTriangleBP), bTraceComplex);
	QueryParams.AddIgnoredActors(ActorsToIgnore);
	if (bIgnoreSelf && WorldContextObject->IsA<AActor>())
	{
		QueryParams.AddIgnoredActor(Cast<AActor>(WorldContextObject));
	}

	bool bHit = SweepTriangleCore(World, A, B, C, TraceChannel, NAME_None, TArray<TEnumAsByte<EObjectTypeQuery>>(), QueryParams, 0, OutHit);

	if (DrawDebugType != EDrawDebugTrace::None)
	{
		DrawDebugTriangleSweep(World, A, B, C, bHit, OutHit, DrawDebugType, TraceColor, TraceHitColor, DrawTime);
	}
	return bHit;
}

bool UTriangleSweepLibrary::SweepTriangleByProfile(
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
	FLinearColor TraceColor, 
	FLinearColor TraceHitColor, 
	float DrawTime)
{
	if (!WorldContextObject) return false;
	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return false;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SweepTriangleBP), bTraceComplex);
	QueryParams.AddIgnoredActors(ActorsToIgnore);
	if (bIgnoreSelf && WorldContextObject->IsA<AActor>())
	{
		QueryParams.AddIgnoredActor(Cast<AActor>(WorldContextObject));
	}

	bool bHit = SweepTriangleCore(World, A, B, C, ECC_WorldStatic, ProfileName, TArray<TEnumAsByte<EObjectTypeQuery>>(), QueryParams, 1, OutHit);

	if (DrawDebugType != EDrawDebugTrace::None)
	{
		DrawDebugTriangleSweep(World, A, B, C, bHit, OutHit, DrawDebugType, TraceColor, TraceHitColor, DrawTime);
	}
	return bHit;
}

bool UTriangleSweepLibrary::SweepTriangleForObjects(
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
	FLinearColor TraceColor, 
	FLinearColor TraceHitColor, 
	float DrawTime)
{
	if (!WorldContextObject) return false;
	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return false;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SweepTriangleBP), bTraceComplex);
	QueryParams.AddIgnoredActors(ActorsToIgnore);
	if (bIgnoreSelf && WorldContextObject->IsA<AActor>())
	{
		QueryParams.AddIgnoredActor(Cast<AActor>(WorldContextObject));
	}

	bool bHit = SweepTriangleCore(World, A, B, C, ECC_WorldStatic, NAME_None, ObjectTypes, QueryParams, 2, OutHit);

	if (DrawDebugType != EDrawDebugTrace::None)
	{
		DrawDebugTriangleSweep(World, A, B, C, bHit, OutHit, DrawDebugType, TraceColor, TraceHitColor, DrawTime);
	}
	return bHit;
}

bool UTriangleSweepLibrary::SweepTriangleCore(
	UWorld* World,
	const FVector& A, 
	const FVector& B, 
	const FVector& C,
	ECollisionChannel TraceChannel,
	FName ProfileName,
	const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes,
	const FCollisionQueryParams& QueryParams,
	uint8 TraceMode,
	FHitResult& OutHit)
{
	OutHit = FHitResult();
	OutHit.TraceStart = A;
	OutHit.TraceEnd = (B + C) * 0.5f;
	OutHit.Time = 1.0f;

#if ENABLE_SWEEP_TIMERS
	double T0 = FPlatformTime::Seconds();
#endif
	
	const FVector V0 = B - A;
	const FVector V1 = C - A;
	const float D00 = FVector::DotProduct(V0, V0);
	const float D01 = FVector::DotProduct(V0, V1);
	const float D11 = FVector::DotProduct(V1, V1);
	const float Denom = (D00 * D11) - (D01 * D01);
	const bool bValidBarycentric = FMath::Abs(Denom) >= KINDA_SMALL_NUMBER;
	
	if (!bValidBarycentric)
	{
		return false;
	}
	
	const float InvDenom = 1.0f / Denom;
	const FVector TriangleNormal = FVector::CrossProduct(V0, V1).GetSafeNormal();

#if ENABLE_SWEEP_TIMERS
	double T1 = FPlatformTime::Seconds();
#endif

	FHitResult TraceHit;
	bool bStartHit = false;
	if (TraceMode == 0) bStartHit = World->LineTraceSingleByChannel(TraceHit, B, A, TraceChannel, QueryParams);
	else if (TraceMode == 1) bStartHit = World->LineTraceSingleByProfile(TraceHit, B, A, ProfileName, QueryParams);
	else if (TraceMode == 2) bStartHit = World->LineTraceSingleByObjectType(TraceHit, B, A, FCollisionObjectQueryParams(ObjectTypes), QueryParams);
	
	if (bStartHit)                                                                                                                                                                            
	{                                                                                                                                                                                                
		OutHit = TraceHit;
		OutHit.Time = 0.f;                                                                                                                                                                               
		OutHit.ImpactPoint = TraceHit.bStartPenetrating ? B : TraceHit.ImpactPoint;
		OutHit.Location = OutHit.ImpactPoint;
		OutHit.TraceStart = B;
		OutHit.TraceEnd = A;
		return true;                                                                                                                        
	}  

#if ENABLE_SWEEP_TIMERS
	double T2 = FPlatformTime::Seconds();
#endif

	FBox TriangleAABB(ForceInit);
	TriangleAABB += A;
	TriangleAABB += B;
	TriangleAABB += C;
	TriangleAABB = TriangleAABB.ExpandBy(0.1f);
	
	FCollisionShape BoxShape = FCollisionShape::MakeBox(TriangleAABB.GetExtent());
	FVector BoxCenter = TriangleAABB.GetCenter();
	TArray<FOverlapResult> OutOverlaps;
	
#if ENABLE_SWEEP_TIMERS
	double T3 = FPlatformTime::Seconds();
#endif
	
	bool bHasOverlaps = false;
	if (TraceMode == 0) bHasOverlaps = World->OverlapMultiByChannel(OutOverlaps, BoxCenter, FQuat::Identity, TraceChannel, BoxShape, QueryParams);
	else if (TraceMode == 1) bHasOverlaps = World->OverlapMultiByProfile(OutOverlaps, BoxCenter, FQuat::Identity, ProfileName, BoxShape, QueryParams);
	else if (TraceMode == 2) bHasOverlaps = World->OverlapMultiByObjectType(OutOverlaps, BoxCenter, FQuat::Identity, FCollisionObjectQueryParams(ObjectTypes), BoxShape, QueryParams);
		
	if (!bHasOverlaps)
	{
		return false;
	}
	
#if ENABLE_SWEEP_TIMERS
	double T4 = FPlatformTime::Seconds();
#endif
	
	const FPlane TrianglePlane = FPlane(A, TriangleNormal);
	
	float MinFTOI = 1.f;
	FVector ClosestHitPoint = FVector::ZeroVector;
	UPrimitiveComponent* ClosestComponent = nullptr;

	TArray<FVector, TInlineAllocator<32>> IntersectionPoints;

	for (const FOverlapResult& OutOverlap : OutOverlaps)
	{
		UPrimitiveComponent* HitComp = OutOverlap.GetComponent();
		if (!HitComp) continue;

		const FBoxSphereBounds& Bounds = HitComp->Bounds;
		if (FMath::Abs(TrianglePlane.PlaneDot(Bounds.Origin)) > Bounds.SphereRadius)
		{
			continue;
		}

		IntersectionPoints.Reset();
		FindPlaneIntersections(HitComp, TrianglePlane, IntersectionPoints);

		for (const FVector& Point : IntersectionPoints)
		{
			const FVector V2 = Point - A;
			const float D20 = FVector::DotProduct(V2, V0);
			const float D21 = FVector::DotProduct(V2, V1);

			const float Beta = (D11 * D20 - D01 * D21) * InvDenom;
			const float Gamma = (D00 * D21 - D01 * D20) * InvDenom;
			const float Alpha = 1.0f - Beta - Gamma;
			
			if (Alpha >= 0 && Beta >= 0 && Gamma >= 0)
			{
				if (Alpha + Gamma >= KINDA_SMALL_NUMBER)
				{
					float t = Gamma / (Alpha + Gamma);
					
					if (t >= 0.f && t <= 1.f && t < MinFTOI)
					{
						MinFTOI = t;
						ClosestHitPoint = Point;
						ClosestComponent = HitComp;
					}
				}
			}
		}
	}
	
#if ENABLE_SWEEP_TIMERS
	double T5 = FPlatformTime::Seconds();
#endif
	
#if ENABLE_SWEEP_TIMERS
	UE_LOG(LogTemp, Warning, TEXT("  SWEEP TIMERS -> Precalc: %f ms | Trace: %f ms | AABB: %f ms | Overlap: %f ms | Math: %f ms"),
        (T1 - T0) * 1000.0,
        (T2 - T1) * 1000.0,
        (T3 - T2) * 1000.0,
        (T4 - T3) * 1000.0,
        (T5 - T4) * 1000.0);
#endif

	if (MinFTOI < 1.f && ClosestComponent)
	{
		OutHit.bBlockingHit = true;
		OutHit.Time = MinFTOI;
		OutHit.ImpactPoint = ClosestHitPoint;
		OutHit.Location = ClosestHitPoint;
		OutHit.Normal = TriangleNormal;
		OutHit.ImpactNormal = -TriangleNormal;
		OutHit.Distance = FVector::Distance(A, ClosestHitPoint);
		OutHit.Component = ClosestComponent;
		OutHit.HitObjectHandle = FActorInstanceHandle(ClosestComponent->GetOwner());
		return true;
	}
	
	return false;
}

void UTriangleSweepLibrary::DrawDebugTriangleSweep(
	UWorld* World, const FVector& A, const FVector& B, const FVector& C, 
	bool bHit, const FHitResult& HitResult, 
	EDrawDebugTrace::Type DrawDebugType, 
	FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
	float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

	DrawDebugLine(World, A, B, TraceColor.ToFColor(true), bPersistent, LifeTime, 0, 1.5f);
	DrawDebugLine(World, B, C, TraceColor.ToFColor(true), bPersistent, LifeTime, 0, 1.5f);
	DrawDebugLine(World, C, A, TraceColor.ToFColor(true), bPersistent, LifeTime, 0, 1.5f);

	if (bHit)
	{
		DrawDebugPoint(World, HitResult.ImpactPoint, 10.0f, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
	}
}

void UTriangleSweepLibrary::FindPlaneIntersections(UPrimitiveComponent* HitComp, const FPlane& Plane,
	TArray<FVector, TInlineAllocator<32>>& IntersectionPoints)
{
	if (!HitComp) return;

	FTransform CompTransform = HitComp->GetComponentTransform();
	UBodySetup* BodySetup = HitComp->GetBodySetup();

	if (BodySetup && BodySetup->AggGeom.GetElementCount() > 0)
	{
		// Spheres
		for (const FKSphereElem& SphereElem : BodySetup->AggGeom.SphereElems)
		{
			const FTransform& ElemTransform = SphereElem.GetTransform();
			FTransform WorldTransform = ElemTransform.Equals(FTransform::Identity) ? CompTransform : ElemTransform * CompTransform;
			float ScaledRadius = SphereElem.Radius * WorldTransform.GetScale3D().GetMax();
			
			IntersectSphere(WorldTransform.GetLocation(), ScaledRadius, Plane, IntersectionPoints);
		}
		
		// Boxes
		for (const FKBoxElem& BoxElem : BodySetup->AggGeom.BoxElems)
		{
			const FTransform& ElemTransform = BoxElem.GetTransform();
			FTransform WorldTransform = ElemTransform.Equals(FTransform::Identity) ? CompTransform : ElemTransform * CompTransform;
			FVector Extent = FVector(BoxElem.X, BoxElem.Y, BoxElem.Z) * 0.5f * WorldTransform.GetScale3D();
            
			IntersectBox(WorldTransform, Extent, Plane, IntersectionPoints);
		}
		
		// Capsules
		for (const FKSphylElem& SphylElem : BodySetup->AggGeom.SphylElems)
		{
			const FTransform& ElemTransform = SphylElem.GetTransform();
			FTransform WorldTransform = ElemTransform.Equals(FTransform::Identity) ? CompTransform : ElemTransform * CompTransform;
			float ScaledRadius = SphylElem.Radius * WorldTransform.GetScale3D().GetMax();
			float ScaledHalfHeight = (SphylElem.Length * 0.5f + SphylElem.Radius) * WorldTransform.GetScale3D().Z; 
            
			IntersectCapsule(WorldTransform.GetLocation(), ScaledHalfHeight, ScaledRadius, WorldTransform.GetUnitAxis(EAxis::Z), Plane, IntersectionPoints);
		}
		
		// Convex Hulls
		for (const FKConvexElem& ConvexElem : BodySetup->AggGeom.ConvexElems)
		{
			const FTransform& ElemTransform = ConvexElem.GetTransform();
			FTransform WorldTransform = ElemTransform.Equals(FTransform::Identity) ? CompTransform : ElemTransform * CompTransform;
          
			IntersectConvex(WorldTransform, ConvexElem, Plane, IntersectionPoints);
		}
	}
	else
	{
		FCollisionShape Shape = HitComp->GetCollisionShape();

		switch (Shape.ShapeType)
		{
		case ECollisionShape::Sphere:
			{
				IntersectSphere(CompTransform.GetLocation(), Shape.GetSphereRadius(), Plane, IntersectionPoints);
				break;
			}
		case ECollisionShape::Capsule:
			{
				IntersectCapsule(CompTransform.GetLocation(), Shape.GetCapsuleHalfHeight(), Shape.GetCapsuleRadius(), CompTransform.GetUnitAxis(EAxis::Z), Plane, IntersectionPoints);
				break;
			}
		case ECollisionShape::Box:
			{
				IntersectBox(CompTransform, Shape.GetBox(), Plane, IntersectionPoints);
				break;
			}
		default:
			break;
		}
	}
}

void UTriangleSweepLibrary::CalculateBarycentric(const FVector& Point, const FVector& A, const FVector& B, const FVector& C,
	float& OutAlpha, float& OutBeta, float& OutGamma)
{
	const FVector V0 = B - A;
	const FVector V1 = C - A;
	const FVector V2 = Point - A;

	const float D00 = FVector::DotProduct(V0, V0);
	const float D01 = FVector::DotProduct(V0, V1);
	const float D11 = FVector::DotProduct(V1, V1);
	const float D20 = FVector::DotProduct(V2, V0);
	const float D21 = FVector::DotProduct(V2, V1);

	const float Denom = (D00 * D11) - (D01 * D01);

	if (FMath::Abs(Denom) < KINDA_SMALL_NUMBER)
	{
		OutAlpha = -1.0f;
		OutBeta  = -1.0f;
		OutGamma = -1.0f;
		return;
	}

	const float InvDenom = 1.0f / Denom;
	OutBeta = (D11 * D20 - D01 * D21) * InvDenom;
	OutGamma = (D00 * D21 - D01 * D20) * InvDenom;
	OutAlpha = 1.0f - OutBeta - OutGamma;
}

void UTriangleSweepLibrary::IntersectSphere(const FVector& Center, float Radius, const FPlane& Plane, TArray<FVector, TInlineAllocator<32>>& OutPoints)
{
	const float DistanceToPlane = Plane.PlaneDot(Center);
	
	if (FMath::Abs(DistanceToPlane) <= Radius)
	{
		FVector IntersectionPoint = Center - Plane.GetNormal() * DistanceToPlane;
		OutPoints.Add(IntersectionPoint);
	}
}

void UTriangleSweepLibrary::IntersectCapsule(const FVector& Center, float HalfHeight, float Radius, const FVector& UpVector, const FPlane& Plane, TArray<FVector, TInlineAllocator<32>>& OutPoints)
{
    const float SegmentHalfLength = FMath::Max(0.0f, HalfHeight - Radius);
    const FVector TopPoint = Center + UpVector * SegmentHalfLength;
    const FVector BottomPoint = Center - UpVector * SegmentHalfLength;

    const float DistTop = Plane.PlaneDot(TopPoint);
    const float DistBottom = Plane.PlaneDot(BottomPoint);
    
    if (DistTop * DistBottom < 0.0f)
    {
        const float t = DistTop / (DistTop - DistBottom);
        OutPoints.Add(TopPoint + t * (BottomPoint - TopPoint));
    }
    else
    {
        if (FMath::Abs(DistTop) <= Radius)
        {
            OutPoints.Add(TopPoint - Plane.GetNormal() * DistTop);
        }
        if (FMath::Abs(DistBottom) <= Radius)
        {
            OutPoints.Add(BottomPoint - Plane.GetNormal() * DistBottom);
        }
    }
}

void UTriangleSweepLibrary::IntersectBox(const FTransform& BoxTransform, const FVector& Extent, const FPlane& Plane, TArray<FVector, TInlineAllocator<32>>& OutPoints)
{
    FVector Normal = Plane.GetNormal();
    float ProjectedRadius = 
        Extent.X * FMath::Abs(FVector::DotProduct(Normal, BoxTransform.GetUnitAxis(EAxis::X))) +
        Extent.Y * FMath::Abs(FVector::DotProduct(Normal, BoxTransform.GetUnitAxis(EAxis::Y))) +
        Extent.Z * FMath::Abs(FVector::DotProduct(Normal, BoxTransform.GetUnitAxis(EAxis::Z)));

    float DistToCenter = Plane.PlaneDot(BoxTransform.GetLocation());

    if (FMath::Abs(DistToCenter) > ProjectedRadius)
    {
        return;
    }

    FBox LocalBox(-Extent, Extent);
    FVector LocalVertices[8];
    LocalBox.GetVertices(LocalVertices);

    FVector WorldVertices[8];
    float Distances[8];

    for (int32 i = 0; i < 8; ++i)
    {
        WorldVertices[i] = BoxTransform.TransformPosition(LocalVertices[i]);
        Distances[i] = Plane.PlaneDot(WorldVertices[i]);
    }

    static const int32 Edges[12][2] = {
        {0,1}, {1,2}, {2,3}, {3,0}, 
        {4,5}, {5,6}, {6,7}, {7,4}, 
        {0,4}, {1,5}, {2,6}, {3,7}  
    };

    for (int32 i = 0; i < 12; ++i)
    {
        int32 IndexA = Edges[i][0];
        int32 IndexB = Edges[i][1];

        float DistA = Distances[IndexA];
        float DistB = Distances[IndexB];

        if (DistA * DistB < 0.0f)
        {
            float t = DistA / (DistA - DistB);
            OutPoints.Add(WorldVertices[IndexA] + t * (WorldVertices[IndexB] - WorldVertices[IndexA]));
        }
    }
}

void UTriangleSweepLibrary::IntersectConvex(const FTransform& ConvexTransform, const FKConvexElem& ConvexElem, const FPlane& Plane, TArray<FVector, TInlineAllocator<32>>& OutPoints)
{
    const TArray<FVector>& LocalVertices = ConvexElem.VertexData;
    const TArray<int32>& Indices = ConvexElem.IndexData;
    
    const int32 VertexCount = LocalVertices.Num();
    const int32 IndexCount = Indices.Num();

    if (VertexCount == 0 || IndexCount < 3)
    {
        return;
    }

    TArray<FVector, TInlineAllocator<64>> WorldVertices;
    TArray<float, TInlineAllocator<64>> Distances;
    WorldVertices.SetNumUninitialized(VertexCount);
    Distances.SetNumUninitialized(VertexCount);

    for (int32 i = 0; i < VertexCount; ++i)
    {
        WorldVertices[i] = ConvexTransform.TransformPosition(FVector(LocalVertices[i]));
        Distances[i] = Plane.PlaneDot(WorldVertices[i]);
    }

    const int32 TriangleCount = IndexCount / 3;

    for (int32 i = 0; i < TriangleCount; ++i)
    {
        const int32 i0 = Indices[i * 3 + 0];
        const int32 i1 = Indices[i * 3 + 1];
        const int32 i2 = Indices[i * 3 + 2];

        if (Distances[i0] * Distances[i1] < 0.0f)
        {
            const float t = Distances[i0] / (Distances[i0] - Distances[i1]);
            OutPoints.Add(WorldVertices[i0] + t * (WorldVertices[i1] - WorldVertices[i0]));
        }

        if (Distances[i1] * Distances[i2] < 0.0f)
        {
            const float t = Distances[i1] / (Distances[i1] - Distances[i2]);
            OutPoints.Add(WorldVertices[i1] + t * (WorldVertices[i2] - WorldVertices[i1]));
        }

        if (Distances[i2] * Distances[i0] < 0.0f)
        {
            const float t = Distances[i2] / (Distances[i2] - Distances[i0]);
            OutPoints.Add(WorldVertices[i2] + t * (WorldVertices[i0] - WorldVertices[i2]));
        }
    }
}

