// Fill out your copyright notice in the Description page of Project Settings.


#include "GameField.h"
#include "Chess_GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Chess_Piece.h"				//	palese da cambiare??vvv fatto!?


// Sets default values
AGameField::AGameField()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// è uno spreco di risorse tickare una scacchiera
	PrimaryActorTick.bCanEverTick = false;

	WinSize = 3;
	// size of the field (3x3)
	Size = 8;

	// tile dimension
	TileSize = 120;

	// tile padding dimension
	CellPadding = 0;
}

void AGameField::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	//normalized tilepadding
	NormalizedCellPadding = FMath::RoundToDouble(((TileSize + CellPadding) / TileSize) * 100) / 100;
}

// Called when the game starts or when spawned
void AGameField::BeginPlay()
{
	Super::BeginPlay();
	GenerateField();
	GeneratePieces();
}

// Assumi che TileSize rappresenti la dimensione di ciascuna tile della scacchiera da 
/*

AGameField* AChess_GameMode::GetGField() const
{
	return GField;
}

*/

void AGameField::GeneratePieces()
{
	//AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());

	//AGameField* GField = GameMode->GetGField();

	// controlla se il puntatore GField non è nullo
	/*if (GField != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("HA FUNZIONATO LA TUA ROBAAA. CIAO DA GAMEFIELD.CPP!"));
	}
	else
	{
		// estisci il caso in cui il puntatore GField sia nullo
		UE_LOG(LogTemp, Warning, TEXT("Il puntatore GField è    nullo!"));
	}
	*/
	// float TileSize = GField->TileSize;					// il debugger mi diceva che se dichiaro TileSize. il membro della classe verrà nascosto quindi boh provo a togliere

	for (int32 x = 0; x < Size; x++)
	{
		for (int32 y = 0; y < Size; y++)
		{

			// dalle coordinate (x,y) ricavo la posizione nello spazio di unreal (x,y,z) in cui spawnerò il pezzo
			//FVector SpawnPosition = FVector(TileSize * x, TileSize * y, 20);												mi sa che non lo uso

			//dalle coordinate (x,y) ricavo la posizione nello spazio di unreal (x,y,z) in cui spawnerò il pezzo 
			FVector Location = (AGameField::GetRelativeLocationByXYPosition(x, y)) + FVector(0, 0, 20);

			// calcola rapporto in scala per dimensionare
			const float TileScale = TileSize / 100;

			// Spawn dei pezzi Pawn
			if (x == 1 || x == 6)
			{
				//UClass* PawnClass = X == 1 ? WhitePawnClass : BlackPawnClass;
				//GetWorld()->SpawnActor<AChess_Piece_Pawn>(PawnClass, GField->GetActorLocation() + SpawnPosition, FRotator::ZeroRotator);

				//FVector Location = AGameField::GetRelativeLocationByXYPosition(x, y);					//metto fuori dall'if
				//const float TileScale = TileSize / 100;												//metto fuori dall'if

				if (x == 1)
				{
					int32 PlayerOwner = 0;

					AChess_Piece* Obj = GetWorld()->SpawnActor<AChess_Piece>(ChessPiecePawnBpWhite, Location, FRotator::ZeroRotator);
					//const float TileScale = TileSize / 100;
					Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
					Obj->SetGridPosition(x, y);
					PiecesArray.Add(Obj);
					PiecesMap.Add(FVector2D(x, y), Obj);

					// ottengo il puntatore alla tile corrispondente
					ATile* TargetTile = TileMap.FindRef(FVector2D(x, y));
					// controllo puntatore
					if (TargetTile)
					{
						// imposto stato e owner della tile
						TargetTile->SetTileStatus(ETileStatus::OCCUPIED);
						TargetTile->SetTileOwner(PlayerOwner);
					}

					// inizializzo il Pezzo Base (Chess_Piece) appena spawnato
					Obj->SetPieceColor(EPieceColor::WHITE);
					// imposto tipo della pedina che occupa la tile
					Obj->SetPieceType(EPieceType::PAWN);
				}

				if (x == 6)
				{
					int32 PlayerOwner = 1;

					AChess_Piece* Obj = GetWorld()->SpawnActor<AChess_Piece>(ChessPiecePawnBpBlack, Location, FRotator::ZeroRotator);
					//const float TileScale = TileSize / 100;
					Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
					Obj->SetGridPosition(x, y);
					PiecesArray.Add(Obj);
					PiecesMap.Add(FVector2D(x, y), Obj);

					// ottengo il puntatore alla tile corrispondente
					ATile* TargetTile = TileMap.FindRef(FVector2D(x, y));
					// controllo puntatore
					if (TargetTile)
					{
						// imposto stato e owner della tile
						TargetTile->SetTileStatus(ETileStatus::OCCUPIED);
						TargetTile->SetTileOwner(PlayerOwner);
					}

					// inizializzo il Pezzo Base (Chess_Piece) appena spawnato
					Obj->SetPieceColor(EPieceColor::BLACK);
					// imposto tipo della pedina che occupa la tile
					Obj->SetPieceType(EPieceType::PAWN);

				}
				/*																	//lo copio per ogni if wtf??
				const float TileScale = TileSize / 100;
				Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
				Obj->SetGridPosition(x, y);
				PiecesArray.Add(Obj);
				PiecesMap.Add(FVector2D(x, y), Obj);
				*/


			}

			// Spawn degli altri pezzi
			else if (x == 0 || x == 7)
			{
				//FVector Location = AGameField::GetRelativeLocationByXYPosition(x, y);

				if (x == 0)
				{
					int32 PlayerOwner = 0;

					if (y == 0 || y == 7)
					{
						AChess_Piece* Obj = GetWorld()->SpawnActor<AChess_Piece>(ChessPieceRookBpWhite, Location, FRotator::ZeroRotator);
						//const float TileScale = TileSize / 100;
						Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
						Obj->SetGridPosition(x, y);
						PiecesArray.Add(Obj);
						PiecesMap.Add(FVector2D(x, y), Obj);

						// ottengo il puntatore alla tile corrispondente
						ATile* TargetTile = TileMap.FindRef(FVector2D(x, y));
						// controllo puntatore
						if (TargetTile)
						{
							// imposto stato e owner della tile
							TargetTile->SetTileStatus(ETileStatus::OCCUPIED);
							TargetTile->SetTileOwner(PlayerOwner);
						}

						// inizializzo il Pezzo Base (Chess_Piece) appena spawnato
						Obj->SetPieceColor(EPieceColor::WHITE);
						// imposto tipo della pedina che occupa la tile
						Obj->SetPieceType(EPieceType::ROOK);
					}


					else if (y == 1 || y == 6)
					{
						AChess_Piece* Obj = GetWorld()->SpawnActor<AChess_Piece>(ChessPieceKnightBpWhite, Location, FRotator::ZeroRotator);
						//const float TileScale = TileSize / 100;
						Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
						Obj->SetGridPosition(x, y);
						PiecesArray.Add(Obj);
						PiecesMap.Add(FVector2D(x, y), Obj);

						// ottengo il puntatore alla tile corrispondente
						ATile* TargetTile = TileMap.FindRef(FVector2D(x, y));
						// controllo puntatore
						if (TargetTile)
						{
							// imposto stato e owner della tile
							TargetTile->SetTileStatus(ETileStatus::OCCUPIED);
							TargetTile->SetTileOwner(PlayerOwner);
						}

						// inizializzo il Pezzo Base (Chess_Piece) appena spawnato
						Obj->SetPieceColor(EPieceColor::WHITE);
						// imposto tipo della pedina che occupa la tile
						Obj->SetPieceType(EPieceType::KNIGHT);

					}

					else if (y == 2 || y == 5)
					{
						AChess_Piece* Obj = GetWorld()->SpawnActor<AChess_Piece>(ChessPieceBishopBpWhite, Location, FRotator::ZeroRotator);
						//const float TileScale = TileSize / 100;
						Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
						Obj->SetGridPosition(x, y);
						PiecesArray.Add(Obj);
						PiecesMap.Add(FVector2D(x, y), Obj);

						// ottengo il puntatore alla tile corrispondente
						ATile* TargetTile = TileMap.FindRef(FVector2D(x, y));
						// controllo puntatore
						if (TargetTile)
						{
							// imposto stato e owner della tile
							TargetTile->SetTileStatus(ETileStatus::OCCUPIED);
							TargetTile->SetTileOwner(PlayerOwner);
						}

						// inizializzo il Pezzo Base (Chess_Piece) appena spawnato
						Obj->SetPieceColor(EPieceColor::WHITE);
						// imposto tipo della pedina che occupa la tile
						Obj->SetPieceType(EPieceType::BISHOP);
					}

					else if (y == 3)
					{
						AChess_Piece* Obj = GetWorld()->SpawnActor<AChess_Piece>(ChessPieceQueenBpWhite, Location, FRotator::ZeroRotator);
						//const float TileScale = TileSize / 100;
						Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
						Obj->SetGridPosition(x, y);
						PiecesArray.Add(Obj);
						PiecesMap.Add(FVector2D(x, y), Obj);

						// ottengo il puntatore alla tile corrispondente
						ATile* TargetTile = TileMap.FindRef(FVector2D(x, y));
						// controllo puntatore
						if (TargetTile)
						{
							// imposto stato e owner della tile
							TargetTile->SetTileStatus(ETileStatus::OCCUPIED);
							TargetTile->SetTileOwner(PlayerOwner);
						}

						// inizializzo il Pezzo Base (Chess_Piece) appena spawnato
						Obj->SetPieceColor(EPieceColor::WHITE);
						// imposto tipo della pedina che occupa la tile
						Obj->SetPieceType(EPieceType::QUEEN);
					}

					else if (y == 4)
					{
						AChess_Piece* Obj = GetWorld()->SpawnActor<AChess_Piece>(ChessPieceKingBpWhite, Location, FRotator::ZeroRotator);
						//const float TileScale = TileSize / 100;
						Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
						Obj->SetGridPosition(x, y);
						PiecesArray.Add(Obj);
						PiecesMap.Add(FVector2D(x, y), Obj);

						// ottengo il puntatore alla tile corrispondente
						ATile* TargetTile = TileMap.FindRef(FVector2D(x, y));
						// controllo puntatore
						if (TargetTile)
						{
							// imposto stato e owner della tile
							TargetTile->SetTileStatus(ETileStatus::OCCUPIED);
							TargetTile->SetTileOwner(PlayerOwner);
						}

						// inizializzo il Pezzo Base (Chess_Piece) appena spawnato
						Obj->SetPieceColor(EPieceColor::WHITE);
						// imposto tipo della pedina che occupa la tile
						Obj->SetPieceType(EPieceType::KING);
					}
				}

				if (x == 7)
				{
					int32 PlayerOwner = 1;

					if (y == 0 || y == 7)
					{
						AChess_Piece* Obj = GetWorld()->SpawnActor<AChess_Piece>(ChessPieceRookBpBlack, Location, FRotator::ZeroRotator);
						//const float TileScale = TileSize / 100;
						Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
						Obj->SetGridPosition(x, y);
						PiecesArray.Add(Obj);
						PiecesMap.Add(FVector2D(x, y), Obj);

						// ottengo il puntatore alla tile corrispondente
						ATile* TargetTile = TileMap.FindRef(FVector2D(x, y));
						// controllo puntatore
						if (TargetTile)
						{
							// imposto stato e owner della tile
							TargetTile->SetTileStatus(ETileStatus::OCCUPIED);
							TargetTile->SetTileOwner(PlayerOwner);
						}

						// inizializzo il Pezzo Base (Chess_Piece) appena spawnato
						Obj->SetPieceColor(EPieceColor::BLACK);
						// imposto tipo della pedina che occupa la tile
						Obj->SetPieceType(EPieceType::ROOK);
					}

					else if (y == 1 || y == 6)
					{
						AChess_Piece* Obj = GetWorld()->SpawnActor<AChess_Piece>(ChessPieceKnightBpBlack, Location, FRotator::ZeroRotator);
						//const float TileScale = TileSize / 100;
						Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
						Obj->SetGridPosition(x, y);
						PiecesArray.Add(Obj);
						PiecesMap.Add(FVector2D(x, y), Obj);

						// ottengo il puntatore alla tile corrispondente
						ATile* TargetTile = TileMap.FindRef(FVector2D(x, y));
						// controllo puntatore
						if (TargetTile)
						{
							// imposto stato e owner della tile
							TargetTile->SetTileStatus(ETileStatus::OCCUPIED);
							TargetTile->SetTileOwner(PlayerOwner);
						}

						// inizializzo il Pezzo Base (Chess_Piece) appena spawnato
						Obj->SetPieceColor(EPieceColor::BLACK);
						// imposto tipo della pedina che occupa la tile
						Obj->SetPieceType(EPieceType::KNIGHT);
					}

					else if (y == 2 || y == 5)
					{
						AChess_Piece* Obj = GetWorld()->SpawnActor<AChess_Piece>(ChessPieceBishopBpBlack, Location, FRotator::ZeroRotator);
						//const float TileScale = TileSize / 100;
						Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
						Obj->SetGridPosition(x, y);
						PiecesArray.Add(Obj);
						PiecesMap.Add(FVector2D(x, y), Obj);

						// ottengo il puntatore alla tile corrispondente
						ATile* TargetTile = TileMap.FindRef(FVector2D(x, y));
						// controllo puntatore
						if (TargetTile)
						{
							// imposto stato e owner della tile
							TargetTile->SetTileStatus(ETileStatus::OCCUPIED);
							TargetTile->SetTileOwner(PlayerOwner);
						}

						// inizializzo il Pezzo Base (Chess_Piece) appena spawnato
						Obj->SetPieceColor(EPieceColor::BLACK);
						// imposto tipo della pedina che occupa la tile
						Obj->SetPieceType(EPieceType::BISHOP);
					}

					else if (y == 3)
					{
						AChess_Piece* Obj = GetWorld()->SpawnActor<AChess_Piece>(ChessPieceQueenBpBlack, Location, FRotator::ZeroRotator);
						//const float TileScale = TileSize / 100;
						Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
						Obj->SetGridPosition(x, y);
						PiecesArray.Add(Obj);
						PiecesMap.Add(FVector2D(x, y), Obj);

						// ottengo il puntatore alla tile corrispondente
						ATile* TargetTile = TileMap.FindRef(FVector2D(x, y));
						// controllo puntatore
						if (TargetTile)
						{
							// imposto stato e owner della tile
							TargetTile->SetTileStatus(ETileStatus::OCCUPIED);
							TargetTile->SetTileOwner(PlayerOwner);
						}

						// inizializzo il Pezzo Base (Chess_Piece) appena spawnato
						Obj->SetPieceColor(EPieceColor::BLACK);
						// imposto tipo della pedina che occupa la tile
						Obj->SetPieceType(EPieceType::QUEEN);
					}
					else if (y == 4)
					{
						AChess_Piece* Obj = GetWorld()->SpawnActor<AChess_Piece>(ChessPieceKingBpBlack, Location, FRotator::ZeroRotator);
						//const float TileScale = TileSize / 100;
						Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
						Obj->SetGridPosition(x, y);
						PiecesArray.Add(Obj);
						PiecesMap.Add(FVector2D(x, y), Obj);

						// ottengo il puntatore alla tile corrispondente
						ATile* TargetTile = TileMap.FindRef(FVector2D(x, y));
						// controllo puntatore
						if (TargetTile)
						{
							// imposto stato e owner della tile
							TargetTile->SetTileStatus(ETileStatus::OCCUPIED);
							TargetTile->SetTileOwner(PlayerOwner);
						}

						// inizializzo il Pezzo Base (Chess_Piece) appena spawnato
						Obj->SetPieceColor(EPieceColor::BLACK);
						// imposto tipo della pedina che occupa la tile
						Obj->SetPieceType(EPieceType::KING);
					}
				}
				/*																	//lo copio per ogni if wtf??
				const float TileScale = TileSize / 100;
				Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
				Obj->SetGridPosition(x, y);
				PiecesArray.Add(Obj);
				PiecesMap.Add(FVector2D(x, y), Obj);
				*/


				else
				{
				}

			}
		}

		/*
		FVector Location = AGameField::GetRelativeLocationByXYPosition(x, y);

		AChess_Piece_Pawn* Obj = GetWorld()->SpawnActor<AChess_Piece_Pawn>(ChessPiecePawnClassOrColorOrBP, Location, FRotator::ZeroRotator);
		const float TileScale = TileSize / 100;
		Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
		Obj->SetGridPosition(x, y);
		PiecesArray.Add(Obj);
		PiecesMap.Add(FVector2D(x, y), Obj);
		*/

		/*
		{
		for (int32 x = 0; x < Size; x++)
		{
			for (int32 y = 0; y < Size; y++)
			{
				FVector Location = AGameField::GetRelativeLocationByXYPosition(x, y);
				ATile* Obj = GetWorld()->SpawnActor<ATile>(TileClass, Location, FRotator::ZeroRotator);									da cambiareeeeee
				const float TileScale = TileSize / 100;
				Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
				Obj->SetGridPosition(x, y);
				TileArray.Add(Obj);
				TileMap.Add(FVector2D(x, y), Obj);
			}
		}
		}*/


	}
}

void AGameField::ResetField()
{
	for (ATile* Obj : TileArray)
	{
		Obj->SetTileStatus(ETileStatus::EMPTY);
		Obj->SetTileOwner(NOT_ASSIGNED);
	}

	// send broadcast event to registered objects 
	OnResetEvent.Broadcast();

	AChess_GameMode* GameMode = Cast<AChess_GameMode
	>(GetWorld()->GetAuthGameMode());
	GameMode->IsGameOver = false;
	GameMode->MoveCounter = 0;
	GameMode->ChoosePlayerAndStartGame();
}

void AGameField::GenerateField()
{
	for (int32 x = 0; x < Size; x++)
	{
		for (int32 y = 0; y < Size; y++)
		{
			FVector Location = AGameField::GetRelativeLocationByXYPosition(x, y);
			if ((x + y) % 2 == 0)
			{
				ATile* Obj = GetWorld()->SpawnActor<ATile>(TileClass1, Location, FRotator::ZeroRotator);
				const float TileScale = TileSize / 100;
				Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
				Obj->SetGridPosition(x, y);
				TileArray.Add(Obj);
				TileMap.Add(FVector2D(x, y), Obj);

			}
			else
			{
				ATile* Obj = GetWorld()->SpawnActor<ATile>(TileClass2, Location, FRotator::ZeroRotator);
				const float TileScale = TileSize / 100;
				Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
				Obj->SetGridPosition(x, y);
				TileArray.Add(Obj);
				TileMap.Add(FVector2D(x, y), Obj);
			}




			/*
			ATile* Obj = GetWorld()->SpawnActor<ATile>(TileClass, Location, FRotator::ZeroRotator);
			const float TileScale = TileSize / 100;
			Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
			Obj->SetGridPosition(x, y);
			TileArray.Add(Obj);
			TileMap.Add(FVector2D(x, y), Obj);
			*/
		}
	}
}

FVector2D AGameField::GetPosition(const FHitResult& Hit)
{
	return Cast<ATile>(Hit.GetActor())->GetGridPosition();
}

TArray<ATile*>& AGameField::GetTileArray()
{
	// TODO: inserire l'istruzione return qui			DONE
	return TileArray;
}

FVector AGameField::GetRelativeLocationByXYPosition(const int32 InX, const int32 InY) const
{
	return TileSize * NormalizedCellPadding * FVector(InX, InY, 0);
}

FVector2D AGameField::GetXYPositionByRelativeLocation(const FVector& Location) const
{
	const double x = Location[0] / (TileSize * NormalizedCellPadding);
	const double y = Location[1] / (TileSize * NormalizedCellPadding);
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("x=%f,y=%f"), x, y));
	return FVector2D(x, y);
}

bool AGameField::IsWinPosition(const FVector2D Position) const
{
	const int32 Offset = WinSize - 1;
	// vertical lines
	for (int32 i = Position[0] - Offset; i <= Position[0]; i++)
	{
		if (IsWinLine(FVector2D(i, Position[1]), FVector2D(i + Offset, Position[1])))
		{
			return true;
		}
	}

	// horizontal lines
	for (int32 i = Position[1] - Offset; i <= Position[1]; i++)
	{
		if (IsWinLine(FVector2D(Position[0], i), FVector2D(Position[0], i + Offset)))
		{
			return true;
		}
	}

	// diagonal lines
	for (int32 i = -Offset; i <= 0; i++)
	{
		if (IsWinLine(FVector2D(Position[0] + i, Position[1] + i), FVector2D(Position[0] + Offset + i, Position[1] + Offset + i)))
		{
			return true;
		}
		if (IsWinLine(FVector2D(Position[0] + i, Position[1] - i), FVector2D(Position[0] + Offset + i, Position[1] - Offset - i)))
		{
			return true;
		}
	}

	return false;
}

inline bool AGameField::IsWinLine(const FVector2D Begin, const FVector2D End) const
{
	return IsValidPosition(Begin) && IsValidPosition(End) && AllEqual(GetLine(Begin, End));
}

inline bool AGameField::IsValidPosition(const FVector2D Position) const
{
	return 0 <= Position[0] && Position[0] < Size && 0 <= Position[1] && Position[1] < Size;
}

TArray<int32> AGameField::GetLine(const FVector2D Begin, const FVector2D End) const
{
	int32 xSign;
	if (Begin[0] == End[0])
	{
		xSign = 0;
	}
	else
	{
		xSign = Begin[0] < End[0] ? 1 : -1;
	}

	int32 ySign;
	if (Begin[1] == End[1])
	{
		ySign = 0;
	}
	else
	{
		ySign = Begin[1] < End[1] ? 1 : -1;
	}

	TArray<int32> Line;
	int32 x = Begin[0] - xSign;
	int32 y = Begin[1] - ySign;
	do
	{
		x += xSign;
		y += ySign;
		Line.Add((TileMap[FVector2D(x, y)])->GetOwner());
	} while (x != End[0] || y != End[1]);

	return Line;
}

bool AGameField::AllEqual(const TArray<int32>& Array) const
{
	if (Array.Num() == 0)
	{
		return false;
	}
	const int32 Value = Array[0];

	if (Value == NOT_ASSIGNED)
	{
		return false;
	}

	for (int32 i = 1; i < Array.Num(); i++)
	{
		if (Value != Array[i])
		{
			return false;
		}
	}

	return true;
}

/*
AGameField* AGameField::GetGField() const
{
	return GField;
}
*/

/*
// Called every frame
void AGameField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
*/
