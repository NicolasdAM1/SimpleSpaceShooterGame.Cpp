#include "raylib.h"
#include <vector>
#include <cmath>
#include <algorithm>

struct Bullet {
    Vector2 pos;
    float speed;
    bool alive;
};

struct Enemy {
    Vector2 pos;
    float speed;
    float size;
    bool alive;
};


int main() {
    InitWindow(800, 900, "Space Shooter");
    SetTargetFPS(60);

    Vector2 player = {400, 820};
    float playerSpeed = 300;

    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;

    int score = 0;
    int lives = 3;

    float shootCooldown = 0.25f;
    float shootTimer = 0;

    float enemyTimer = 0;
    float enemySpawnRate = 0.8f;

    bool gameOver = false;

    // TELA INICIAL
    bool showStartMessage = true;

    Rectangle base;
    base.width = 800;
    base.height = 140;
    base.x = 0;
    base.y = 900 - base.height;

    // CÉU ESTRELADO
    const int STAR_COUNT = 100;
    Vector2 stars[STAR_COUNT];
    for (int i = 0; i < STAR_COUNT; i++) {
        stars[i] = {
            (float)GetRandomValue(0, 800),
            (float)GetRandomValue(0, 900)
        };
    }

    // CARREGA A IMAGEM DA BASE
    Texture2D baseTexture = LoadTexture("nave_base.png");
    Texture2D meteoro = LoadTexture("meteoro.png");


    Rectangle destBase = {
        base.x + 10.0f,
        base.y + 10.0f,
        base.width - 20.0f,
        base.height - 20.0f
    };

    Rectangle srcBase = {
        0.0f,
        (float)baseTexture.height - destBase.height,
        (float)baseTexture.width,
        destBase.height
    };

    //  MUNIÇÃO
    int ammo = 12;
    const int maxAmmo = 12;
    float reloadTimer = 0.0f;
    float reloadInterval = 0.5f;

    // CARREGA A NAVE DO JOGADOR
    Texture2D nave = LoadTexture("nave.png");

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // TELA INICIAL NOVA

        if (showStartMessage) {

            BeginDrawing();
            ClearBackground(BLACK);

            // fundo estrelado igual ao jogo
            for (int i = 0; i < STAR_COUNT; i++)
                DrawPixelV(stars[i], WHITE);

            // textos centralizados
            const char* title = "PROTEJA SUA NAVE DOS METEOROS";
            int titleWidth = MeasureText(title, 38);
            int titleX = (800 - titleWidth) / 2;

            const char* press = "Pressione ENTER para comecar";
            int pressWidth = MeasureText(press, 22);
            int pressX = (800 - pressWidth) / 2;

            const char* credits = "Um space shooter desenvolvido por Sandyele, Giulia, Marco e Nicolas";
            int credWidth = MeasureText(credits, 14);
            int credX = (800 - credWidth) / 2;

            DrawText(title, titleX, 300, 38, RED);
            DrawText(press, pressX, 360, 22, YELLOW);
            DrawText(credits, credX, 540, 14, DARKBLUE);

            EndDrawing();

            if (IsKeyPressed(KEY_ENTER)) {
                showStartMessage = false;
            }

            continue; // não deixa o jogo rodar até ENTER
        }

        //        LÓGICA DO JOGO
        if (!gameOver) {
            // Movimento do player
            if (IsKeyDown(KEY_LEFT))  player.x -= playerSpeed * dt;
            if (IsKeyDown(KEY_RIGHT)) player.x += playerSpeed * dt;
            if (IsKeyDown(KEY_UP))    player.y -= playerSpeed * dt;
            if (IsKeyDown(KEY_DOWN))  player.y += playerSpeed * dt;

            // Limites
            if (player.x < 20) player.x = 20;
            if (player.x > 780) player.x = 780;
            if (player.y < 40) player.y = 40;
            if (player.y > base.y - 40) player.y = base.y - 40;

            // ATIRAR
            shootTimer += dt;
            if (IsKeyDown(KEY_SPACE) && shootTimer >= shootCooldown && ammo > 0) {
                bullets.push_back({{player.x, player.y - 20}, 420, true});
                shootTimer = 0;
                ammo--;
            }

            // RECARREGAR
            reloadTimer += dt;
            if (reloadTimer >= reloadInterval) {
                reloadTimer = 0;
                if (ammo < maxAmmo) ammo++;
            }

            // ENEMY SPAWN
            enemyTimer += dt;
            if (enemyTimer >= enemySpawnRate) {
                enemyTimer = 0;

                float randomSize = (float)GetRandomValue(40, 120); // tamanho aleatório entre 40 e 120px

                enemies.push_back({
                    {(float)GetRandomValue(40, 760), -30},
                    (float)(110 + GetRandomValue(0, 40)),
                    randomSize,
                    true
                });
            }


            // BALAS
            for (auto &b : bullets) {
                if (!b.alive) continue;
                b.pos.y -= b.speed * dt;
                if (b.pos.y < -20) b.alive = false;
            }

            // INIMIGOS
            for (auto &e : enemies) {
                if (!e.alive) continue;
                e.pos.y += e.speed * dt;

                if (e.pos.y + 18 >= base.y) {
                    e.alive = false;
                    lives--;
                    if (lives <= 0) gameOver = true;
                }
            }

            // COLISÕES
            for (auto &b : bullets) {
                if (!b.alive) continue;

                for (auto &e : enemies) {
                    if (!e.alive) continue;

                    float dx = b.pos.x - e.pos.x;
                    float dy = b.pos.y - e.pos.y;

                    if (sqrtf(dx*dx + dy*dy) < e.size/2) {
                        b.alive = false;
                        e.alive = false;
                        score += 10;
                    }

                }
            }

            // remover mortos
            bullets.erase(
                remove_if(bullets.begin(), bullets.end(),
                          [](Bullet &b){ return !b.alive; }),
                bullets.end()
            );
            enemies.erase(
                remove_if(enemies.begin(), enemies.end(),
                          [](Enemy &e){ return !e.alive; }),
                enemies.end()
            );
        }

        // Reiniciar
        if (gameOver && IsKeyPressed(KEY_R)) {
            enemies.clear();
            bullets.clear();
            score = 0;
            lives = 3;
            ammo = maxAmmo;
            gameOver = false;
        }

        //         DESENHO
        BeginDrawing();
        ClearBackground(BLACK);

        // Estrelas
        for (int i = 0; i < STAR_COUNT; i++)
            DrawPixelV(stars[i], WHITE);

        // Base com imagem
        DrawTexturePro(baseTexture, srcBase, destBase, {0,0}, 0, WHITE);

        //     DESENHA A NAVE NOVA
        // ---- NAVE MAIOR (96x96) ----
        float naveSize = 80;  // deixe 80 se quiser menor

        Rectangle srcNave = { 0, 0, (float)nave.width, (float)nave.height };

        Rectangle destNave = {
            player.x,
            player.y,
            naveSize,
            naveSize
        };

        Vector2 originNave = { naveSize / 2, naveSize / 2 };

        DrawTexturePro(nave, srcNave, destNave, originNave, 0, WHITE);


        // Balas
        for (auto &b : bullets)
            DrawCircleV(b.pos, 5, BLUE);

        // Inimigos (meteoros)
        for (auto &e : enemies) {
            Rectangle src = { 0, 0, (float)meteoro.width, (float)meteoro.height };
            Rectangle dest = { e.pos.x, e.pos.y, e.size, e.size };
            Vector2 origin = { e.size/2, e.size/2 };

            DrawTexturePro(meteoro, src, dest, origin, 0, WHITE);
        }



        DrawText(TextFormat("Score: %d", score), 10, 10, 20, WHITE);
        DrawText(TextFormat("Lives: %d", lives), 10, 35, 20, WHITE);
        DrawText(TextFormat("Ammo: %d/%d", ammo, maxAmmo), 10, 60, 20, WHITE);

        if (gameOver) {
            DrawText("GAME OVER", 260, 380, 40, RED);
            DrawText("Pressione R para reiniciar", 240, 430, 20, GRAY);
        }

        EndDrawing();
    }

    UnloadTexture(baseTexture);
    UnloadTexture(nave);
    UnloadTexture(meteoro);
    CloseWindow();
    return 0;
}
