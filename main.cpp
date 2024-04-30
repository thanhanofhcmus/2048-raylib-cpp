#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <format>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <type_traits>

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

static constexpr std::array BLOCK_COLORS{
    Color{225, 225, 225, 255}, // 0|1 - None
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

using Value = std::uint64_t;
using Row = std::array<Value, BOARD_SIZE>;
using Board = std::array<Row, BOARD_SIZE>;
using PushFn = Board (*)(Board const &);

std::ostream &operator<<(std::ostream &o, Row const &r) {
  for (auto v : r) {
    o << v << ' ';
  }
  return o;
}

void print_board(Board const &b) {
  for (auto const &r : b) {
    std::cout << r << '\n';
  }
  std::cout << '\n';
}

Board generate_new_title(Board const &board) {
  Board result{board};
  int len = static_cast<int>(board.size());
  int x = GetRandomValue(0, len - 1);
  int y = GetRandomValue(0, len - 1);
  while (board[x][y] != 0) {
    x = GetRandomValue(0, len - 1);
    y = GetRandomValue(0, len - 1);
  }
  result[x][y] = 2;
  return result;
}

Value calculate_score(Board const &b) {
  return std::transform_reduce(
      b.begin(), b.end(), Value{0}, std::plus<>(),
      [](Row const &r) -> Value { return std::reduce(r.begin(), r.end()); });
}

Row merge_row_left(Row const &input) {
  Row row{input};
  int left_index = -1;
  int len = static_cast<int>(row.size());

  // merge
  for (int i = 0; i < len; ++i) {
    Value v = row[i];
    if (v == 0) {
      continue;
    }
    if (left_index >= 0 && v == row[left_index]) {
      row[left_index] *= 2;
      row[i] = 0;
      left_index = -1;
    } else {
      left_index = i;
    }
  }

  // push non-zero to left
  left_index = 0;
  for (int i = 0; i < len; ++i) {
    Value v = row[i];
    if (v == 0) {
      continue;
    }
    if (left_index != i) {
      row[left_index] = v;
      row[i] = 0;
    }
    left_index++;
  }
  return row;
}

Row reverse_row(Row const &row) {
  Row r{};
  std::reverse_copy(row.begin(), row.end(), r.begin());
  return r;
}

bool is_board_full(Board const &board) {
  for (auto const &r : board) {
    for (auto const &v : r) {
      if (v == 0) {
        return false;
      }
    }
  }
  return true;
}

Board transpose(Board const &board) {
  Board result{};
  for (size_t i = 0; i < board.size(); ++i) {
    for (size_t j = 0; j < board[i].size(); ++j) {
      result[i][j] = board[j][i];
    }
  }
  return result;
}

Board rotate_cw(Board const &board) {
  Board result = transpose(board);
  // reverse column
  std::transform(result.begin(), result.end(), result.begin(), reverse_row);
  return result;
}

Board rotate_ccw(Board const &board) {
  Board result = transpose(board);
  // reverse row
  std::reverse(result.begin(), result.end());
  return result;
}

Board push_left(Board const &board) {
  Board result{board};
  std::transform(result.begin(), result.end(), result.begin(), merge_row_left);
  return result;
}

Board push_right(Board const &board) {
  auto fn = [](Row const &r) {
    auto x = reverse_row(r);
    x = merge_row_left(x);
    x = reverse_row(x);
    return x;
  };
  Board result{board};
  std::transform(result.begin(), result.end(), result.begin(), fn);

  return result;
}

Board push_up(Board const &board) { return rotate_ccw(push_left(rotate_cw(board))); }

Board push_down(Board const &board) { return rotate_cw(push_left(rotate_ccw(board))); }

bool is_game_over(Board const &board) {
  std::array fns{push_left, push_right, push_up, push_down};

  for (auto fn : fns) {
    Board tmp = fn(board);
    if (board != tmp) {
      return false;
    }
  }
  return true;
}

void draw_game_board(Board const &board) {
  constexpr int padding = 2;
  constexpr int font_padding = 5;
  char text[100]{};

  const Value score = calculate_score(board);
  snprintf(text, sizeof(text), "Score: %lld", score);
  DrawText(text, GAME_PADDING, GAME_HEIGHT + GAME_PADDING, FONT_SIZE, WHITE);

  for (size_t i = 0; i < board.size(); ++i) {
    for (size_t j = 0; j < board[i].size(); ++j) {
      const int top_left_x = j * (BLOCK_WIDTH + padding);
      const int top_left_y = i * (BLOCK_WIDTH + padding);
      const Value value = board[i][j];
      const int color_idx = value == 0 ? 0 : std::countr_zero(value);

      DrawRectangle(top_left_x, top_left_y, BLOCK_WIDTH, BLOCK_WIDTH, BLOCK_COLORS[color_idx]);
      if (value != 0) {
        std::snprintf(text, sizeof(text), "%llu", value);
        DrawText(text, top_left_x + font_padding, top_left_y + font_padding, FONT_SIZE, WHITE);
      }
    }
  }
}

Board update_board(Board const &board) {
  PushFn push_fn = nullptr;

  int c = GetCharPressed();
  if (c == 'a') {
    push_fn = push_left;
  }
  if (c == 'd') {
    push_fn = push_right;
  }
  if (c == 's') {
    push_fn = push_up;
  }
  if (c == 'w') {
    push_fn = push_down;
  }

  if (push_fn == nullptr) {
    return board;
  }

  Board result = push_fn(board);
  if (result == board) {
    std::cout << "Cannot move\n";
    return result;
  }
  if (!is_board_full(result)) {
    result = generate_new_title(result);
  }
  if (is_game_over(result)) {
    std::cout << "Game Over\n";
  }

  return result;
}

int main() {
  SetRandomSeed(124);

  Board board = generate_new_title(Board{});

  InitWindow(SCR_WIDTH, SCR_HEIGHT, "2048");

  SetExitKey(KEY_Q);

  while (!WindowShouldClose()) {
    board = update_board(board);

    BeginDrawing();
    ClearBackground(GRAY);
    draw_game_board(board);
    EndDrawing();
  }

  CloseWindow();
}
