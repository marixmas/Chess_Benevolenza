// Fill out your copyright notice in the Description page of Project Settings.

//DA CONTROLLARE LE FUNZIONI DI ONTURN ONWIN ONLOSE ON CLICK^?^?^^^?**************  V V V V V V V VVVVvvvvvvvv

#include "Chess_HumanPlayer.h"
#include "GameField.h"
#include "Tile.h"
#include "Chess_GameMode.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


// Sets default values
AChess_HumanPlayer::AChess_HumanPlayer()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set this pawn to be controlled by the lowest-numbered player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	// create a camera component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	//set the camera as RootComponent
	SetRootComponent(Camera);

	//metto la camera al di sopra della scacchiera e puntata verso il basso ortogonalmente				messe in gamemode.cpp!!
	//FRotator CameraRotation = Camera->GetComponentRotation();
	//la pitch va settata a -90 gradi
	//CameraRotation.Pitch = -90.0f;
	//imposto la nuova rotazione per la fotocamera
	//Camera->SetWorldRotation(CameraRotation);

	//float CameraPosX = ((GField->TileSize * (FieldSize + ((FieldSize - 1) * GField->NormalizedCellPadding) - (FieldSize - 1))) / 2) - (GField->TileSize / 2);
	//FVector CameraPos(CameraPosX, CameraPosX, 1000.0f);
	//HumanPlayer->SetActorLocationAndRotation(CameraPos, FRotationMatrix::MakeFromX(FVector(0, 0, -1)).Rotator());


	// get the game instance reference
	GameInstance = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	// default init values
	PlayerNumber = -1;
	ColorOfPieces = EColorOfPieces::NONE;

}

// Called when the game starts or when spawned
void AChess_HumanPlayer::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AChess_HumanPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AChess_HumanPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AChess_HumanPlayer::OnTurn()
{
	IsMyTurn = true;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Your Turn"));
	GameInstance->SetTurnMessage(TEXT("Human Turn"));
}

void AChess_HumanPlayer::OnWin()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("You Win!"));
	GameInstance->SetTurnMessage(TEXT("Human Wins!"));
	GameInstance->IncrementScoreHumanPlayer();
}

void AChess_HumanPlayer::OnLose()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("You Lose!"));
	GameInstance->SetTurnMessage(TEXT("Human Loses!"));
}

void AChess_HumanPlayer::OnClick()
{
	//Structure containing information about one hit of a trace, such as point of impact and surface normal at that point
	FHitResult Hit = FHitResult(ForceInit);
	// GetHitResultUnderCursor function sends a ray from the mouse position and gives the corresponding hit results
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);
	// se qualcosa è stato cliccato durante il mio turno
	if (Hit.bBlockingHit && IsMyTurn)
	{
		//AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
		//AGameField* GField = GameMode->GetGField();

		// Verifica se GameMode è valido
		AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
		if (!GameMode)
		{
			UE_LOG(LogTemp, Error, TEXT("GameMode non valido in HighlightGameFieldTiles!"));
			return;
		}

		// Verifica se GField è valido
		AGameField* GField = GameMode->GetGField();
		if (!GField)
		{
			UE_LOG(LogTemp, Error, TEXT("GField non valido in HighlightGameFieldTiles!"));
			return;
		}


		// se un pezzo è stato cliccato
		if (AChess_Piece* CurrPiece = Cast<AChess_Piece>(Hit.GetActor()))
		{
			if (CurrPiece->GetPieceColor() == EPieceColor::WHITE)
			{
				// imposto il pezzo selezionato come pezzo attuale del giocatore umano
				SelectedWhitePiece = CurrPiece;

				// indico che è stato selezionato un pezzo
				bPieceSelected = true;

				InfoOfClickedPiece(CurrPiece);

				// chiedo alla classe del singolo pezzo di calcolarmi le mosse possibili
				PossibleMoves = CurrPiece->CalculatePossibleMoves();

				// illumino le tile che sono le possibili nuove posizioni del pezzo sulla scacchiera
				HighlightGameFieldTiles(PossibleMoves);						//HighlightGameFieldTiles(PossibleMoves, GField);
			}

			else if (CurrPiece->GetPieceColor() == EPieceColor::BLACK)
			{
				SelectedBlackPiece = CurrPiece;

				if (bPieceSelected == false)
				{
					GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, TEXT("Queste sono le pedine del tuo avversario!"));
				}

				else if (bPieceSelected == true)
				{
					BlackPieceLocation = SelectedBlackPiece->GetGridPosition();
					
					if (PossibleMoves.Contains(BlackPieceLocation))
					{
						SelectedBlackPiece->Destroy();

						MoveSelectedPiece(BlackPieceLocation);
						
						GameMode->TurnNextPlayer();


					}
				}
			}
		}

		else if (ATile* SelectedTile = Cast<ATile>(Hit.GetActor()))
		{
			// se un pezzo era già stato cliccato e la tile è nelle mosse possibili...
			if (bPieceSelected == true && PossibleMoves.Contains(SelectedTile->GetGridPosition()))
			{
				//....allora muovere pedina nella tile selezionata
				MoveSelectedPiece(SelectedTile->GetGridPosition());

				// Resetta lo stato di selezione
				bPieceSelected = false;
				CurrPiece = nullptr;

			}
			// se un pezzo era già stato cliccato e la tile NON è nelle mosse possibili, no si può
			else if (bPieceSelected == true && !PossibleMoves.Contains(SelectedTile->GetGridPosition()))
			{
				GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, TEXT("La pedina selezionata non può andare in questa casella."));
				// non cambio bSelectedPiece e nemmeno SelectedPiece
			}

		}
		
		

	}
	else
		{
			GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, TEXT("Per favore, aspetta il tuo turno"));
		}
}


 void AChess_HumanPlayer::HighlightGameFieldTiles(const TArray<FVector2D>& TilePositions)				//c'era anche AChess_GameMode* GameMode, AGameField* GField
{
	 AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
	 AGameField* GField = GameMode->GetGField();
	 // Verifica se GField è valido
	 if (!GameMode || !GameMode->GField)							//la condizione era !GameMode || !GameMode->GField
	 {
		 UE_LOG(LogTemp, Error, TEXT("GField o GameMode non valido per fare HighlightGameFieldTiles!"));
		 
	 }

	// Loop attraverso le posizioni delle caselle
	for (const FVector2D& TilePosition : TilePositions)
	{
		// Trasforma le coordinate della casella in coordinate del mondo
		//FVector WorldLocation = TransformTileToWorld(TilePosition);											//inutile mi sa??

		// Illumina la casella utilizzando un materiale o un effetto di luce
		// Ad esempio, puoi cambiare il colore del materiale o aggiungere una luce sopra la casella
		
		ATile* Tile = GField->TileMap.FindRef(TilePosition);
		if (Tile)
		{
			// Applica il materiale di evidenziazione alla casella
			Tile->SetTileMaterial(GField->TileHighlightMaterial);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Materiale di Highlight non valido."));
		}
	}
}

 /*
FVector AChess_HumanPlayer::TransformTileToWorld(const FVector2D& TilePosition)
{
	// Implementa la logica per trasformare le coordinate della casella nella posizione del mondo
	// Ad esempio, puoi mappare le coordinate della scacchiera 2D a coordinate 3D nel mondo di gioco
	// ...
}
*/
/*
FVector2D AChess_HumanPlayer::GetTileCoordinatesFromHit(const FHitResult& Hit)
{
	// Implementa la logica per ottenere le coordinate della casella dalla hit result
	// Ad esempio, potresti usare le informazioni della hit result per trovare la casella più vicina

	// Esempio di implementazione di base:
	FVector HitLocation = Hit.ImpactPoint;
	// Converti la posizione del colpo in coordinate locali del campo di gioco
	FVector LocalHitLocation = GField->GetActorTransform().InverseTransformPosition(HitLocation);
	// Calcola le coordinate della casella (arrotondate)
	FVector2D TileCoordinates(FMath::RoundToInt(LocalHitLocation.X), FMath::RoundToInt(LocalHitLocation.Y));
	return TileCoordinates;
}
*/
void AChess_HumanPlayer::MoveSelectedPiece(const FVector2D& NewPosition)
{

	// Assicurati che ci sia un pezzo selezionato
	if (SelectedWhitePiece)
	{
		// Effettua il movimento del pezzo selezionato alla nuova posizione
		// Ad esempio, potresti chiamare una funzione nel pezzo stesso per eseguire il movimento
		SelectedWhitePiece->MovePieceToPosition(NewPosition);

		// Dopo il movimento, reimposta le variabili di stato
		SelectedWhitePiece = nullptr;
		bPieceSelected = false;
		IsMyTurn = false; // Probabilmente dovrai gestire meglio il cambio di turno
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("è stata chiamata la MoveSelectedPiece su un pezzo non bianco."));
	}
}




void AChess_HumanPlayer::InfoOfClickedPiece(AChess_Piece* CurrentPiece)
{
	EPieceType PieceType = CurrentPiece->GetPieceType();

	FString TypeString = (PieceType == EPieceType::KING) ? TEXT("King") :
		(PieceType == EPieceType::QUEEN) ? TEXT("Queen") :
		(PieceType == EPieceType::ROOK) ? TEXT("Rook") :
		(PieceType == EPieceType::BISHOP) ? TEXT("Bishop") :
		(PieceType == EPieceType::KNIGHT) ? TEXT("Knight") :
		(PieceType == EPieceType::PAWN) ? TEXT("Pawn") :
		TEXT("Unknown");

	FString Message = FString::Printf(TEXT("Piece Color: White, Piece Type: %s"), *TypeString);

	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, Message);
}

	
/*	if (ATile* CurrTile = Cast<ATile>(Hit.GetActor()))
		{
			if (CurrTile->GetTileStatus() == ETileStatus::EMPTY)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("clicked"));
				CurrTile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);
				FVector SpawnPosition = CurrTile->GetActorLocation();
				AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
				GameMode->SetCellSign(PlayerNumber, SpawnPosition);
				IsMyTurn = false;
			}
		}
	*/


