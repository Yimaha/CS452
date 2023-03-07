#pragma once
#include "../interrupt/clock.h"
#include "../kernel.h"
#include "../rpi.h"
#include "../utils/printf.h"
#include "../utils/utility.h"

#define MAX_MATCHES 64

namespace RockPaperScissors
{
// tasks for the RPS game
extern "C" void RPSServer();
extern "C" void RPSClient();

enum class RPSMessage : uint8_t { SIGNUP, ROCK, PAPER, SCISSORS, WIN, LOSE, DRAW, QUIT, NONE, ERROR };
}

struct Match {
	int player1;
	RockPaperScissors::RPSMessage player1_move;
	int player2;
	RockPaperScissors::RPSMessage player2_move;
	bool active;
};

extern "C" void RockPaperScissors::RPSServer() {
	const char name[] = "RPSServer";
	int res = Name::RegisterAs(name);
	kernel_assert(res == 0);
	Match matches[MAX_MATCHES];
	const int NO_PLAYER = -1;
	for (int i = 0; i < MAX_MATCHES; ++i) {
		matches[i].player1 = NO_PLAYER;
		matches[i].player1_move = RockPaperScissors::RPSMessage::NONE;
		matches[i].player2 = NO_PLAYER;
		matches[i].player2_move = RockPaperScissors::RPSMessage::NONE;
		matches[i].active = false;
	}

	while (1) {
		int from = -1;
		RockPaperScissors::RPSMessage msg;
		Message::Receive::Receive(&from, (char*)&msg, sizeof(msg));
		switch (msg) {
		case RockPaperScissors::RPSMessage::SIGNUP: {
			int first_inactive = -1;
			bool found_opponent = false;
			for (int i = 0; i < MAX_MATCHES; ++i) {
				if (matches[i].player2 == NO_PLAYER && matches[i].player1 != NO_PLAYER) {
					matches[i].player2 = from;
					found_opponent = true;
					break;
				} else if (matches[i].player1 == NO_PLAYER && matches[i].player2 == NO_PLAYER && first_inactive == -1) {
					first_inactive = i;
				}
			}

			if (!found_opponent) {
				matches[first_inactive].player1 = from;
			}

			RockPaperScissors::RPSMessage reply = RockPaperScissors::RPSMessage::NONE;
			Message::Reply::Reply(from, (char*)&reply, 0);
			break;
		}
		case RockPaperScissors::RPSMessage::QUIT:
		case RockPaperScissors::RPSMessage::ROCK:
		case RockPaperScissors::RPSMessage::PAPER:
		case RockPaperScissors::RPSMessage::SCISSORS: {
			int i = 0;
			const int DRAW_GAME = -1;
			const int P1_QUIT = -2;
			const int P2_QUIT = -3;
			bool found = false;
			for (; i < MAX_MATCHES; ++i) {
				if (matches[i].player1 == from) {
					matches[i].player1_move = msg;
					found = true;
					break;
				} else if (matches[i].player2 == from) {
					matches[i].player2_move = msg;
					found = true;
					break;
				}
			}

			if (!found) {
				// player not found, hasn't signed up
				Message::Reply::Reply(from, nullptr, 0);
				break;
			}

			if (matches[i].player1_move != RockPaperScissors::RPSMessage::NONE && matches[i].player2_move != RockPaperScissors::RPSMessage::NONE) {
				int winner_tid, loser_tid;
				switch (matches[i].player1_move) {
				case RockPaperScissors::RPSMessage::ROCK:
					switch (matches[i].player2_move) {
					case RockPaperScissors::RPSMessage::ROCK:
						winner_tid = loser_tid = DRAW_GAME;
						break;
					case RockPaperScissors::RPSMessage::PAPER:
						winner_tid = matches[i].player2;
						loser_tid = matches[i].player1;
						break;
					case RockPaperScissors::RPSMessage::SCISSORS:
						winner_tid = matches[i].player1;
						loser_tid = matches[i].player2;
						break;
					case RockPaperScissors::RPSMessage::QUIT:
						winner_tid = loser_tid = P2_QUIT;
						break;
					default:
						break;
					}

					break;
				case RockPaperScissors::RPSMessage::PAPER:
					switch (matches[i].player2_move) {
					case RockPaperScissors::RPSMessage::ROCK:
						winner_tid = matches[i].player1;
						loser_tid = matches[i].player2;
						break;
					case RockPaperScissors::RPSMessage::PAPER:
						winner_tid = loser_tid = DRAW_GAME;
						break;
					case RockPaperScissors::RPSMessage::SCISSORS:
						winner_tid = matches[i].player2;
						loser_tid = matches[i].player1;
						break;
					case RockPaperScissors::RPSMessage::QUIT:
						winner_tid = loser_tid = P2_QUIT;
						break;
					default:
						break;
					}
					break;
				case RockPaperScissors::RPSMessage::SCISSORS:
					switch (matches[i].player2_move) {
					case RockPaperScissors::RPSMessage::ROCK:
						winner_tid = matches[i].player2;
						loser_tid = matches[i].player1;
						break;
					case RockPaperScissors::RPSMessage::PAPER:
						winner_tid = matches[i].player1;
						loser_tid = matches[i].player2;
						break;
					case RockPaperScissors::RPSMessage::SCISSORS:
						winner_tid = loser_tid = DRAW_GAME;
						break;
					case RockPaperScissors::RPSMessage::QUIT:
						winner_tid = loser_tid = P2_QUIT;
						break;
					default:
						break;
					}
					break;
				case RockPaperScissors::RPSMessage::QUIT:
					winner_tid = loser_tid = P1_QUIT;
					break;
				default:
					break;
				}

				if (winner_tid >= 0 && loser_tid >= 0) {
					RockPaperScissors::RPSMessage winner_msg = RockPaperScissors::RPSMessage::WIN;
					RockPaperScissors::RPSMessage loser_msg = RockPaperScissors::RPSMessage::LOSE;
					Message::Reply::Reply(winner_tid, (char*)&winner_msg, sizeof(RockPaperScissors::RPSMessage));
					Message::Reply::Reply(loser_tid, (char*)&loser_msg, sizeof(RockPaperScissors::RPSMessage));

					print("The winner is: ", 15);
					print_int(winner_tid);
					print("!\r\n", 3);
				} else if (winner_tid == DRAW_GAME && loser_tid == DRAW_GAME) {
					RockPaperScissors::RPSMessage draw_msg = RockPaperScissors::RPSMessage::DRAW;
					Message::Reply::Reply(matches[i].player1, (char*)&draw_msg, sizeof(RockPaperScissors::RPSMessage));
					Message::Reply::Reply(matches[i].player2, (char*)&draw_msg, sizeof(RockPaperScissors::RPSMessage));
					print("It's a draw!\r\n", 14);
				} else if (winner_tid == P1_QUIT) {
					RockPaperScissors::RPSMessage quit_msg = RockPaperScissors::RPSMessage::QUIT;
					Message::Reply::Reply(matches[i].player1, (char*)&quit_msg, sizeof(RockPaperScissors::RPSMessage));
					if (matches[i].player2 != NO_PLAYER) {
						Message::Reply::Reply(matches[i].player2, (char*)&quit_msg, sizeof(RockPaperScissors::RPSMessage));
					}

					matches[i].player1 = NO_PLAYER;
					matches[i].player2 = NO_PLAYER;
				} else if (winner_tid == P2_QUIT) {
					RockPaperScissors::RPSMessage quit_msg = RockPaperScissors::RPSMessage::QUIT;
					Message::Reply::Reply(matches[i].player2, (char*)&quit_msg, sizeof(RockPaperScissors::RPSMessage));
					if (matches[i].player1 != NO_PLAYER) {
						Message::Reply::Reply(matches[i].player1, (char*)&quit_msg, sizeof(RockPaperScissors::RPSMessage));
					}

					matches[i].player1 = NO_PLAYER;
					matches[i].player2 = NO_PLAYER;
				}

				matches[i].player1_move = RockPaperScissors::RPSMessage::NONE;
				matches[i].player2_move = RockPaperScissors::RPSMessage::NONE;
				printf("Print any key to continue...\r\n");
				uart_getc(0, 0);			   // wait for a keypress
				uart_puts(0, 0, "\r \r\n", 4); // clear the keypress
			}
		}
		default:
			break;
		} // switch (msg)

	} // while (1)
}

extern "C" void RockPaperScissors::RPSClient() {
	int tid = Task::MyTid();
	int server_tid = Name::WhoIs("RPSServer");

	RockPaperScissors::RPSMessage msg = RockPaperScissors::RPSMessage::SIGNUP;
	RockPaperScissors::RPSMessage reply;
	Message::Send::Send(server_tid, (char*)&msg, sizeof(RockPaperScissors::RPSMessage), (char*)&reply, sizeof(RockPaperScissors::RPSMessage));

	print("My tid is: ", 11);
	print_int(tid);
	print(" and I am ready to play!\r\n", 26);
	int moves = 0;

	while (1) {
		// Get a "random" move
		uint64_t rand_int = Clock::system_time() % 3;
		if (moves > 3 + static_cast<int>(rand_int)) {
			rand_int = 3;
		}

		print("My move is: ", 12);
		print_int(rand_int);
		print("\r\n", 2);
		RockPaperScissors::RPSMessage move = RockPaperScissors::RPSMessage::NONE;
		switch (rand_int) {
		case 0:
			move = RockPaperScissors::RPSMessage::ROCK;
			break;
		case 1:
			move = RockPaperScissors::RPSMessage::PAPER;
			break;
		case 2:
			move = RockPaperScissors::RPSMessage::SCISSORS;
			break;
		case 3:
			move = RockPaperScissors::RPSMessage::QUIT;
			break;
		default:
			break;
		}

		if (move == RockPaperScissors::RPSMessage::ROCK) {
			print("Task ", 5);
			print_int(tid);
			print(" played ROCK\r\n", 14);
		} else if (move == RockPaperScissors::RPSMessage::PAPER) {
			print("Task ", 5);
			print_int(tid);
			print(" played PAPER\r\n", 15);
		} else if (move == RockPaperScissors::RPSMessage::SCISSORS) {
			print("Task ", 5);
			print_int(tid);
			print(" played SCISSORS\r\n", 18);
		} else if (move == RockPaperScissors::RPSMessage::QUIT) {
			print("Task ", 5);
			print_int(tid);
			print(" quits!\r\n", 9);
		}

		// Send the move
		Message::Send::Send(server_tid, (char*)&move, sizeof(RockPaperScissors::RPSMessage), (char*)&reply, sizeof(RockPaperScissors::RPSMessage));

		// Wait for the reply
		if (reply == RockPaperScissors::RPSMessage::QUIT) {
			printf(":(\r\n");

			// Do another random check. 25% of the time, rejoin.
			rand_int = Clock::system_time() % 4;
			if (rand_int == 0) {
				printf("Task %d is rejoining!\r\n", tid);
				msg = RockPaperScissors::RPSMessage::SIGNUP;
				Message::Send::Send(server_tid, (char*)&msg, sizeof(RockPaperScissors::RPSMessage), (char*)&reply, sizeof(RockPaperScissors::RPSMessage));
				printf("My tid is: %d and I am ready to play!\r\n", tid);
				moves = 0;
			} else {
				break;
			}
		}

		moves += 1;
	}

	Task::Exit();
}