// Copyright 2019 DeepMind Technologies Limited
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "open_spiel/games/tic_tac_toe/tic_tac_toe.h"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "open_spiel/spiel_utils.h"
#include "open_spiel/utils/tensor_view.h"

namespace open_spiel {
namespace tic_tac_toe {
namespace {

// Facts about the game.
const GameType kGameType{
    /*short_name=*/"tic_tac_toe",
    /*long_name=*/"Tic Tac Toe",
    GameType::Dynamics::kSequential,
    GameType::ChanceMode::kDeterministic,
    GameType::Information::kPerfectInformation,
    GameType::Utility::kZeroSum,
    GameType::RewardModel::kTerminal,
    /*max_num_players=*/2,
    /*min_num_players=*/2,
    /*provides_information_state_string=*/true,
    /*provides_information_state_tensor=*/false,
    /*provides_observation_string=*/true,
    /*provides_observation_tensor=*/true,
    /*parameter_specification=*/{}  // no parameters
};

std::shared_ptr<const Game> Factory(const GameParameters& params) {
  return std::shared_ptr<const Game>(new TicTacToeGame(params));
}

REGISTER_SPIEL_GAME(kGameType, Factory);

RegisterSingleTensorObserver single_tensor(kGameType.short_name);

}  // namespace

CellState PlayerToState(Player player) {
  switch (player) {
    case 0:
      return CellState::kCross;
    case 1:
      return CellState::kNought;
    default:
      SpielFatalError(absl::StrCat("Invalid player id ", player));
      return CellState::kEmpty;
  }
}

std::string StateToString(CellState state) {
  switch (state) {
    case CellState::kEmpty:
      return ".";
    case CellState::kNought:
      return "o";
    case CellState::kCross:
      return "x";
    default:
      SpielFatalError("Unknown state.");
  }
}
// 8,8,5 chyba
// bool BoardHasLine(const std::array<CellState, kNumCells>& board, const Player player) {
//     CellState c = PlayerToState(player);

//     // Check rows
//     for (int i = 0; i < 8; ++i) {
//         for (int j = 0; j <= 3; ++j) {  // zmiana zakresu z 6 na 3
//             if (board[i * 8 + j] == c && board[i * 8 + j + 1] == c && 
//                 board[i * 8 + j + 2] == c && board[i * 8 + j + 3] == c &&
//                 board[i * 8 + j + 4] == c) {  // sprawdzanie 5 komórek
//                 return true;
//             }
//         }
//     }

//     // Check columns
//     for (int i = 0; i < 8; ++i) {
//         for (int j = 0; j <= 3; ++j) {  // zmiana zakresu z 6 na 3
//             if (board[j * 8 + i] == c && board[(j + 1) * 8 + i] == c && 
//                 board[(j + 2) * 8 + i] == c && board[(j + 3) * 8 + i] == c &&
//                 board[(j + 4) * 8 + i] == c) {  // sprawdzanie 5 komórek
//                 return true;
//             }
//         }
//     }

//     // Check diagonals (from top-left to bottom-right)
//     for (int i = 0; i <= 3; ++i) {  // zmiana zakresu z 6 na 3
//         for (int j = 0; j <= 3; ++j) {  // zmiana zakresu z 6 na 3
//             if (board[i * 8 + j] == c && board[(i + 1) * 8 + j + 1] == c && 
//                 board[(i + 2) * 8 + j + 2] == c && board[(i + 3) * 8 + j + 3] == c &&
//                 board[(i + 4) * 8 + j + 4] == c) {  // sprawdzanie 5 komórek
//                 return true;
//             }
//         }
//     }

//     // Check diagonals (from top-right to bottom-left)
//     for (int i = 0; i <= 3; ++i) {  // zmiana zakresu z 6 na 3
//         for (int j = 4; j < 8; ++j) {  // zmiana zakresu z 2 na 3
//             if (board[i * 8 + j] == c && board[(i + 1) * 8 + j - 1] == c && 
//                 board[(i + 2) * 8 + j - 2] == c && board[(i + 3) * 8 + j - 3] == c &&
//                 board[(i + 4) * 8 + j - 4] == c) {  // sprawdzanie 5 komórek
//                 return true;
//             }
//         }
//     }

//     return false;
// }

// bool BoardHasLine(const std::array<CellState, kNumCells>& board, const Player player) {
//     CellState c = PlayerToState(player);

//     // Check rows
//     for (int i = 0; i < 7; ++i) {
//         for (int j = 0; j <= 2; ++j) {  // zmiana zakresu z 3 na 2
//             if (board[i * 7 + j] == c && board[i * 7 + j + 1] == c && 
//                 board[i * 7 + j + 2] == c && board[i * 7 + j + 3] == c && 
//                 board[i * 7 + j + 4] == c) {  // sprawdzanie 5 komórek
//                 return true;
//             }
//         }
//     }

//     // Check columns
//     for (int i = 0; i < 7; ++i) {
//         for (int j = 0; j <= 2; ++j) {  // zmiana zakresu z 3 na 2
//             if (board[j * 7 + i] == c && board[(j + 1) * 7 + i] == c && 
//                 board[(j + 2) * 7 + i] == c && board[(j + 3) * 7 + i] == c && 
//                 board[(j + 4) * 7 + i] == c) {  // sprawdzanie 5 komórek
//                 return true;
//             }
//         }
//     }

//     // Check diagonals (from top-left to bottom-right)
//     for (int i = 0; i <= 2; ++i) {  // zmiana zakresu z 3 na 2
//         for (int j = 0; j <= 2; ++j) {  // zmiana zakresu z 3 na 2
//             if (board[i * 7 + j] == c && board[(i + 1) * 7 + j + 1] == c && 
//                 board[(i + 2) * 7 + j + 2] == c && board[(i + 3) * 7 + j + 3] == c && 
//                 board[(i + 4) * 7 + j + 4] == c) {  // sprawdzanie 5 komórek
//                 return true;
//             }
//         }
//     }

//     // Check diagonals (from top-right to bottom-left)
//     for (int i = 0; i <= 2; ++i) {  // zmiana zakresu z 3 na 2
//         for (int j = 4; j < 7; ++j) {  // zmiana zakresu z 3 na 4
//             if (board[i * 7 + j] == c && board[(i + 1) * 7 + j - 1] == c && 
//                 board[(i + 2) * 7 + j - 2] == c && board[(i + 3) * 7 + j - 3] == c && 
//                 board[(i + 4) * 7 + j - 4] == c) {  // sprawdzanie 5 komórek
//                 return true;
//             }
//         }
//     }

//     return false;
// }


// bool BoardHasLine(const std::array<CellState, kNumCells>& board, const Player player) {
//     CellState c = PlayerToState(player);

//     // Check rows
//     for (int i = 0; i < 6; ++i) {
//         for (int j = 0; j <= 1; ++j) {  // Adjust range for 6x6 board
//             if (board[i * 6 + j] == c && board[i * 6 + j + 1] == c && 
//                 board[i * 6 + j + 2] == c && board[i * 6 + j + 3] == c &&
//                 board[i * 6 + j + 4] == c) {  // Check 5 cells
//                 return true;
//             }
//         }
//     }

//     // Check columns
//     for (int i = 0; i < 6; ++i) {
//         for (int j = 0; j <= 1; ++j) {  // Adjust range for 6x6 board
//             if (board[j * 6 + i] == c && board[(j + 1) * 6 + i] == c && 
//                 board[(j + 2) * 6 + i] == c && board[(j + 3) * 6 + i] == c &&
//                 board[(j + 4) * 6 + i] == c) {  // Check 5 cells
//                 return true;
//             }
//         }
//     }

//     // Check diagonals
//     for (int i = 0; i <= 1; ++i) {
//         for (int j = 0; j <= 1; ++j) {
//             if (board[i * 6 + j] == c && board[(i + 1) * 6 + j + 1] == c && 
//                 board[(i + 2) * 6 + j + 2] == c && board[(i + 3) * 6 + j + 3] == c &&
//                 board[(i + 4) * 6 + j + 4] == c) {  // Check 5 cells diagonally
//                 return true;
//             }
//             if (board[i * 6 + j + 4] == c && board[(i + 1) * 6 + j + 3] == c && 
//                 board[(i + 2) * 6 + j + 2] == c && board[(i + 3) * 6 + j + 1] == c &&
//                 board[(i + 4) * 6 + j] == c) {  // Check 5 cells diagonally (other direction)
//                 return true;
//             }
//         }
//     }

//     return false;
// }


// bool BoardHasLine(const std::array<CellState, kNumCells>& board, const Player player) {
//     CellState c = PlayerToState(player);

//     // Check rows
//     for (int i = 0; i < 5; ++i) {
//         for (int j = 0; j <= 1; ++j) {  // zmiana zakresu z 2 na 1
//             if (board[i * 5 + j] == c && board[i * 5 + j + 1] == c && 
//                 board[i * 5 + j + 2] == c && board[i * 5 + j + 3] == c) {  // sprawdzanie 4 komórek
//                 return true;
//             }
//         }
//     }

//     // Check columns
//     for (int i = 0; i < 5; ++i) {
//         for (int j = 0; j <= 1; ++j) {  // zmiana zakresu z 2 na 1
//             if (board[j * 5 + i] == c && board[(j + 1) * 5 + i] == c && 
//                 board[(j + 2) * 5 + i] == c && board[(j + 3) * 5 + i] == c) {  // sprawdzanie 4 komórek
//                 return true;
//             }
//         }
//     }

//     // Check diagonals
//     for (int i = 0; i <= 1; ++i) {  // zmiana zakresu z 2 na 1
//         for (int j = 0; j <= 1; ++j) {  // zmiana zakresu z 2 na 1
//             if (board[i * 5 + j] == c && board[(i + 1) * 5 + j + 1] == c && 
//                 board[(i + 2) * 5 + j + 2] == c && board[(i + 3) * 5 + j + 3] == c) {  // sprawdzanie 4 komórek
//                 return true;
//             }
//             if (board[i * 5 + j + 3] == c && board[(i + 1) * 5 + j + 2] == c && 
//                 board[(i + 2) * 5 + j + 1] == c && board[(i + 3) * 5 + j] == c) {  // sprawdzanie 4 komórek
//                 return true;
//             }
//         }
//     }

//     return false;
// }

// bool BoardHasLine(const std::array<CellState, kNumCells>& board, const Player player) {
//     CellState c = PlayerToState(player);

//     // Check rows
//     for (int i = 0; i < 5; ++i) {
//         for (int j = 0; j <= 1; ++j) {
//             if (board[i * 5 + j] == c && board[i * 5 + j + 1] == c && board[i * 5 + j + 2] == c && board[i * 5 + j + 3] == c) {
//                 return true;
//             }
//         }
//     }

//     // Check columns
//     for (int i = 0; i < 5; ++i) {
//         for (int j = 0; j <= 1; ++j) {
//             if (board[j * 5 + i] == c && board[(j + 1) * 5 + i] == c && board[(j + 2) * 5 + i] == c && board[(j + 3) * 5 + i] == c) {
//                 return true;
//             }
//         }
//     }

//     // Check diagonals
//     for (int i = 0; i <= 1; ++i) {
//         for (int j = 0; j <= 1; ++j) {
//             if (board[i * 5 + j] == c && board[(i + 1) * 5 + j + 1] == c && board[(i + 2) * 5 + j + 2] == c && board[(i + 3) * 5 + j + 3] == c) {
//                 return true;
//             }
//             if (board[i * 5 + j + 3] == c && board[(i + 1) * 5 + j + 2] == c && board[(i + 2) * 5 + j + 1] == c && board[(i + 3) * 5 + j] == c) {
//                 return true;
//             }
//         }
//     }

//     // Check anti-diagonals for the remaining possible positions
//     for (int i = 0; i <= 1; ++i) {
//         for (int j = 3; j <= 4; ++j) {
//             if (board[i * 5 + j] == c && board[(i + 1) * 5 + j - 1] == c && board[(i + 2) * 5 + j - 2] == c && board[(i + 3) * 5 + j - 3] == c) {
//                 return true;
//             }
//         }
//     }

//     return false;
// }
//tego nie wiem?
// bool BoardHasLine(const std::array<CellState, kNumCells>& board, const Player player) {
//     CellState c = PlayerToState(player);
//     return
//         // Sprawdzenie wierszy
//         (board[0] == c && board[1] == c && board[2] == c && board[3] == c) ||
//         (board[4] == c && board[5] == c && board[6] == c && board[7] == c) ||
//         (board[8] == c && board[9] == c && board[10] == c && board[11] == c) ||
//         (board[12] == c && board[13] == c && board[14] == c && board[15] == c) ||

//         // Sprawdzenie kolumn
//         (board[0] == c && board[4] == c && board[8] == c && board[12] == c) ||
//         (board[1] == c && board[5] == c && board[9] == c && board[13] == c) ||
//         (board[2] == c && board[6] == c && board[10] == c && board[14] == c) ||
//         (board[3] == c && board[7] == c && board[11] == c && board[15] == c) ||

//         // Sprawdzenie przekątnych
//         (board[0] == c && board[5] == c && board[10] == c && board[15] == c) ||
//         (board[3] == c && board[6] == c && board[9] == c && board[12] == c);
// }

// zwykłe 5x5x5
// bool BoardHasLine(const std::array<CellState, kNumCells>& board, const Player player) {
//   CellState c = PlayerToState(player);
//   for (int i = 0; i < 5; ++i) {
//     // Check rows
//     if (board[i * 5] == c && board[i * 5 + 1] == c && board[i * 5 + 2] == c && board[i * 5 + 3] == c && board[i * 5 + 4] == c) {
//       return true;
//     }
//     // Check columns
//     if (board[i] == c && board[i + 5] == c && board[i + 10] == c && board[i + 15] == c && board[i + 20] == c) {
//       return true;
//     }
//   }
//   // Check diagonals
//   if (board[0] == c && board[6] == c && board[12] == c && board[18] == c && board[24] == c) {
//     return true;
//   }
//   if (board[4] == c && board[8] == c && board[12] == c && board[16] == c && board[20] == c) {
//     return true;
//   }
//   return false;
// }
// to jest chyba (6,6,6)
// bool BoardHasLine(const std::array<CellState, kNumCells>& board, const Player player) {
//   CellState c = PlayerToState(player);
  
//   // Check rows
//   for (int i = 0; i < 6; ++i) {
//     if (board[i * 6] == c && board[i * 6 + 1] == c && board[i * 6 + 2] == c && board[i * 6 + 3] == c && board[i * 6 + 4] == c && board[i * 6 + 5] == c) {
//       return true;
//     }
//   }

//   // Check columns
//   for (int i = 0; i < 6; ++i) {
//     if (board[i] == c && board[i + 6] == c && board[i + 12] == c && board[i + 18] == c && board[i + 24] == c && board[i + 30] == c) {
//       return true;
//     }
//   }

//   // Check main diagonal
//   if (board[0] == c && board[7] == c && board[14] == c && board[21] == c && board[28] == c && board[35] == c) {
//     return true;
//   }

//   // Check anti-diagonal
//   if (board[5] == c && board[10] == c && board[15] == c && board[20] == c && board[25] == c && board[30] == c) {
//     return true;
//   }

//   return false;
// }


bool BoardHasLine(const std::array<CellState, kNumCells>& board,
                  const Player player) {
  CellState c = PlayerToState(player);
  return (board[0] == c && board[1] == c && board[2] == c) ||
         (board[3] == c && board[4] == c && board[5] == c) ||
         (board[6] == c && board[7] == c && board[8] == c) ||
         (board[0] == c && board[3] == c && board[6] == c) ||
         (board[1] == c && board[4] == c && board[7] == c) ||
         (board[2] == c && board[5] == c && board[8] == c) ||
         (board[0] == c && board[4] == c && board[8] == c) ||
         (board[2] == c && board[4] == c && board[6] == c);
}
void TicTacToeState::DoApplyAction(Action move) {
  SPIEL_CHECK_EQ(board_[move], CellState::kEmpty);
  board_[move] = PlayerToState(CurrentPlayer());
  if (HasLine(current_player_)) {
    outcome_ = current_player_;
  }
  current_player_ = 1 - current_player_;
  num_moves_ += 1;
}

std::vector<Action> TicTacToeState::LegalActions() const {
  if (IsTerminal()) return {};
  // Can move in any empty cell.
  std::vector<Action> moves;
  for (int cell = 0; cell < kNumCells; ++cell) {
    if (board_[cell] == CellState::kEmpty) {
      moves.push_back(cell);
    }
  }
  return moves;
}

std::string TicTacToeState::ActionToString(Player player,
                                           Action action_id) const {
  return game_->ActionToString(player, action_id);
}

bool TicTacToeState::HasLine(Player player) const {
  return BoardHasLine(board_, player);
}

bool TicTacToeState::IsFull() const { return num_moves_ == kNumCells; }

TicTacToeState::TicTacToeState(std::shared_ptr<const Game> game) : State(game) {
  std::fill(begin(board_), end(board_), CellState::kEmpty);
}

std::string TicTacToeState::ToString() const {
  std::string str;
  for (int r = 0; r < kNumRows; ++r) {
    for (int c = 0; c < kNumCols; ++c) {
      absl::StrAppend(&str, StateToString(BoardAt(r, c)));
    }
    if (r < (kNumRows - 1)) {
      absl::StrAppend(&str, "\n");
    }
  }
  return str;
}

bool TicTacToeState::IsTerminal() const {
  return outcome_ != kInvalidPlayer || IsFull();
}

std::vector<double> TicTacToeState::Returns() const {
  if (HasLine(Player{0})) {
    return {1.0, -1.0};
  } else if (HasLine(Player{1})) {
    return {-1.0, 1.0};
  } else {
    return {0.0, 0.0};
  }
}

std::string TicTacToeState::InformationStateString(Player player) const {
  SPIEL_CHECK_GE(player, 0);
  SPIEL_CHECK_LT(player, num_players_);
  return HistoryString();
}

std::string TicTacToeState::ObservationString(Player player) const {
  SPIEL_CHECK_GE(player, 0);
  SPIEL_CHECK_LT(player, num_players_);
  return ToString();
}

void TicTacToeState::ObservationTensor(Player player,
                                       absl::Span<float> values) const {
  SPIEL_CHECK_GE(player, 0);
  SPIEL_CHECK_LT(player, num_players_);

  // Treat `values` as a 2-d tensor.
  TensorView<2> view(values, {kCellStates, kNumCells}, true);
  for (int cell = 0; cell < kNumCells; ++cell) {
    view[{static_cast<int>(board_[cell]), cell}] = 1.0;
  }
}

void TicTacToeState::UndoAction(Player player, Action move) {
  board_[move] = CellState::kEmpty;
  current_player_ = player;
  outcome_ = kInvalidPlayer;
  num_moves_ -= 1;
  history_.pop_back();
  --move_number_;
}

std::unique_ptr<State> TicTacToeState::Clone() const {
  return std::unique_ptr<State>(new TicTacToeState(*this));
}

std::string TicTacToeGame::ActionToString(Player player,
                                          Action action_id) const {
  return absl::StrCat(StateToString(PlayerToState(player)), "(",
                      action_id / kNumCols, ",", action_id % kNumCols, ")");
}

TicTacToeGame::TicTacToeGame(const GameParameters& params)
    : Game(kGameType, params) {}

}  // namespace tic_tac_toe
}  // namespace open_spiel
