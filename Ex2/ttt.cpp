#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// Initialize the board
vector<vector<char>> board = {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}};
// making static var to know how much room left in the board
int roomLeft = 9;

string merge(string left, string right)
{
    string result = "";
    int i = 0, j = 0;
    while (i < left.size() && j < right.size())
    {
        if (left[i] <= right[j])
        {
            result += left[i++];
        }
        else
        {
            result += right[j++];
        }
    }
    while (i < left.size())
    {
        result += left[i++];
    }
    while (j < right.size())
    {
        result += right[j++];
    }
    return result;
}

string merge_sort(string s)
{
    if (s.size() <= 1)
    {
        return s;
    }
    int mid = s.size() / 2;
    string left = merge_sort(s.substr(0, mid));
    string right = merge_sort(s.substr(mid));
    return merge(left, right);
}

bool isValidMove(size_t move)
{
    return move < 9 && board[move / 3][move % 3] == ' ';
}

void makeMove(size_t move, char player)
{
    roomLeft--;
    board[move / 3][move % 3] = player;
}

void printBoard()
{
    for (size_t i = 0; i < board.size(); i++)
    {
        for (size_t j = 0; j < board[i].size(); j++)
        {
            cout << board[i][j] << " ";
        }
        cout << endl;
    }
}

bool gameEnd()
{
    if (roomLeft == 0)
    {
        cout << "Draw" << endl;
        return true;
    }
    for (size_t i = 0; i < board.size(); i++)
    {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != ' ')
        {
            cout << "Player " << board[i][0] << " wins" << endl;
            return true;
        }
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != ' ')
        {
            cout << "Player " << board[0][i] << " wins" << endl;
            return true;
        }
    }
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != ' ')
    {
        cout << "Player " << board[0][0] << " wins" << endl;
        return true;
    }
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != ' ')
    {
        cout << "Player " << board[0][2] << " wins" << endl;
        return true;
    }
    return false;
}

string validateStrategy(int strategy)
{
    string s_strategy = to_string(strategy);
    if (s_strategy.size() != 9)
    {
        cout << "Invalid strategy" << endl;
        exit(0);
    }
    s_strategy = merge_sort(s_strategy);
    for (size_t i = 1; i <= s_strategy.size(); i++)
    {
        if (i != s_strategy[i - 1] - '0')
        {
            cout << "Invalid strategy" << endl;
            exit(0);
        }
    }
    return to_string(strategy);
}

int game(int strategy)
{
    size_t playerMove = 0;
    string s_strategy = validateStrategy(strategy);

    for (size_t i = 0; i < 9; i++)
    { // 4 rounds of 2 moves each
        size_t move = s_strategy[i] - '1';
        if (isValidMove(move))
        {
            makeMove(move, 'X');
            printBoard();
            if (gameEnd())
            {
                return 0;
            }
            cout << "Enter your move (1-9): ";
            cin >> playerMove;
            while (playerMove < 1 || playerMove > 9 || !isValidMove(playerMove - 1))
            {
                cout << "Invalid move. Enter your move (1-9): ";
                cin >> playerMove;
            }
            makeMove(playerMove - 1, 'O');
            printBoard();
            if (gameEnd())
            {
                return 1;
            }
        }
    }
    if (gameEnd())
    {
        return 0;
    }
    cout << "Draw" << endl;
    return 0;
}

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        cout << "Usage: " << argv[0] << " <strategy>" << endl;
        return 1;
    }
    int num = stoi(argv[1]);
    game(num);
    return 0;
}