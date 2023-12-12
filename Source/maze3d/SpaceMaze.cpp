// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceMaze.h"


// Sets default values 
ASpaceMaze::ASpaceMaze()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
    SetRootComponent(Scene);
    PrimaryActorTick.bCanEverTick = true;
}

void ASpaceMaze::BeginPlay()
{
    Super::BeginPlay();

    srand(time(0));  // Инициализация генератора случайных чисел

    FString FileContent;

    LoadAndSpawnItems();

}

// Called every frame
void ASpaceMaze::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ASpaceMaze::LoadAndSpawnItems()
{
    // Создаем массив лабиринта

    TArray<FString> Lines;
    FFileHelper::LoadFileToStringArray(Lines, *FileName);
    for (FString& Line : Lines)
    {
        TArray<int32> Row;
        for (TCHAR Char : Line)
        {
            int32 Number = Char - '0';  // Преобразование символа в число
            Row.Add(Number);
        }
        MazeArray.Add(Row);
    }
    int MazeCellsAmount = CountMazeCells(MazeArray);
    int MazeHeight = MazeArray.Num();
    int MazeWidth = MazeArray[0].Num();
    int MazeCellsVisited = 0;

    // Выбираем стартовую точку из символов '2' и помечаем её как посещенную
    FVector debug_cube = FVector();//FindRandomStartPosition(MazeArray);
    debug_cube.Set(17, 15, 2);
    //SpawnItem(debug_cube, Blue);
    FVector StartPosition = FVector();
    StartPosition.Set(16, 15, 2);
    //SpawnItem(StartPosition, Blue);

    auto VisitCell = [&](FVector& Position) { // Функция посещения клеток
        //UE_LOG(LogTemp, Display, TEXT("(X:%f Y:%f Z:%f)"), Position.X, Position.Y, Position.Z);
        BacktrackingStack.push(Position);
        MazeCellsVisited += 1;
        MazeArray[Position.Y][Position.X] |= CELL_VISITED;
        };

    VisitCell(StartPosition);
    int debug = 0;
    // Backtracking Algorithm
    while (MazeCellsVisited < MazeCellsAmount && debug == 0)
    {
        debug = 0;
        FVector CurrentCell = BacktrackingStack.top();

        // Получаем координаты соседних клеток
        FVector NorthCell(CurrentCell.X, CurrentCell.Y - 1, 0);
        FVector SouthCell(CurrentCell.X, CurrentCell.Y + 1, 0);
        FVector EastCell(CurrentCell.X + 1, CurrentCell.Y, 0);
        FVector WestCell(CurrentCell.X - 1, CurrentCell.Y, 0);

        TMap<Direction, FVector> NearbyCells;

        auto IsCellUnvisited = [&](FVector cell) // ОН В ПРИНЦИПЕ НЕ ДОЛЖЕН ЗАХОДИТЬ ЗА ГРАНИ Т.К ПРОВЕРЯЕТСЯ НА CELL_MAZE
            {
                return ((MazeArray[cell.Y][cell.X] & CELL_MAZE) && !(MazeArray[cell.Y][cell.X] & CELL_VISITED) && !(MazeArray[cell.Y][cell.X] & EDGE_MAZE));
            };

        if (NorthCell.Y >= 0 && IsCellUnvisited(NorthCell)
            && MazeArray[NorthCell.Y][NorthCell.X] != EDGE_MAZE) NearbyCells.Add(NORTH, NorthCell);
        if (SouthCell.Y < MazeHeight && IsCellUnvisited(SouthCell)
            && MazeArray[SouthCell.Y][SouthCell.X] != EDGE_MAZE) NearbyCells.Add(SOUTH, SouthCell); // TODO Ошибка как раз в проверке на размер. Т.к размер не квадратный
        if (EastCell.X < MazeWidth && IsCellUnvisited(EastCell)
            && MazeArray[EastCell.Y][EastCell.X] != EDGE_MAZE) NearbyCells.Add(EAST, EastCell);
        if (WestCell.X >= 0 && IsCellUnvisited(WestCell)
            && MazeArray[WestCell.Y][WestCell.X] != EDGE_MAZE) NearbyCells.Add(WEST, WestCell);

        TArray<Direction> Keys;
        NearbyCells.GetKeys(Keys);

        if (Keys.Num() == 0)
        {
            // In case if there are no nearby free unvisited cells, we go back to previous cell and repeat.
            // That is the backtracking algorithm itself
            //UE_LOG(LogTemp, Display, TEXT("UNVISITED DETECTED AT CURRENT WHICH IS %f:%f"), CurrentCell.X, CurrentCell.Y);
            BacktrackingStack.pop();
            continue;
        }

        int RandomDirection = rand() % Keys.Num();

        switch (Keys[RandomDirection])
        {
            // In cells between which there are no path, there will walls
        case NORTH:
            MazeArray[NorthCell.Y][NorthCell.X] |= PATH_SOUTH;
            MazeArray[CurrentCell.Y][CurrentCell.X] |= PATH_NORTH;
            VisitCell(NorthCell);
            break;
        case SOUTH:
            MazeArray[SouthCell.Y][SouthCell.X] |= PATH_NORTH;
            MazeArray[CurrentCell.Y][CurrentCell.X] |= PATH_SOUTH;
            VisitCell(SouthCell);
            break;
        case EAST:
            MazeArray[EastCell.Y][EastCell.X] |= PATH_WEST;
            MazeArray[CurrentCell.Y][CurrentCell.X] |= PATH_EAST;
            VisitCell(EastCell);
            break;
        case WEST:
            MazeArray[WestCell.Y][WestCell.X] |= PATH_EAST;
            MazeArray[CurrentCell.Y][CurrentCell.X] |= PATH_WEST;
            VisitCell(WestCell);
            break;
        }

    }// Все так же, только юг и север поменяны местами


    for (int32 Y = 0; Y < MazeArray.Num(); ++Y)
    {
        const TArray<int32>& Row = MazeArray[Y];
        for (int32 X = 0; X < Row.Num(); ++X)
        {
            FVector CurrentCell = FVector(X, Y, 0);
            if (Row[X] == EDGE_MAZE)
            {
                SpawnItem(CurrentCell, Visited);
            }
            else if (Row[X] & CELL_VISITED)
            {
                SpawnItem(CurrentCell, Blue);
            }
            // МЕТОД CREATEWALL если south и east
            // Не проверяет Next

            // НАДО ДОБАВИТЬ if not path east, if not path west, if not path north, итд

            if (!(Row[X] & PATH_EAST) && (Row[X] & CELL_MAZE) && MazeArray[Y][X + 1] & CELL_MAZE) // Вот тут проблема, которая далее за границы выходит
                // Ведь x идет в ++, и y тоже, следовательно надо использовать не запад/север, а восток/юг
            {
                FVector EastCell(CurrentCell.X + 1, CurrentCell.Y, 0); // Восток выше по X
                SpawnWallBetweenCells(CurrentCell, EastCell);
            }
            if (!(Row[X] & PATH_SOUTH) && (Row[X] & CELL_MAZE) && (MazeArray[Y + 1][X] & CELL_MAZE))
            {
                FVector SouthCell(CurrentCell.X, CurrentCell.Y + 1, 0); // Юг выше по Y
                SpawnWallBetweenCells(CurrentCell, SouthCell);
            }
        }
    }
}

void ASpaceMaze::SpawnWallBetweenCells(FVector& CurrentCell, FVector& NextCell)
{
    if (CurrentCell.X != NextCell.X)
    {
        FVector WallPosition(NextCell);
        SpawnItem(WallPosition, Wall);
    }
    else if (CurrentCell.Y != NextCell.Y)
    {
        FVector WallPosition(NextCell);
        SpawnItem(WallPosition, Wall, FRotator(0, -90, 0));
    }

}

void ASpaceMaze::SpawnItem(FVector& Position, UClass* Class, FRotator Rotation)
{
    AActor* Actor = GetWorld()->SpawnActor<AActor>(Class);
    Actor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
    Actor->SetActorLocation(FVector(Position.X * 100, Position.Y * 100, Position.Z * 100));
    Actor->SetActorRotation(Rotation);
}

void ASpaceMaze::printDebug(FVector DebugCube)
{
    double f = MazeArray[DebugCube.Y / 100][DebugCube.X / 100];
    UE_LOG(LogTemp, Display, TEXT("%f"), f);
}

FVector ASpaceMaze::FindRandomStartPosition(const TArray<TArray<int32>>& Maze)
{
    TArray<FVector> Positions;

    for (int32 Y = 0; Y < Maze.Num(); ++Y)
    {
        const TArray<int32>& Row = Maze[Y];
        for (int32 X = 0; X < Row.Num(); ++X)
        {
            if (Row[X] == 2)
            {
                Positions.Add(FVector(X, Y, 0));
            }
        }
    }

    if (Positions.Num() > 0)
    {
        int32 RandomIndex = rand() % Positions.Num();
        return Positions[RandomIndex];
    }

    return FVector();  // Возвращает нулевую позицию, если '2' не найдены
}

int32 ASpaceMaze::CountMazeCells(const TArray<TArray<int32>>& Maze)
{
    int32 Count = 0;
    for (const TArray<int32>& Row : Maze)
        for (int32 Element : Row)
            if (Element == CELL_MAZE) Count++;
    return Count;
}