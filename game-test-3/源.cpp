#define _CRT_SECURE_NO_WARNINGS 1
#include<stdio.h>
#include<graphics.h>
#include<windows.h>
#include<string.h>
#include <string>
#include <wingdi.h>


#define fps 240

const int  PLAYER_ANIM_NUM = 6;

int idx_current_anim = 0;

IMAGE img_player_left[PLAYER_ANIM_NUM];
IMAGE img_player_right[PLAYER_ANIM_NUM];
POINT player_pos = { 500,500 };



//用循环重载图像
void LoadAnimation()
{
	for (size_t i = 0; i < PLAYER_ANIM_NUM; i++)
	{
		std::wstring path = L"img/player_left_" + std::to_wstring(i) + L".png";
		loadimage(&img_player_left[i], path.c_str());
	}

	for (size_t i = 0; i < PLAYER_ANIM_NUM; i++)
	{
		std::wstring path = L"img/player_right" + std::to_wstring(i) + L".png";
		loadimage(&img_player_right[i], path.c_str());
	}
}

////调整混合模式，解决角色黑框
inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}

int main()
{
	//初始化
	initgraph(1280, 720);
	BeginBatchDraw();
	
	ExMessage msg;
	IMAGE img_background;

	LoadAnimation();

	loadimage(&img_background, _T("img/background.png"));








	//主循环塞子
	bool running = true;
	while (running)
	{
		//开始时间戳
		DWORD start_time = GetTickCount();
		//监听
		while (peekmessage(&msg))
		{
			if (msg.message == WM_KEYDOWN)
			{
				switch (msg.vkcode)
				{
				default:
					break;
				}
			}
		}

		setbkmode(TRANSPARENT);
		//动画帧循环索引
		static int counter = 0;
		if (++counter % 5 == 0)
			idx_current_anim++;
		idx_current_anim %= 6;

		//二重缓冲+背景图片
		cleardevice();

		putimage(0, 0, &img_background);
		putimage_alpha(player_pos.x, player_pos.y, &img_player_left[idx_current_anim]);

		FlushBatchDraw();//结束时间戳+计算运行时间
		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;

		//动态休眠
		if (delta_time < 1000 / fps)
		{
			Sleep(1000 / fps - delta_time);
		}
	}

	EndBatchDraw();

}