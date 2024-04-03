#define _CRT_SECURE_NO_WARNINGS 1

#include<stdio.h>
#include<graphics.h>
#include<windows.h>


//messagebox要用到

//来玩一个井字棋小游戏，刷出一个啤酒就可以下一个子（雾）


//棋盘初始化
char board_date[3][3] = { {'-', '-', '-'}, {'-', '-', '-'}, {'-', '-', '-'} };


//当前棋子
char current_piece = 'o';


//检测是否胜利
bool CheckWin(char c)
{
	//横
	if (board_date[0][0] == c && board_date[0][1] == c && board_date[0][2] == c) return true;
	if (board_date[1][0] == c && board_date[1][1] == c && board_date[1][2] == c) return true;
	if (board_date[2][0] == c && board_date[2][1] == c && board_date[2][2] == c) return true;
	//竖
	if (board_date[0][0] == c && board_date[1][0] == c && board_date[2][0] == c) return true;
	if (board_date[0][1] == c && board_date[1][1] == c && board_date[2][1] == c) return true;
	if (board_date[0][2] == c && board_date[1][2] == c && board_date[2][2] == c) return true;
	//斜
	if (board_date[0][0] == c && board_date[1][1] == c && board_date[2][2] == c) return true;
	if (board_date[0][2] == c && board_date[1][1] == c && board_date[2][0] == c) return true;

	return false;
}


//检测平局
bool CheckDraw()
{
	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			if (board_date[i][j] == '-')
				return false;
		}
	}
	return true;
}


//绘制棋盘
void DrawBoard()
{
	//横
	line(0, 200, 600, 200);
	line(0, 400, 600, 400);
	//竖
	line(200, 0, 200, 600);
	line(400, 0, 400, 600);
}


//绘制棋子
void Drawpiece()
{
	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			switch (board_date[i][j])
			{
			case 'o':
				circle(200 * j + 100, 200 * i + 100, 100);
				break;

			case 'x':
				//   (\)
				line(200 * j, 200 * i, 200 * (j + 1), 200 * (i + 1));
				//   (/)
				line(200 * (j + 1), 200 * i, 200 * j, 200 * (i + 1));
				break;

			case '-':
				break;
			}
		}
	}
}

//提示信息
void DrawTipText()
{
	static TCHAR str[64];
	_stprintf_s(str, _T("当前棋子类型:%c"), current_piece);

	settextcolor(RGB(225, 175, 45));
	outtextxy(0, 0, str);
}


int main()
{
	//初始化
	initgraph(600, 600);
	BeginBatchDraw();
	
	ExMessage msg;

	bool running = true;
	//用布尔类型卡主循环
	while (running)
	{
		DWORD start_time = GetTickCount();
		//读取操作
		while (peekmessage(&msg))
		{
			//监听鼠标左键
			if (msg.message == WM_LBUTTONDOWN)
			{
				//计算点击位置(在二维数组中)
				int x = msg.x;
				int y = msg.y;

				int index_x = x / 200;
				int index_y = y / 200;

				//落子  注意二维数组排序顺序
				if (board_date[index_y][index_x] == '-')
				{
					board_date[index_y][index_x] = current_piece;

					//切换棋子类型
					if (current_piece == 'o')
						current_piece = 'x';
					else
						current_piece = 'o';
				}
			}
		}


		cleardevice();
		DrawBoard();
		Drawpiece();
		DrawTipText();
		FlushBatchDraw();


		if (CheckWin('x'))
		{
			MessageBox(GetHWnd(), _T("x玩家获胜"), _T("游戏结束"), MB_OK);
			running = false;
		}

		else if (CheckWin('o'))
		{
			MessageBox(GetHWnd(), _T("o玩家获胜"), _T("游戏结束"), MB_OK);
			running = false;
		}

		else if (CheckDraw())
		{
			MessageBox(GetHWnd(), _T("平局"), _T("游戏结束"), MB_OK);
			running = false;
		}

		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
		if (delta_time < 1000 / 240)
		{
			Sleep(1000 / 240 - delta_time);
		}
	}
	EndBatchDraw();
	closegraph();
	return 0;
}