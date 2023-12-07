// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze.h"


// Sets default values 
AMaze::AMaze()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);
	PrimaryActorTick.bCanEverTick = true;
}

void AMaze::BeginPlay()
{
	Super::BeginPlay();

	MazeCells = new int[MazeWidth * MazeHeight];
	memset(MazeCells, 0x00, MazeWidth * MazeHeight * sizeof(int));

	Stack.push(FVector2D(0, 0)); // Start point
	MazeCells[0] = CELL_VISITED;
	VisitedCells = 1;
	AllowToBuildMaze = true;

	while (AllowToBuildMaze) Backtrack();
}

void AMaze::BuildMaze(){
	for (int x = 0; x < MazeWidth; x++)
	{
		for (int y = 0; y < MazeHeight; y++)
		{
			for (int px = 0; px < PathWidth; px++)
			{
				for (int py = 0; py < PathWidth; py++)
				{
					if (MazeCells[(y * MazeWidth) + x] & CELL_VISITED)
					{
						SpawnItem(FVector(x * (PathWidth + 1) + px, y * (PathWidth + 1) + py, 0), Visited);
					}
					else
					{
						SpawnItem(FVector(x * (PathWidth + 1) + px, y * (PathWidth + 1) + py, 0), Blue);
					}
				}
			}

			int random = rand() % 3;
			for (int p = 0; p < PathWidth; p++)
			{
				if (MazeCells[y * MazeWidth + x] & CELL_PATH_S)
					SpawnItem(FVector(x * (PathWidth + 1) + p, y * (PathWidth + 1 ) + PathWidth, 0), Visited);
				if (MazeCells[y * MazeWidth + x] & CELL_PATH_E)
					SpawnItem(FVector(x * (PathWidth + 1) + PathWidth, y * (PathWidth + 1 ) + p, 0), Visited);
			}
		}
	}
}

void AMaze::Backtrack()
{
	if ((VisitedCells >= MazeWidth * MazeHeight))
	{
		BuildMaze();
		AllowToBuildMaze = false;
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("upd10"));

	std::vector<int> UnvisitedNearCells;

	auto offset = [&](int ox, int oy) // Get offseted neighbour cell. If 0,0 the cell is current
		{
			int sx = Stack.top().X;
			int sy = Stack.top().Y;
			return ((sy + oy) * MazeWidth + (sx + ox));
		};

	// North
	if (Stack.top().Y > 0 && (MazeCells[offset(0, -1)] & CELL_VISITED) == 0)
		UnvisitedNearCells.push_back(0);
	// East
	if (Stack.top().X < MazeWidth - 1 && (MazeCells[offset(1, 0)] & CELL_VISITED) == 0)
		UnvisitedNearCells.push_back(1);
	// South
	if (Stack.top().Y < MazeHeight - 1 && (MazeCells[offset(0, 1)] & CELL_VISITED) == 0)
		UnvisitedNearCells.push_back(2);
	// West
	if (Stack.top().X > 0 && (MazeCells[offset(-1, 0)] & CELL_VISITED) == 0)
		UnvisitedNearCells.push_back(3);



	if (!UnvisitedNearCells.empty())
	{
		int NextCell = UnvisitedNearCells[rand() % UnvisitedNearCells.size()];

		switch (NextCell) // Put next cell in stack
		{
		case 0: // North
			MazeCells[offset(0, -1)] |= CELL_PATH_S;
			MazeCells[offset(0, 0)] |= CELL_PATH_N;
			Stack.push(FVector2D(Stack.top().X + 0, Stack.top().Y - 1));
			break;
		case 1: // East
			MazeCells[offset(1, 0)] |= CELL_PATH_W;
			MazeCells[offset(0, 0)] |= CELL_PATH_E;
			Stack.push(FVector2D(Stack.top().X + 1, Stack.top().Y));
			break;
		case 2: // South
			MazeCells[offset(0, 1)] |= CELL_PATH_N;
			MazeCells[offset(0, 0)] |= CELL_PATH_S;
			Stack.push(FVector2D(Stack.top().X + 0, Stack.top().Y + 1));
			break;
		case 3: // West
			MazeCells[offset(-1, 0)] |= CELL_PATH_E;
			MazeCells[offset(0, 0)] |= CELL_PATH_W;
			Stack.push(FVector2D(Stack.top().X - 1, Stack.top().Y));
			break;


		}
		// New cell
		VisitedCells += 1;
		MazeCells[offset(0, 0)] |= CELL_VISITED;
	}
	else
	{
		Stack.pop(); // Backtrack
	}
}

// Called every frame
void AMaze::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void AMaze::SpawnItem(FVector Position, UClass* Class)
{
	AActor* Actor = GetWorld()->SpawnActor<AActor>(Class);


	Actor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	Actor->SetActorLocation(FVector(Position.X*100, Position.Y*100, Position.Z * 100));
}

