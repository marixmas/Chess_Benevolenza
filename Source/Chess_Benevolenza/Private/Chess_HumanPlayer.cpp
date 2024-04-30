// Fill out your copyright notice in the Description page of Project Settings.

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
	PrimaryActorTick.bCanEverTick = false;																				// tanto la telecamera non si deve muovere	         //era true in 04/27 16:00

	// Set this pawn to be controlled by the lowest-numbered player
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// create a camera component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	//set the camera as RootComponent
	SetRootComponent(Camera);

	// get the game instance reference
	GameInstance = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	// default init values
	PlayerNumber = -1;
	ColorOfPieces = EColorOfPieces::NONE;
	IsMyTurn = false;
	SelectedWhitePiece = nullptr;
	SelectedBlackPiece = nullptr;
	bPieceSelected = false;

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
	// Structure containing information about one hit of a trace, such as point of impact and surface normal at that point
	FHitResult Hit = FHitResult(ForceInit);
	// GetHitResultUnderCursor function sends a ray from the mouse position and gives the corresponding hit results
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);
	AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());

	if (IsInCheck == false)
	{
		// controllo di aver cliccato qualcosa durante il mio turno
		if (Hit.bBlockingHit && IsMyTurn && !GameMode->IsGameOver)
		{
			// Verifica che GameMode ptr sia valido
			//AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
			if (!GameMode)
			{
				UE_LOG(LogTemp, Error, TEXT("GameMode non valido in HighlightGameFieldTiles!"));
				return;
			}

			// Verifica che GField ptr sia valido
			AGameField* GField = GameMode->GetGField();
			if (!GField)
			{
				UE_LOG(LogTemp, Error, TEXT("GField non valido in HighlightGameFieldTiles!"));
				return;
			}

			// se è stato cliccato un Piece, controllane il colore
			if (AChess_Piece* CurrPiece = Cast<AChess_Piece>(Hit.GetActor()))
			{
				// il Piece è White
				if (CurrPiece->GetPieceColor() == EPieceColor::WHITE)
				{
					// Se era già stato selezionato un altro White Piece nel Click precedente,
					// deselezionalo e imposta questo come Current Piece
					if (SelectedWhitePiece != nullptr)
					{
						// deselezione
						SelectedWhitePiece = nullptr;
						bPieceSelected = false;

						// "Spengo" i suggerimenti (attivati precedentemente)
						TurnOffHighlightedTiles();
					}

					// Se NON era già stato selezionato un White Piece precedentemente
					// imposta il pezzo come Current Piece
					SelectedWhitePiece = CurrPiece;

					// ottengo la sua posizione sulla scacchiera
					WhitePieceLocation = SelectedWhitePiece->GetGridPosition();

					// indico che è stato selezionato un pezzo
					bPieceSelected = true;

					// mostro in screen le info del pezzo cliccato
					InfoOfClickedPiece(CurrPiece);

					// chiedo alla classe del singolo pezzo di calcolarmi le mosse possibili
					PossibleMoves = CurrPiece->CalculatePossibleMoves();

					// illumino le tile delle mosse possibili
					HighlightGameFieldTiles(PossibleMoves);
				}


				// Se il Click è su un Black Piece
				else if (CurrPiece->GetPieceColor() == EPieceColor::BLACK)
				{
					// me lo memorizzo
					SelectedBlackPiece = CurrPiece;

					// se è il primo click dello human player ed è su un Black Piece do il messaggio
					if (bPieceSelected == false)
					{
						GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, TEXT("Queste sono le pedine del tuo avversario!"));
					}

					// se lo human player aveva selezionato un White Piece
					else if (bPieceSelected == true)
					{
						// ottengo la posizione sulla GameField del Black Piece
						BlackPieceLocation = SelectedBlackPiece->GetGridPosition();

						// se la Posizione del Black Piece appartiene alla mosse possibili del White Piece
						if (PossibleMoves.Contains(BlackPieceLocation))
						{
							// mangio il Black Piece
							SelectedBlackPiece->PieceIsEaten(BlackPieceLocation);

							// muovo il White Piece
							MoveSelectedPiece(WhitePieceLocation, BlackPieceLocation);

							// "spengo" i suggerimenti
							TurnOffHighlightedTiles();


							// Dopo il movimento, controlla se il pezzo è un pedone e se ha raggiunto l'ultima fila
							AChess_Piece* RecentlyMovedPiece = GField->PiecesMap.FindRef(BlackPieceLocation);
							if (RecentlyMovedPiece->GetPieceType() == EPieceType::PAWN && BlackPieceLocation.X == 7)
							{
								GField->PromotionToQueen(RecentlyMovedPiece);
							}

							// controllo: dopo la mia mossa il re avversario (del player 1) é in scacco?
							if (GameMode->IsKingInCheck(GameMode->GetGField(), 1))
							{
								IsMyTurn = false;
								FString Message = FString::Printf(TEXT("Black King is in Check!"));
								GEngine->AddOnScreenDebugMessage(2, 2.f, FColor::Orange, Message);
								//GameInstance->SetTurnMessage(TEXT("Black is in Check!"));
								//GameInstance->GetTurnMessage();
							}

							// controllo: dopo la mia mossa il re avversario (del player 1) é in scacco matto?
							if (GameMode->IsCheckmate(GameMode->GetGField(), 1))
							{
								GameMode->Players[0]->OnWin();
								GameMode->Players[1]->OnLose();
								GameMode->IsGameOver = true;
								IsMyTurn = false;
								GameInstance->SetTurnMessage(TEXT("Black King is in Checkmate! GAME OVER"));
								GameInstance->GetTurnMessage();
							}

							else
							{
								IsMyTurn = false;
								GameMode->TurnNextPlayer();
							}
						}
					}
				}
			}

			// se il Click è su una Tile
			else if (ATile* SelectedTile = Cast<ATile>(Hit.GetActor()))
			{
				// se un pezzo era già stato selezionato e la tile cliccata è nelle mosse possibili...
				if (bPieceSelected == true && PossibleMoves.Contains(SelectedTile->GetGridPosition()))
				{
					//....allora muovere pedina nella tile selezionata
					MoveSelectedPiece(WhitePieceLocation, SelectedTile->GetGridPosition());

					// Resetta lo stato di selezione della pedina
					bPieceSelected = false;


					// "spengo" i suggerimenti
					TurnOffHighlightedTiles();

					// Dopo il movimento, controlla se il pezzo è un pedone e se ha raggiunto l'ultima fila
					AChess_Piece* RecentlyMovedPiece = GField->PiecesMap.FindRef(SelectedTile->GetGridPosition());
					if (RecentlyMovedPiece->GetPieceType() == EPieceType::PAWN && SelectedTile->GetGridPosition().X == 7)
					{
						(GameMode->GetGField())->PromotionToQueen(RecentlyMovedPiece);
					}

					CurrPiece = nullptr;

					// controllo: dopo la mia mossa il re avversario (del player 1) é in scacco?
					if (GameMode->IsKingInCheck(GameMode->GetGField(), 1))
					{
						IsMyTurn = false;
						FString Message = FString::Printf(TEXT("Black King is in Check!"));
						GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Orange, Message);
						//GameInstance->SetTurnMessage(TEXT("Black is in Check!"));
						//GameInstance->GetTurnMessage();
						GameMode->Players[1]->IsInCheck = true;
					}

					// controllo: dopo la mia mossa il re avversario (del player 1) é in scacco matto?
					if (GameMode->IsCheckmate(GameMode->GetGField(), 1))
					{
						GameMode->Players[0]->OnWin();
						GameMode->Players[1]->OnLose();
						GameMode->IsGameOver = true;
						IsMyTurn = false;
						GameInstance->SetTurnMessage(TEXT("Black King is in Checkmate! GAME OVER"));
						GameInstance->GetTurnMessage();
					}

					IsMyTurn = false;
					GameMode->TurnNextPlayer();


				}
				// se un pezzo era già stato cliccato e la tile NON è nelle mosse possibili, non si può e do messaggio
				else if (bPieceSelected == true && !PossibleMoves.Contains(SelectedTile->GetGridPosition()))
				{
					GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Blue, TEXT("La pedina selezionata non può andare in questa casella."));
					// non cambio bSelectedPiece e nemmeno SelectedPiece
				}
			}
		}

		// Se clicco su qualcosa di valido ma non è il mio turno
		else
		{
			GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Blue, TEXT("Per favore, aspetta il tuo turno"));
		}
	}

	if (IsInCheck == true)
	{
		// controllo di aver cliccato qualcosa durante il mio turno
		if (Hit.bBlockingHit && IsMyTurn && !GameMode->IsGameOver)
		{
			// Verifica che GameMode ptr sia valido
			//AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
			if (!GameMode)
			{
				UE_LOG(LogTemp, Error, TEXT("GameMode non valido in HighlightGameFieldTiles!"));
				return;
			}

			// Verifica che GField ptr sia valido
			AGameField* GField = GameMode->GetGField();
			if (!GField)
			{
				UE_LOG(LogTemp, Error, TEXT("GField non valido in HighlightGameFieldTiles!"));
				return;
			}

			// se è stato cliccato un Piece, controllane il colore
			if (AChess_Piece* CurrPiece = Cast<AChess_Piece>(Hit.GetActor()))
			{
				// il Piece è White
				if (CurrPiece->GetPieceColor() == EPieceColor::WHITE)
				{
					// Se era già stato selezionato un altro White Piece nel Click precedente,
					// deselezionalo e imposta questo come Current Piece
					if (SelectedWhitePiece != nullptr)
					{
						// deselezione
						SelectedWhitePiece = nullptr;
						bPieceSelected = false;

						// "Spengo" i suggerimenti (attivati precedentemente)
						TurnOffHighlightedTiles();
					}

					// Se NON era già stato selezionato un White Piece precedentemente
					// imposta il pezzo come Current Piece
					SelectedWhitePiece = CurrPiece;

					// ottengo la sua posizione sulla scacchiera
					WhitePieceLocation = SelectedWhitePiece->GetGridPosition();

					// indico che è stato selezionato un pezzo
					bPieceSelected = true;

					// mostro in screen le info del pezzo cliccato
					InfoOfClickedPiece(CurrPiece);

					// chiedo alla classe del singolo pezzo di calcolarmi le mosse possibili
					PossibleMoves = CurrPiece->CalculatePossibleMoves();

					// illumino le tile delle mosse possibili
					HighlightGameFieldTiles(PossibleMoves);
				}


				// Se il Click è su un Black Piece
				else if (CurrPiece->GetPieceColor() == EPieceColor::BLACK)
				{
					// me lo memorizzo
					SelectedBlackPiece = CurrPiece;

					// se è il primo click dello human player ed è su un Black Piece do il messaggio
					if (bPieceSelected == false)
					{
						GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, TEXT("Queste sono le pedine del tuo avversario!"));
					}

					// se lo human player aveva selezionato un White Piece
					else if (bPieceSelected == true)
					{
						// ottengo la posizione sulla GameField del Black Piece
						BlackPieceLocation = SelectedBlackPiece->GetGridPosition();

						// se la Posizione del Black Piece appartiene alla mosse possibili del White Piece
						if (PossibleMoves.Contains(BlackPieceLocation))
						{






















							// mangio il Black Piece
							SelectedBlackPiece->PieceIsEaten(BlackPieceLocation);

							// muovo il White Piece
							MoveSelectedPiece(WhitePieceLocation, BlackPieceLocation);

							// "spengo" i suggerimenti
							TurnOffHighlightedTiles();

							// Dopo il movimento, controlla se il pezzo è un pedone e se ha raggiunto l'ultima fila
							AChess_Piece* RecentlyMovedPiece = GField->PiecesMap.FindRef(BlackPieceLocation);
							if (RecentlyMovedPiece->GetPieceType() == EPieceType::PAWN && BlackPieceLocation.X == 7)
							{
								GField->PromotionToQueen(RecentlyMovedPiece);
							}

							// controllo: dopo la mia mossa il re avversario (del player 1) é in scacco?
							if (GameMode->IsKingInCheck(GameMode->GetGField(), 1))
							{
								IsMyTurn = false;
								FString Message = FString::Printf(TEXT("Black King is in Check!"));
								GEngine->AddOnScreenDebugMessage(2, 2.f, FColor::Orange, Message);
								//GameInstance->SetTurnMessage(TEXT("Black is in Check!"));
								//GameInstance->GetTurnMessage();
							}

							// controllo: dopo la mia mossa il re avversario (del player 1) é in scacco matto?
							if (GameMode->IsCheckmate(GameMode->GetGField(), 1))
							{
								GameMode->Players[0]->OnWin();
								GameMode->Players[1]->OnLose();
								GameMode->IsGameOver = true;
								IsMyTurn = false;
								GameInstance->SetTurnMessage(TEXT("Black King is in Checkmate! GAME OVER"));
								GameInstance->GetTurnMessage();
							}

							else
							{
								IsMyTurn = false;
								GameMode->TurnNextPlayer();
							}
						}
					}
				}
			}

			// se il Click è su una Tile
			else if (ATile* SelectedTile = Cast<ATile>(Hit.GetActor()))
			{
				// se un pezzo era già stato selezionato e la tile cliccata è nelle mosse possibili...
				if (bPieceSelected == true && PossibleMoves.Contains(SelectedTile->GetGridPosition()))
				{

















					//....allora muovere pedina nella tile selezionata
					MoveSelectedPiece(WhitePieceLocation, SelectedTile->GetGridPosition());

					// Resetta lo stato di selezione della pedina
					bPieceSelected = false;


					// "spengo" i suggerimenti
					TurnOffHighlightedTiles();

					// Dopo il movimento, controlla se il pezzo è un pedone e se ha raggiunto l'ultima fila
					AChess_Piece* RecentlyMovedPiece = GField->PiecesMap.FindRef(SelectedTile->GetGridPosition());
					if (RecentlyMovedPiece->GetPieceType() == EPieceType::PAWN && SelectedTile->GetGridPosition().X == 7)
					{
						(GameMode->GetGField())->PromotionToQueen(RecentlyMovedPiece);
					}

					CurrPiece = nullptr;

					// controllo: dopo la mia mossa il re avversario (del player 1) é in scacco?
					if (GameMode->IsKingInCheck(GameMode->GetGField(), 1))
					{
						IsMyTurn = false;
						FString Message = FString::Printf(TEXT("Black King is in Check!"));
						GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Orange, Message);
						//GameInstance->SetTurnMessage(TEXT("Black is in Check!"));
						//GameInstance->GetTurnMessage();
						GameMode->Players[1]->IsInCheck = true;
					}

					// controllo: dopo la mia mossa il re avversario (del player 1) é in scacco matto?
					if (GameMode->IsCheckmate(GameMode->GetGField(), 1))
					{
						GameMode->Players[0]->OnWin();
						GameMode->Players[1]->OnLose();
						GameMode->IsGameOver = true;
						IsMyTurn = false;
						GameInstance->SetTurnMessage(TEXT("Black King is in Checkmate! GAME OVER"));
						GameInstance->GetTurnMessage();
					}

					IsMyTurn = false;
					GameMode->TurnNextPlayer();


				}
				// se un pezzo era già stato cliccato e la tile NON è nelle mosse possibili, non si può e do messaggio
				else if (bPieceSelected == true && !PossibleMoves.Contains(SelectedTile->GetGridPosition()))
				{
					GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Blue, TEXT("La pedina selezionata non può andare in questa casella."));
					// non cambio bSelectedPiece e nemmeno SelectedPiece
				}
			}
		}

		// Se clicco su qualcosa di valido ma non è il mio turno
		else
		{
			GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Blue, TEXT("Per favore, aspetta il tuo turno"));
		}
	}

}


// Accende le tile dei suggerimenti ovvero le tile delle mosse possibili dei pezzi
 void AChess_HumanPlayer::HighlightGameFieldTiles(const TArray<FVector2D>& TilePositions)				//c'era anche AChess_GameMode* GameMode, AGameField* GField
{
	 // prendo puntatori a GameMode e a GField e controllo che siano validi
	 AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
	 AGameField* GField = GameMode->GetGField();
	 if (!GameMode || !GameMode->GField)	
	 {
		 UE_LOG(LogTemp, Error, TEXT("GField o GameMode non valido per fare HighlightGameFieldTiles!"));
	 }


	// Loop attraverso tutte le Tile della GameField
	for (const FVector2D& TilePosition : TilePositions)
	{
		// prendo i puntatori alle Tile delle mosse possibili
		ATile* Tile = GField->TileMap.FindRef(TilePosition);
		if (Tile)
		{
			// cambio il materiale della Tile in giallo
			Tile->SetTileMaterial(GField->TileHighlightMaterial);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Materiale di Highlight non valido."));
		}
	}
}

 void AChess_HumanPlayer::HighlightInRedGameFieldTiles(const TArray<FVector2D>& TilePositions)				//c'era anche AChess_GameMode* GameMode, AGameField* GField
 {
	 // prendo puntatori a GameMode e a GField e controllo che siano validi
	 AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
	 AGameField* GField = GameMode->GetGField();
	 if (!GameMode || !GameMode->GField)
	 {
		 UE_LOG(LogTemp, Error, TEXT("GField o GameMode non valido per fare HighlightGameFieldTiles!"));
	 }


	 // Loop attraverso tutte le Tile della GameField
	 for (const FVector2D& TilePosition : TilePositions)
	 {
		 // prendo i puntatori alle Tile delle mosse possibili
		 ATile* Tile = GField->TileMap.FindRef(TilePosition);
		 if (Tile)
		 {
			 // cambio il materiale della Tile in giallo
			 Tile->SetTileMaterial(GField->TileRedHighlightMaterial);
		 }
		 else
		 {
			 UE_LOG(LogTemp, Error, TEXT("Materiale di Highlight non valido."));
		 }
	 }
 }


 void AChess_HumanPlayer::TurnOffHighlightedTiles()
 {
	// prendo puntatori a GameMode e a GField e controllo che siano validi
	AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
	AGameField* GField = GameMode->GetGField();
	if (!GameMode || !GameMode->GField)
	{
		UE_LOG(LogTemp, Error, TEXT("GField o GameMode non valido per fare TurnOffHighlightedTiles!"));
	}

	// scorro tutte le tile della gamefield
	for (int32 x = 0; x < GField->Size; x++)
	{
		for (int32 y = 0; y < GField->Size; y++)
		{
			// prendo un puntatore alla tile per ogni coppia (x,y) corrispondente
			ATile* Tile = GField->TileMap.FindRef(FVector2D(x, y));
			if (Tile)
			{
				if (Tile->GetTileMaterial() == GField->TileRedHighlightMaterial)
				{

				}
				else 
				{
				// Applica il materiale originale alla tile in base alla posizione sulla scacchiera
				if ((x + y) % 2 == 0)
				{
					Tile->SetTileMaterial(GField->TileMaterial1);
				}
				else
				{
					Tile->SetTileMaterial(GField->TileMaterial2);
				}
}
				
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Materiale di TurnOffHighlight non valido."));
			}
		}
	}
}

 void AChess_HumanPlayer::TurnOffRedHighlightedTiles()
 {
	 // prendo puntatori a GameMode e a GField e controllo che siano validi
	 AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
	 AGameField* GField = GameMode->GetGField();
	 if (!GameMode || !GameMode->GField)
	 {
		 UE_LOG(LogTemp, Error, TEXT("GField o GameMode non valido per fare TurnOffHighlightedTiles!"));
	 }

	 // scorro tutte le tile della gamefield
	 for (int32 x = 0; x < GField->Size; x++)
	 {
		 for (int32 y = 0; y < GField->Size; y++)
		 {
			 // prendo un puntatore alla tile per ogni coppia (x,y) corrispondente
			 ATile* Tile = GField->TileMap.FindRef(FVector2D(x, y));
			 if (Tile)
			 {
				 if (Tile->GetTileMaterial() == GField->TileRedHighlightMaterial)
				 {
					// Applica il materiale originale alla tile in base alla posizione sulla scacchiera
					 if ((x + y) % 2 == 0)
					 {
						 Tile->SetTileMaterial(GField->TileMaterial1);
					 }
					 else
					 {
						 Tile->SetTileMaterial(GField->TileMaterial2);
					 }
				 }
				 else
				 {

				 }

			 }
			 else
			 {
				 UE_LOG(LogTemp, Error, TEXT("Materiale di TurnOffHighlight non valido."));
			 }
		 }
	 }
 }

void AChess_HumanPlayer::MoveSelectedPiece(const FVector2D& OldPosition, const FVector2D& NewPosition)
{

	// Assicurati che ci sia un pezzo selezionato
	if (SelectedWhitePiece)
	{
		// Effettua il movimento del pezzo selezionato alla nuova posizione
		// Ad esempio, potresti chiamare una funzione nel pezzo stesso per eseguire il movimento
		SelectedWhitePiece->MovePieceFromToPosition(OldPosition, NewPosition);

		// Dopo il movimento, reimposta le variabili di stato
		SelectedWhitePiece = nullptr;
		bPieceSelected = false;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("è stata chiamata la MoveSelectedPiece su un pezzo non bianco."));
	}
}

// da le info sul pezzo cliccato
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

	GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Blue, Message);
}

	
