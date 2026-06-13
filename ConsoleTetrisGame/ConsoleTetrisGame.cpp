#include <iostream>
#include <Windows.h>
#include <thread>
#include <chrono>
using namespace std;

wstring tetromino[7];
int nFieldWidth = 10*2+4;
int nFieldHeight = 22;
unsigned char* pField = nullptr;

int nScreenWidth = 120;      // Console Screen Sixe X (columns)
int nScreenHeight = 30;     // Console Screen Sixe Y (rows)
int nOffset = 2;

int Rotate(int px, int py, int r) {
    switch (r % 4)
    {
    case 0: return py * 4 + px;         //   0 degrees
    case 1: return 12 + py - (4 * px);  //  90 degrees
    case 2: return 15 - (4 * py) - px;  // 180 degrees
    case 3: return  3 - py + (4 * px);  // 270 degrees
    }
}

bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY) {
    for (int px = 0; px < 4; px++) {
        for (int py = 0; py < 4; py++) {
            // Get index into piece
            int pi = Rotate(px, py, nRotation);

            // Only care about occupied blocks
            if (tetromino[nTetromino][pi] != L'X')
                continue;

            int fx = nPosX + px * 2;
            int fy = nPosY + py;

            // If block is outside boundary
            if (fy >= nFieldHeight || fx < 0 || (fx + 1) >= nFieldWidth)
                return false;

            // Allowed 0 and 1 field value
            if (!((pField[fy * nFieldWidth + fx] == 0 || pField[fy * nFieldWidth + fx] == 1) && (pField[fy * nFieldWidth + fx + 1] == 0 || pField[fy * nFieldWidth + fx + 1] == 1)))
                return false; // bye bye
        }
    }

    return true;
}

int main()
{
    // Create assets
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");

    tetromino[1].append(L"..X.");
    tetromino[1].append(L".XX.");
    tetromino[1].append(L".X..");
    tetromino[1].append(L"....");

    tetromino[2].append(L".X..");
    tetromino[2].append(L".XX.");
    tetromino[2].append(L"..X.");
    tetromino[2].append(L"....");

    tetromino[3].append(L"....");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L"....");

    tetromino[4].append(L"..X.");
    tetromino[4].append(L".XX.");
    tetromino[4].append(L"..X.");
    tetromino[4].append(L"....");

    tetromino[5].append(L"....");
    tetromino[5].append(L".XX.");
    tetromino[5].append(L"..X.");
    tetromino[5].append(L"..X.");

    tetromino[6].append(L"....");
    tetromino[6].append(L".XX.");
    tetromino[6].append(L".X..");
    tetromino[6].append(L".X..");

    pField = new unsigned char[nFieldWidth * nFieldHeight]; // Create play field buffer

    for (int x = 0; x < nFieldWidth; x++) {  // Create play field Boundary
        for (int y = 0; y < nFieldHeight; y++) {
            if (x == 0 && !(y == nFieldHeight - 1)) {   // Left Boundary
                pField[y * nFieldWidth + x] = 5;
            }
            else if (x == nFieldWidth - 1 && !(y == nFieldHeight - 1)) { // Right Boundary
                pField[y * nFieldWidth + x] = 7;
            }
            else if ((x == 1 || x == nFieldWidth - 2) && !(y == nFieldHeight - 1)) { // ! Boundary
                pField[y * nFieldWidth + x] = 6;
            }
            else if ((x >= 2 && x <= nFieldWidth - 3) && y == nFieldHeight - 2) {   // * Bottom Boundary
                pField[y * nFieldWidth + x] = 8;
            }
            else if ((x >= 2 && x <= nFieldWidth - 3) && y == nFieldHeight - 1) {   // Bottom Boundary
                if (x % 2 == 0)
                    pField[y * nFieldWidth + x] = 9;    // \ Bottom Boundary
                else
                    pField[y * nFieldWidth + x] = 10;   // / Bottom Boundary
            }
            else if (x % 2 == 1 && x > 1 && x < nFieldWidth - 2 && y <= nFieldHeight - 2) { // 
                pField[y * nFieldWidth + x] = 1;
            }
            else
            {
                pField[y * nFieldWidth + x] = 0;
            }
            //pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;

        }
    }

    wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];

    for (int i = 0; i < nScreenWidth * nScreenHeight; i++)
        screen[i] = L' ';
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    // Game Logic Stuff
    bool bGameOver = false;

    int nCurrentPiece = 0;
    int nCurrentRotation = 0;
    int nCurrentX = nFieldWidth / 2;
    int nCurrentY = 0;

    bool bKey[4];
    bool bRotateHold = false;

    int nSpeed = 20;
    int nSpeedCounter = 0;
    bool bForceDown = false;
    int nPieceCount = 0;
    int nScore = 0;

    vector<int> vLines;

    while (!bGameOver) {
        // GAME TIMING ==================================
        this_thread::sleep_for(50ms);   // Game Tick
        nSpeedCounter++;
        bForceDown = (nSpeedCounter == nSpeed);

        // INPUT ========================================
        for (int k = 0; k < 4; k++)                             // R    L   D Z
            bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;

        // GAME LOGIC ===================================
        nCurrentX += (bKey[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 2, nCurrentY)) ? 2 : 0; // Right key press
        nCurrentX -= (bKey[1] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 2, nCurrentY)) ? 2 : 0; // Left key press
        nCurrentY += (bKey[2] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0; // Down key press

        if (bKey[3]) {   // Z key press
            nCurrentRotation += (bKey[3] && !bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
            bRotateHold = true;
        }
        else {
            bRotateHold = false;
        }

        if (bForceDown) {
            if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
                nCurrentY++;    // It can, so do it!
            else {
                // Lock the current piece in the field
                for (int px = 0; px < 4; px++)
                    for (int py = 0; py < 4; py++)
                        if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X') {
                            pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px * 2)] = 2;
                            pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px * 2 + 1)] = 3;
                        }

                nPieceCount++;
                if (nPieceCount % 10 == 0)
                    if (nSpeed >= 10) nSpeed--;

                // Check have we got any lines
                for (int py = 0; py < 4; py++)
                    if (nCurrentY + py < nFieldHeight - 2) {
                        bool bLine = true;
                        for (int px = 1; px < nFieldWidth - 1; px++)
                            bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;

                        if (bLine) {
                            // Remove Line, set to =
                            for (int px = 2; px < nFieldWidth - 2; px++)
                                pField[(nCurrentY + py) * nFieldWidth + px] = 4;

                            vLines.push_back(nCurrentY + py);
                        }
                    }
                nScore += 25;
                if (!vLines.empty()) nScore += (1 << vLines.size()) * 100;

                //  Choose next piece
                nCurrentX = nFieldWidth / 2;
                nCurrentY = 0;
                nCurrentRotation = 0;
                nCurrentPiece = 
                    rand() % 7;

                // if piece does not fit game over!!
                bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
            }

            nSpeedCounter = 0;
        }

        // RENDER OUTPUT ================================

        // Draw Field
        for (int x = 0; x < nFieldWidth; x++)
            for (int y = 0; y < nFieldHeight; y++)
                screen[(y + nOffset) * nScreenWidth + (x + nOffset)] = L" .[]=<!>*\\/"[pField[y * nFieldWidth + x]];

        // Draw Current Piece
        for (int px = 0; px < 4; px++)
            for (int py = 0; py < 4; py++)
                if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X') {
                    screen[(nCurrentY + py + nOffset) * nScreenWidth + (nCurrentX + px * 2 + nOffset)] = L'[';
                    screen[(nCurrentY + py + nOffset) * nScreenWidth + (nCurrentX + 1 + px * 2 + nOffset)] = L']';
                }

        // Draw Score
        swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);

        if (!vLines.empty()) {
            // Display Frame (cheekily to draw lines)
            WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
            this_thread::sleep_for(400ms);  // Delay a bit

            for (auto &v : vLines)
                for (int px = 2; px < nFieldWidth - 2; px++) {
                    for (int py = v; py > 0; py--)
                        pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
                    if (px%2)
                        pField[px] = 1;
                    else
                        pField[px] = 0;
                }
            vLines.clear();

        }

        // Display Frame
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
    }

    // oh Dear
    CloseHandle(hConsole);
    cout << "Game Over!! Score:" << nScore << endl;
    system("pause");

    return 0;
}
