#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <utility>

extern int rows;         // The count of rows of the game map.
extern int columns;      // The count of columns of the game map.
extern int total_mines;  // The count of mines of the game map.

// You MUST NOT use any other external variables except for rows, columns and total_mines.

// Client game state variables
char** client_map;    // Current map state from ReadMap()
bool** client_safe;   // Grids that are known to be safe
bool** client_mine;   // Grids that are known to be mines
bool** client_visited;// Grids that have been visited
bool** client_marked; // Grids that have been marked
int client_unvisited_count;  // Count of unvisited grids

// Helper function to check if coordinates are valid
bool IsValidClient(int r, int c) {
  return r >= 0 && r < rows && c >= 0 && c < columns;
}

// Helper function to count unvisited neighbors
int CountUnvisitedNeighbors(int r, int c) {
  int count = 0;
  for (int dr = -1; dr <= 1; dr++) {
    for (int dc = -1; dc <= 1; dc++) {
      if (dr == 0 && dc == 0) continue;
      int nr = r + dr, nc = c + dc;
      if (IsValidClient(nr, nc) && !client_visited[nr][nc] && !client_marked[nr][nc]) {
        count++;
      }
    }
  }
  return count;
}

// Helper function to count marked neighbors
int CountMarkedNeighbors(int r, int c) {
  int count = 0;
  for (int dr = -1; dr <= 1; dr++) {
    for (int dc = -1; dc <= 1; dc++) {
      if (dr == 0 && dc == 0) continue;
      int nr = r + dr, nc = c + dc;
      if (IsValidClient(nr, nc) && client_marked[nr][nc]) {
        count++;
      }
    }
  }
  return count;
}

// Helper function to find a safe move
bool FindSafeMove(int& safe_r, int& safe_c) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (client_visited[i][j] && client_map[i][j] >= '1' && client_map[i][j] <= '8') {
        int number = client_map[i][j] - '0';
        int marked_count = CountMarkedNeighbors(i, j);
        int unvisited_count = CountUnvisitedNeighbors(i, j);

        // If number equals marked + unvisited, all unvisited are mines
        if (number == marked_count + unvisited_count) {
          for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
              if (dr == 0 && dc == 0) continue;
              int nr = i + dr, nc = j + dc;
              if (IsValidClient(nr, nc) && !client_visited[nr][nc] && !client_marked[nr][nc]) {
                // Mark this as mine
                safe_r = nr;
                safe_c = nc;
                return true;
              }
            }
          }
        }

        // If number equals marked, all unvisited are safe
        if (number == marked_count && unvisited_count > 0) {
          for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
              if (dr == 0 && dc == 0) continue;
              int nr = i + dr, nc = j + dc;
              if (IsValidClient(nr, nc) && !client_visited[nr][nc] && !client_marked[nr][nc]) {
                // Visit this safe cell
                safe_r = nr;
                safe_c = nc;
                return true;
              }
            }
          }
        }
      }
    }
  }
  return false;
}

// Helper function to find a random unvisited cell
bool FindRandomUnvisited(int& r, int& c) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (!client_visited[i][j] && !client_marked[i][j]) {
        r = i;
        c = j;
        return true;
      }
    }
  }
  return false;
}

/**
 * @brief The definition of function Execute(int, int, bool)
 *
 * @details This function is designed to take a step when player the client's (or player's) role, and the implementation
 * of it has been finished by TA. (I hope my comments in code would be easy to understand T_T) If you do not understand
 * the contents, please ask TA for help immediately!!!
 *
 * @param r The row coordinate (0-based) of the block to be visited.
 * @param c The column coordinate (0-based) of the block to be visited.
 * @param type The type of operation to a certain block.
 * If type == 0, we'll execute VisitBlock(row, column).
 * If type == 1, we'll execute MarkMine(row, column).
 * If type == 2, we'll execute AutoExplore(row, column).
 * You should not call this function with other type values.
 */
void Execute(int r, int c, int type);

/**
 * @brief The definition of function InitGame()
 *
 * @details This function is designed to initialize the game. It should be called at the beginning of the game, which
 * will read the scale of the game map and the first step taken by the server (see README).
 */
void InitGame() {
  // Initialize all your global variables!
  client_map = new char*[rows];
  client_safe = new bool*[rows];
  client_mine = new bool*[rows];
  client_visited = new bool*[rows];
  client_marked = new bool*[rows];

  for (int i = 0; i < rows; i++) {
    client_map[i] = new char[columns];
    client_safe[i] = new bool[columns];
    client_mine[i] = new bool[columns];
    client_visited[i] = new bool[columns];
    client_marked[i] = new bool[columns];

    for (int j = 0; j < columns; j++) {
      client_map[i][j] = '?';
      client_safe[i][j] = false;
      client_mine[i][j] = false;
      client_visited[i][j] = false;
      client_marked[i][j] = false;
    }
  }

  client_unvisited_count = rows * columns;

  int first_row, first_column;
  std::cin >> first_row >> first_column;
  Execute(first_row, first_column, 0);
}

/**
 * @brief The definition of function ReadMap()
 *
 * @details This function is designed to read the game map from stdin when playing the client's (or player's) role.
 * Since the client (or player) can only get the limited information of the game map, so if there is a 3 * 3 map as
 * above and only the block (2, 0) has been visited, the stdin would be
 *     ???
 *     12?
 *     01?
 */
void ReadMap() {
  // Read the current map state from stdin
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      std::cin >> client_map[i][j];

      // Update visited and marked states
      if (client_map[i][j] != '?' && client_map[i][j] != '@' && client_map[i][j] != 'X') {
        // It's a number (visited non-mine)
        client_visited[i][j] = true;
        client_marked[i][j] = false;
        client_mine[i][j] = false;
        client_safe[i][j] = true;
      } else if (client_map[i][j] == '@') {
        // Marked mine
        client_marked[i][j] = true;
        client_visited[i][j] = false;
        client_mine[i][j] = true;
        client_safe[i][j] = false;
      } else if (client_map[i][j] == 'X') {
        // Wrong mark or visited mine (game over)
        client_marked[i][j] = false;
        client_visited[i][j] = true;
        client_mine[i][j] = false;
        client_safe[i][j] = false;
      } else {
        // Unknown
        client_visited[i][j] = false;
        client_marked[i][j] = false;
      }
    }
  }

  // Update unvisited count
  client_unvisited_count = 0;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (!client_visited[i][j] && !client_marked[i][j]) {
        client_unvisited_count++;
      }
    }
  }
}

/**
 * @brief The definition of function Decide()
 *
 * @details This function is designed to decide the next step when playing the client's (or player's) role. Open up your
 * mind and make your decision here! Caution: you can only execute once in this function.
 */
void Decide() {
  int r, c;
  int type = 0; // Default to visit

  // Strategy 1: Look for obvious safe moves
  if (FindSafeMove(r, c)) {
    // Found a safe move - either visit safe cell or mark mine
    // Check if we should mark or visit
    bool should_mark = false;

    // If the cell is already known to be a mine, mark it
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < columns; j++) {
        if (client_visited[i][j] && client_map[i][j] >= '1' && client_map[i][j] <= '8') {
          int number = client_map[i][j] - '0';
          int marked_count = CountMarkedNeighbors(i, j);
          int unvisited_count = CountUnvisitedNeighbors(i, j);

          // If number equals marked + unvisited, all unvisited are mines
          if (number == marked_count + unvisited_count) {
            for (int dr = -1; dr <= 1; dr++) {
              for (int dc = -1; dc <= 1; dc++) {
                if (dr == 0 && dc == 0) continue;
                int nr = i + dr, nc = j + dc;
                if (nr == r && nc == c) {
                  should_mark = true;
                  break;
                }
              }
              if (should_mark) break;
            }
          }
        }
      }
    }

    if (should_mark) {
      type = 1; // Mark mine
    } else {
      type = 0; // Visit safe cell
    }
  }
  // Strategy 2: Make a random safe move
  else if (FindRandomUnvisited(r, c)) {
    type = 0; // Visit random unvisited cell
  }
  // Strategy 3: If no unvisited cells, try auto-explore
  else {
    // Look for visited cells that might benefit from auto-explore
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < columns; j++) {
        if (client_visited[i][j] && client_map[i][j] >= '1' && client_map[i][j] <= '8') {
          int number = client_map[i][j] - '0';
          int marked_count = CountMarkedNeighbors(i, j);

          // If number equals marked count, auto-explore might be useful
          if (number == marked_count && CountUnvisitedNeighbors(i, j) > 0) {
            r = i;
            c = j;
            type = 2; // Auto-explore
            break;
          }
        }
      }
      if (type == 2) break;
    }
  }

  // Execute the chosen move
  Execute(r, c, type);
}

#endif