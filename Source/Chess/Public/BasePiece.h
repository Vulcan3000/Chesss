// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasePiece.generated.h"

UENUM(BlueprintType)
enum class EPieceType : uint8
{
	WPawn,
	BPawn,
	Rook,
	Knight,
	Bishop,
	Queen,
	King,
	None,
	Blue,
	Yellow
};

UENUM(BlueprintType)
enum class EPieceColor : uint8
{
	WHITE,
	BLACK
};

UCLASS()
class CHESS_API ABasePiece : public AActor
{
	GENERATED_BODY()
	
public:	
	
	// Sets default values for this actor's properties
	ABasePiece();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Piece")
	EPieceType PieceType;

	EPieceColor Color;

	FVector Position;

	// Imposta il tipo di pezzo
	void SetPieceType(EPieceType NewType);

	// Ottieni il tipo di pezzo
	EPieceType GetPieceType() const { return PieceType; };

	// Imposta il tipo di pezzo
	void SetPieceColor(EPieceColor NewColor) { Color = NewColor;};

	// Ottieni il tipo di pezzo
	EPieceColor GetPieceColor() const { return Color; };

	void SetPiecePosition(FVector NewPosition) { Position = NewPosition; };

	// Ottieni il tipo di pezzo
	FVector GetPiecePosition() const { return Position; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//destroy a base piece actor
	UFUNCTION()
	void SelfDestroy();
};

USTRUCT(BlueprintType)
struct FChessPieceInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Piece")
	EPieceType PieceType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Piece")
	FVector Position;

	UPROPERTY()
	ABasePiece* Piece;

	TSubclassOf<ABasePiece> ChessPieceType;

	FChessPieceInfo() : PieceType(EPieceType::None), Piece(nullptr) {}

	FChessPieceInfo(ABasePiece* InPiece, const FVector& InPosition)
		: PieceType(EPieceType::None), Position(InPosition), Piece(InPiece)
	{
		if (InPiece)
		{
			PieceType = InPiece->GetPieceType();
		}
	}
};

USTRUCT(BlueprintType)
struct FChessPieceMoves
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Piece")
	ABasePiece* Piece;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Piece")
	TArray<FVector> PossibleMoves;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Piece")
	FVector Position;

	FChessPieceMoves() : Piece(nullptr) {}
};

USTRUCT(BlueprintType)
struct FChessboard
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FChessPieceInfo> WPieces;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FChessPieceInfo> BPieces;


};