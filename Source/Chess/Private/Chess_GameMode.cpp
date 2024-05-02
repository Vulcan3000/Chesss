// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_GameMode.h"
#include "Chess_PlayerController.h"
#include "HumanPlayer.h"
#include "RandomPlayer.h"
#include "MinimaxPlayer.h"
#include "EngineUtils.h"

AChess_GameMode::AChess_GameMode()
{
	PlayerControllerClass = AChess_PlayerController::StaticClass();
	DefaultPawnClass = AHumanPlayer::StaticClass();
	FieldSize = 8;
}

void AChess_GameMode::BeginPlay()
{
	Super::BeginPlay();

	IsGameOver = false;

	MoveCounter = 0;

	AHumanPlayer* HumanPlayer = Cast<AHumanPlayer>(*TActorIterator<AHumanPlayer>(GetWorld()));

	if (GameFieldClass != nullptr)
	{
		GField = GetWorld()->SpawnActor<AGameField>(GameFieldClass);
		//GField->Size = FieldSize;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Game Field is null"));
	}
	SetupChessPieces();

	float CameraPosX = (GField->TileSize * FieldSize) / 2 - (GField->TileSize / 2);
	FVector CameraPos(CameraPosX, CameraPosX, 1000.0f);
	HumanPlayer->SetActorLocationAndRotation(CameraPos, FRotationMatrix::MakeFromX(FVector(0, 0, -1)).Rotator());

	// Human player = 0
	Players.Add(HumanPlayer);
	// Random Player
	
	auto* AI = GetWorld()->SpawnActor<ARandomPlayer>(FVector(), FRotator());

	// MiniMax Player
	//auto* AI = GetWorld()->SpawnActor<AMiniMaxPlayer>(FVector(), FRotator());

	// AI player = 1
	Players.Add(AI);

	CalculateMoves();

	this->ChoosePlayerAndStartGame();
}

void AChess_GameMode::ChoosePlayerAndStartGame()
{
	CurrentPlayer = 0;

	for (int32 i = 0; i < Players.Num(); i++)
	{
		Players[i]->PlayerNumber = i;
		Players[i]->Sign = i == CurrentPlayer ? ESign::B : ESign::N;
	}
	FChessboard Disposition;
	for (const FChessPieceInfo& currPiece : WhitePieces) {
		Disposition.WPieces.Add(currPiece);
	}
	for (const FChessPieceInfo& currPiece : BlackPieces) {
		Disposition.BPieces.Add(currPiece);
	}
	Positions.Add(Disposition);
	Players[CurrentPlayer]->OnTurn();

}

int32 AChess_GameMode::GetNextPlayer(int32 Player)
{
	Player++;
	if (!Players.IsValidIndex(Player))
	{
		Player = 0;
	}
	return Player;
}

void AChess_GameMode::TurnNextPlayer()
{
	MoveCounter += 1;
	CurrentPlayer = GetNextPlayer(CurrentPlayer);
	Players[CurrentPlayer]->OnTurn();
}

void AChess_GameMode::SetupChessPieces()
{

	SpawnPiece(WhiteKing, FVector(0, 480, 0));
	SpawnPiece(WhiteRook, FVector(0, 0, 0));
	SpawnPiece(WhiteRook, FVector(0, 840, 0));
	SpawnPiece(WhiteKnight, FVector(0, 120, 0));
	SpawnPiece(WhiteKnight, FVector(0, 720, 0));
	SpawnPiece(WhiteBishop, FVector(0, 240, 0));
	SpawnPiece(WhiteBishop, FVector(0, 600, 0));
	SpawnPiece(WhiteQueen, FVector(0, 360, 0));
	for (int32 x = 0; x < 8; x++) {
		SpawnPiece(WhitePawn, FVector(120, x*120, 0));
	}
	for (int32 x = 0; x < 2; x++) {
		for (int32 y = 0; y < 8; y++) {
			GField->SetTileStatusByPosition(x, y, ETileStatus::WHITE);
		}
	}
	SpawnPiece(BlackKing, FVector(840, 480, 0));
	SpawnPiece(BlackRook, FVector(840, 0, 0));
	SpawnPiece(BlackRook, FVector(840, 840, 0));
	SpawnPiece(BlackKnight, FVector(840, 120, 0));
	SpawnPiece(BlackKnight, FVector(840, 720, 0));
	SpawnPiece(BlackBishop, FVector(840, 240, 0));
	SpawnPiece(BlackBishop, FVector(840, 600, 0));
	SpawnPiece(BlackQueen, FVector(840, 360, 0));
	for (int32 x = 0; x < 8; x++) {
		SpawnPiece(BlackPawn, FVector(720, x*120, 0));
	}
	for (int32 x = 6; x < 8; x++) {
		for (int32 y = 0; y < 8; y++) {
			GField->SetTileStatusByPosition(x, y, ETileStatus::BLACK);
			

		}
	}
}

void AChess_GameMode::SpawnPiece(TSubclassOf<ABasePiece> Piece, const FVector& SpawnPosition)
{
	FVector Location = GField->GetActorLocation() + SpawnPosition + FVector(0, 0, 10);
	FRotator SpawnRotation = FRotator(0.0f, 90.0f, 0.0f);
	ABasePiece* NewPiece = Cast<ABasePiece>(GetWorld()->SpawnActor(Piece, &Location, &SpawnRotation));
	
	if (Piece == WhiteRook || Piece == BlackRook) {
		NewPiece->SetPieceType(EPieceType::Rook);
	}
	if (Piece == WhiteBishop || Piece == BlackBishop) {
		NewPiece->SetPieceType(EPieceType::Bishop);
	}
	if (Piece == WhiteKnight || Piece == BlackKnight) {
		NewPiece->SetPieceType(EPieceType::Knight);
	}
	if (Piece == WhiteQueen || Piece == BlackQueen) {
		NewPiece->SetPieceType(EPieceType::Queen);
	}
	if (Piece == WhiteKing || Piece == BlackKing) {
		NewPiece->SetPieceType(EPieceType::King);
	}
	if (Piece == WhitePawn) {
		NewPiece->SetPieceType(EPieceType::WPawn);
	}
	if (Piece == BlackPawn) {
		NewPiece->SetPieceType(EPieceType::BPawn);
	}
	if (Piece == YellowSign) {
		NewPiece->SetPieceType(EPieceType::Yellow);
	}
	if (Piece == BlueSign) {
		NewPiece->SetPieceType(EPieceType::Blue);
	}

	if (Piece == WhitePawn || Piece == WhiteRook || Piece == WhiteKnight || Piece == WhiteBishop || Piece == WhiteQueen || Piece == WhiteKing) {
		FChessPieceInfo WhitePiece(NewPiece, Location);
		WhitePiece.ChessPieceType = Piece;
		WhitePieces.Add(WhitePiece);
	}
	if (Piece == BlackPawn || Piece == BlackRook || Piece == BlackKnight || Piece == BlackBishop || Piece == BlackQueen || Piece == BlackKing) {
		FChessPieceInfo BlackPiece(NewPiece, Location);
		BlackPiece.ChessPieceType = Piece;
		BlackPieces.Add(BlackPiece);
	}
	if (Piece == BlueSign || Piece == YellowSign) {
		FChessPieceInfo BlackPiece(NewPiece, Location);
		YellowPieces.Add(BlackPiece);
	}
}

void AChess_GameMode::CalculateMoves()
{
	WhiteMoves.Empty();
	BlackMoves.Empty();

	// Calcola le mosse per i pezzi bianchi
	for (int32 Index = WhitePieces.Num()-1; Index>=0; Index--)
	{
		const FChessPieceInfo& WhitePiece = WhitePieces[Index];
		EPieceType PieceType = WhitePiece.PieceType;

		switch (PieceType)
		{
		case EPieceType::WPawn:
			WPawnMove(WhitePiece);
			break;
		case EPieceType::Rook:
			RookMove(WhitePiece);
			break;
		case EPieceType::Bishop:
			BishopMove(WhitePiece);
			break;
		case EPieceType::Knight:
			KnightMove(WhitePiece);
			break;
		case EPieceType::Queen:
			QueenMove(WhitePiece);
			break;
		case EPieceType::King:
			KingMove(WhitePiece);
			break;
		default:;
			break;
		}
	}

	// Calcola le mosse per i pezzi neri
	for (const FChessPieceInfo& BlackPiece : BlackPieces)
	{
		EPieceType PieceType = BlackPiece.PieceType;
		switch (PieceType)
		{
		case EPieceType::BPawn:
			BPawnMove(BlackPiece);
			break;
		case EPieceType::Rook:
			RookMove(BlackPiece);
			break;
		case EPieceType::Bishop:
			BishopMove(BlackPiece);
			break;
		case EPieceType::Knight:
			KnightMove(BlackPiece);
			break;
		case EPieceType::Queen:
			QueenMove(BlackPiece);
			break;
		case EPieceType::King:
			KingMove(BlackPiece);
		default:
			break;
		}
	}
}

//controlla se un re è raggiungibile dal nemico
bool AChess_GameMode::OnCheck(const FChessPieceInfo& Piece)
{
	FVector KingPosition = Piece.Position;
	ATile* KingTile = GField->GetTileAtPosition(KingPosition);
	ETileStatus ColorKing = KingTile->GetTileStatus();
	TArray<FChessPieceMoves>& EnemyMoves = (ColorKing==ETileStatus::WHITE) ? BlackMoves : WhiteMoves;
	for (int32 index = EnemyMoves.Num() - 1; index >= 0; index--) {
		for (int32 indey = EnemyMoves[index].PossibleMoves.Num() - 1; indey >= 0; indey--) {
			if (EnemyMoves[index].PossibleMoves[indey] == KingPosition) {
				return true;
			}
		}
	}
	return false;
}

//elimina le mosse che facendole ti mettono in scacco
void AChess_GameMode::AvoidCheck(const FChessPieceInfo& Piece)
{
	bool IsWhite = false;
	bool EatMove;
	ATile* OneTile = GField->GetTileAtPosition(Piece.Position);
	ETileStatus Color = OneTile->GetTileStatus();
	if (Color==ETileStatus::WHITE) {
		IsWhite = true;
	}
	TArray<FChessPieceMoves> MyCopies = IsWhite ? WhiteMoves : BlackMoves;
	TArray<FChessPieceMoves> EnemyCopies = IsWhite ? BlackMoves : WhiteMoves;
	TArray<FChessPieceInfo> WhiteCopies = WhitePieces;
	TArray<FChessPieceInfo> BlackCopies = BlackPieces;

	for (int32 index = MyCopies.Num() - 1; index >= 0; index--) {
		for (int32 indey = MyCopies[index].PossibleMoves.Num() - 1; indey >= 0; indey--) {
			EatMove = false;
			ATile* StartTile = GField->GetTileAtPosition(MyCopies[index].Position);
			ETileStatus StartStatus = StartTile->GetTileStatus();
			ATile* DestTile = GField->GetTileAtPosition(MyCopies[index].PossibleMoves[indey]);
			ETileStatus DestStatus = DestTile->GetTileStatus();

			if ((StartStatus == ETileStatus::WHITE && DestStatus == ETileStatus::BLACK) || (StartStatus == ETileStatus::BLACK && DestStatus == ETileStatus::WHITE)) {
				
				EatMove = true;
			}
			// Sposta lo scacco alla nuova posizione
			doMove(MyCopies[index].PossibleMoves[indey], MyCopies[index].Piece);

			CalculateMoves();

			FChessPieceInfo MyKing = IsWhite ? WhitePieces[0] : BlackPieces[0];
			if (OnCheck(MyKing)) {
				MyCopies[index].PossibleMoves.RemoveAt(indey);
				if (MyCopies[index].PossibleMoves.Num() < 1) {
					MyCopies.RemoveAt(index);
				}
			}
			
			StartTile->SetTileStatus(Color);
			if (EatMove) {
				DestTile->SetTileStatus(IsWhite? ETileStatus::BLACK : ETileStatus::WHITE);
			}
			else {
				DestTile->SetTileStatus(ETileStatus::EMPTY);
			}
			IsWhite ? WhiteMoves = MyCopies : BlackMoves = MyCopies;
			IsWhite ? BlackMoves = EnemyCopies : WhiteMoves = EnemyCopies;
			WhitePieces = WhiteCopies;
			BlackPieces = BlackCopies;
		}
	}
}

void AChess_GameMode::WPawnMove(const FChessPieceInfo& Piece)
{
	// Verifica se il pedone può muoversi in avanti di una casella
	FVector DestinationPosition = Piece.Position + FVector(120, 0, 0);
	ATile* DestinationTile = GField->GetTileAtPosition(DestinationPosition);

	FChessPieceMoves NewPawnPiece;
	NewPawnPiece.Piece = Piece.Piece;
	NewPawnPiece.Position = Piece.Position;

	if (DestinationTile && DestinationTile->GetTileStatus() == ETileStatus::EMPTY)
	{
		NewPawnPiece.PossibleMoves.Add(DestinationPosition);

		if (Piece.Position[0] == 120) {
			DestinationPosition = Piece.Position + FVector(240, 0, 0);
			DestinationTile = GField->GetTileAtPosition(DestinationPosition);

			if (DestinationTile && DestinationTile->GetTileStatus() == ETileStatus::EMPTY)
			{
				NewPawnPiece.PossibleMoves.Add(DestinationPosition);
			}
		}		
	}

	TArray<FVector> Directions = { FVector(120, 120, 0), FVector(120, -120, 0) };
	for (const FVector& Direction : Directions) {
		DestinationPosition = Piece.Position + Direction;
		DestinationTile = GField->GetTileAtPosition(DestinationPosition);
		if (DestinationTile && DestinationTile->GetTileStatus() == ETileStatus::BLACK)
		{
			NewPawnPiece.PossibleMoves.Add(DestinationPosition);
		}
	}
	if (NewPawnPiece.PossibleMoves.Num() > 0) {

		WhiteMoves.Add(NewPawnPiece);
	}
	
}

void AChess_GameMode::BPawnMove(const FChessPieceInfo& Piece)
{
	// Verifica se il pedone può muoversi in avanti di una casella
	FVector DestinationPosition = Piece.Position + FVector(-120, 0, 0);
	ATile* DestinationTile = GField->GetTileAtPosition(DestinationPosition);
	FChessPieceMoves NewPawnPiece;
	NewPawnPiece.Piece = Piece.Piece;
	NewPawnPiece.Position = Piece.Position;

	if (DestinationTile && DestinationTile->GetTileStatus() == ETileStatus::EMPTY)
	{		
		NewPawnPiece.PossibleMoves.Add(DestinationPosition);

		if (Piece.Position[0] == 720) {
			DestinationPosition = Piece.Position + FVector(-240, 0, 0);
			DestinationTile = GField->GetTileAtPosition(DestinationPosition);

			if (DestinationTile && DestinationTile->GetTileStatus() == ETileStatus::EMPTY)
			{
				NewPawnPiece.PossibleMoves.Add(DestinationPosition);
			}
		}
	}
	TArray<FVector> Directions = { FVector(-120, 120, 0), FVector(-120, -120, 0) };
	for (const FVector& Direction : Directions) {
		DestinationPosition = Piece.Position + Direction;
		DestinationTile = GField->GetTileAtPosition(DestinationPosition);
		if (DestinationTile && DestinationTile->GetTileStatus() == ETileStatus::WHITE)
		{
			NewPawnPiece.PossibleMoves.Add(DestinationPosition);
		}
	}
	if (NewPawnPiece.PossibleMoves.Num() > 0) {

		BlackMoves.Add(NewPawnPiece);
	}
}

void AChess_GameMode::RookMove(const FChessPieceInfo& Piece)
{
	// Array per memorizzare le direzioni di movimento possibili per la torre
	TArray<FVector> Directions = { FVector(120, 0, 0), FVector(-120, 0, 0), FVector(0, 120, 0), FVector(0, -120, 0) };
	FChessPieceMoves NewRookPiece;
	NewRookPiece.Piece = Piece.Piece;
	NewRookPiece.Position = Piece.Position;
	ATile* StartTile = GField->GetTileAtPosition(Piece.Position);
	ETileStatus StartStatus = StartTile->GetTileStatus();

	// Per ogni direzione possibile
	for (const FVector& Direction : Directions)
	{
		FVector NextPosition = Piece.Position + Direction;

		FVector2D destination = GField->GetXYPositionByRelativeLocation(NextPosition);
		
		// Continua a muoversi lungo la direzione finché non raggiunge un ostacolo o il bordo della scacchiera
		while (GField->IsValidPosition(destination))
		{
			ATile* NextTile = GField->GetTileAtPosition(NextPosition);
			ETileStatus NextStatus = NextTile->GetTileStatus();

			// Se la prossima casella è vuota, aggiungi la posizione come possibile mossa
			if (NextStatus == ETileStatus::EMPTY)
			{
				NewRookPiece.PossibleMoves.Add(NextPosition);
				
			} else if ((StartStatus == ETileStatus::WHITE && NextStatus == ETileStatus::BLACK) || (StartStatus == ETileStatus::BLACK && NextStatus == ETileStatus::WHITE)){
				NewRookPiece.PossibleMoves.Add(NextPosition);
				break;
			} else { 
				break; }
			NextPosition += Direction;
			destination = GField->GetXYPositionByRelativeLocation(NextPosition);
		}
	}
	if (NewRookPiece.PossibleMoves.Num() > 0) {
		if (StartStatus == ETileStatus::WHITE) {
			WhiteMoves.Add(NewRookPiece);
		}
		else {
			BlackMoves.Add(NewRookPiece);
		}
	}
}

void AChess_GameMode::BishopMove(const FChessPieceInfo& Piece)
{
	TArray<FVector> Directions = { FVector(120, 120, 0), FVector(-120, -120, 0), FVector(-120, 120, 0), FVector(120, -120, 0) };
	FChessPieceMoves BishopMoves;
	BishopMoves.Piece = Piece.Piece;
	BishopMoves.Position = Piece.Position;
	ATile* StartTile = GField->GetTileAtPosition(Piece.Position);
	ETileStatus StartStatus = StartTile->GetTileStatus();

	// Per ogni direzione possibile
	for (const FVector& Direction : Directions)
	{
		FVector NextPosition = Piece.Position + Direction;

		FVector2D destination = GField->GetXYPositionByRelativeLocation(NextPosition);

		// Continua a muoversi lungo la direzione finché non raggiunge un ostacolo o il bordo della scacchiera
		while (GField->IsValidPosition(destination))
		{
			ATile* NextTile = GField->GetTileAtPosition(NextPosition);
			ETileStatus NextStatus = NextTile->GetTileStatus();

			// Se la prossima casella è vuota, aggiungi la posizione come possibile mossa
			if (NextStatus == ETileStatus::EMPTY)
			{
				BishopMoves.PossibleMoves.Add(NextPosition);

			}
			else if ((StartStatus == ETileStatus::WHITE && NextStatus == ETileStatus::BLACK) || (StartStatus == ETileStatus::BLACK && NextStatus == ETileStatus::WHITE)) {
				BishopMoves.PossibleMoves.Add(NextPosition);
				break;
			}
			else {
				break;
			}
			NextPosition += Direction;
			destination = GField->GetXYPositionByRelativeLocation(NextPosition);
		}
	}
	if (BishopMoves.PossibleMoves.Num() > 0) {
		if (StartStatus == ETileStatus::WHITE) {
			WhiteMoves.Add(BishopMoves);
		}
		else {
			BlackMoves.Add(BishopMoves);
		}
	}
}

void AChess_GameMode::KnightMove(const FChessPieceInfo& Piece)
{
	TArray<FVector> Directions = { FVector(240, 120, 0), FVector(120, -240, 0), FVector(-240, 120, 0), FVector(120, 240, 0), FVector(240, -120, 0), FVector(-120, -240, 0), FVector(-240, -120, 0), FVector(-120, 240, 0) };
	FChessPieceMoves KnightMoves;
	KnightMoves.Piece = Piece.Piece;
	KnightMoves.Position = Piece.Position;
	ATile* StartTile = GField->GetTileAtPosition(Piece.Position);
	ETileStatus StartStatus = StartTile->GetTileStatus();

	// Per ogni direzione possibile
	for (const FVector& Direction : Directions)
	{
		FVector NextPosition = Piece.Position + Direction;

		FVector2D destination = GField->GetXYPositionByRelativeLocation(NextPosition);

		// Continua a muoversi lungo la direzione finché non raggiunge un ostacolo o il bordo della scacchiera
		if (GField->IsValidPosition(destination))
		{
			ATile* NextTile = GField->GetTileAtPosition(NextPosition);
			ETileStatus NextStatus = NextTile->GetTileStatus();

			// Se la prossima casella è vuota, aggiungi la posizione come possibile mossa
			if (NextStatus == ETileStatus::EMPTY)
			{
				KnightMoves.PossibleMoves.Add(NextPosition);

			}
			else if ((StartStatus == ETileStatus::WHITE && NextStatus == ETileStatus::BLACK) || (StartStatus == ETileStatus::BLACK && NextStatus == ETileStatus::WHITE)) {
				KnightMoves.PossibleMoves.Add(NextPosition);
			}
		}
	}
	if (KnightMoves.PossibleMoves.Num() > 0) {
		if (StartStatus == ETileStatus::WHITE) {
			WhiteMoves.Add(KnightMoves);
		}
		else {
			BlackMoves.Add(KnightMoves);
		}
	}
}

void AChess_GameMode::QueenMove(const FChessPieceInfo& Piece)
{
	TArray<FVector> Directions = { FVector(120, 0, 0), FVector(-120, 0, 0), FVector(0, 120, 0), FVector(0, -120, 0), FVector(120, 120, 0), FVector(-120, -120, 0), FVector(-120, 120, 0), FVector(120, -120, 0) };
	FChessPieceMoves QueenMoves;
	QueenMoves.Piece = Piece.Piece;
	QueenMoves.Position = Piece.Position;
	ATile* StartTile = GField->GetTileAtPosition(Piece.Position);
	ETileStatus StartStatus = StartTile->GetTileStatus();

	// Per ogni direzione possibile
	for (const FVector& Direction : Directions)
	{
		FVector NextPosition = Piece.Position + Direction;
		FVector2D destination = GField->GetXYPositionByRelativeLocation(NextPosition);

		// Continua a muoversi lungo la direzione finché non raggiunge un ostacolo o il bordo della scacchiera
		while (GField->IsValidPosition(destination))
		{
			ATile* NextTile = GField->GetTileAtPosition(NextPosition);
			ETileStatus NextStatus = NextTile->GetTileStatus();

			// Se la prossima casella è vuota, aggiungi la posizione come possibile mossa
			if (NextStatus == ETileStatus::EMPTY)
			{
				QueenMoves.PossibleMoves.Add(NextPosition);

			}
			else if ((StartStatus == ETileStatus::WHITE && NextStatus == ETileStatus::BLACK) || (StartStatus == ETileStatus::BLACK && NextStatus == ETileStatus::WHITE)) {
				QueenMoves.PossibleMoves.Add(NextPosition);
				break;
			}
			else {
				break;
			}
			NextPosition += Direction;
			destination = GField->GetXYPositionByRelativeLocation(NextPosition);
		}
	}
	if (QueenMoves.PossibleMoves.Num() > 0) {
		if (StartStatus == ETileStatus::WHITE) {
			WhiteMoves.Add(QueenMoves);
		}
		else {
			BlackMoves.Add(QueenMoves);
		}
	}
}

void AChess_GameMode::KingMove(const FChessPieceInfo& Piece)
{
	TArray<FVector> Directions = { FVector(120, 0, 0), FVector(-120, 0, 0), FVector(0, 120, 0), FVector(0, -120, 0), FVector(120, 120, 0), FVector(-120, -120, 0), FVector(-120, 120, 0), FVector(120, -120, 0) };
	FChessPieceMoves KingMoves;
	KingMoves.Piece = Piece.Piece;
	KingMoves.Position = Piece.Position;
	ATile* StartTile = GField->GetTileAtPosition(Piece.Position);
	ETileStatus StartStatus = StartTile->GetTileStatus();

	// Per ogni direzione possibile
	for (const FVector& Direction : Directions)
	{
		FVector NextPosition = Piece.Position + Direction;

		FVector2D destination = GField->GetXYPositionByRelativeLocation(NextPosition);

		// Continua a muoversi lungo la direzione finché non raggiunge un ostacolo o il bordo della scacchiera
		if (GField->IsValidPosition(destination))
		{
			ATile* NextTile = GField->GetTileAtPosition(NextPosition);
			ETileStatus NextStatus = NextTile->GetTileStatus();

			// Se la prossima casella è vuota, aggiungi la posizione come possibile mossa
			if (NextStatus == ETileStatus::EMPTY)
			{
				KingMoves.PossibleMoves.Add(NextPosition);

			}
			else if ((StartStatus == ETileStatus::WHITE && NextStatus == ETileStatus::BLACK) || (StartStatus == ETileStatus::BLACK && NextStatus == ETileStatus::WHITE)) {
				KingMoves.PossibleMoves.Add(NextPosition);
			}
		}
	}
	if (KingMoves.PossibleMoves.Num() > 0) {
		if (StartStatus == ETileStatus::WHITE) {
			WhiteMoves.Add(KingMoves);
		}
		else {
			BlackMoves.Add(KingMoves);
		}
	}
}

//muove un pezzo
void AChess_GameMode::ChessPieceMoves(ABasePiece* SelectedPiece, FVector Location)
{
	// Verifica che il pezzo selezionato non sia nullo
	if (!SelectedPiece)
	{
		UE_LOG(LogTemp, Warning, TEXT("SelectedPiece is nullptr"));
		return;
	}
	bool IsWhite = false;
	bool Capture = false;
	FVector StartPosition = SelectedPiece->GetActorLocation();
	ATile* StartTile = GField->GetTileAtPosition(StartPosition);
	ETileStatus StartStatus = StartTile->GetTileStatus();
	if (StartStatus == ETileStatus::WHITE) {
		IsWhite = true;
	}
	ATile* DestTile = GField->GetTileAtPosition(Location);
	ETileStatus DestStatus = DestTile->GetTileStatus();

	if ((StartStatus == ETileStatus::WHITE && DestStatus == ETileStatus::BLACK) || (StartStatus == ETileStatus::BLACK && DestStatus == ETileStatus::WHITE)) {
	
		Capture = true;
		TArray<FChessPieceInfo>& PiecesList = IsWhite ? BlackPieces : WhitePieces;
		for (int32 Index = PiecesList.Num() - 1; Index >= 0; --Index)
		{
			FChessPieceInfo& PieceInfo = PiecesList[Index];
			if (PieceInfo.Position == Location)
			{
				PieceInfo.Piece->SelfDestroy();
				PiecesList.RemoveAt(Index);
				break;
			}
		}
	}
	// Sposta lo scacco alla nuova posizione
	SelectedPiece->SetActorLocation(Location);
	StartTile->SetTileStatus(ETileStatus::EMPTY);
	DestTile->SetTileStatus(StartStatus);

	//promozione pedone
	if ((SelectedPiece->GetPieceType() == EPieceType::WPawn) && (Location[0] == 840)) {
		for (int32 indey = WhitePieces.Num() - 1; indey >= 0; indey--) {
			if (WhitePieces[indey].Piece == SelectedPiece) {

				WhitePieces[indey].Piece->SelfDestroy();
				WhitePieces.RemoveAt(indey);
				SpawnPiece(WhiteQueen, Location + FVector(0, 0, -10));
			}
		}
	}
	if ((SelectedPiece->GetPieceType() == EPieceType::BPawn) && (Location[0] == 0)) {
		for (int32 indey = WhitePieces.Num() - 1; indey >= 0; indey--) {
			if (BlackPieces[indey].Piece == SelectedPiece) {

				BlackPieces[indey].Piece->SelfDestroy();
				BlackPieces.RemoveAt(indey);
				SpawnPiece(BlackQueen, Location + FVector(0, 0, -10));
			}
		}
	}

	TArray<FChessPieceInfo>& PiecesList = IsWhite ? WhitePieces : BlackPieces;
	for (FChessPieceInfo& PieceInfo : PiecesList)
	{
		if ((SelectedPiece->GetPieceType() == EPieceType::WPawn && Location[0] == 840) || (SelectedPiece->GetPieceType() == EPieceType::BPawn && Location[0] == 0)) {
			SelectedPiece->SelfDestroy();

		}
		if (PieceInfo.Piece == SelectedPiece)
		{
			PieceInfo.Position = Location;
			break;
		}
	}
	

	FChessboard Disposition;
	for (const FChessPieceInfo& currPiece : WhitePieces) {
		Disposition.WPieces.Add(currPiece);
	}
	for (const FChessPieceInfo& currPiece : BlackPieces) {
		Disposition.BPieces.Add(currPiece);
	}
	Positions.Add(Disposition);

	//NameMove(SelectedPiece, Capture);

	CalculateMoves();

	TArray<FChessPieceInfo>& EnemyPieces = IsWhite ? BlackPieces : WhitePieces;
	AvoidCheck(EnemyPieces[0]);
	TArray<FChessPieceMoves>& EnemyMoves = IsWhite ? BlackMoves : WhiteMoves;

	if ((OnCheck(EnemyPieces[0]) == true)&& (EnemyMoves.Num() < 1)) {
		IsGameOver = true;
		Players[CurrentPlayer]->OnWin();
		for (int32 i = 0; i < Players.Num(); i++)
		{
			if (i != CurrentPlayer)
			{
				Players[i]->OnLose();
			}
		}
	}
	else if ((OnCheck(EnemyPieces[0]) == false) && (EnemyMoves.Num() < 1)) {
		IsGameOver = true;
		Players[CurrentPlayer]->OnTie();
		for (int32 i = 0; i < Players.Num(); i++)
		{
			if (i != CurrentPlayer)
			{
				Players[i]->OnTie();
			}
		}
	}
	else {
		TurnNextPlayer();
	}

}
//evidenzia le mosse possibili
void AChess_GameMode::ShowMoves(ABasePiece* SelectedPiece)
{
	// Verifica che il pezzo selezionato non sia nullo
	if (!SelectedPiece)
	{
		UE_LOG(LogTemp, Warning, TEXT("SelectedPiece invalid"));
		return;
	}

	FVector Location = SelectedPiece->GetActorLocation();
	FChessPieceInfo StartPiece(SelectedPiece, Location);
	YellowPieces.Add(StartPiece);
	SpawnPiece(BlueSign, Location);

	for (const FChessPieceMoves& PieceMove : WhiteMoves)
	{
		if (PieceMove.Position == Location)
		{
			for (const FVector& MovePosition : PieceMove.PossibleMoves)
			{
				SpawnPiece(YellowSign, MovePosition);
			}
		}
	}
}

void AChess_GameMode::HideMoves()
{
	if (YellowPieces.Num() == 0)
	{
		return;
	}

	for (int32 index= YellowPieces.Num()-1; index>0; index--)
	{
		// Ottieni l'attore ABasePiece dalla struct FChessPieceInfo
		ABasePiece* Piece = YellowPieces[index].Piece;
		if (Piece)
		{
			Piece->SelfDestroy();
		}
	}

	YellowPieces.Empty();
}

//"simula" una mossa senza spostare gli attori
void AChess_GameMode::doMove(FVector Location, ABasePiece* Piece)
{
	if (!Piece)
	{
		UE_LOG(LogTemp, Warning, TEXT("SelectedPiece is nullptr"));
		return;
	}
	bool IsWhite = false;
	FVector StartPosition = Piece->GetActorLocation();
	ATile* StartTile = GField->GetTileAtPosition(StartPosition);
	ETileStatus StartStatus = StartTile->GetTileStatus();
	if (StartStatus == ETileStatus::WHITE) {
		IsWhite = true;
	}
	ATile* DestTile = GField->GetTileAtPosition(Location);
	ETileStatus DestStatus = DestTile->GetTileStatus();

	if ((StartStatus == ETileStatus::WHITE && DestStatus == ETileStatus::BLACK) || (StartStatus == ETileStatus::BLACK && DestStatus == ETileStatus::WHITE)) {
		TArray<FChessPieceInfo>& PiecesList = IsWhite ? BlackPieces : WhitePieces;
		for (int32 Index = PiecesList.Num() - 1; Index >= 0; --Index)
		{
			FChessPieceInfo& PieceInfo = PiecesList[Index];
			if (PieceInfo.Position == Location)
			{
				PiecesList.RemoveAt(Index);
				break;
			}
		}
	}
	// Sposta lo scacco alla nuova posizione
	StartTile->SetTileStatus(ETileStatus::EMPTY);
	DestTile->SetTileStatus(StartStatus);

	TArray<FChessPieceInfo>& PiecesList = IsWhite ? WhitePieces : BlackPieces;
	for (FChessPieceInfo& PieceInfo : PiecesList)
	{
		if (PieceInfo.Piece == Piece)
		{
			PieceInfo.Position = Location;
			break;
		}
	}
}

void AChess_GameMode::setChessboardAtIndex(int32 index)
{
	for (int32 indey = WhitePieces.Num() - 1; indey >= 0; indey--) {
		FChessPieceInfo& currPiece = WhitePieces[indey];
		currPiece.Piece->SelfDestroy();
		WhitePieces.RemoveAt(indey);
	}
	for (int32 indey = BlackPieces.Num() - 1; indey >= 0; indey--) {
		FChessPieceInfo& currPiece = BlackPieces[indey];
		currPiece.Piece->SelfDestroy();
		BlackPieces.RemoveAt(indey);
	}

	TArray<ATile*>& Tiles = GField->GetTileArray();
	for (ATile* currTile : Tiles) {
		currTile->SetTileStatus(ETileStatus::EMPTY);
	}
	for (const FChessPieceInfo& currPiece : Positions[index].WPieces) {
		FVector Location = currPiece.Position +FVector(0, 0, -10);
		TSubclassOf<ABasePiece> Type = currPiece.ChessPieceType;
		SpawnPiece(Type, Location);
		ATile* currTile = GField->GetTileAtPosition(Location);
		currTile->SetTileStatus(ETileStatus::WHITE);
	}
	for (const FChessPieceInfo& currPiece : Positions[index].BPieces) {
		FVector Location = currPiece.Position + FVector(0, 0, -10);
		TSubclassOf<ABasePiece> Type = currPiece.ChessPieceType;
		SpawnPiece(Type, Location);
		ATile* currTile = GField->GetTileAtPosition(Location);
		currTile->SetTileStatus(ETileStatus::BLACK);
	}
	CalculateMoves();
}

void AChess_GameMode::setPieces(TArray<FChessPieceInfo> x, TArray<FChessPieceInfo> y)
{
	WhitePieces = x;
	BlackPieces = y;
}

//nomenclatura mosse
void AChess_GameMode::NameMove(const ABasePiece* Piece, bool capture)
{
	FString name = FString::FromInt(MoveCounter);
	name = name + ".";
	EPieceType type = Piece->GetPieceType();
	if (type == EPieceType::Rook) name = name + "R";
	if (type == EPieceType::Bishop) name = name + "B";
	if (type == EPieceType::Knight) name = name + "N";
	if (type == EPieceType::Queen) name = name + "Q";
	if (type == EPieceType::King) name = name + "K";

	if (capture) name = name + "x";

	FVector Position = Piece->GetActorLocation();
	if (Position[1] == 0) name = name + "a";
	if (Position[1] == 120) name = name + "b";
	if (Position[1] == 240) name = name + "c";
	if (Position[1] == 360) name = name + "d";
	if (Position[1] == 480) name = name + "e";
	if (Position[1] == 600) name = name + "f";
	if (Position[1] == 720) name = name + "g";
	if (Position[1] == 840) name = name + "h";

	if (Position[0] == 0) name = name + "1";
	if (Position[0] == 120) name = name + "2";
	if (Position[0] == 240) name = name + "3";
	if (Position[0] == 360) name = name + "4";
	if (Position[0] == 480) name = name + "5";
	if (Position[0] == 600) name = name + "6";
	if (Position[0] == 720) name = name + "7";
	if (Position[0] == 840) name = name + "8";

	UChess_GameInstance* GameInstance = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GameInstance->SetMoveName(name);
}



