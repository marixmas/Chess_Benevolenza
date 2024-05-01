// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_GameMode.h"
#include "Chess_PlayerController.h"
#include "Chess_HumanPlayer.h"
#include "Chess_RandomPlayer.h"
#include "EngineUtils.h"


AChess_GameMode::AChess_GameMode()
{
	PlayerControllerClass = AChess_PlayerController::StaticClass();
	DefaultPawnClass = AChess_HumanPlayer::StaticClass();
	FieldSize = 8;

	// bool per controllare che la gamefield venga spawnata una sola volta
	bFieldGenerated = false;
	// bool per controllare che i pezzi vengano spawnati una sola volta
	bPiecesGenerated = false;

	// inizializzo il puntatore alla GameField a null
	GField = nullptr;

	//ClonedGameField = nullptr;


	///Draw = false;/////////////////////////////////////////////////////////////

}


void AChess_GameMode::BeginPlay()
{
	Super::BeginPlay();

	// inizializzo alcuni valori
	IsGameOver = false;
	MoveCounter = 0;

	AChess_HumanPlayer* HumanPlayer = Cast<AChess_HumanPlayer>(*TActorIterator<AChess_HumanPlayer>(GetWorld()));

	// Spawn di AGameField e dei Pezzi solo se non è già stato fatto
	if (GameFieldClass != nullptr && !bFieldGenerated && !bPiecesGenerated)
	{
		UE_LOG(LogTemp, Error, TEXT("Game Field is NOT NULL    OK"));
		
		GField = GetWorld()->SpawnActor<AGameField>(GameFieldClass);
		GField->GenerateField();
		GField->Size = FieldSize;	

		bFieldGenerated = true;
		bPiecesGenerated = true;

		GField->GeneratePieces();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Game Field is null"));
	}

	if (GField != nullptr && GField->TileSize > 0 && FieldSize > 0)
	{
		//float CameraPosX = (GField->TileSize * FieldSize) / 2;
		float CameraPosX = ((GField->TileSize * (FieldSize)) / 2);
		FVector CameraPos(CameraPosX, CameraPosX, 1600.0f);
		HumanPlayer->SetActorLocationAndRotation(CameraPos, FRotationMatrix::MakeFromX(FVector(0, 0, -1)).Rotator());
	}
	else
	{
		if (GField == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Game Field is null"));
		}
		else if (GField->TileSize <= 0)
		{
			UE_LOG(LogTemp, Error, TEXT("TileSize is not valid"));
		}
		else if (FieldSize <= 0)
		{
			UE_LOG(LogTemp, Error, TEXT("FieldSize is not valid"));
		}
	}

	/*
	float CameraPosX = ((GField->TileSize * (FieldSize)) / 2);																			// metto dentro su dove ho fatto i controlli
	FVector CameraPos(CameraPosX, CameraPosX, 1600.0f);
	HumanPlayer->SetActorLocationAndRotation(CameraPos, FRotationMatrix::MakeFromX(FVector(0, 0, -1)).Rotator());

	*/
	

	// aggiungo all'array dei giocatori i due player
	// Human player = 0
	Players.Add(HumanPlayer);

	// Random Player
	auto* AI = GetWorld()->SpawnActor<AChess_RandomPlayer>(FVector(), FRotator());

	// AI player = 1
	Players.Add(AI);

	this->ChooseHumanPlayerAndStartGame();
}


AGameField* AChess_GameMode::GetGField() const
{
	return GField;
}


void AChess_GameMode::ChooseHumanPlayerAndStartGame()
{
	CurrentPlayer = 0;

	Players[0]->PlayerNumber = 0;
	Players[0]->ColorOfPieces = EColorOfPieces::WHITE;
	Players[1]->PlayerNumber = 1;
	Players[1]->ColorOfPieces = EColorOfPieces::BLACK;

	MoveCounter += 1;

	// il  primo turno è del giocatore umano
	Players[CurrentPlayer]->OnTurn();
}

int32 AChess_GameMode::GetCurrentPlayer()
{
	return CurrentPlayer;
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

/*

bool AChess_GameMode::IsKingInCheck(int32 Player)
{
	// ottengo l'array dei pezzi del colore del giocatore di cui voglio controllare lo Scacco
	//AGameField* GField = GetGField();
	ColorPiecesArray = nullptr;

	if (Player == 0)
	{
		ColorPiecesArray = &(GField->GetWhitePiecesArray());
	}
	else if (Player == 1)
	{
		ColorPiecesArray = &(GField->GetBlackPiecesArray());
	}
     // ritorno false se l'array è nullo
	if (!ColorPiecesArray)
	{
		UE_LOG(LogTemp, Error, TEXT("Array di pezzi nullo!"));
		return false;
	}

	// cerco il re nel colore corrispondente
	King = nullptr;
	for (AChess_Piece* Piece : *ColorPiecesArray)
	{
		if (Piece->GetPieceType() == EPieceType::KING)
		{
			King = Piece;
			break;
		}
	}
	 // ritorno false se non viene trovato il re
	if (!King)
	{
		UE_LOG(LogTemp, Error, TEXT("Re non trovato tra le pedine del giocatore numero %d"), Player);
		return false;
	}

	// Ottieni la posizione del re
	KingPosition = King->GetGridPosition();


	// prendo array dei pezzi dell'avversario 
	OpponentColorPiecesArray = nullptr;

	if (Player == 0)
	{
		OpponentColorPiecesArray = &(GField->GetBlackPiecesArray());
	}
	else if (Player == 1)
	{
		OpponentColorPiecesArray = &(GField->GetWhitePiecesArray());
	}
	// ritorno false se l'array è nullo
	if (!OpponentColorPiecesArray)
	{
		UE_LOG(LogTemp, Error, TEXT("Array di pezzi dell'avversario nullo!"));
		return false;
	}

	// scansiono tutti i pezzi dell'avversario per controllare se minacciano il re
	for (AChess_Piece* OpponentPiece : *OpponentColorPiecesArray)
	{
		// cast del pezzo per avere l'opportuna funzione overridata per il calcolo delle mosse
		AChess_Piece* CastedOpponentPiece = Cast<AChess_Piece>(OpponentPiece);

		// Ottieni le mosse possibili del pezzo avversario
		TArray<FVector2D> PossibleMoves = CastedOpponentPiece->CalculatePossibleMoves();

		// Controlla se le mosse possibili includono la posizione del re
		if (PossibleMoves.Contains(KingPosition))
		{
			// Il re è sotto scacco
			return true;
		}
	}

	// Il re non è sotto scacco
	return false;

}

*/
/*
bool AChess_GameMode::IsCheckmate(int32 Player)
{
	

	// ottengo l'array dei pezzi del colore del giocatore di cui voglio controllare lo Scacco Matto
	//AGameField* GField = GetGField();

	if (Player == 0)
	{
		CopyOfColorPiecesArray = (GField->GetCopyOfWhitePiecesArray());
	}
	else if (Player == 1)
	{
		CopyOfColorPiecesArray = (GField->GetCopyOfBlackPiecesArray());
	}
	// ritorno false se l'array è nullo
	if (CopyOfColorPiecesArray.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Array di pezzi nullo!"));
		return false;
	}

	// cerco il re nel colore corrispondente
	King = nullptr;
	for (AChess_Piece* Piece : CopyOfColorPiecesArray)
	{
		if (Piece->GetPieceType() == EPieceType::KING)
		{
			King = Piece;
			break;
		}
	}
	// ritorno false se non viene trovato il re
	if (!King)
	{
		UE_LOG(LogTemp, Error, TEXT("Re non trovato tra le pedine del giocatore numero %d"), Player);
		return false;
	}

	// Ottieni la posizione del re
	KingPosition = King->GetGridPosition();
	

	// Controlla se il re del giocatore è in scacco
	if (!IsKingInCheck(Player))
	{
		return false; // Il re non è in scacco, quindi non può essere scacco matto
	}

	// Controlla se il giocatore non ha mosse legali che possono salvare il re dallo scacco

	// creo una copia della scacchiera per i calcoli per lo scacco matto
	AGameField* ClonedField = GField->CloneEmptyGameField();

	for (AChess_Piece* Piece : CopyOfColorPiecesArray)
	{
		TArray<FVector2D> PossibleMoves = (Cast<AChess_Piece>(Piece))->CalculatePossibleMoves();

		for (FVector2D Move : PossibleMoves)
		{
			// Creazione di una copia della scacchiera all'inizio di ogni iterazione
			//AGameField* ClonedField = GField->CloneGameField();

			// creo copia di tutti i pezzi a ogni iterazione

			GField->CloneAllPiecesToField(ClonedField);
			// Muove il pezzo
			Piece->MovePieceFromToPosition(Piece->GetGridPosition(), Move);

			// Controlla se il re non è più in scacco dopo la mossa
			if (!IsKingInCheck(Player))
			{
				for (AChess_Piece* ClonedPiece : ClonedField->PiecesArray)
				{
					ClonedPiece->Destroy();
				}
				ClonedField->Destroy();
				return false; // Il re può evitare lo scacco matto
			}

			for (AChess_Piece* ClonedPiece : ClonedField->PiecesArray)
			{
				ClonedPiece->Destroy();
			}
			
		}
	}
	ClonedField->Destroy();
	// Nessuna mossa legale può salvare il re dallo scacco, quindi è scacco matto
	return true;
}

*/

/*
bool AChess_GameMode::IsDraw(int32 Player)
{
	// Verifica se il giocatore corrente non ha mosse legali disponibili
	if (!HasLegalMoves(Player))
	{
		// Verifica se il re del giocatore corrente non è in scacco
		if (!IsKingInCheck(Player))
		{
			return true; // Patta per stallo
		}
	}

	// Verifica se la stessa posizione della scacchiera si ripete per tre volte
	if (IsThreefoldRepetition())
	{
		return true; // Patta per ripetizione della posizione
	}

	// Verifica se non è stata effettuata alcuna cattura o alcuna mossa di pedone nei precedenti 50 movimenti
	if (IsFiftyMovesRuleDraw())
	{
		return true; // Patta per regola dei 50 movimenti
	}

	// Altre condizioni di patta possono essere aggiunte qui

	return false; // Nessuna condizione di patta è soddisfatta
}
*/

/*
AGameField* AChess_GameMode::GetClonedGameField() const
{
	return ClonedGameField;
}
*/



/*
AChess_Piece* AChess_GameMode::GetPieceToMoveToExitTheCheck()										///////////////////////
{
	return PieceToMoveToExitTheCheck;
}
*/

/*
FVector2D AChess_GameMode::GetMoveToExitTheCheck()																	////////////////////////////////////////////
{
	return MoveToExitTheCheck;;
}



*/









/*

TArray<FEscapeMoveInfo> CalculateEscapesMoves(AGameField* GameField, int32 PlayerInCheck)
{
	TArray<FEscapeMoveInfo> EscapeMoves;

	// ottengo le mosse possibili disponibili per il giocatore
	TArray<FEscapeMoveInfo> PossibleMoves = GetPossibleMoves(GameField, PlayerInScacco);

	// Filtra le mosse per rimuovere quelle che non fanno uscire il re dallo scacco
	for (const FEscapeMoveInfo& MoveInfo : PossibleMoves)
	{
		if (!IsKingInCheckAfterMove(GameField, MoveInfo.Move, PlayerInScacco))
		{
			EscapeMoves.Add(MoveInfo);
		}
	}

	return EscapeMoves;
}

*/


bool AChess_GameMode::IsKingInCheck(AGameField* GenericGameField, int32 OpponentPlayer)
{
	// ottengo l'array dei pezzi del colore del giocatore di cui voglio controllare lo Scacco
	if (OpponentPlayer == 0)
	{
		//TArray <AChess_Piece*> OpponentPiecesArray = WhitePiecesArray;

		// cerco il re nel colore corrispondente
		King = nullptr;
		for (AChess_Piece* Piece : GenericGameField->WhitePiecesArray)
		{
			if (Piece->GetPieceType() == EPieceType::KING)
			{
				King = Piece;
				break;
			}
		}
	}
	else if (OpponentPlayer == 1)
	{
		// cerco il re nel colore corrispondente
		King = nullptr;
		for (AChess_Piece* Piece : GenericGameField->BlackPiecesArray)
		{
			if (Piece->GetPieceType() == EPieceType::KING)
			{
				King = Piece;
				break;
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("giocatore su cui controllare lo scacco non valido"));
	}
	
	// ritorno false se non viene trovato il re
	if (!King)
	{
		UE_LOG(LogTemp, Error, TEXT("nella funzione di scacco: Re non trovato tra le pedine del giocatore numero %d"), OpponentPlayer);
		return false;
	}

	// Ottieni la posizione del re avversario
	KingPosition = King->GetGridPosition();

	// prendo array dei pezzi del giocatore principale (quello che ha chiamato la funzione IsInCheck sull'avversario)

	if (OpponentPlayer == 0)
	{
		// scansiono tutti i pezzi del player 1 per controllare se minacciano il re
		for (AChess_Piece* PlayerPiece : GenericGameField->BlackPiecesArray)
		{
			// cast del pezzo per avere l'opportuna funzione overridata per il calcolo delle mosse
			AChess_Piece* CastedPlayerPiece = Cast<AChess_Piece>(PlayerPiece);

			// Ottieni le mosse possibili del pezzo avversario
			TArray<FVector2D> PossibleMoves = CastedPlayerPiece->CalculatePossibleMoves();

			// Controlla se le mosse possibili includono la posizione del re
			if (PossibleMoves.Contains(KingPosition))
			{
				// Il re è sotto scacco
				return true;
			}
		}

	}
	else if (OpponentPlayer == 1)
	{
		// scansiono tutti i pezzi dell'avversario per controllare se minacciano il re
		for (AChess_Piece* PlayerPiece : GenericGameField->WhitePiecesArray)
		{
			// cast del pezzo per avere l'opportuna funzione overridata per il calcolo delle mosse
			AChess_Piece* CastedPlayerPiece = Cast<AChess_Piece>(PlayerPiece);

			// Ottieni le mosse possibili del pezzo avversario
			TArray<FVector2D> PossibleMoves = CastedPlayerPiece->CalculatePossibleMoves();

			// Controlla se le mosse possibili includono la posizione del re
			if (PossibleMoves.Contains(KingPosition))
			{
				// Il re è sotto scacco
				return true;
			}
		}

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("giocatore su cui controllare lo scacco non valido"));
	}

	// Il re non è sotto scacco
	return false;

}





// quella vecchia che per ora va bene



/*
bool AChess_GameMode::IsCheckmate(/AGameField* GenericGameField, */ /*int32 OpponentPlayer)
{
	// Controlla se il re del giocatore è in scacco
	if (!IsKingInCheck(GField, OpponentPlayer))
	{
		return false; // Il re non è in scacco, quindi non può essere scacco matto
	}


// Controlla se il giocatore non ha mosse legali che possono salvare il re dallo scacco

	// creo una copia della scacchiera per i calcoli per lo scacco matto
	//ClonedGameField = GetGField()->CloneEmptyGameField();
	//GetGField()->CloneAllPiecesToField(ClonedGameField);

	// ottengo l'array dei pezzi del colore del giocatore di cui voglio controllare lo Scacco
	if (OpponentPlayer == 0)
	{
		for (AChess_Piece* Piece : GField->WhitePiecesArray)
		{
			TArray<FVector2D> PossibleMoves = (Cast<AChess_Piece>(Piece))->CalculatePossibleMoves();

			for (FVector2D Move : PossibleMoves)
			{

				if (!GField->SimulateMoveAndCheck(Piece, Move))
				{
					//MoveToExitTheCheck = Move;						// tengo da parte per il random player        noo mi sa che é sbagliato
					//PieceToMoveToExitTheCheck = Piece;
					return false;
				}
			}
		}
	}
	else if (OpponentPlayer == 1)
	{
		for (AChess_Piece* Piece : GField->BlackPiecesArray)
		{
			TArray<FVector2D> PossibleMoves = (Cast<AChess_Piece>(Piece))->CalculatePossibleMoves();

			for (FVector2D Move : PossibleMoves)
			{

				if (!GField->SimulateMoveAndCheck(Piece, Move))
				{
					MoveToExitTheCheck = Move;						// tengo da parte per il random player
					PieceToMoveToExitTheCheck = Piece;
					return false;
				}

			}
		}
	}

	else
	{
		UE_LOG(LogTemp, Error, TEXT("giocatore su cui viene indagato lo scacco non valido"));
	}

	//ClonedGameField->Destroy();

	// Nessuna mossa legale può salvare il re dallo scacco, quindi è scacco matto
	// passo una mossa IMpossibile per uscire dallo scacco al giocatore in scacco

	MoveToExitTheCheck = FVector2D(100, 100);
	PieceToMoveToExitTheCheck = nullptr;
	return true;
}

*/
// vecchia sostituita probab (sopra)

/*
bool AChess_GameMode::IsDraw(int32 Player)
{
	// Verifica se il giocatore corrente non ha mosse legali disponibili
	if (!HasLegalMoves(Player))
	{
		// Verifica se il re del giocatore corrente non è in scacco
		if (!IsKingInCheck(GField, Player))
		{
			return true; // Patta per stallo
		}
	}

	return false; // Nessuna condizione di patta è soddisfatta
}


*/
/*
bool AChess_GameMode::HasLegalMoves(int32 Player)
{


	// Ottieni l'array dei pezzi del giocatore corrente
	TArray<AChess_Piece*> PlayerPieces;
	if (Player == 0)
	{
		PlayerPieces = WhitePiecesArray;
	}
	else if (Player == 1)
	{
		PlayerPieces = BlackPiecesArray;
	}

	// Scansiona tutti i pezzi del giocatore corrente
	for (AChess_Piece* Piece : PlayerPieces)
	{
		// Ottieni le mosse possibili del pezzo
		TArray<FVector2D> PossibleMoves = (Cast<AChess_Piece>(Piece))->CalculatePossibleMoves();

		// Controlla se ci sono mosse legali disponibili per questo pezzo
		for (FVector2D Move : PossibleMoves)
		{
			// Simula la mossa e controlla se il re è in scacco dopo la mossa
			AChess_Piece* PieceAtDestination = this->PiecesMap[Move];
			if (!PieceAtDestination || PieceAtDestination->GetPieceColor() != Piece->GetPieceColor())
			{
				// Simula la mossa
				FVector2D OriginalPosition = Piece->GetGridPosition();
				Piece->SetGridPosition(Move);

				// Controlla se il re è in scacco dopo la mossa
				if (!IsKingInCheck(Player))
				{
					// Reverti la mossa
					Piece->SetGridPosition(OriginalPosition);
					return true; // Ci sono mosse legali disponibili
				}

				// Reverti la mossa
				Piece->SetGridPosition(OriginalPosition);
			}
		}
	}

	// Non ci sono mosse legali disponibili
	return false;
	//////////
	// Verifica se il giocatore corrente è in scacco matto
	if (IsCheckmate(GField, Player))
	{
		return false; // Il giocatore è in scacco matto, quindi non ci sono mosse legali disponibili
	}

	// Il giocatore non è in scacco matto, quindi ci sono mosse legali disponibili
	return true;
	
}
*/


