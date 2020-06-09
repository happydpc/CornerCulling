// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "CullingBox.h"
#include "UObject/UObjectGlobals.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "Utils.h"
#include <stdlib.h> 

// Sets default values
AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	// Need to update bounds before GameMode uses them.
	PrimaryActorTick.TickGroup = TG_PrePhysics;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	
	VisibleMaterial = CreateDefaultSubobject<UMaterial>(TEXT("VisibleMaterial"));
	InvisibleMaterial = CreateDefaultSubobject<UMaterial>(TEXT("InvisibleMaterial"));
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitCorners(4);
	UpdateBounds();
	RevealTimer = RevealTimerMax;
}

void AEnemy::SetVisible() {
	Mesh->SetMaterial(0, VisibleMaterial);
}

void AEnemy::SetInvisible() {
	Mesh->SetMaterial(0, InvisibleMaterial);
}

// Update bounds of the player.
// This implementation may appear to be code duplication of CullingBox.
// Howerver, note that one can change the position and number of corners.
// For example, pushing forward a corner if the player picks up a long gun.
void AEnemy::UpdateBounds() {
	FVector Center = GetActorLocation();
	// Actual extents are (50, 50, 50).
	// Padding allows us to trade unnecessary pixel-perfect accuracy
	// for the blistering speed of culling every other frame.
	FVector Extents = FVector(54, 54, 54);
	FTransform T = GetActorTransform();
	Corners[0] = FVector2D(Center + T.TransformVector(FVector(Extents.X, Extents.Y, 0)));
	Corners[1] = FVector2D(Center + T.TransformVector(FVector(Extents.X, -Extents.Y, 0)));
	Corners[2] = FVector2D(Center + T.TransformVector(FVector(-Extents.X, Extents.Y, 0)));
	Corners[3] = FVector2D(Center + T.TransformVector(FVector(-Extents.X, -Extents.Y, 0)));
	ZTop = Center.Z + Extents.Z;
}

// Reveal the enemy, maxing its reveal timer
void AEnemy::Reveal() {
	SetVisible();
	RevealTimer = RevealTimerMax;
}

// Return if the enemy is almost visible.
// Prevents flickering by allowing us to run a visibility check at the last invisible frame.
bool AEnemy::IsAlmostVisible() {
	return (RevealTimer == 1);
}

// Return if the enemy is visible.
bool AEnemy::IsVisible() {
	return (RevealTimer > 0);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateBounds();
	if (IsVisible()) {
		if (--RevealTimer == 0) {
			SetInvisible();
		}
	}
}
