#include<graphics.h>
#include<string>
#include <wingdi.h>
#include<vector>
#include <chrono>
#include <thread>
#pragma comment(lib,"MSIMG32.LIB")

const int fps=240;//游戏fps

const int  PLAYER_ANIM_NUM = 6;//角色动画帧数+1

const int WINDOW_WIDTH = 1024;//窗口宽度
const int WINDOW_HEIGHT = 900;//窗口高度

int idx_current_anim = 0;//当前动画帧索引

IMAGE img_player_left[PLAYER_ANIM_NUM];
IMAGE img_player_right[PLAYER_ANIM_NUM];

void LoadAnimation()//加载动画
{
	for (size_t i = 0; i < PLAYER_ANIM_NUM; i++)
	{
		std::wstring path = L"img/player_left_" + std::to_wstring(i) + L".png";
		loadimage(&img_player_left[i], path.c_str());
	}
	for (size_t i = 0; i < PLAYER_ANIM_NUM; i++)
	{
		std::wstring path = L"img/player_right_" + std::to_wstring(i) + L".png";
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

class Animation
{
public:
	Animation(LPCTSTR path, int num, int interval)//路径命名方式 数量 帧间隔
	{

		interval_ms = interval;

		TCHAR path_file[256];

		for (size_t i = 0; i < num; i++)
		{
			_stprintf_s(path_file, path, i);//格式化字符串

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
	void Play(int x, int y, int delta)//播放
	{
		timer += delta;//计时器
		if (timer >= interval_ms)//计时大于delta
		{
			idx_frame = (idx_frame + 1) % frame_list.size();//下一帧，取模防溢出
			timer = 0;//计时器归零
		}
		putimage_alpha(x, y, frame_list[idx_frame]);//渲染
	}

private:
	int timer = 0;//计时器
	int idx_frame = 0;//帧索引
	int interval_ms = 0;//帧间隔
	std::vector<IMAGE*> frame_list;//帧列表
};

class Player
{
public:
	const int PLAYER_WIDTH = 80;//角色宽度
	const int PLAYER_HEIGHT = 80;//角色高度

public:
	Player()
	{
		loadimage(&img_shadow, _T("img/shadow_player.png"));//拼写错误
		anim_left = new Animation(_T("img/player_left_%d.png"), 6, 45);
		anim_right = new Animation(_T("img/player_right_%d.png"), 6, 45);
	}

	~Player()
	{
		delete anim_left;
		delete anim_right;
	}

	void Draw(int delta)
	{
		int pos_shadow_x = player_pos.x + (PLAYER_WIDTH - SHADOW_WIDTH) / 2;
		int pos_shadow_y = player_pos.y + PLAYER_HEIGHT;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);
		static bool facing_left = false;

		int dir_x = is_move_right - is_move_left;//方向
		if (dir_x > 0)
			facing_left = false;
		else if (dir_x < 0)
			facing_left = true;

		if (facing_left)
			anim_left->Play(player_pos.x, player_pos.y, delta);
		else
			anim_right->Play(player_pos.x, player_pos.y, delta);
	}

	void ProcessEvent(const ExMessage& msg)//处理事件
	{
		switch (msg.message)
		{
		case WM_KEYDOWN:
			switch (msg.vkcode)
			{
			case VK_UP:
			case 0x57:
				is_move_up = true;
				break;

			case VK_DOWN:
			case 0x53:
				is_move_down = true;
				break;

			case VK_LEFT:
			case 0x41:
				is_move_left = true;
				break;

			case VK_RIGHT:
			case 0x44:
				is_move_right = true;
				break;
			}
			break;

		case WM_KEYUP:
			switch (msg.vkcode)
			{
			case VK_UP:
			case 0x57:
				is_move_up = false;
				break;

			case VK_DOWN:
			case 0x53:
				is_move_down = false;
				break;

			case VK_LEFT:
			case 0x41:
				is_move_left = false;
				break;

			case VK_RIGHT:
			case 0x44:
				is_move_right = false;
				break;
			}
			break;
		}
	}


	void Move()//人物移动
	{
		int dir_x = is_move_right - is_move_left;//方向
		int dir_y = is_move_down - is_move_up;

		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);//向量长度

		if (len_dir != 0)//移动
		{

			double normalized_x = dir_x / len_dir;	//单位向量
			double normalized_y = dir_y / len_dir;//单位向量

			player_pos.x += (int)(normalized_x * PLAYER_SPEED);
			player_pos.y += (int)(normalized_y * PLAYER_SPEED);

		}
		if (player_pos.x < 0) player_pos.x = 0;
		if (player_pos.y < 0) player_pos.y = 0;
		if (player_pos.x + PLAYER_WIDTH > WINDOW_WIDTH) player_pos.x = WINDOW_WIDTH - PLAYER_WIDTH;
		if (player_pos.y + PLAYER_HEIGHT > WINDOW_HEIGHT) player_pos.y = WINDOW_HEIGHT - PLAYER_HEIGHT;
	}
	const POINT& GetPosition()const
	{
		return player_pos;
	}
private:
	POINT player_pos{ 500, 500 };//角色位置

	const int PLAYER_SPEED = 3;//角色速度
	const int SHADOW_WIDTH = 32;

	Animation* anim_left;
	Animation* anim_right;

	IMAGE img_shadow;

	//人物各方向状态
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;
};

class Bullet
{
public:
	POINT position = { 0,0 };

public:
	Bullet() = default;
	~Bullet() = default;
	void Draw() const
	{
		setlinecolor(RGB(255, 155, 50));
		setfillcolor(RGB(171, 84, 90));
		fillcircle(position.x, position.y, RADIUS);
	}

private:
	const int RADIUS = 10;

};

class Enemy
{
public:
	Enemy()
	{
		loadimage(&img_shadow, _T("img/shawdow_enemy.png"));
		anim_left = new Animation(_T("img/enemy_left_%d.png"), 6, 45);
		anim_right = new Animation(_T("img/enemy_right_%d.png"), 6, 45);

		enum class SpawnEdge
		{
			Up = 0,
			Down,
			Left,
			Right
		};
		SpawnEdge edge = (SpawnEdge)(rand() % 4);//随机生成边界

		switch (edge)
		{
		case SpawnEdge::Up:

			position.x = rand() % WINDOW_WIDTH;
			position.y = -FRAME_HEIGHT;
			break;

		case SpawnEdge::Down:

			position.x = rand() % WINDOW_WIDTH;
			position.y = WINDOW_HEIGHT;
			break;

		case SpawnEdge::Left:

			position.x = -FRAME_WIDTH;
			position.y = rand() % WINDOW_HEIGHT;
			break;

		case SpawnEdge::Right:

			position.x = WINDOW_WIDTH;
			position.y = rand() % WINDOW_HEIGHT;
			break;

		}
	}

	~Enemy()
	{
		delete anim_left;
		delete anim_right;
	}

	bool CheckBullerCollisiion(const Bullet& bullet)
	{
		bool is_overlap_x= bullet.position.x  >= position.x && bullet.position.x <= position.x + FRAME_WIDTH;
		bool is_overlap_y = bullet.position.y >= position.y && bullet.position.y <= position.y + FRAME_HEIGHT;
		return is_overlap_x && is_overlap_y;
	}

	bool  CheckPlayerCollision(const Player& player)
	{
		POINT check_position = { position.x + FRAME_WIDTH / 2,position.y + FRAME_HEIGHT/2 };
		POINT player_position = player.GetPosition();

		bool is_overlap_x = check_position.x >= player_position.x && check_position.x <= player_position.x + FRAME_WIDTH;
		bool is_overlap_y = check_position.y >= player_position.y && check_position.y <= player_position.y + FRAME_HEIGHT;
		return is_overlap_x && is_overlap_y;
	}

	void Move(const Player& player)
	{
		const POINT& player_pos = player.GetPosition();

		int dir_x = player_pos.x - position.x;
		int dir_y = player_pos.y - position.y;

		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);//向量长度
		if (len_dir != 0)
		{
			double normalized_x = dir_x / len_dir;	//单位向量
			double normalized_y = dir_y / len_dir;//单位向量

			position.x += (int)(normalized_x * SPEED);
			position.y += (int)(normalized_y * SPEED);

			if (dir_x < 0)
				facing_left = true;
			else if (dir_x > 0)
				facing_left = false;
		}

	}
	void Draw(int delta)
	{
		int pos_shadow_x = position.x + (FRAME_WIDTH - SHADOW_WIDTH) / 2;
		int pos_shadow_y = position.y - 35;

		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);

		if (facing_left)
			anim_left->Play(position.x, position.y, delta);
		else
			anim_right->Play(position.x, position.y, delta);

	}
	void hurt()
	{
		alive = false;
	}
	bool Checkalive()
	{
		return alive;
	}
private:
	const int SPEED = 2;//速度
	const int FRAME_WIDTH = 80;//帧宽度
	const int FRAME_HEIGHT = 80;//帧高度
	const int SHADOW_WIDTH = 48;//阴影宽度

private:
	IMAGE img_shadow;
	Animation* anim_left;
	Animation* anim_right;
	POINT position{ 0,0 };
	bool facing_left = false;
	bool alive = true;
};

//生成敌人
void TryGenerateEnemy(std::vector<Enemy*>& enemy_list)
{
	const int INTERVAL = 50000/fps;
	static int counter = 0;
	if ((++counter) % INTERVAL == 0)//每隔一段时间生成敌人
		enemy_list.push_back(new Enemy());//创建敌人
}
//子弹位置
void UpdateBullets(std::vector<Bullet>& bullet_list,const Player& player)
{
	const double RADIAL_SPEED = 0.0045;
	const double TANGENT_SPEED = 0.0055;
	double radin_interval = 2 * 3.1415926535 / bullet_list.size();//
	POINT player_position = player.GetPosition();
	double radius = 100 + 25 * sin(GetTickCount() * RADIAL_SPEED);
	for (size_t i = 0; i < bullet_list.size(); i++)
	{
		double radian = GetTickCount() * TANGENT_SPEED + radin_interval * i;
		bullet_list[i].position.x = player_position.x + player.PLAYER_WIDTH / 2 + (int)(radius * sin(radian));
		bullet_list[i].position.y = player_position.y + player.PLAYER_HEIGHT / 2 + (int)(radius * cos(radian));
	}

}int main()
{
	//初始化
	initgraph(1024,900);
	auto next_frame = std::chrono::steady_clock::now();
	
	ExMessage msg;
	Player player;
	Bullet bullet;

	IMAGE img_background;
	std::vector<Enemy*> enemy_list;
	std::vector<Bullet> bullet_list(2);
	LoadAnimation();
	loadimage(&img_background, _T("img/background_1.png"));

	BeginBatchDraw();
	bool running = true;//运行状态
	while (running)
	{	
		//开始时间戳
		/*DWORD start_time = GetTickCount();*/

		auto start_time = std::chrono::steady_clock::now();

		//轮询
		while (peekmessage(&msg))
		{
			player.ProcessEvent(msg);
		}
		//角色移动
		player.Move();
		//更新子弹位置
		UpdateBullets(bullet_list, player);
		//生成敌人
		TryGenerateEnemy(enemy_list);
		
		//敌人移动
		for (Enemy* enemy : enemy_list)
			enemy->Move(player);

		//碰撞检测
		for (Enemy* enemy : enemy_list)
		{
			if (enemy->CheckPlayerCollision(player))
			{
				MessageBox(GetHWnd(), _T("点此退出"), _T("Game Over"), MB_OK);
				running = false;
				break;
			}
		}

		//二重缓冲+背景图片
		cleardevice();
		putimage(0, 0, &img_background);
		player.Draw(1000/fps);

		for (Enemy* enemy : enemy_list)
			enemy->Draw(1000 / fps);
		for(const Bullet& bullet:bullet_list)
			bullet.Draw();
		/*	for (const Bullet& bullet : bullet_list)
				bullet.Draw();*/

		FlushBatchDraw();//结束时间戳+计算运行时间

		//DWORD end_time = GetTickCount();
		auto end_time = std::chrono::steady_clock::now();
		//DWORD delta_time = end_time - start_time;
		////动态休眠
		//if (delta_time < 1000 / fps)
		//{
		//	Sleep(1000 / fps - delta_time);
		//}
		 // 计算本帧消耗的时间
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
		auto target_frame_duration = std::chrono::milliseconds(1000 / fps);

		// 如果实际消耗时间小于目标帧时间，则等待差值
		if (elapsed < target_frame_duration) 
		{
			std::this_thread::sleep_for(target_frame_duration - elapsed);
		}

		// 更新下一帧的时间
		next_frame += target_frame_duration;
	}
	EndBatchDraw();
	//关闭图形窗口
	closegraph();
	return 0;
}

