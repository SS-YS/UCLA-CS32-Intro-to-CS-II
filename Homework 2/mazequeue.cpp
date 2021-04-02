#include <queue>
#include <string>
#include <iostream>
using namespace std;

class Coord
{
  public:
    Coord(int rr, int cc) : m_row(rr), m_col(cc) {}
    int r() const { return m_row; }
    int c() const { return m_col; }
  private:
    int m_row;
    int m_col;
};

bool pathExists(string maze[], int nRows, int nCols, int sr, int sc, int er, int ec)
  // Return true if there is a path from (sr,sc) to (er,ec)
  // through the maze; return false otherwise
{
    // Push the starting coordinate onto the stack
    queue<Coord> coordQueue;
    Coord start(sr, sc);
    coordQueue.push(start);
    maze[sr][sc] = 'E'; // mark as encountered
    
    while (!coordQueue.empty())
    {
        // Pop the top coordinate off the stack
        Coord cur = coordQueue.front();
        coordQueue.pop();
        int curR = cur.r();
        int curC = cur.c();
        
        if (curR == er && curC == ec) // has reached the end
            return true;
        
        // Check every adjacent "walkway" cell that hasn't been encountered
        if (maze[curR-1][curC] == '.')
        {
            Coord next(curR-1, curC);
            coordQueue.push(next);
            maze[curR-1][curC] = 'E'; // mark as encountered
        }
        if (maze[curR][curC+1] == '.')
        {
            Coord next(curR, curC+1);
            coordQueue.push(next);
            maze[curR][curC+1] = 'E'; // mark as encountered
        }
        if (maze[curR+1][curC] == '.')
        {
            Coord next(curR+1, curC);
            coordQueue.push(next);
            maze[curR+1][curC] = 'E'; // mark as encountered
        }
        if (maze[curR][curC-1] == '.')
        {
            Coord next(curR, curC-1);
            coordQueue.push(next);
            maze[curR][curC-1] = 'E'; // mark as encountered
        }
    }
    return false;
}
