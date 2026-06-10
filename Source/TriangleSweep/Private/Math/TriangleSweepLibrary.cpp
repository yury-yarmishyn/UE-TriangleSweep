#include "Math/TriangleSweepLibrary.h"
#include "Engine/OverlapResult.h"
#include "PhysicsEngine/BodySetup.h"

#if UE_BUILD_SHIPPING
#define ENABLE_SWEEP_TIMERS 0
#else
#ifndef ENABLE_SWEEP_TIMERS
#define ENABLE_SWEEP_TIMERS 1
#endif
#endif

FORCEINLINE static bool TestBarycentric(
	const FVector& Point, 
	const FVector& A, 
	const FVector& V0, 
	const FVector& V1, 
	float D00, 
	float D11, 
	float D01, 
	float InvDenom, 
	float& OutT)
{
	const FVector V2 = Point - A;
	const float D20 = FVector::DotProduct(V2, V0);
	const float D21 = FVector::DotProduct(V2, V1);

	const float Beta = (D11 * D20 - D01 * D21) * InvDenom;
	const float Gamma = (D00 * D21 - D01 * D20) * InvDenom;
	const float Alpha = 1.0f - Beta - Gamma;
			
	if (Alpha >= 0 && Beta >= 0 && Gamma >= 0 && (Alpha + Gamma >= KINDA_SMALL_NUMBER))
	{
		OutT = Gamma / (Alpha + Gamma);
		return OutT >= 0.f && OutT <= 1.f;
	}
	
	return false;
}

bool UTriangleSweepLibrary::InitSweepParams(
	UObject* WorldContextObject, 
	bool bTraceComplex, 
	const TArray<AActor*>& ActorsToIgnore, 
	bool bIgnoreSelf, 
	UWorld*& OutWorld, 
	FCollisionQueryParams& OutQueryParams)
{
	if (!WorldContextObject)
	{
		return false;
	}
	
	OutWorld = WorldContextObject->GetWorld();
	if (!OutWorld)
	{
		return false;
	}

	OutQueryParams = FCollisionQueryParams(SCENE_QUERY_STAT(SweepTriangleBP), bTraceComplex);
	OutQueryParams.AddIgnoredActors(ActorsToIgnore);
	
	if (bIgnoreSelf && WorldContextObject->IsA<AActor>())
	{
		OutQueryParams.AddIgnoredActor(Cast<AActor>(WorldContextObject));
	}
	
	return true;
}

bool UTriangleSweepLibrary::SweepTriangleByChannel(
	UObject* WorldContextObject, 
	const FVector& A, const FVector& B, const FVector& C, 
	TEnumAsByte<ECollisionChannel> TraceChannel, 
	bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, 
	EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf, 
	FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	UWorld* World = nullptr;
	FCollisionQueryParams QueryParams;
	if (!InitSweepParams(WorldContextObject, bTraceComplex, ActorsToIgnore, bIgnoreSelf, World, QueryParams))
	{
		return false;
	}

	TArray<FHitResult> DummyHits;
	bool bHit = SweepTriangleGenericCore(World, A, B, C, TraceChannel, NAME_None, TArray<TEnumAsByte<EObjectTypeQuery>>(), QueryParams, 0, false, OutHit, DummyHits);

	if (DrawDebugType != EDrawDebugTrace::None)
	{
		DrawDebugTriangleSweep(World, A, B, C, bHit, OutHit, DrawDebugType, TraceColor, TraceHitColor, DrawTime);
	}
	
	return bHit;
}

bool UTriangleSweepLibrary::SweepTriangleByProfile(
	UObject* WorldContextObject, 
	const FVector& A, const FVector& B, const FVector& C, 
	FName ProfileName, 
	bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, 
	EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf, 
	FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	UWorld* World = nullptr;
	FCollisionQueryParams QueryParams;
	if (!InitSweepParams(WorldContextObject, bTraceComplex, ActorsToIgnore, bIgnoreSelf, World, QueryParams))
	{
		return false;
	}

	TArray<FHitResult> DummyHits;
	bool bHit = SweepTriangleGenericCore(World, A, B, C, ECC_WorldStatic, ProfileName, TArray<TEnumAsByte<EObjectTypeQuery>>(), QueryParams, 1, false, OutHit, DummyHits);

	if (DrawDebugType != EDrawDebugTrace::None)
	{
		DrawDebugTriangleSweep(World, A, B, C, bHit, OutHit, DrawDebugType, TraceColor, TraceHitColor, DrawTime);
	}
	
	return bHit;
}

bool UTriangleSweepLibrary::SweepTriangleForObjects(
	UObject* WorldContextObject, 
	const FVector& A, const FVector& B, const FVector& C, 
	const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes, 
	bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, 
	EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf, 
	FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	UWorld* World = nullptr;
	FCollisionQueryParams QueryParams;
	if (!InitSweepParams(WorldContextObject, bTraceComplex, ActorsToIgnore, bIgnoreSelf, World, QueryParams))
	{
		return false;
	}

	TArray<FHitResult> DummyHits;
	bool bHit = SweepTriangleGenericCore(World, A, B, C, ECC_WorldStatic, NAME_None, ObjectTypes, QueryParams, 2, false, OutHit, DummyHits);

	if (DrawDebugType != EDrawDebugTrace::None)
	{
		DrawDebugTriangleSweep(World, A, B, C, bHit, OutHit, DrawDebugType, TraceColor, TraceHitColor, DrawTime);
	}
	
	return bHit;
}

bool UTriangleSweepLibrary::SweepTriangleMultiByChannel(
	UObject* WorldContextObject, 
	const FVector& A, const FVector& B, const FVector& C, 
	TEnumAsByte<ECollisionChannel> TraceChannel, 
	bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, 
	EDrawDebugTrace::Type DrawDebugType, TArray<FHitResult>& OutHits, bool bIgnoreSelf, 
	FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	UWorld* World = nullptr;
	FCollisionQueryParams QueryParams;
	if (!InitSweepParams(WorldContextObject, bTraceComplex, ActorsToIgnore, bIgnoreSelf, World, QueryParams))
	{
		return false;
	}

	FHitResult DummyHit;
	bool bHit = SweepTriangleGenericCore(World, A, B, C, TraceChannel, NAME_None, TArray<TEnumAsByte<EObjectTypeQuery>>(), QueryParams, 0, true, DummyHit, OutHits);

	if (DrawDebugType != EDrawDebugTrace::None)
	{
		DrawDebugTriangleSweepMulti(World, A, B, C, bHit, OutHits, DrawDebugType, TraceColor, TraceHitColor, DrawTime);
	}
	
	return bHit;
}

bool UTriangleSweepLibrary::SweepTriangleMultiByProfile(
	UObject* WorldContextObject, 
	const FVector& A, const FVector& B, const FVector& C, 
	FName ProfileName, 
	bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, 
	EDrawDebugTrace::Type DrawDebugType, TArray<FHitResult>& OutHits, bool bIgnoreSelf, 
	FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	UWorld* World = nullptr;
	FCollisionQueryParams QueryParams;
	if (!InitSweepParams(WorldContextObject, bTraceComplex, ActorsToIgnore, bIgnoreSelf, World, QueryParams))
	{
		return false;
	}

	FHitResult DummyHit;
	bool bHit = SweepTriangleGenericCore(World, A, B, C, ECC_WorldStatic, ProfileName, TArray<TEnumAsByte<EObjectTypeQuery>>(), QueryParams, 1, true, DummyHit, OutHits);

	if (DrawDebugType != EDrawDebugTrace::None)
	{
		DrawDebugTriangleSweepMulti(World, A, B, C, bHit, OutHits, DrawDebugType, TraceColor, TraceHitColor, DrawTime);
	}
	
	return bHit;
}

bool UTriangleSweepLibrary::SweepTriangleMultiForObjects(
	UObject* WorldContextObject, 
	const FVector& A, const FVector& B, const FVector& C, 
	const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes, 
	bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, 
	EDrawDebugTrace::Type DrawDebugType, TArray<FHitResult>& OutHits, bool bIgnoreSelf, 
	FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	UWorld* World = nullptr;
	FCollisionQueryParams QueryParams;
	if (!InitSweepParams(WorldContextObject, bTraceComplex, ActorsToIgnore, bIgnoreSelf, World, QueryParams))
	{
		return false;
	}

	FHitResult DummyHit;
	bool bHit = SweepTriangleGenericCore(World, A, B, C, ECC_WorldStatic, NAME_None, ObjectTypes, QueryParams, 2, true, DummyHit, OutHits);

	if (DrawDebugType != EDrawDebugTrace::None)
	{
		DrawDebugTriangleSweepMulti(World, A, B, C, bHit, OutHits, DrawDebugType, TraceColor, TraceHitColor, DrawTime);
	}
	
	return bHit;
}

bool UTriangleSweepLibrary::SweepTriangleGenericCore(
	UWorld* World,
	const FVector& A, const FVector& B, const FVector& C,
	ECollisionChannel TraceChannel, FName ProfileName, const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes,
	const FCollisionQueryParams& QueryParams, uint8 TraceMode, bool bIsMulti, 
	FHitResult& OutSingleHit, TArray<FHitResult>& OutMultiHits)
{
	if (bIsMulti) 
	{
		OutMultiHits.Empty();
	}
	else 
	{
		OutSingleHit = FHitResult();
		OutSingleHit.TraceStart = A;
		OutSingleHit.TraceEnd = (B + C) * 0.5f;
		OutSingleHit.Time = 1.0f;
	}

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
	
	if (TraceMode == 0) 
	{
		bStartHit = World->LineTraceSingleByChannel(TraceHit, B, A, TraceChannel, QueryParams);
	}
	else if (TraceMode == 1) 
	{
		bStartHit = World->LineTraceSingleByProfile(TraceHit, B, A, ProfileName, QueryParams);
	}
	else if (TraceMode == 2) 
	{
		bStartHit = World->LineTraceSingleByObjectType(TraceHit, B, A, FCollisionObjectQueryParams(ObjectTypes), QueryParams);
	}
	
	if (bStartHit)
	{
		FHitResult StartHit = TraceHit;
		StartHit.Time = 0.f;
		StartHit.ImpactPoint = TraceHit.bStartPenetrating ? B : TraceHit.ImpactPoint;
		StartHit.Location = StartHit.ImpactPoint;
		StartHit.TraceStart = B;
		StartHit.TraceEnd = A;

		if (bIsMulti) OutMultiHits.Add(StartHit);
		else 
		{
			OutSingleHit = StartHit;
			return true;
		}
	}

#if ENABLE_SWEEP_TIMERS
	double T2 = FPlatformTime::Seconds();
#endif

	FVector MinBox = A.ComponentMin(B).ComponentMin(C);
	FVector MaxBox = A.ComponentMax(B).ComponentMax(C);
	FBox TriangleAABB(MinBox - 0.1f, MaxBox + 0.1f);
	
	FCollisionShape BoxShape = FCollisionShape::MakeBox(TriangleAABB.GetExtent());
	FVector BoxCenter = TriangleAABB.GetCenter();
	TArray<FOverlapResult> OutOverlaps;
	
#if ENABLE_SWEEP_TIMERS
	double T3 = FPlatformTime::Seconds();
#endif
	
	bool bHasOverlaps = false;
	
	if (TraceMode == 0) 
	{
		bHasOverlaps = World->OverlapMultiByChannel(OutOverlaps, BoxCenter, FQuat::Identity, TraceChannel, BoxShape, QueryParams);
	}
	else if (TraceMode == 1) 
	{
		bHasOverlaps = World->OverlapMultiByProfile(OutOverlaps, BoxCenter, FQuat::Identity, ProfileName, BoxShape, QueryParams);
	}
	else if (TraceMode == 2) 
	{
		bHasOverlaps = World->OverlapMultiByObjectType(OutOverlaps, BoxCenter, FQuat::Identity, FCollisionObjectQueryParams(ObjectTypes), BoxShape, QueryParams);
	}
		
	if (!bHasOverlaps && (bIsMulti ? OutMultiHits.Num() == 0 : true))
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
		if (!HitComp)
		{
			continue;
		}

		const FBoxSphereBounds& Bounds = HitComp->Bounds;

		FVector ClosestToCenter = FMath::ClosestPointOnTriangleToPoint(Bounds.Origin, A, B, C);
		if (FVector::DistSquared(ClosestToCenter, Bounds.Origin) > FMath::Square(Bounds.SphereRadius))
		{
			continue;
		}

		IntersectionPoints.Reset();
		TestComponentAgainstTriangle(HitComp, A, B, C, TrianglePlane, IntersectionPoints);

		for (const FVector& Point : IntersectionPoints)
		{
			float t = 0.f;
			if (TestBarycentric(Point, A, V0, V1, D00, D11, D01, InvDenom, t))
			{
				if (bIsMulti)
				{
					FHitResult Hit;
					Hit.bBlockingHit = true;
					Hit.Time = t;
					Hit.ImpactPoint = Point;
					Hit.Location = Point;
					Hit.Normal = TriangleNormal;
					Hit.ImpactNormal = -TriangleNormal;
					Hit.Distance = FVector::Distance(A, Point);
					Hit.Component = HitComp;
					Hit.HitObjectHandle = FActorInstanceHandle(HitComp->GetOwner());
					Hit.TraceStart = A;
					Hit.TraceEnd = (B + C) * 0.5f;
					
					OutMultiHits.Add(Hit);
				}
				else
				{
					if (t < MinFTOI)
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
	if (!bIsMulti) 
	{
		UE_LOG(LogTemp, Warning, 
			TEXT("  SWEEP TIMERS -> Precalc: %f ms | Trace: %f ms | AABB: %f ms | Overlap: %f ms | Math: %f ms"),
			(T1 - T0) * 1000.0, (T2 - T1) * 1000.0, (T3 - T2) * 1000.0, (T4 - T3) * 1000.0, (T5 - T4) * 1000.0);
	}
#endif

	if (bIsMulti)
	{
		if (OutMultiHits.Num() > 0)
		{
			OutMultiHits.Sort([](const FHitResult& HitA, const FHitResult& HitB) { return HitA.Time < HitB.Time; });
			return true;
		}
	}
	else
	{
		if (MinFTOI < 1.f && ClosestComponent)
		{
			OutSingleHit.bBlockingHit = true;
			OutSingleHit.Time = MinFTOI;
			OutSingleHit.ImpactPoint = ClosestHitPoint;
			OutSingleHit.Location = ClosestHitPoint;
			OutSingleHit.Normal = TriangleNormal;
			OutSingleHit.ImpactNormal = -TriangleNormal;
			OutSingleHit.Distance = FVector::Distance(A, ClosestHitPoint);
			OutSingleHit.Component = ClosestComponent;
			OutSingleHit.HitObjectHandle = FActorInstanceHandle(ClosestComponent->GetOwner());
			return true;
		}
	}
	
	return false;
}

void UTriangleSweepLibrary::TestComponentAgainstTriangle(
	UPrimitiveComponent* HitComp, 
	const FVector& A, const FVector& B, const FVector& C, 
	const FPlane& TrianglePlane,
	TArray<FVector, TInlineAllocator<32>>& OutPoints)
{
	FTransform CompTransform = HitComp->GetComponentTransform();
	UBodySetup* BodySetup = HitComp->GetBodySetup();

	if (BodySetup && BodySetup->AggGeom.GetElementCount() > 0)
	{
		for (const FKSphereElem& SphereElem : BodySetup->AggGeom.SphereElems)
		{
			FTransform WorldTransform = SphereElem.GetTransform() * CompTransform;
			float ScaledRadius = SphereElem.Radius * WorldTransform.GetScale3D().GetMax();
			
			FVector Closest = FMath::ClosestPointOnTriangleToPoint(WorldTransform.GetLocation(), A, B, C);
			if (FVector::DistSquared(Closest, WorldTransform.GetLocation()) <= FMath::Square(ScaledRadius))
			{
				OutPoints.Add(Closest);
			}
		}

		for (const FKBoxElem& BoxElem : BodySetup->AggGeom.BoxElems)
		{
			FTransform WorldTransform = BoxElem.GetTransform() * CompTransform;
			FVector Extent = FVector(BoxElem.X, BoxElem.Y, BoxElem.Z) * 0.5f * WorldTransform.GetScale3D();
            
			if (TestBoxTriangleSAT(WorldTransform, Extent, A, B, C))
			{
				OutPoints.Add(FMath::ClosestPointOnTriangleToPoint(WorldTransform.GetLocation(), A, B, C));
			}
		}

		for (const FKSphylElem& SphylElem : BodySetup->AggGeom.SphylElems)
		{
			FTransform WorldTransform = SphylElem.GetTransform() * CompTransform;
			float ScaledRadius = SphylElem.Radius * WorldTransform.GetScale3D().GetMax();
			float ScaledHalfHeight = (SphylElem.Length * 0.5f + SphylElem.Radius) * WorldTransform.GetScale3D().Z; 
            
			IntersectCapsule(WorldTransform.GetLocation(), ScaledHalfHeight, ScaledRadius, WorldTransform.GetUnitAxis(EAxis::Z), TrianglePlane, OutPoints);
		}

		for (const FKConvexElem& ConvexElem : BodySetup->AggGeom.ConvexElems)
		{
			FTransform WorldTransform = ConvexElem.GetTransform() * CompTransform;
			IntersectConvex(WorldTransform, ConvexElem, A, B, C, OutPoints);
		}
	}
	else
	{
		FCollisionShape Shape = HitComp->GetCollisionShape();
		switch (Shape.ShapeType)
		{
			case ECollisionShape::Sphere:
			{
				FVector Closest = FMath::ClosestPointOnTriangleToPoint(CompTransform.GetLocation(), A, B, C);
				if (FVector::DistSquared(Closest, CompTransform.GetLocation()) <= FMath::Square(Shape.GetSphereRadius()))
				{
					OutPoints.Add(Closest);
				}
				break;
			}
			case ECollisionShape::Box:
			{
				if (TestBoxTriangleSAT(CompTransform, Shape.GetBox(), A, B, C))
				{
					OutPoints.Add(FMath::ClosestPointOnTriangleToPoint(CompTransform.GetLocation(), A, B, C));
				}
				break;
			}
			case ECollisionShape::Capsule:
			{
				IntersectCapsule(CompTransform.GetLocation(), Shape.GetCapsuleHalfHeight(), Shape.GetCapsuleRadius(), CompTransform.GetUnitAxis(EAxis::Z), TrianglePlane, OutPoints);
				break;
			}
			default: break;
		}
	}
}

bool UTriangleSweepLibrary::TestBoxTriangleSAT(
	const FTransform& BoxTransform, const FVector& Extent, 
	const FVector& A, const FVector& B, const FVector& C)
{
	FTransform InverseTransform = BoxTransform.Inverse();
	FVector LocalA = InverseTransform.TransformPosition(A);
	FVector LocalB = InverseTransform.TransformPosition(B);
	FVector LocalC = InverseTransform.TransformPosition(C);
	
	FVector E0 = LocalB - LocalA;
	FVector E1 = LocalC - LocalB;
	FVector E2 = LocalA - LocalC;
	FVector LocalTriNormal = FVector::CrossProduct(E0, E1).GetSafeNormal();

	FVector Axes[13] = {
		LocalTriNormal, 
		FVector(1,0,0), FVector(0,1,0), FVector(0,0,1),
		FVector::CrossProduct(E0, FVector(1,0,0)), FVector::CrossProduct(E0, FVector(0,1,0)), FVector::CrossProduct(E0, FVector(0,0,1)),
		FVector::CrossProduct(E1, FVector(1,0,0)), FVector::CrossProduct(E1, FVector(0,1,0)), FVector::CrossProduct(E1, FVector(0,0,1)),
		FVector::CrossProduct(E2, FVector(1,0,0)), FVector::CrossProduct(E2, FVector(0,1,0)), FVector::CrossProduct(E2, FVector(0,0,1))
	};

	for (const FVector& Axis : Axes)
	{
		if (Axis.SizeSquared() < KINDA_SMALL_NUMBER) continue;
		FVector N = Axis.GetSafeNormal();

		float pA = FVector::DotProduct(LocalA, N);
		float pB = FVector::DotProduct(LocalB, N);
		float pC = FVector::DotProduct(LocalC, N);
		float TriMin = FMath::Min3(pA, pB, pC);
		float TriMax = FMath::Max3(pA, pB, pC);

		float r = Extent.X * FMath::Abs(N.X) + Extent.Y * FMath::Abs(N.Y) + Extent.Z * FMath::Abs(N.Z);

		if (-r > TriMax || r < TriMin)
		{
			return false;
		}
	}
	return true;
}

void UTriangleSweepLibrary::IntersectCapsule(
	const FVector& Center, float HalfHeight, float Radius, const FVector& UpVector, 
	const FPlane& Plane, TArray<FVector, TInlineAllocator<32>>& OutPoints)
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

void UTriangleSweepLibrary::IntersectConvex(
	const FTransform& ConvexTransform, const FKConvexElem& ConvexElem, 
	const FVector& A, const FVector& B, const FVector& C, 
	TArray<FVector, TInlineAllocator<32>>& OutPoints)
{
    const TArray<FVector>& LocalVertices = ConvexElem.VertexData;
    const TArray<int32>& Indices = ConvexElem.IndexData;
    
    if (LocalVertices.Num() == 0 || Indices.Num() < 3)
    {
        return;
    }

	FTransform InverseTransform = ConvexTransform.Inverse();
	FVector LocalA = InverseTransform.TransformPosition(A);
	FVector LocalB = InverseTransform.TransformPosition(B);
	FVector LocalC = InverseTransform.TransformPosition(C);
	FPlane LocalPlane(LocalA, LocalB, LocalC);

    TArray<float, TInlineAllocator<64>> Distances;
    Distances.SetNumUninitialized(LocalVertices.Num());

    for (int32 i = 0; i < LocalVertices.Num(); ++i)
    {
        Distances[i] = LocalPlane.PlaneDot(FVector(LocalVertices[i]));
    }

    const int32 TriangleCount = Indices.Num() / 3;
    for (int32 i = 0; i < TriangleCount; ++i)
    {
        const int32 i0 = Indices[i * 3 + 0];
        const int32 i1 = Indices[i * 3 + 1];
        const int32 i2 = Indices[i * 3 + 2];

		auto CheckEdge = [&](int32 idx0, int32 idx1)
		{
			if (Distances[idx0] * Distances[idx1] < 0.0f)
			{
				float t = Distances[idx0] / (Distances[idx0] - Distances[idx1]);
				FVector LocalPoint = FVector(LocalVertices[idx0]) + t * (FVector(LocalVertices[idx1]) - FVector(LocalVertices[idx0]));
				OutPoints.Add(ConvexTransform.TransformPosition(LocalPoint));
			}
		};

		CheckEdge(i0, i1);
		CheckEdge(i1, i2);
		CheckEdge(i2, i0);
    }
}

void UTriangleSweepLibrary::DrawDebugTriangleSweep(
	UWorld* World, const FVector& A, const FVector& B, const FVector& C, 
	bool bHit, const FHitResult& HitResult, EDrawDebugTrace::Type DrawDebugType, 
	FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	bool bPersistent = (DrawDebugType == EDrawDebugTrace::Persistent);
	float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

	DrawDebugLine(World, A, B, TraceColor.ToFColor(true), bPersistent, LifeTime, 0, 1.5f);
	DrawDebugLine(World, B, C, TraceColor.ToFColor(true), bPersistent, LifeTime, 0, 1.5f);
	DrawDebugLine(World, C, A, TraceColor.ToFColor(true), bPersistent, LifeTime, 0, 1.5f);

	if (bHit)
	{
		DrawDebugPoint(World, HitResult.ImpactPoint, 10.0f, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
	}
}

void UTriangleSweepLibrary::DrawDebugTriangleSweepMulti(
	UWorld* World, const FVector& A, const FVector& B, const FVector& C, 
	bool bHit, const TArray<FHitResult>& HitResults, EDrawDebugTrace::Type DrawDebugType, 
	FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	bool bPersistent = (DrawDebugType == EDrawDebugTrace::Persistent);
	float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

	DrawDebugLine(World, A, B, TraceColor.ToFColor(true), bPersistent, LifeTime, 0, 1.5f);
	DrawDebugLine(World, B, C, TraceColor.ToFColor(true), bPersistent, LifeTime, 0, 1.5f);
	DrawDebugLine(World, C, A, TraceColor.ToFColor(true), bPersistent, LifeTime, 0, 1.5f);

	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			DrawDebugPoint(World, Hit.ImpactPoint, 10.0f, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
		}
	}
}
