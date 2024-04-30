#include <array>
#include <cstdint>
#include <iostream>
#include <iterator>

#include "raylib.h"

constexpr int BOARD_SIZE = 4;
constexpr int GAME_WIDTH = 400;

constexpr int BLOCK_WIDTH = GAME_WIDTH / BOARD_SIZE;
constexpr int FONT_SIZE = BLOCK_WIDTH / 4;

constexpr int GAME_PADDING = 10;
constexpr int UI_HEIGHT = FONT_SIZE + 10;
constexpr int GAME_HEIGHT = GAME_WIDTH;
constexpr int SCR_WIDTH = GAME_WIDTH + GAME_PADDING;
constexpr int SCR_HEIGHT = GAME_HEIGHT + UI_HEIGHT + GAME_PADDING;

[[maybe_unused]] static constexpr std::array BLOCK_COLORS{
    Color{225, 225, 225, 255}, // 0 - None
    Color{225, 225, 225, 255}, // 1 - None
    Color{255, 204, 21, 255},  // 2
    Color{251, 146, 60, 255},  // 4
    Color{248, 113, 113, 255}, // 8
    Color{96, 165, 250, 255},  // 16
    Color{74, 222, 128, 255},  // 32
    Color{163, 230, 53, 255},  // 64
    Color{52, 211, 153, 255},  // 128
    Color{45, 212, 191, 255},  // 256
    Color{129, 140, 248, 255}, // 512
    Color{167, 139, 250, 255}, // 1024
    Color{192, 132, 252, 255}, // 2048
    Color{232, 121, 249, 255}, // 4096
    Color{251, 113, 133, 255}, // 8192
};

using Value = std::int64_t;
using Board = std::array<std::array<Value, BOARD_SIZE>, BOARD_SIZE>;
using PushFn = void (*)(Value board[BOARD_SIZE][BOARD_SIZE]);

int main() {

  SetRandomSeed(124);

  // Board board{};

  // generate_new_title(board);

  InitWindow(SCR_WIDTH, SCR_HEIGHT, "2048");

  SetExitKey(KEY_Q);

  while (!WindowShouldClose()) {
    // PollInputEvents();

    // update_board(board);

    BeginDrawing();
    ClearBackground(GRAY);
    // draw_game_board(board);
    EndDrawing();
  }

  CloseWindow();
}
