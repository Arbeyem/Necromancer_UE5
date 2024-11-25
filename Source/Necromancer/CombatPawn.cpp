// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatPawn.h"

// Sets default values
ACombatPawn::ACombatPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACombatPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACombatPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACombatPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

