// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniMaxPlayer.h"
#include "Chess_GameMode.h"
#include "Gamefield.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AMiniMaxPlayer::AMiniMaxPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GameInstance = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
}

// Called when the game starts or when spawned
void AMiniMaxPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMiniMaxPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMiniMaxPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMiniMaxPlayer::OnTurn()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Minimax) Turn"));
	GameInstance->SetTurnMessage(TEXT("AI (Minimax) Turn"));

	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			AChess_GameMode* GameMode = (AChess_GameMode*)(GetWorld()->GetAuthGameMode());
			FChessPieceMoves BestMove = FindBestMove();
			GameMode->ChessPieceMoves(BestMove.Piece, BestMove.PossibleMoves[0]);

		}, 3, false);
}

void AMiniMaxPlayer::OnWin()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Minimax) Wins!"));
	GameInstance->SetTurnMessage(TEXT("AI Wins!"));
	GameInstance->IncrementScoreAiPlayer();
}

void AMiniMaxPlayer::OnLose()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Minimax) Loses!"));
	GameInstance->SetTurnMessage(TEXT("AI Loses!"));
}

void AMiniMaxPlayer::OnTie()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (MiniMax) Ties!"));
	GameInstance->SetTurnMessage(TEXT("AI Ties!"));
}

FChessPieceMoves AMiniMaxPlayer::FindBestMove()
{
	int32 bestVal = -1000;
	FChessPieceMoves bestMove;
	bestMove.PossibleMoves.Add(FVector(- 1, -1, 0));

	AChess_GameMode* GameMode = (AChess_GameMode*)(GetWorld()->GetAuthGameMode());
	MyMoves = GameMode->GetBlackMoves();
	EnemyMoves = GameMode->GetWhiteMoves();
	EnemyPieces = GameMode->GetWhitePieces();
	MyPieces = GameMode->GetBlackPieces();

	AllMyMoves.SetNum(4);
	AllEnemyMoves.SetNum(4);
	AllMyPieces.SetNum(4);
	AllMyPieces.SetNum(4);

	AllMyMoves[0] = MyMoves;
	AllEnemyMoves[0] = EnemyMoves;
	AllMyPieces[0] = MyPieces;
	AllEnemyPieces[0] = EnemyPieces;

	for (const FChessPieceMoves& move : AllMyMoves[0])
	{
		for (const FVector& possiblemove : move.PossibleMoves)
		{
			bool eatMove = false;
			ATile* startTile = GameMode->GField->GetTileAtPosition(move.Position);
			ATile* destTile = GameMode->GField->GetTileAtPosition(possiblemove);
			if (destTile->GetTileStatus() == ETileStatus::WHITE) eatMove = true;

			GameMode->doMove(possiblemove, move.Piece);
			GameMode->CalculateMoves();
			GameMode->AvoidCheck(EnemyPieces[0]);
				
			int32 moveVal = MiniMax(0, false);

			// Undo the move
			GameMode->setPieces(AllEnemyPieces[0], AllMyPieces[0]);
			startTile->SetTileStatus(ETileStatus::BLACK);
			if (eatMove) {
				destTile->SetTileStatus(ETileStatus::WHITE);
			}
			else {
				destTile->SetTileStatus(ETileStatus::EMPTY);
			}

			// If the value of the current move is more than the best value update best
			if (moveVal > bestVal)
			{
				bestMove.Piece = move.Piece;
				bestMove.PossibleMoves[0] = possiblemove;
				bestVal = moveVal;
			}
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("AI (Minimax) bestVal = %d "), bestVal));

	return bestMove;
}

int32 AMiniMaxPlayer::MiniMax(int32 Depth, bool IsMax)
{
	int score = EvaluateGrid(IsMax);

	// If Maximizer has won the game return his/her
	// evaluated score
	if (score == 100)
	{
		return score;
	}

	// If Minimizer has won the game return his/her
	// evaluated score
	if (score == -100)
	{
		return score;
	}

	if (Depth == 3) {
		return score;
	}
	// If there are no more moves and no winner then
	// it is a tie
	//if (IsMovesLeft(Board) == false)
	//{
		//return 0;
	//}

	AChess_GameMode* GameMode = (AChess_GameMode*)(GetWorld()->GetAuthGameMode());
	MyPieces = GameMode->GetBlackPieces();
	EnemyPieces = GameMode->GetWhitePieces();
	MyMoves = GameMode->GetBlackMoves();
	EnemyMoves = GameMode->GetWhiteMoves();
	int32 index = Depth + 1;
	AllMyPieces[index] = MyPieces;
	AllEnemyPieces[index] = EnemyPieces;
	AllMyMoves[index] = MyMoves;
	AllEnemyMoves[index] = EnemyMoves;

	
	// If this maximizer's move
	if (IsMax)
	{
		int32 best = -1000;
		for (const FChessPieceMoves& move : AllMyMoves[index]) {
			for (const FVector& possiblemove : move.PossibleMoves) {
				
				bool eatMove = false;
				ATile* startTile = GameMode->GField->GetTileAtPosition(move.Position);
				ATile* destTile = GameMode->GField->GetTileAtPosition(possiblemove);
				if (destTile->GetTileStatus() == ETileStatus::WHITE) eatMove = true;

				//move
				GameMode->doMove(possiblemove, move.Piece);
				GameMode->CalculateMoves();
				GameMode->AvoidCheck(EnemyPieces[0]);

				// Call minimax recursively and choose
					// the maximum value
				best = FMath::Max(best,
					MiniMax(Depth+1, !IsMax));

				// Undo the move
				GameMode->setPieces(AllEnemyPieces[index], AllMyPieces[index]);
				counter = counter - 1;
				startTile->SetTileStatus(ETileStatus::BLACK);
				if (eatMove) {
					destTile->SetTileStatus(ETileStatus::WHITE);
				}
				else {
					destTile->SetTileStatus(ETileStatus::EMPTY);
				}
			}
		}
		return best;
	}

	// If this minimizer's move
	else
	{
		int32 best = 1000;
		for (const FChessPieceMoves& move : AllEnemyMoves[index]) {
			for (const FVector& possiblemove : move.PossibleMoves) {

				bool eatMove = false;
				ATile* startTile = GameMode->GField->GetTileAtPosition(move.Position);
				ATile* destTile = GameMode->GField->GetTileAtPosition(possiblemove);
				if (destTile->GetTileStatus() == ETileStatus::BLACK) eatMove = true;

				//move
				GameMode->doMove(possiblemove, move.Piece);
				GameMode->CalculateMoves();
				GameMode->AvoidCheck(MyPieces[0]);

				// Call minimax recursively and choose
					// the min value
				best = FMath::Min(best,
					MiniMax(Depth + 1, !IsMax));

				// Undo the move
				counter = counter - 1;
				GameMode->setPieces(EnemyPieces, MyPieces);
				startTile->SetTileStatus(ETileStatus::WHITE);
				if (eatMove) {
					destTile->SetTileStatus(ETileStatus::BLACK);
				}
				else {
					destTile->SetTileStatus(ETileStatus::EMPTY);
				}
			}
		}
		return best;
	}
}

int32 AMiniMaxPlayer::EvaluateGrid(bool IsMax)
{
	int32 myscore = 0;
	int32 enemyscore = 0;
	AChess_GameMode* GameMode = (AChess_GameMode*)(GetWorld()->GetAuthGameMode());
	MyPieces = GameMode->GetBlackPieces();
	EnemyPieces = GameMode->GetWhitePieces();
	MyMoves = GameMode->GetBlackMoves();
	EnemyMoves = GameMode->GetWhiteMoves();

	if (IsMax && (GameMode->OnCheck(MyPieces[0]) == true) && (MyMoves.Num() < 1)) {
		return -100;
	}
	if (!IsMax && (GameMode->OnCheck(EnemyPieces[0]) == true) && (EnemyMoves.Num() < 1)) {
		return 100;
	}

	for (FChessPieceInfo myPiece : MyPieces) {
		if (myPiece.PieceType == EPieceType::BPawn) myscore = myscore + 1;
		if (myPiece.PieceType == EPieceType::Knight) myscore = myscore + 3;
		if (myPiece.PieceType == EPieceType::Bishop) myscore = myscore + 3;
		if (myPiece.PieceType == EPieceType::Rook) myscore = myscore + 5;
		if (myPiece.PieceType == EPieceType::Queen) myscore = myscore + 9;
	}
	for (FChessPieceInfo enemyPiece : EnemyPieces) {
		if (enemyPiece.PieceType == EPieceType::WPawn) enemyscore = enemyscore + 1;
		if (enemyPiece.PieceType == EPieceType::Knight) enemyscore = enemyscore + 3;
		if (enemyPiece.PieceType == EPieceType::Bishop) enemyscore = enemyscore + 3;
		if (enemyPiece.PieceType == EPieceType::Rook) enemyscore = enemyscore + 5;
		if (enemyPiece.PieceType == EPieceType::Queen) enemyscore = enemyscore + 9;
	}
	TArray<ATile*>& Tiles = GameMode->GField->GetTileArray();
	TArray<FVector2D> innercenter = { FVector2D(3, 3), FVector2D(3,4), FVector2D(4,3), FVector2D(4,4) };
	TArray<FVector2D> outercenter = { FVector2D(2, 2), FVector2D(2,3), FVector2D(2,4), FVector2D(2,5), FVector2D(3,2), FVector2D(3,5), FVector2D(4,2), FVector2D(4,5),
										FVector2D(5,2), FVector2D(5,3), FVector2D(5,4), FVector2D(5,5)};
	for (FVector2D tileposition : innercenter) {
		FVector position = FVector(tileposition[0] * 120, tileposition[1] * 120, 0);
		ATile* tile = GameMode->GField->GetTileAtPosition(position);
		if (tile->GetTileStatus() == ETileStatus::BLACK) myscore = myscore + 2;
		if (tile->GetTileStatus() == ETileStatus::WHITE) enemyscore = enemyscore + 2;
	}
	for (FVector2D tileposition : outercenter) {
		FVector position = FVector(tileposition[0] * 120, tileposition[1] * 120, 0);
		ATile* tile = GameMode->GField->GetTileAtPosition(position);
		if (tile->GetTileStatus() == ETileStatus::BLACK) myscore = myscore + 1;
		if (tile->GetTileStatus() == ETileStatus::WHITE) enemyscore = enemyscore + 1;
	}
	if (IsMax) {
		return myscore;
	}
	else {
		return -enemyscore;
	}
}

