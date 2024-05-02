// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerInterface.h"
#include "Chess_GameInstance.h"
#include "Tile.h"
#include "BasePiece.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "MiniMaxPlayer.generated.h"

UCLASS()
class CHESS_API AMiniMaxPlayer : public APawn, public IPlayerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMiniMaxPlayer();

	UChess_GameInstance* GameInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnTurn() override;
	virtual void OnWin() override;
	virtual void OnLose() override;
	virtual void OnTie() override;

	FChessPieceMoves FindBestMove();
	int32 MiniMax(int32 Depth, bool IsMax);
	int32 EvaluateGrid(bool turn);

	TArray<FChessPieceInfo> MyPieces;
	TArray<FChessPieceInfo> EnemyPieces;
	TArray<FChessPieceMoves> MyMoves;
	TArray<FChessPieceMoves> EnemyMoves;
	int32 counter;

	TArray<TArray<FChessPieceMoves>> AllMyMoves = {
	TArray<FChessPieceMoves>(),
	TArray<FChessPieceMoves>(),
	TArray<FChessPieceMoves>(),
	TArray<FChessPieceMoves>()
	};
	TArray<TArray<FChessPieceMoves>> AllEnemyMoves = {
	TArray<FChessPieceMoves>(),
	TArray<FChessPieceMoves>(),
	TArray<FChessPieceMoves>(),
	TArray<FChessPieceMoves>()
	};
	TArray<TArray<FChessPieceInfo>> AllMyPieces = {
	TArray<FChessPieceInfo>(),
	TArray<FChessPieceInfo>(),
	TArray<FChessPieceInfo>(),
	TArray<FChessPieceInfo>()
	};
	TArray<TArray<FChessPieceInfo>> AllEnemyPieces = {
	TArray<FChessPieceInfo>(),
	TArray<FChessPieceInfo>(),
	TArray<FChessPieceInfo>(),
	TArray<FChessPieceInfo>()
	};
};
