// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_GameMode.h"
#include "Chess_PlayerController.h"
#include "Chess_HumanPlayer.h"
#include "Chess_RandomPlayer.h"
#include "Chess_MinMaxPlayer.h"
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

	// Spawn di AGameField solo se non è già stato fatto
	if (GameFieldClass != nullptr && !bFieldGenerated && !bPiecesGenerated)
	{
		UE_LOG(LogTemp, Error, TEXT("Game Field is NOT NULL    OK"));
		
		GField = GetWorld()->SpawnActor<AGameField>(GameFieldClass);
		GField->Size = FieldSize;	


		bFieldGenerated = true;
		bPiecesGenerated = true;


		//bFieldGenerated = true;


		// Check if the field has already been generated
		//if (!GField->bFieldGenerated && !GField->bPiecesGenerated)
		//{

			
			
			 
		//	if (!GField->bFieldGenerated) 
		//	{
				//GField->Size = FieldSize;
				
				
				// Generazione del campo di gioco
		//		GField->GenerateField();

				// bool per controllare che la gamefield venga spawnata una sola volta
		//		bFieldGenerated = true;

				// Impostazione del flag di generazione del campo di gioco su true
				//GField->
			//	GField->bFieldGenerated = true;

				// Generazione dei pezzi solo se non sono già stati generati
		//		if (!GField->bPiecesGenerated)
		//		{
		//			GField->GeneratePieces();

					// bool per controllare che i pezzi vengano spawnati una sola volta
		//			GField->bPiecesGenerated = true;

					// Impostazione del flag di generazione dei pezzi su true
					//GField->
		//			bPiecesGenerated = true;
		//		}
				//GField->GeneratePieces();											// grande passo per maria, piccolo passo per l'umanità
			//}
		//	}
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

	//non uso ma tengo per ora
	//metto la camera al di sopra della scacchiera e puntata verso il basso ortogonalmente
	/*
	UCameraComponent* Camera = HumanPlayer->Camera;
	if (Camera != nullptr)
	{
		FRotator CameraRotation = Camera->GetComponentRotation();
		//la pitch va settata a -90 gradi
		CameraRotation.Pitch = -90.0f;
		//imposto la nuova rotazione per la fotocamera
		Camera->SetWorldRotation(CameraRotation);

		float CameraPosX = ((GField->TileSize * (FieldSize + ((FieldSize - 1) * GField->NormalizedCellPadding) - (FieldSize - 1))) / 2) - (GField->TileSize / 2);
		FVector CameraPos(CameraPosX, CameraPosX, 1000.0f);
		HumanPlayer->SetActorLocationAndRotation(CameraPos, FRotationMatrix::MakeFromX(FVector(0, 0, -1)).Rotator());
	}

	else
	{
		UE_LOG(LogTemp, Error, TEXT("Camera pointer is null! Check the configuration."));

	}
	*/


	// Human player = 0
	Players.Add(HumanPlayer);
	// Random Player
	auto* AI = GetWorld()->SpawnActor<AChess_RandomPlayer>(FVector(), FRotator());

	// MiniMax Player
	//auto* AI = GetWorld()->SpawnActor<AChess_MinMaxPlayer>(FVector(), FRotator());

	// AI player = 1
	Players.Add(AI);

	this->ChoosePlayerAndStartGame();
}

void AChess_GameMode::ChoosePlayerAndStartGame()
{
	//CurrentPlayer = FMath::RandRange(0, Players.Num() - 1);
	CurrentPlayer = 0;

	Players[0]->PlayerNumber = 0;
	Players[0]->ColorOfPieces = EColorOfPieces::WHITE;
	Players[1]->PlayerNumber = 1;
	Players[1]->ColorOfPieces = EColorOfPieces::BLACK;

	MoveCounter += 1;
	Players[CurrentPlayer]->OnTurn();
	//MakeMove();
}

void AChess_GameMode::MakeMove()
{
	////????

	TurnNextPlayer();
}


void AChess_GameMode::SetCellSign(const int32 PlayerNumber, const FVector& SpawnPosition)
{
	if (IsGameOver || PlayerNumber != CurrentPlayer)
	{
		return;
	}

	//UClass* SignActor = Players[CurrentPlayer]->Sign == ESign::X ? SignXActor : SignOActor;
	//FVector Location = GField->GetActorLocation() + SpawnPosition + FVector(0, 0, 10);
	//GetWorld()->SpawnActor(SignActor, &Location);

	if (GField->IsWinPosition(GField->GetXYPositionByRelativeLocation(SpawnPosition)))
	{
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
	else if (MoveCounter == (FieldSize * FieldSize))
	{
		// add a timer (3 seconds)
		FTimerHandle TimerHandle;

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
			{
				// function to delay
				GField->ResetField();
			}, 3, false);
	}
	else
	{
		TurnNextPlayer();
	}
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


AGameField* AChess_GameMode::GetGField() const
{
	return GField;
}

