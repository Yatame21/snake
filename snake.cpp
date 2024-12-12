#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;

Color grey = { 50, 50, 50, 1 };
Color purple = { 128, 0, 128, 255 };
Color darkTeal = { 0, 100, 100, 255 };
Color black = { 0, 0, 0, 255 };

float cellSize = 30;
float cellCount = 25;
float offset = 75;

double lastUpdateTime = 0;

bool ElementInDeque(Vector2 element, deque<Vector2> deque)
{
    for (unsigned int i = 0; i < deque.size(); i++)
    {
        if (Vector2Equals(deque[i], element))
        {
            return true;
        }
    }
    return false;
}

bool eventTriggered(double interval)
{
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

class Snake
{
public:
    deque<Vector2> body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };
    Vector2 direction = { 1, 0 };
    bool addSegment = false;

    void Draw()
    {
        for (unsigned int i = 0; i < body.size(); i++)
        {
            int x = body[i].x;
            int y = body[i].y;
            Rectangle segment = Rectangle{ offset + x * cellSize, offset + y * cellSize, cellSize, cellSize };
            DrawRectangleRounded(segment, 0.5, 6, purple);
        }
    }

    void Update()
    {
        body.push_front(Vector2Add(body[0], direction));
        if (addSegment)
        {
            addSegment = false;
        }
        else
        {
            body.pop_back();
        }
    }

    void Reset()
    {
        body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };
        direction = { 1, 0 };
    }
};

class Food
{
public:
    Vector2 position;
    Texture2D texture;

    Food(deque<Vector2> snakeBody)
    {
        Image image = LoadImage("fish.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = RandomPos(snakeBody);
    }

    ~Food()
    {
        UnloadTexture(texture);
    }

    void Draw()
    {
        Rectangle source = { 0, 0, (float)texture.width, (float)texture.height };
        Rectangle dest = { offset + position.x * cellSize, offset + position.y * cellSize, cellSize, cellSize };
        DrawTexturePro(texture, source, dest, { 0, 0 }, 0.0f, WHITE);
    }

    Vector2 GenerateRandomCell()
    {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{ x, y };
    }

    Vector2 RandomPos(deque<Vector2> snakeBody)
    {
        Vector2 position = GenerateRandomCell();
        while (ElementInDeque(position, snakeBody))
        {
            position = GenerateRandomCell();
        }
        return position;
    }
};

class Game
{
public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool running = true;
    int score = 0;
    Sound eatSound;
    Sound stukSound;

    Game()
    {
        InitAudioDevice();
        eatSound = LoadSound("eat.mp3");
        stukSound = LoadSound("stuk.mp3");
    }

    ~Game()
    {
        UnloadSound(eatSound);
        UnloadSound(stukSound);
        CloseAudioDevice();
    }

    void Draw()
    {
        food.Draw();
        snake.Draw();
    }

    void Update()
    {
        if (running)
        {
            snake.Update();
            CheckFoodCollision();
            CheckWallCollision();
            CheckSelfCollision();
        }
    }

    void CheckFoodCollision()
    {
        if (Vector2Equals(snake.body[0], food.position))
        {
            food.position = food.RandomPos(snake.body);
            snake.addSegment = true;
            score++;
            PlaySound(eatSound);
        }
    }

    void CheckWallCollision()
    {
        if (snake.body[0].x == cellCount || snake.body[0].x == -1 || snake.body[0].y == cellCount || snake.body[0].y == -1)
        {
            GameOver();
        }
    }

    void GameOver()
    {
        snake.Reset();
        food.position = food.RandomPos(snake.body);
        running = false;
        score = 0;
        PlaySound(stukSound);
    }

    void CheckSelfCollision()
    {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (ElementInDeque(snake.body[0], headlessBody))
        {
            GameOver();
        }
    }
};

int main()
{
    cout << "starting the game..." << endl;
    InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Underwater Snake");
    SetTargetFPS(60);

    Game game = Game();

    while (WindowShouldClose() == false)
    {
        BeginDrawing();

        if (eventTriggered(0.2))
        {
            game.Update();
        }

        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
        {
            game.snake.direction = { 0, -1 };
            game.running = true;
        }
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
        {
            game.snake.direction = { 0, 1 };
            game.running = true;
        }
        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1)
        {
            game.snake.direction = { 1, 0 };
            game.running = true;
        }
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1)
        {
            game.snake.direction = { -1, 0 };
            game.running = true;
        }

        if (IsKeyPressed(KEY_W) && game.snake.direction.y != 1)
        {
            game.snake.direction = { 0, -1 };
            game.running = true;
        }
        if (IsKeyPressed(KEY_S) && game.snake.direction.y != -1)
        {
            game.snake.direction = { 0, 1 };
            game.running = true;
        }
        if (IsKeyPressed(KEY_D) && game.snake.direction.x != -1)
        {
            game.snake.direction = { 1, 0 };
            game.running = true;
        }
        if (IsKeyPressed(KEY_A) && game.snake.direction.x != 1)
        {
            game.snake.direction = { -1, 0 };
            game.running = true;
        }

        ClearBackground(grey);
        DrawRectangleLinesEx(Rectangle{ offset - 5, offset - 5, cellSize * cellCount + 10, cellSize * cellCount + 10 }, 5, darkTeal);
        DrawText("Underwater", offset - 5, 20, 40, darkTeal);
        DrawText("Snake", offset + 240, 20, 40, purple);
        DrawText(TextFormat("%i", game.score), GetScreenWidth() - 100, 20, 40, black);

        game.Draw();

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
