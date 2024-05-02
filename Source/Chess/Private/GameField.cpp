// Fill out your copyright notice in the Description page of Project Settings.


#include "GameField.h"
#include "Chess_GameMode.h"


// Sets default values
AGameField::AGameField()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	// size of the field (3x3)
	Size = 8;
	// tile dimension
	TileSize = 120;
}

void AGameField::undomove()
{
	AChess_GameMode* GameMode = (AChess_GameMode*)(GetWorld()->GetAuthGameMode());
	int32 num = GameMode->getmovecounter();
	GameMode->setChessboardAtIndex(num-2);
}

// Called when the game starts or when spawned
void AGameField::BeginPlay()
{
	Super::BeginPlay();
	GenerateField();
}


void AGameField::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AGameField::ResetField()
{
	AChess_GameMode* GameMode = (AChess_GameMode*)(GetWorld()->GetAuthGameMode());
	GameMode->setChessboardAtIndex(0);
}

void AGameField::GenerateField()
{
	bool bIsWhite = true; 
	
	for (int32 x = 0; x < 8; x++)
	{
		for (int32 y = 0; y < 8; y++)
		{
			FVector Location = AGameField::GetRelativeLocationByXYPosition(x, y);
			
			UClass* TileBlueprintClass = bIsWhite ? WhiteTileClass : BlackTileClass;
			ATile* Obj = GetWorld()->SpawnActor<ATile>(TileBlueprintClass, Location, FRotator::ZeroRotator);
			const float TileScale = TileSize / 100;
			Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
			Obj->SetGridPosition(x, y);
			TileArray.Add(Obj);
			TileMap.Add(FVector2D(x, y), Obj);

			bIsWhite = !bIsWhite;
		}

		bIsWhite = !bIsWhite;	
	}
}


FVector2D AGameField::GetPosition(const FHitResult& Hit)
{
	return Cast<ATile>(Hit.GetActor())->GetGridPosition();
}

TArray<ATile*>& AGameField::GetTileArray()
{
	return TileArray;
}

FVector AGameField::GetRelativeLocationByXYPosition(const int32 InX, const int32 InY) const
{
	return TileSize * FVector(InX, InY, 0);
}

FVector2D AGameField::GetXYPositionByRelativeLocation(const FVector& Location) const
{
	const double x = Location[0] / TileSize;
	const double y = Location[1] / TileSize;
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("x=%f,y=%f"), x, y));
	return FVector2D(x, y);
}


inline bool AGameField::IsValidPosition(const FVector2D Position) const
{
	return 0 <= Position[0] && Position[0] < Size && 0 <= Position[1] && Position[1] < Size;
}

void AGameField::SetTileStatusByPosition(int32 Row, int32 Column, ETileStatus TileStatus)
{
	// Verifica se le coordinate sono valide
	if (TileMap.Contains(FVector2D(Row, Column)))
	{
		// Ottieni l'oggetto ATile dalla mappa
		ATile* Tile = TileMap[FVector2D(Row, Column)];

		// Imposta lo stato della casella
		Tile->SetTileStatus(TileStatus);
	}
	else
	{
		// Se le coordinate non sono valide, stampa un avviso
		UE_LOG(LogTemp, Warning, TEXT("Invalid tile position!"));
	}

}

ATile* AGameField::GetTileAtPosition(const FVector Location) const
{
	FVector2D Position = GetXYPositionByRelativeLocation(Location);
	if (TileMap.Contains(Position))
	{
		// Ottieni l'oggetto ATile dalla mappa
		ATile* Tile = TileMap[Position];

		return Tile;
	}
	return nullptr;
}