#pragma once
#include <string>

namespace Protocol {
    const std::string CREATE_GAME = "CREATE";
    const std::string JOIN_GAME = "JOIN";
    const std::string GET_STATISTICS = "STATS";
    const std::string LIST_GAMES = "LIST";
    const std::string PLACE_SHIP = "PLACE_SHIP";
    const std::string MAKE_SHOT = "SHOT";
    const std::string QUIT = "QUIT";
    const std::string LOGIN = "LOGIN";
    const std::string READY = "READY";
    const std::string GET_BOARD = "GET_BOARD";

    const std::string OK = "OK";
    const std::string ERROR = "ERROR";
    const std::string GAME_CREATED = "GAME_CREATED";
    const std::string GAME_JOINED = "GAME_JOINED";
    const std::string YOUR_TURN = "YOUR_TURN";
    const std::string OPPONENT_TURN = "OPPONENT_TURN";
    const std::string HIT = "HIT";
    const std::string MISS = "MISS";
    const std::string SUNK = "SUNK";
    const std::string WIN = "WIN";
    const std::string LOSE = "LOSE";
    const std::string GAME_OVER = "GAME_OVER";
    const std::string STATS_RESPONSE = "STATS_DATA";
    const std::string GAME_LIST = "GAME_LIST";
    const std::string BOARD_DATA = "BOARD_DATA";
    const std::string WAITING = "WAITING";
    const std::string OPPONENT_FOUND = "OPPONENT_FOUND";
}