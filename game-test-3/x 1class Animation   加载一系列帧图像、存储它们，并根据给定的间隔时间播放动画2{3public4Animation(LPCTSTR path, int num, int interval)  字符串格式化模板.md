```
class Animation   //加载一系列帧图像、存储它们，并根据给定的间隔时间播放动画
{
public:
Animation(LPCTSTR path, int num, int interval)	//字符串格式化模板，图片数量，帧间隔 
//LPCTSTR：LP长指针 CT：constant STR：const 保证path不被修改
{

interval_ms = interval;//帧间隔时间（以毫秒为单位），即每帧显示的时间。

TCHAR path_file[256];//字符数组，长度为256个宽字符单元，可以用来存储一个字符串

for (size_t i = 0; i < num; i++)
{
_stprintf_s(path_file, path, i);//更安全的sprintf 将path写入path_file

IMAGE* frame = new IMAGE();		//使用new函数来开辟内存
loadimage(frame, path_file);
frame_list.push_back(frame);
}
}

~Animation()
{
for (size_t i = 0; i < frame_list.size(); i++)//遍历 frame_list 中的每个 IMAGE 指针
delete frame_list[i];//使用 delete 操作符释放每个指针指向的内存
}

void Play(int x, int y, int delta)
{
timer += delta;
if (timer >= interval_ms)
{
idx_frame = (idx_frame + 1) % frame_list.size();
timer = 0;
}

putimage_alpha(x, y, frame_list[idx_frame]);
}

private:
int timer = 0;		//动画计时器
int idx_frame = 0;	//动画帧索引
int interval_ms = 0;
std::vector<IMAGE*> frame_list;
};
```

