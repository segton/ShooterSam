// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Kismet/GameplayStatics.h"
// Sets default values
class Shape
{
protected:
	int32 sides;
};

class Rectangle : public Shape
{
public:
	void SetSides();
};

void Rectangle::SetSides()
{
	sides = 4;
}
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GunMesh = CreateDefaultSubobject<USceneComponent>(TEXT("GunMesh"));
	RootComponent = GunMesh;
	
	GunSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunSkeletalMesh"));
	GunSkeletalMesh->SetupAttachment(GunMesh);

	MuzzleFlashParticleSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MuzzleFlash"));
	MuzzleFlashParticleSystem->SetupAttachment(GunMesh);

	
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	
	MuzzleFlashParticleSystem->Deactivate();
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGun::PullTrigger()
{
	if (OwnerController)
	{
		MuzzleFlashParticleSystem->Activate(true);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShootSound, GetActorLocation());

		FVector ViewPointLocation;
		FRotator ViewPointRotation;	
		OwnerController->GetPlayerViewPoint(ViewPointLocation, ViewPointRotation);
		FHitResult HitResult;	
		FVector EndLocation = ViewPointLocation + (ViewPointRotation.Vector() * MaxRange);
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(GetOwner());
		bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, ViewPointLocation, EndLocation, ECC_GameTraceChannel2, Params);

		if (HasHit)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactParticleSystem, HitResult.ImpactPoint, HitResult.ImpactPoint.Rotation());
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, HitResult.ImpactPoint);
			AActor* HitActor = HitResult.GetActor();
			if (HitActor)
			{
				UGameplayStatics::ApplyDamage(HitActor, BulletDamage, OwnerController, this, UDamageType::StaticClass());
			}
		}
	}
}

