#include "stdafx.h"
#include "морской бой.h"
#include <vector>
#include <ctime>
#include <WindowsX.h>
#define CELL 30

using namespace std;

struct Ship
{
	int x, y, len;
	bool hor;

	Ship(): x(0), y(0), len(0), hor(true) {}
};

HINSTANCE hInst;
HWND hWnd;
bool turn = 1;
Ship pl1[10], pl2[10];
int map1[10][10], map2[10][10];
int count1 = 0, count2 = 0;
int won = 0;
bool game = false;
int ind = 0;
bool rot = false;
pair<int, int> mov[8] = {make_pair(-1, -1),
						make_pair(-1, 0),
						make_pair(-1, 1),
						make_pair(0, -1),
						make_pair(0, 1),
						make_pair(1, -1),
						make_pair(1, 0),
						make_pair(1, 1)};
pair<int, int> mov2[4] = {make_pair(-1, 0),
						make_pair(1, 0),
						make_pair(0, -1),
						make_pair(0, 1)};
pair<int, int> prev_shot = make_pair(-1, -1);
int nei_prev = 0;
HBRUSH tmp_br, ship_brush, void_brush, shot_brush;

LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

void ai_turn()
{
	int x, y;
	if (prev_shot.first == -1 || nei_prev == 4)
	{
		do
		{
			x = rand() % 10;
			y = rand() % 10;
		} while (map1[x][y] >= 11);
		prev_shot = make_pair(-1, -1);
		nei_prev = 0;
	}
	else
	{
		int i;
		for (i = nei_prev; i < 4 && !(0 <= prev_shot.first + mov2[i].first && prev_shot.first + mov2[i].first < 10
			&& 0 <= prev_shot.second + mov2[i].second && prev_shot.second + mov2[i].second < 10
			&& map1[prev_shot.first + mov2[i].first][prev_shot.second + mov2[i].second] < 11); ++i) {}
		nei_prev = i;
		if (i == 4)
		{
			ai_turn();
			return;
		}
		x = prev_shot.first + mov2[i].first;
		y = prev_shot.second + mov2[i].second;
		++nei_prev;
	}
	map1[x][y] += 11;
	Sleep(500);
	if (map1[x][y] != 11)
	{
		prev_shot = make_pair(x, y);
		nei_prev = 0;
		InvalidateRect(hWnd, 0, true);
		UpdateWindow(hWnd);
		Ship sh = pl1[map1[x][y] - 11 - 1];
		int id = map1[x][y] - 11;
		bool flag = true;
		for (int i = 0, x = sh.x, y = sh.y; i < sh.len; ++i, x += sh.hor, y += (1 - sh.hor))
			if (map1[x][y] == id)
			{
				flag = false;
				break;
			}
		if (flag)
		{
			++count1;
			prev_shot = make_pair(-1, -1);
			nei_prev = 0;
			int j;
			for (j = 0, x = sh.x, y = sh.y; j < sh.len; ++j, x += sh.hor, y += (1 - sh.hor))
			{
				for (int i = 0; i < 8; ++i)
				{
					int a = x + mov[i].first;
					int b = y + mov[i].second;
					if (0 <= a && a < 10 && 0 <= b && b < 10 && map1[a][b] == 0)
						map1[a][b] = 11;
				}
			}
		}
		if (count1 >= 10)
		{
			won = 2;
			return;
		}
		ai_turn();
	}
	turn = true;
	InvalidateRect(hWnd, 0, true);
	UpdateWindow(hWnd);
}

bool bad(Ship ship, int map[10][10])
{
	int x, y, j;
	for (x = ship.x, y = ship.y, j = 0; j < ship.len; ++j, x += ship.hor, y += (1 - ship.hor))
		for (int i = 0; i < 8; ++i)
		{
			if (x < 0 || x >= 10 || y < 0 || y >= 10 || map[x][y] != 0)
				return true;
			int a = x + mov[i].first;
			int b = y + mov[i].second;
			if (0 <= a && a < 10 && 0 <= b && b < 10)
				if (map[a][b] != 0)
					return true;
		}
	return false;
}

int APIENTRY _tWinMain(HINSTANCE hInstance, 
                     HINSTANCE hPrevInstance, 
                     LPTSTR    lpCmdLine, 
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	WNDCLASSEX wcex;

   hInst = hInstance;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= L"MyClass";
	wcex.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);


	RegisterClassEx(&wcex);

    hWnd = CreateWindow(L"MyClass",	
						L"Main Window", 
						WS_OVERLAPPEDWINDOW, 
						0, 0, 
						GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 
						NULL, 
						NULL, 
						hInstance, 
						NULL);

   if (hWnd == 0)
   {
	   MessageBox(NULL, L"Main window error!", L"Error window", MB_OK);
       return 1;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, 
						UINT message, 
						WPARAM wParam, 
						LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	WCHAR s[100];
	int x, y;
	int size;

	switch (message)
	{
		case WM_CREATE:
			srand(time(0));
			ship_brush = CreateSolidBrush(RGB(0, 255, 0));
			void_brush = CreateSolidBrush(RGB(0, 0, 255));
			shot_brush = CreateSolidBrush(RGB(255, 0, 0));
			for (int i = 0; i < 10; ++i)
			{
				int j;
				if (i < 1)
				{
					do
					{
						pl2[i].x = rand() % 10;
						pl2[i].y = rand() % 10;
						pl2[i].len = 4;
						pl2[i].hor = rand() % 2;
					}
					while (bad(pl2[i], map2));
					for (x = pl2[i].x, y = pl2[i].y, j = 0; j < pl2[i].len; ++j, x += pl2[i].hor, y += (1 - pl2[i].hor))
						map2[x][y] = i + 1;
				}
				else if (i < 3)
				{
					do
					{
						pl2[i].x = rand() % 10;
						pl2[i].y = rand() % 10;
						pl2[i].len = 3;
						pl2[i].hor = rand() % 2;
					}
					while (bad(pl2[i], map2));
					for (x = pl2[i].x, y = pl2[i].y, j = 0; j < pl2[i].len; ++j, x += pl2[i].hor, y += (1 - pl2[i].hor))
						map2[x][y] = i + 1;
				}
				else if (i < 6)
				{
					do
					{
						pl2[i].x = rand() % 10;
						pl2[i].y = rand() % 10;
						pl2[i].len = 2;
						pl2[i].hor = rand() % 2;
					}
					while (bad(pl2[i], map2));
					for (x = pl2[i].x, y = pl2[i].y, j = 0; j < pl2[i].len; ++j, x += pl2[i].hor, y += (1 - pl2[i].hor))
						map2[x][y] = i + 1;
				}
				else
				{
					do
					{
						pl2[i].x = rand() % 10;
						pl2[i].y = rand() % 10;
						pl2[i].len = 1;
						pl2[i].hor = rand() % 2;
					}
					while (bad(pl2[i], map2));
					for (x = pl2[i].x, y = pl2[i].y, j = 0; j < pl2[i].len; ++j, x += pl2[i].hor, y += (1 - pl2[i].hor))
						map2[x][y] = i + 1;
				}
			}
			break;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			for (int i = 0; i < 11; ++i)
			{
				Rectangle(hdc, 200 + CELL * i, 100, 201 + CELL * i, 100 + CELL * 10);
				Rectangle(hdc, 200, 100 + CELL * i, 200 + CELL * 10, 101 + CELL * i);
				Rectangle(hdc, 800 + CELL * i, 100, 801 + CELL * i, 100 + CELL * 10);
				Rectangle(hdc, 800, 100 + CELL * i, 800 + CELL * 10, 101 + CELL * i);
			}
			for (int i = 0; i < 10; ++i)
			{
				wsprintf(s, L"%c", 'a' + i);
				TextOut(hdc, 200 + CELL * i + CELL / 2, 83, s, 1);
				TextOut(hdc, 800 + CELL * i + CELL / 2, 83, s, 1);
			}
			for (int i = 0; i < 9; ++i)
			{
				wsprintf(s, L"%d", (i + 1));
				TextOut(hdc, 187, 100 + CELL * i + CELL / 4, s, 1);
				TextOut(hdc, 787, 100 + CELL * i + CELL / 4, s, 1);
			}
			TextOut(hdc, 180, 377, L"10", 2);
			TextOut(hdc, 780, 377, L"10", 2);
			if (turn && won == 0)
				TextOut(hdc, 600, 10, L"Your turn.", 10);
			size = wsprintf(s, L"%d:%d", count2, count1);
			TextOut(hdc, 620, 30, s, size);
			for (int i = 0; i < 10; ++i)
			{
				for (int j = 0; j < 10; ++j)
				{
					if (map1[i][j] == 11)
					{
						tmp_br = SelectBrush(hdc, void_brush);
						Rectangle(hdc, 200 + CELL * i, 100 + CELL* j, 200 + CELL * (i + 1), 100 + CELL * (j + 1));
						void_brush = SelectBrush(hdc, tmp_br);
					}
					else if (map1[i][j] > 11)
					{
						tmp_br = SelectBrush(hdc, shot_brush);
						Rectangle(hdc, 200 + CELL * i, 100 + CELL* j, 200 + CELL * (i + 1), 100 + CELL * (j + 1));
						shot_brush = SelectBrush(hdc, tmp_br);
					}
					else if (map1[i][j] > 0)
					{
						tmp_br = SelectBrush(hdc, ship_brush);
						Rectangle(hdc, 200 + CELL * i, 100 + CELL* j, 200 + CELL * (i + 1), 100 + CELL * (j + 1));
						ship_brush = SelectBrush(hdc, tmp_br);
					}
					if (map2[i][j] == 11)
					{
						tmp_br = SelectBrush(hdc, void_brush);
						Rectangle(hdc, 800 + CELL * i, 100 + CELL* j, 800 + CELL * (i + 1), 100 + CELL * (j + 1));
						void_brush = SelectBrush(hdc, tmp_br);
					}
					else if (map2[i][j] > 11)
					{
						tmp_br = SelectBrush(hdc, shot_brush);
						Rectangle(hdc, 800 + CELL * i, 100 + CELL* j, 800 + CELL * (i + 1), 100 + CELL * (j + 1));
						shot_brush = SelectBrush(hdc, tmp_br);
					}
				}
			}
			if (won == 1)
				TextOut(hdc, 605, 10, L"You win.", 8);
			else if (won == 2)
			{
				TextOut(hdc, 600, 10, L"You lose.", 9);
				for (int i = 0; i < 10; ++i)
				{
					for (int j = 0; j < 10; ++j)
					{
						if (1 <= map2[i][j] && map2[i][j] <= 10)
						{
							tmp_br = SelectBrush(hdc, ship_brush);
							Rectangle(hdc, 800 + CELL * i, 100 + CELL* j, 800 + CELL * (i + 1), 100 + CELL * (j + 1));
							ship_brush = SelectBrush(hdc, tmp_br);
						}
					}
				}
			}
			EndPaint(hWnd, &ps);
			break;

		case WM_RBUTTONDOWN:
			if (!game)
				rot = !rot;
			break;

		case WM_LBUTTONDOWN:
			if (game)
			{
				if (won == 0)
				{
					x = GET_X_LPARAM(lParam);
					y = GET_Y_LPARAM(lParam);
					if (turn && 800 < x && x < 800 + CELL * 10 && 100 < y && y < 100 + CELL * 10)
					{
						int cx = (x - 800) / CELL;
						int cy = (y - 100) / CELL;
						if (0 <= map2[cx][cy] && map2[cx][cy] <= 10)
						{
							map2[cx][cy] += 11;
							if (map2[cx][cy] == 11)
								turn = false;
							else
							{
								int id = map2[cx][cy] - 11;
								Ship sh = pl2[id - 1];
								int j;
								bool flag = true;
								for (j = 0, x = sh.x, y = sh.y; j < sh.len; ++j, x += sh.hor, y += (1 - sh.hor))
									if (map2[x][y] == id)
									{
										flag = false;
										break;
									}
								if (flag)
								{
									++count2;
									for (j = 0, x = sh.x, y = sh.y; j < sh.len; ++j, x += sh.hor, y += (1 - sh.hor))
									{
										for (int i = 0; i < 8; ++i)
										{
											int a = x + mov[i].first;
											int b = y + mov[i].second;
											if (0 <= a && a < 10 && 0 <= b && b < 10 && map2[a][b] == 0)
												map2[a][b] = 11;
										}
									}
								}
								if (count2 >= 10)
									won = 1;
							}
						}
						InvalidateRect(hWnd, 0, true);
						UpdateWindow(hWnd);
						if (!turn)
							ai_turn();
					}
				}
			}
			else
			{
				x = GET_X_LPARAM(lParam);
				y = GET_Y_LPARAM(lParam);
				if (200 < x && x < 200 + CELL * 10 && 100 < y && y < 100 + CELL * 10)
				{
					int cx = (x - 200) / CELL;
					int cy = (y - 100) / CELL;
					Ship tmp;
					tmp.x = cx;
					tmp.y = cy;
					tmp.hor = rot;
					if (ind < 1)
						tmp.len = 4;
					else if (ind < 3)
						tmp.len = 3;
					else if (ind < 6)
						tmp.len = 2;
					else
						tmp.len = 1;
					if (!bad(tmp, map1))
					{
						int j;
						pl1[ind] = tmp;
						for (x = tmp.x, y = tmp.y, j = 0; j < tmp.len; ++j, x += tmp.hor, y += (1 - tmp.hor))
							map1[x][y] = ind + 1;
						++ind;
						if (ind >= 10)
							game = true;
					}
				}
			}
			InvalidateRect(hWnd, 0, true);
			UpdateWindow(hWnd);
			break;

		case WM_DESTROY:
			DeleteBrush(ship_brush);
			DeleteBrush(void_brush);
			DeleteBrush(shot_brush);
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
