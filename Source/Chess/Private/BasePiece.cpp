// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePiece.h"
#include "GameField.h"
#include "Chess_GameMode.h"

// Sets default values
ABasePiece::ABasePiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ABasePiece::SetPieceType(EPieceType NewType)
{
	PieceType = NewType;
}

// Called when the game starts or when spawned
void ABasePiece::BeginPlay()
{
	Super::BeginPlay();
	AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
	GameMode->GField->OnResetEvent.AddDynamic(this, &ABasePiece::SelfDestroy);
	
}

// Called every frame
void ABasePiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABasePiece::SelfDestroy()
{
	Destroy();
}

