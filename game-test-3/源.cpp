#include<graphics.h>
#include<string>
#include <wingdi.h>
#include<vector>

#define fps 240//��Ϸfps

const int  PLAYER_ANIM_NUM = 6;//��ɫ����֡��+1

const int WINDOW_WIDTH = 1024;//���ڿ��
const int WINDOW_HEIGHT = 900;//���ڸ߶�

int idx_current_anim = 0;//��ǰ����֡����

IMAGE img_player_left[PLAYER_ANIM_NUM];
IMAGE img_player_right[PLAYER_ANIM_NUM];


void LoadAnimation()//���ض���
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

//�������ģʽ�������ɫ�ڿ�
inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}

class Animation //������
{
public:
	Animation(LPCTSTR path, int num, int interval)//·��������ʽ ���� ֡���
	{

		interval_ms = interval;

		TCHAR path_file[256];

		for (size_t i = 0; i < num; i++)
		{
			_stprintf_s(path_file, path, i);//��ʽ���ַ���

			IMAGE* frame = new IMAGE();//malloc
			loadimage(frame, path_file);
			frame_list.push_back(frame);//���
		}

	}
	~Animation()//����
	{
		for (size_t i = 0; i < frame_list.size(); i++)//��size��ȡԪ�ظ���
			delete frame_list[i];  //free
	}
	void Play(int x, int y, int delta)//����
	{
		timer += delta;//��ʱ��
		if (timer >= interval_ms)//��ʱ����delta
		{
			idx_frame = (idx_frame + 1) % frame_list.size();//��һ֡��ȡģ�����
			timer = 0;//��ʱ������
		}
		putimage_alpha(x, y, frame_list[idx_frame]);//��Ⱦ
	}

private:
	int timer = 0;//��ʱ��
	int idx_frame = 0;//֡����
	int interval_ms = 0;//֡���
	std::vector<IMAGE*> frame_list;//֡�б�
};

class Player
{
public:
	Player()
	{
		loadimage(&img_shadow, _T("img/shawdow_player.png"));
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
		int pos_shadow_y = player_pos.y - 8;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);
		static bool facing_left = false;

		int dir_x = is_move_right - is_move_left;//����
		if (dir_x > 0)
			facing_left = false;
		else if (dir_x < 0)
			facing_left = true;

		if (facing_left)
			anim_left->Play(player_pos.x, player_pos.y, delta);
		else
			anim_right->Play(player_pos.x, player_pos.y, delta);
	}

	void ProcessEvent(const ExMessage& msg)//�����¼�
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


	void Move()
	{
		int dir_x = is_move_right - is_move_left;//����
		int dir_y = is_move_down - is_move_up;

		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);//��������

		if (len_dir != 0)//�ƶ�
		{

			double normalized_x = dir_x / len_dir;	//��λ����
			double normalized_y = dir_y / len_dir;//��λ����

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
	POINT player_pos{ 500, 500 };//��ɫλ��

	const int PLAYER_SPEED = 6;//��ɫ�ٶ�
	const int PLAYER_WIDTH = 80;//��ɫ���
	const int PLAYER_HEIGHT = 80;//��ɫ�߶�
	const int SHADOW_WIDTH = 32;

	Animation* anim_left;
	Animation* anim_right;

	IMAGE img_shadow;

	//���������״̬

	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;

};

Player player;

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

Bullet bullet;

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
		SpawnEdge edge = (SpawnEdge)(rand() % 4);//������ɱ߽�

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
		return false;
	}

	bool  CheckPlayerCollision(const Player& player)
	{
		return false;
	}

	void Move(const Player& player)
	{
		const POINT& player_pos = player.GetPosition();

		int dir_x = player_pos.x - position.x;
		int dir_y = player_pos.y - position.y;

		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);//��������
		if (len_dir != 0)
		{
			double normalized_x = dir_x / len_dir;	//��λ����
			double normalized_y = dir_y / len_dir;//��λ����

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

private:
	const int SPEED = 2;
	const int FRAME_WIDTH = 80;
	const int FRAME_HEIGHT = 80;
	const int SHADOW_WIDTH = 48;

private:
	IMAGE img_shadow;
	Animation* anim_left;
	Animation* anim_right;
	POINT position{ 0,0 };
	bool facing_left = false;
};

void TryGenerateEnemy(std::vector<Enemy*>& enemy_list)
{
	const int INTERVAL = 100;
	static int counter = 0;
	if ((++counter) % INTERVAL == 0)
		enemy_list.push_back(new Enemy());
}

int main()
{
	//��ʼ��
	initgraph(1024,900);

	ExMessage msg;

	IMAGE img_background;
	std::vector<Enemy*> enemy_list;;

	LoadAnimation();
	loadimage(&img_background, _T("img/background_1.png"));

	BeginBatchDraw();

	bool running = true;//����״̬
	while (running)
	{
		//��ʼʱ���
		DWORD start_time = GetTickCount();
		static DWORD delta_time = 1000 / fps;
		//��ѯ
		while (peekmessage(&msg))
		{
			player.ProcessEvent(msg);
		}
		player.Move();
		TryGenerateEnemy(enemy_list);
		for (Enemy* enemy : enemy_list)
			enemy->Move(player);
		//����֡ѭ������
		static int counter = 0;
		if (++counter % 5 == 0)
			idx_current_anim++;
		idx_current_anim %= 6;

		//���ػ���+����ͼƬ
		cleardevice();

		putimage(0, 0, &img_background);
		player.Draw(1000 / fps);

		for (Enemy* enemy : enemy_list)
			enemy->Draw(1000 / 144);
		/*	for (const Bullet& bullet : bullet_list)
				bullet.Draw();*/

		FlushBatchDraw();//����ʱ���+��������ʱ��

		DWORD end_time = GetTickCount();

		delta_time = end_time - start_time;
		//��̬����
		if (delta_time < 1000 / fps)
		{
			Sleep(1000 / fps - delta_time);
		}

	}
	EndBatchDraw();
	//�ر�ͼ�δ���
	closegraph();
	return 0;
}

