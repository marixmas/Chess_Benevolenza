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

bool AChess_GameMode::IsKingInCheck(AGameField* GenericGameField, int32 OpponentPlayer)
{
	// ottengo l'array dei pezzi del colore del giocatore di cui voglio controllare lo Scacco
	if (OpponentPlayer == 0)
	{
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


