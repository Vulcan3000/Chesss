// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PlayerInterface.h"
#include "GameField.h"
#include "BasePiece.h"
#include "Chess_GameInstance.h"
#include "Chess_GameMode.generated.h"

class AActor;

struct FPosition;

/**
 * 
 */
UCLASS()
class CHESS_API AChess_GameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	// tracks if the game is over
	bool IsGameOver;
	// array of player interfaces
	TArray<IPlayerInterface*> Players;
	int32 CurrentPlayer;
	// tracks the number of moves in order to signal a drawn game
	int32 MoveCounter;
	FString MoveName;

	// TSubclassOf is a template class that provides UClass type safety.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGameField> GameFieldClass;

	// field size
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 FieldSize;

	// reference to a GameField object
	UPROPERTY(VisibleAnywhere)
	AGameField* GField;

	// Dichiarazione delle sottoclassi dei blueprint per gli scacchi
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "White Pieces")
	TSubclassOf<ABasePiece> WhitePawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "White Pieces")
	TSubclassOf<ABasePiece> WhiteRook;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "White Pieces")
	TSubclassOf<ABasePiece> WhiteKnight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "White Pieces")
	TSubclassOf<ABasePiece> WhiteBishop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "White Pieces")
	TSubclassOf<ABasePiece> WhiteQueen;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "White Pieces")
	TSubclassOf<ABasePiece> WhiteKing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Black Pieces")
	TSubclassOf<ABasePiece> BlackPawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Black Pieces")
	TSubclassOf<ABasePiece> BlackRook;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Black Pieces")
	TSubclassOf<ABasePiece> BlackKnight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Black Pieces")
	TSubclassOf<ABasePiece> BlackBishop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Black Pieces")
	TSubclassOf<ABasePiece> BlackQueen;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Black Pieces")
	TSubclassOf<ABasePiece> BlackKing;

	AChess_GameMode();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// called at the start of the game
	void ChoosePlayerAndStartGame();

	// get the next player index
	int32 GetNextPlayer(int32 Player);

	// called at the end of the game turn
	void TurnNextPlayer();

	UFUNCTION(BlueprintCallable)
	void SetupChessPieces();

	void SpawnPiece(TSubclassOf<ABasePiece> Piece, const FVector& SpawnPosition);

	// Array di pezzi per il giocatore bianco
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FChessPieceInfo> WhitePieces;

	// Array di pezzi per il giocatore nero
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FChessPieceInfo> BlackPieces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FChessPieceInfo> YellowPieces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FChessPieceMoves> WhiteMoves;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FChessPieceMoves> BlackMoves;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ABasePiece*> ShownMoves;

	void CalculateMoves();
	bool OnCheck(const FChessPieceInfo& Piece);
	void AvoidCheck(const FChessPieceInfo& Piece);

	void WPawnMove(const FChessPieceInfo& Piece);
	void BPawnMove(const FChessPieceInfo& Piece);
	void RookMove(const FChessPieceInfo& Piece);
	void BishopMove(const FChessPieceInfo& Piece);
	void KnightMove(const FChessPieceInfo& Piece);
	void QueenMove(const FChessPieceInfo& Piece);
	void KingMove(const FChessPieceInfo& Piece);

	void ChessPieceMoves(ABasePiece* SelectedPiece, FVector Location);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ABasePiece> BlueSign;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ABasePiece> YellowSign;


	void ShowMoves(ABasePiece* SelectedPiece);
	void HideMoves();

	

	TArray<FChessPieceInfo> GetShownMoves() const { return YellowPieces; }
	TArray<FChessPieceMoves> GetWhiteMoves() const { return WhiteMoves; }
	TArray<FChessPieceMoves> GetBlackMoves() const { return BlackMoves; }
	TArray<FChessPieceInfo> GetWhitePieces() const { return WhitePieces; }
	TArray<FChessPieceInfo> GetBlackPieces() const { return BlackPieces; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FChessboard> Positions;
	
	void doMove(FVector Location, ABasePiece* Piece);

	UFUNCTION(BlueprintCallable)
	void setChessboardAtIndex(int32 index);

	void setPieces(TArray<FChessPieceInfo> x, TArray<FChessPieceInfo> y);

	void NameMove(const ABasePiece* Piece, bool capture);

	int32 getmovecounter() const { return MoveCounter; }
};
