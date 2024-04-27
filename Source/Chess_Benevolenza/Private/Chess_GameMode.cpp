// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_GameMode.h"
#include "Chess_PlayerController.h"
#include "Chess_HumanPlayer.h"
#include "Chess_RandomPlayer.h"
#include "EngineUtils.h"


AChess_GameMode::AChess_GameMode()
{
	PlayerControllerClass = AChess_PlayerController::StaticClass();				//punta all'oggetto che rappresenta la classe AChess_Player controller
	DefaultPawnClass = AChess_HumanPlayer::StaticClass();						//same
	FieldSize = 8;

	// bool per controllare che la gamefield venga spawnata una sola volta
	bFieldGenerated = false;
	// bool per controllare che i pezzi vengano spawnati una sola volta
	bPiecesGenerated = false;

	GField = nullptr;

}


void AChess_GameMode::BeginPlay()
{
	Super::BeginPlay();

	IsGameOver = false;
	MoveCounter = 0;

	AChess_HumanPlayer* HumanPlayer = Cast<AChess_HumanPlayer>(*TActorIterator<AChess_HumanPlayer>(GetWorld()));

	// Spawn di AGameField e dei Pezzi solo se non è già stato fatto
	if (GameFieldClass != nullptr && !bFieldGenerated && !bPiecesGenerated)
	{
		UE_LOG(LogTemp, Error, TEXT("Game Field is NOT NULL    OK"));
		
		GField = GetWorld()->SpawnActor<AGameField>(GameFieldClass);
		GField->Size = FieldSize;	

		bFieldGenerated = true;
		bPiecesGenerated = true;

		GField->GeneratePieces();

	}

		
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Game Field is null"));
	}

	//NON useremo NormalizedPadding
	//float CameraPosX = ((GField->TileSize * (FieldSize + ((FieldSize - 1) * GField->NormalizedCellPadding) - (FieldSize - 1))) / 2) - (GField->TileSize / 2);

	//alcune righe per il debug
	if (GField != nullptr && GField->TileSize > 0 && FieldSize > 0)
	{
		float CameraPosX = (GField->TileSize * FieldSize) / 2;
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
	//fino a qui

	float CameraPosX = ((GField->TileSize * (FieldSize)) / 2);
	FVector CameraPos(CameraPosX, CameraPosX, 1600.0f);
	HumanPlayer->SetActorLocationAndRotation(CameraPos, FRotationMatrix::MakeFromX(FVector(0, 0, -1)).Rotator());



	// Human player = 0
	Players.Add(HumanPlayer);
	// Random Player
	auto* AI = GetWorld()->SpawnActor<AChess_RandomPlayer>(FVector(), FRotator());

	// AI player = 1
	Players.Add(AI);

	this->ChooseHumanPlayerAndStartGame();
}

void AChess_GameMode::ChooseHumanPlayerAndStartGame()
{
	CurrentPlayer = 0;

	Players[0]->PlayerNumber = 0;
	Players[0]->ColorOfPieces = EColorOfPieces::WHITE;
	Players[1]->PlayerNumber = 1;
	Players[1]->ColorOfPieces = EColorOfPieces::BLACK;

	MoveCounter += 1;
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
void AChess_GameMode::EatenKing(bool IsKingEaten)
{
	
}
*/

void AChess_GameMode::CheckIfKingIsEaten()
{
	AGameField* GameField = GetGField();
	if (!GameField)
	{
		UE_LOG(LogTemp, Error, TEXT("GField non valido in CheckIfKingIsEaten"));
		return;
	}

	bool IsWhiteKingEaten = false;
	bool IsBlackKingEaten = false;

	// Scansiona la mappa dei pezzi per verificare se il re bianco o nero è stato mangiato
	for (auto& PieceEntry : GameField->PiecesMap)
	{
		AChess_Piece* Piece = PieceEntry.Value;
		if (!Piece)
		{
			continue;
		}

		if (Piece->GetPieceType() == EPieceType::KING)
		{
			if (Piece->GetPieceColor() == EPieceColor::WHITE)
			{
				IsWhiteKingEaten = true;
			}
			else if (Piece->GetPieceColor() == EPieceColor::BLACK)
			{
				IsBlackKingEaten = true;
			}
		}
	}



	// Esegui le azioni appropriate in base a se il re bianco o nero è stato mangiato
	if (IsWhiteKingEaten)
	{
		
		Players[0]->OnLose();
		Players[1]->OnWin();


		
	}

	if (IsBlackKingEaten)
	{
		Players[1]->OnLose();
		Players[0]->OnWin();
	}
}


AGameField* AChess_GameMode::GetGField() const
{
	return GField;
}

