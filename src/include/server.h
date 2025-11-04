#ifndef SERVER_H
#define SERVER_H

#include <cstdlib>
#include <iostream>

/*
 * You may need to define some global variables for the information of the game map here.
 * Although we don't encourage to use global variables in real cpp projects, you may have to use them because the use of
 * class is not taught yet. However, if you are member of A-class or have learnt the use of cpp class, member functions,
 * etc., you're free to modify this structure.
 */
int rows;         // The count of rows of the game map. You MUST NOT modify its name.
int columns;      // The count of columns of the game map. You MUST NOT modify its name.
int total_mines;  // The count of mines of the game map. You MUST NOT modify its name. You should initialize this
                  // variable in function InitMap. It will be used in the advanced task.
int game_state;  // The state of the game, 0 for continuing, 1 for winning, -1 for losing. You MUST NOT modify its name.

// Game state variables
char** game_map;      // Original game map ('.' for non-mine, 'X' for mine)
char** display_map;   // Current display state ('?' for unvisited, numbers for visited non-mine, 'X' for visited mine, '@' for marked mine)
bool** visited;       // Whether each cell has been visited
bool** marked;        // Whether each cell has been marked as mine
int visited_count;    // Number of visited non-mine cells
int marked_mine_count;// Number of correctly marked mines

// Helper function to check if coordinates are valid
bool IsValid(int r, int c) {
  return r >= 0 && r < rows && c >= 0 && c < columns;
}

// Helper function to count adjacent mines
int CountAdjacentMines(int r, int c) {
  int count = 0;
  for (int dr = -1; dr <= 1; dr++) {
    for (int dc = -1; dc <= 1; dc++) {
      if (dr == 0 && dc == 0) continue;
      int nr = r + dr, nc = c + dc;
      if (IsValid(nr, nc) && game_map[nr][nc] == 'X') {
        count++;
      }
    }
  }
  return count;
}

// Helper function to recursively visit blocks with 0 mine count
void VisitRecursive(int r, int c) {
  if (!IsValid(r, c) || visited[r][c] || marked[r][c]) {
    return;
  }

  visited[r][c] = true;

  if (game_map[r][c] == 'X') {
    // Should not happen in recursive calls
    display_map[r][c] = 'X';
    return;
  }

  // Count adjacent mines
  int mine_count = CountAdjacentMines(r, c);
  display_map[r][c] = '0' + mine_count;
  visited_count++;

  // If mine count is 0, recursively visit neighbors
  if (mine_count == 0) {
    for (int dr = -1; dr <= 1; dr++) {
      for (int dc = -1; dc <= 1; dc++) {
        if (dr == 0 && dc == 0) continue;
        VisitRecursive(r + dr, c + dc);
      }
    }
  }
}

// Helper function to check if game is won
bool CheckWin() {
  return visited_count == (rows * columns - total_mines);
}

/**
 * @brief The definition of function InitMap()
 *
 * @details This function is designed to read the initial map from stdin. For example, if there is a 3 * 3 map in which
 * mines are located at (0, 1) and (1, 2) (0-based), the stdin would be
 *     3 3
 *     .X.
 *     ...
 *     ..X
 * where X stands for a mine block and . stands for a normal block. After executing this function, your game map
 * would be initialized, with all the blocks unvisited.
 */
void InitMap() {
  std::cin >> rows >> columns;

  // Initialize game state
  game_state = 0;
  total_mines = 0;
  visited_count = 0;
  marked_mine_count = 0;

  // Allocate memory for maps
  game_map = new char*[rows];
  display_map = new char*[rows];
  visited = new bool*[rows];
  marked = new bool*[rows];

  for (int i = 0; i < rows; i++) {
    game_map[i] = new char[columns];
    display_map[i] = new char[columns];
    visited[i] = new bool[columns];
    marked[i] = new bool[columns];

    for (int j = 0; j < columns; j++) {
      std::cin >> game_map[i][j];
      display_map[i][j] = '?';
      visited[i][j] = false;
      marked[i][j] = false;

      if (game_map[i][j] == 'X') {
        total_mines++;
      }
    }
  }
}

/**
 * @brief The definition of function VisitBlock(int, int)
 *
 * @details This function is designed to visit a block in the game map. We take the 3 * 3 game map above as an example.
 * At the beginning, if you call VisitBlock(0, 0), the return value would be 0 (game continues), and the game map would
 * be
 *     1??
 *     ???
 *     ???
 * If you call VisitBlock(0, 1) after that, the return value would be -1 (game ends and the players loses) , and the
 * game map would be
 *     1X?
 *     ???
 *     ???
 * If you call VisitBlock(0, 2), VisitBlock(2, 0), VisitBlock(1, 2) instead, the return value of the last operation
 * would be 1 (game ends and the player wins), and the game map would be
 *     1@1
 *     122
 *     01@
 *
 * @param r The row coordinate (0-based) of the block to be visited.
 * @param c The column coordinate (0-based) of the block to be visited.
 *
 * @note You should edit the value of game_state in this function. Precisely, edit it to
 *    0  if the game continues after visit that block, or that block has already been visited before.
 *    1  if the game ends and the player wins.
 *    -1 if the game ends and the player loses.
 *
 * @note For invalid operation, you should not do anything.
 */
void VisitBlock(int r, int c) {
  // Check if game is already over or coordinates are invalid
  if (game_state != 0 || !IsValid(r, c)) {
    return;
  }

  // If already visited or marked, do nothing
  if (visited[r][c] || marked[r][c]) {
    return;
  }

  // If it's a mine, game over
  if (game_map[r][c] == 'X') {
    visited[r][c] = true;
    display_map[r][c] = 'X';
    game_state = -1;
    return;
  }

  // Visit the block (recursively if mine count is 0)
  VisitRecursive(r, c);

  // Check if game is won
  if (CheckWin()) {
    game_state = 1;
  }
}

/**
 * @brief The definition of function MarkMine(int, int)
 *
 * @details This function is designed to mark a mine in the game map.
 * If the block being marked is a mine, show it as "@".
 * If the block being marked isn't a mine, END THE GAME immediately. (NOTE: This is not the same rule as the real
 * game) And you don't need to
 *
 * For example, if we use the same map as before, and the current state is:
 *     1?1
 *     ???
 *     ???
 * If you call MarkMine(0, 1), you marked the right mine. Then the resulting game map is:
 *     1@1
 *     ???
 *     ???
 * If you call MarkMine(1, 0), you marked the wrong mine(There's no mine in grid (1, 0)).
 * The game_state would be -1 and game ends immediately. The game map would be:
 *     1?1
 *     X??
 *     ???
 * This is different from the Minesweeper you've played. You should beware of that.
 *
 * @param r The row coordinate (0-based) of the block to be marked.
 * @param c The column coordinate (0-based) of the block to be marked.
 *
 * @note You should edit the value of game_state in this function. Precisely, edit it to
 *    0  if the game continues after visit that block, or that block has already been visited before.
 *    1  if the game ends and the player wins.
 *    -1 if the game ends and the player loses.
 *
 * @note For invalid operation, you should not do anything.
 */
void MarkMine(int r, int c) {
  // Check if game is already over or coordinates are invalid
  if (game_state != 0 || !IsValid(r, c)) {
    return;
  }

  // If already visited or marked, do nothing
  if (visited[r][c] || marked[r][c]) {
    return;
  }

  // Mark the block
  marked[r][c] = true;

  // If it's a mine, mark as @ and update count
  if (game_map[r][c] == 'X') {
    display_map[r][c] = '@';
    marked_mine_count++;
  } else {
    // If it's not a mine, game over immediately
    display_map[r][c] = 'X';
    game_state = -1;
    return;
  }

  // Check if game is won (all non-mine blocks visited)
  if (CheckWin()) {
    game_state = 1;
  }
}

/**
 * @brief The definition of function AutoExplore(int, int)
 *
 * @details This function is designed to auto-visit adjacent blocks of a certain block.
 * See README.md for more information
 *
 * For example, if we use the same map as before, and the current map is:
 *     ?@?
 *     ?2?
 *     ??@
 * Then auto explore is available only for block (1, 1). If you call AutoExplore(1, 1), the resulting map will be:
 *     1@1
 *     122
 *     01@
 * And the game ends (and player wins).
 */
void AutoExplore(int r, int c) {
  // Check if game is already over or coordinates are invalid
  if (game_state != 0 || !IsValid(r, c)) {
    return;
  }

  // Auto-explore can only target visited non-mine grids
  if (!visited[r][c] || game_map[r][c] == 'X') {
    return;
  }

  // Count marked mines around the target
  int marked_count = 0;
  for (int dr = -1; dr <= 1; dr++) {
    for (int dc = -1; dc <= 1; dc++) {
      if (dr == 0 && dc == 0) continue;
      int nr = r + dr, nc = c + dc;
      if (IsValid(nr, nc) && marked[nr][nc]) {
        marked_count++;
      }
    }
  }

  // Get the mine count of the target block
  int mine_count = CountAdjacentMines(r, c);

  // If marked count equals mine count, visit all non-mine neighbors
  if (marked_count == mine_count) {
    for (int dr = -1; dr <= 1; dr++) {
      for (int dc = -1; dc <= 1; dc++) {
        if (dr == 0 && dc == 0) continue;
        int nr = r + dr, nc = c + dc;
        if (IsValid(nr, nc) && !visited[nr][nc] && !marked[nr][nc]) {
          // Visit the neighbor (this will handle recursive expansion)
          VisitBlock(nr, nc);
          // Check if game ended during visit
          if (game_state != 0) {
            return;
          }
        }
      }
    }
  }

  // Check if game is won after auto-explore
  if (CheckWin()) {
    game_state = 1;
  }
}

/**
 * @brief The definition of function ExitGame()
 *
 * @details This function is designed to exit the game.
 * It outputs a line according to the result, and a line of two integers, visit_count and marked_mine_count,
 * representing the number of blocks visited and the number of marked mines taken respectively.
 *
 * @note If the player wins, we consider that ALL mines are correctly marked.
 */
void ExitGame() {
  // Output result message
  if (game_state == 1) {
    std::cout << "YOU WIN!" << std::endl;
    // On victory, marked_mine_count should be total mines
    std::cout << visited_count << " " << total_mines << std::endl;
  } else {
    std::cout << "GAME OVER!" << std::endl;
    std::cout << visited_count << " " << marked_mine_count << std::endl;
  }

  exit(0);  // Exit the game immediately
}

/**
 * @brief The definition of function PrintMap()
 *
 * @details This function is designed to print the game map to stdout. We take the 3 * 3 game map above as an example.
 * At the beginning, if you call PrintMap(), the stdout would be
 *    ???
 *    ???
 *    ???
 * If you call VisitBlock(2, 0) and PrintMap() after that, the stdout would be
 *    ???
 *    12?
 *    01?
 * If you call VisitBlock(0, 1) and PrintMap() after that, the stdout would be
 *    ?X?
 *    12?
 *    01?
 * If the player visits all blocks without mine and call PrintMap() after that, the stdout would be
 *    1@1
 *    122
 *    01@
 * (You may find the global variable game_state useful when implementing this function.)
 *
 * @note Use std::cout to print the game map, especially when you want to try the advanced task!!!
 */
void PrintMap() {
  // If game is won, show all mines as @
  if (game_state == 1) {
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < columns; j++) {
        if (game_map[i][j] == 'X') {
          std::cout << '@';
        } else {
          std::cout << display_map[i][j];
        }
      }
      std::cout << std::endl;
    }
  } else {
    // Normal display
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < columns; j++) {
        std::cout << display_map[i][j];
      }
      std::cout << std::endl;
    }
  }
}

#endif
