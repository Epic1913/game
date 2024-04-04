#include<graphics.h>
#include<string>
#include <wingdi.h>
#include<vector>


#define fps 30//游戏fps

const int  PLAYER_ANIM_NUM = 6;//角色动画帧数
const int PLAYER_SPEED = 3;//角色速度

int idx_current_anim = 0;

IMAGE img_player_left[PLAYER_ANIM_NUM];//向左
IMAGE img_player_right[PLAYER_ANIM_NUM];//向右
POINT player_pos = { 500,500 };//角色位置



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



//调整混合模式，解决角色黑框
inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}



class Animation //动画
{
public:
	Animation(LPCTSTR path, int num, int interval)//路径命名方式 数量 帧间隔
	{

		interval_ms = interval;

		TCHAR path_file[256];

		for (size_t i = 0; i < num; i++)
		{
			_stprintf_s(path_file, path, i);

			IMAGE* frame = new IMAGE();//malloc
			loadimage(frame, path_file);
			frame_list.push_back(frame);//添加
		}

	}
	~Animation()//析构
	{
		for (size_t i = 0; i < frame_list.size(); i++)//用size获取元素个数
			delete frame_list[i];  //free
	}
	void Play(int x, int y, int delta)
	{
		timer += delta;//计时器
		if (timer >= interval_ms)//计时大于delta
		{
			idx_frame = (idx_frame + 1) % frame_list.size();//下一帧，取模防溢出
			timer = 0;//计时器归零
		}
		putimage_alpha(x, y, frame_list[idx_frame]);
	}
private:
	int timer = 0;
	int idx_frame = 0;
	int interval_ms = 0;
	std::vector<IMAGE*> frame_list;
};


int main()
{
	//初始化
	initgraph(1280, 720);
	BeginBatchDraw();
	
	ExMessage msg;

	IMAGE img_background;


	//人物各方向状态
	bool is_up = false;
	bool is_down = false;
	bool is_right = false;
	bool is_left = false;

	LoadAnimation();

	loadimage(&img_background, _T("img/background.png"));







	//主循环塞子
	bool running = true;
	while (running)
	{
		//开始时间戳
		DWORD start_time = GetTickCount();


		//轮询
		while (peekmessage(&msg))
		{
			//按下键盘修改状态
			if (msg.message == WM_KEYDOWN)
			{
				switch (msg.vkcode)
				{
				case VK_UP:
				case 0x57:
					is_up = true;
					break;

				case VK_DOWN:
				case 0x53:
					is_down = true;
					break;

				case VK_LEFT:
				case 0x41:
					is_left = true;
					break;

				case VK_RIGHT:
				case 0x44:
					is_right = true;
					break;
				}
			}

			//抬起键盘修改状态
			else if (msg.message == WM_KEYUP)
			{
				switch (msg.vkcode)
				{
				case VK_UP:
				case 0x57:
					is_up = false;
					break;

				case VK_DOWN:
				case 0x53:
					is_down = false;
					break;

				case VK_LEFT:
				case 0x41:
					is_left = false;
					break;

				case VK_RIGHT:
				case 0x44:
					is_right = false;
					break;
				}
			}
		}

			if (is_up) player_pos.y -= PLAYER_SPEED;
			if (is_down) player_pos.y += PLAYER_SPEED;
			if (is_left) player_pos.x -= PLAYER_SPEED;
			if (is_right) player_pos.x += PLAYER_SPEED;


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