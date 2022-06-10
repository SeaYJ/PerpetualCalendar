#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

// 按钮名称最大长度
#define _MAX_BUTTONNAME 128

// 非按钮文字行数（只计算首行到第一个按钮的非按钮文字），用于后面计算
#define _NONE_BUTTON_SPACE_LINES 4
// 按钮行数间隔行序列（= 按钮之间间隔行数 + 1<一个按钮行>）
#define _BUTTON_SPACE 2

// 第一个按钮的行号（行号从 0 开始）
#define _FIRST_BUTTON_LINE 4
// “引导界面”的最后一个按钮行号（行号从 0 开始）
#define _LAST_BUTTON_LINE_INDEXMENU 12
// “查看日历”界面的最后一个按钮行号（行号从 0 开始）
#define _LAST_BUTTON_LINE_VIEWCALENDAR 8

// 返回上一个界面（返回值）
#define _RETURN_LAST_PAGE_WND -1
// “查看日历”界面（返回值）
#define _VIEW_CALENDAR_WND 0
// “闰年判断”界面（返回值）
#define _LEAP_YEAR_JUDGMENT_WND 1
// “星期查询”界面（返回值）
#define _WEEKLY_QUERY_WND 2
// “查询最大日数”界面（返回值）
#define _QUERY_MAX_DAYS_WND 3
// “退出程序”（返回值）
#define _EXIT_WND 4

#define _VIEW_ALMANAC_WND 0
#define _VIEW_MONTHLY_CALENDAR_WND 1
#define _RETURN_INDEX_MENU_WND 2

// 自定义 bool 数据类型，低版本的 VC++ 可能会报错，但是不影响编译运行
typedef enum{false=0, true=1}bool;

/*******************************************************\
* 申明：												*
*   每一个界面的刷新都在自己函数内部完成，不依赖返回值。*
* 当函数返回返回值时，此界面一定已经销毁！				*
\******************************************************/ 
// “引导界面”处理函数
int indexMenu();
// “查看日历”界面处理函数
int viewCalendar();
// “闰年计算”界面处理函数
int leapYearJudgment();
// “星期计算”界面处理函数
int weeklyQuery();
// “查询最大日数”界面处理函数
int queryMaxDays();

/****************************\ 
* 隶属于“查看日历”的子界面 *
\****************************/
// “查看年历”界面处理函数
int viewAlmanac();
// “查看月历”界面处理函数
int viewMonthlyCalendar();

// 控制台光标定位函数（辅助函数）
void gotoxy(int x, int y)
{
	COORD pos = {x, y};
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hOut, pos);
}

// 判断是否是闰年函数，返回值 true（1）代表是闰年，返回值 false（2）代表是平年（不是闰年）
bool isLeapYear(unsigned int year)
{
	if ((year % 4 == 0 && year %100 != 0) || (year % 400 == 0))
	{
		return true;
	}
	else
	{
		return false;
	}
}

// 返回某年某月最多有多少天（主要正对 2 月 - 可能 28 天，也可能 29 天）
unsigned int returnNum_DaysOfMonth(unsigned int year, unsigned int month)
{
	unsigned int day[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	if (isLeapYear(year) == true)
	{
		day[1] = 29;
	}

	return day[month - 1];
}

// 辅助函数 - 用于计算某年某月某日是星期几
int total_day(unsigned int year, unsigned int month, unsigned int day)
{
	unsigned int sum = 0, i = 1;

	for (i = 1; i < month; i++)
	{
		sum += returnNum_DaysOfMonth(year, i);
	}
	sum += day;

	return sum;
}

// 计算某年某月某日是星期几
int weekdayOfDay(unsigned int year, unsigned int month, unsigned int day)
{
	int count;

	count = (year - 1) + (year - 1) / 4 - (year - 1) / 100 + (year - 1) / 400 + total_day(year, month, day);
	count %= 7;

	return count;
}

int main(int argc, char *argv[])
{
	// 用于接收各个界面函数返回值的变量，
	// 当接收到 -2 时代表发生错误，接收到 -1 （_RETURN_LAST_PAGE_WND） 时代表返回上一个界面...
	// 具体可看前面宏定义部分的注释
	int retIndexMenu, 
		retViewCalendar, retViewAlmanac, retViewMonthlyCalendar,
		retQueryMaxDays,
		retWeeklyQuery,
		retLeapYearJudgment;

Index_Start:
	// 打开“引导界面”
	retIndexMenu = indexMenu();

	// “引导界面”函数返回值操作映射
	switch (retIndexMenu)
	{
	// 打开“查看日历”界面
	case _VIEW_CALENDAR_WND:
ViewCalendar_START:
		retViewCalendar = viewCalendar();

		// “查看日历”界面函数返回值操作映射
		switch (retViewCalendar)
		{
		// 打开“查看日历 - 查看年历”界面
		case _VIEW_ALMANAC_WND:
			retViewAlmanac = viewAlmanac();
			if (retViewAlmanac == _RETURN_LAST_PAGE_WND)
			{
				goto ViewCalendar_START;
			}
			else
			{
				printf("发生了错误！请联系开发者反馈此情况！\n");
				system("pause & exit /1");
			}
			break;

		// 打开“查看日历 - 查看月历”界面
		case _VIEW_MONTHLY_CALENDAR_WND:
			retViewMonthlyCalendar = viewMonthlyCalendar();
			if (retViewMonthlyCalendar == _RETURN_LAST_PAGE_WND)
			{
				goto ViewCalendar_START;
			}
			else
			{
				printf("发生了错误！请联系开发者反馈此情况！\n");
				system("pause & exit /1");
			}
			break;

		case _RETURN_INDEX_MENU_WND:
			goto Index_Start;
			break;

		case _RETURN_LAST_PAGE_WND:
			goto Index_Start;
			break;
		}

		break;

	// 打开“闰年判断”界面
	case _LEAP_YEAR_JUDGMENT_WND:
		retLeapYearJudgment = leapYearJudgment();
		if (retLeapYearJudgment == _RETURN_LAST_PAGE_WND)
		{
			goto Index_Start;
		}
		else
		{
			printf("发生了错误！请联系开发者反馈此情况！\n");
			system("pause & exit /1");
		}
		break;

	// 打开“星期判断”界面
	case _WEEKLY_QUERY_WND:
		retWeeklyQuery = weeklyQuery();
		if (retWeeklyQuery == _RETURN_LAST_PAGE_WND)
		{
			goto Index_Start;
		}
		else
		{
			printf("发生了错误！请联系开发者反馈此情况！\n");
			system("pause & exit /1");
		}
		break;

	// 打开“查询最大日数”界面
	case _QUERY_MAX_DAYS_WND:
		retQueryMaxDays = queryMaxDays();
		if (retQueryMaxDays == _RETURN_LAST_PAGE_WND)
		{
			goto Index_Start;
		}
		else
		{
			printf("发生了错误！请联系开发者反馈此情况！\n");
			system("pause & exit /1");
		}
		break;

	// 退出程序
	case _EXIT_WND:
		exit(0);// 退出程序
		break;// 纯属为了对称美学，嘿嘿－O－

	// 返回上一个界面（还是“引导界面”自己）
	case _RETURN_LAST_PAGE_WND:
		goto Index_Start;

	default:
		system("cls");
		printf("对不起，未找到窗口索引！请将此情况反馈给开发者！\n");
	}
	return 0;
}

int indexMenu()
{
	// 该界面下的光标位置 POS(0， 第一个按钮行号)
	COORD currentPos = {0, _FIRST_BUTTON_LINE};
	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	INPUT_RECORD keyrec;
	DWORD res;

	// 按钮文字（未选中）
	char selectButton[5][_MAX_BUTTONNAME] = {
		"\033[0;37;40m* 1. 查看日历                                    *",
		"\033[0;37;40m* 2. 闰年判断                                    *",
		"\033[0;37;40m* 3. 星期查询                                    *",
		"\033[0;37;40m* 4. 查询最大日数                                *",
		"\033[0;37;40m* 5. 退出程序                                    *"
		};

	// 按钮文字（选中时）
	char selectButton_hover[5][_MAX_BUTTONNAME] = {
		"*\033[0;30;47m 1. 查看日历                                    \033[0;37;40m*",
		"*\033[0;30;47m 2. 闰年判断                                    \033[0;37;40m*",
		"*\033[0;30;47m 3. 星期查询                                    \033[0;37;40m*",
		"*\033[0;30;47m 4. 查询最大日数                                \033[0;37;40m*",
		"*\033[0;30;47m 5. 退出程序                                    \033[0;37;40m*"
		};

	// 初始化控制台
	system("cls && title 万年历 && mode con cols=50 lines=21");

	// 初始化界面
	printf(	"**************************************************\n"
			"*             欢迎使用万年历查询系统             *\n"
			"**************************************************\n"
			"*                                                *\n"
			"%s\n"
			"*                                                *\n"
			"%s\n"
			"*                                                *\n"
			"%s\n"
			"*                                                *\n"
			"%s\n"
			"*                                                *\n"
			"%s\n"
			"*                                                *\n"
			"**************************************************\n"
			"*      \033[0;33;40m_\033[1;33;40m/\033[0;33;40m_\033[1;33;40m/\033[0;33;40m_\033[1;33;40m/                    \033[0;33;40m_\033[1;33;40m/      \033[0;33;40m_\033[1;33;40m/    \033[0;33;40m_\033[1;33;40m/\033[0;37;40m*\n"
			"*   \033[0;33;40m_\033[1;33;40m/         \033[0;33;40m_\033[1;33;40m/\033[0;33;40m_\033[1;33;40m/     \033[0;33;40m_\033[1;33;40m/\033[0;33;40m_\033[1;33;40m/\033[0;33;40m_\033[1;33;40m/    \033[0;33;40m_\033[1;33;40m/  \033[0;33;40m_\033[1;33;40m/      \033[0;33;40m_\033[1;33;40m/ \033[0;37;40m*\n"
			"*    \033[0;33;40m_\033[1;33;40m/\033[0;33;40m_\033[1;33;40m/   \033[0;33;40m_\033[1;33;40m/\033[0;33;40m_\033[1;33;40m/\033[0;33;40m_\033[1;33;40m/\033[0;33;40m_\033[1;33;40m/ \033[0;33;40m_\033[1;33;40m/    \033[0;33;40m_\033[1;33;40m/      \033[0;33;40m_\033[1;33;40m/        \033[0;33;40m_\033[1;33;40m/  \033[0;37;40m*\n"
			"*       \033[0;33;40m_\033[1;33;40m/ \033[0;33;40m_\033[1;33;40m/       \033[0;33;40m_\033[1;33;40m/    \033[0;33;40m_\033[1;33;40m/      \033[0;33;40m_\033[1;33;40m/  \033[0;33;40m_\033[1;33;40m/    \033[0;33;40m_\033[1;33;40m/   \033[0;37;40m*\n"
			"*\033[0;33;40m_\033[1;33;40m/\033[0;33;40m_\033[1;33;40m/\033[0;33;40m_\033[1;33;40m/     \033[0;33;40m_\033[1;33;40m/\033[0;33;40m_\033[1;33;40m/\033[0;33;40m_\033[1;33;40m/   \033[0;33;40m_\033[1;33;40m/\033[0;33;40m_\033[1;33;40m/\033[0;33;40m_\033[1;33;40m/      \033[0;33;40m_\033[1;33;40m/    \033[0;33;40m_\033[1;33;40m/\033[0;33;40m_\033[1;33;40m/      \033[0;37;40m*\n"
			"**************************************************",
			selectButton_hover[0], selectButton[1], selectButton[2], selectButton[3], selectButton[4]);
	gotoxy(currentPos.X, currentPos.Y);// 将光标初始化到第一个按钮（因为默认第一个按钮是被选中的）

	// 键盘事件监听
	while(true)
	{
		ReadConsoleInput(handle_in, &keyrec, 1, &res);// 监听输入事件
		if ( keyrec.EventType == KEY_EVENT )// 输入事件是键盘事件
		{
			if (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_UP
				&& keyrec.Event.KeyEvent.bKeyDown == true)// 按下了↓键
			{
				printf("%s", selectButton[(currentPos.Y - _NONE_BUTTON_SPACE_LINES)/2]);
				(currentPos.Y > _FIRST_BUTTON_LINE) ? (currentPos.Y -= _BUTTON_SPACE) : (currentPos.Y = _LAST_BUTTON_LINE_INDEXMENU);
				gotoxy(currentPos.X, currentPos.Y);
				printf("%s", selectButton_hover[(currentPos.Y - _NONE_BUTTON_SPACE_LINES)/2]);
				gotoxy(currentPos.X, currentPos.Y);
			}
			else if (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_DOWN// 按下了↑键
				&& keyrec.Event.KeyEvent.bKeyDown == true)
			{
				printf("%s", selectButton[(currentPos.Y-_NONE_BUTTON_SPACE_LINES)/2]);// 将当前按钮文字恢复未选中状态(重新输出按钮文字<selectButton>)
				(currentPos.Y < _LAST_BUTTON_LINE_INDEXMENU) ? (currentPos.Y += _BUTTON_SPACE) : (currentPos.Y = _FIRST_BUTTON_LINE);// 按钮移动（坐标计算）
				gotoxy(currentPos.X, currentPos.Y);// 移动光标到选中按钮位置
				printf("%s", selectButton_hover[(currentPos.Y-_NONE_BUTTON_SPACE_LINES)/2]);// 将当前按钮文字设置为选中状态（重新输出按钮文字<selectButton_hover>）
				gotoxy(currentPos.X, currentPos.Y);// 因为设置选中状态后会导致 x 坐标后移，为了下一次按钮移动必须重置光标 x 位置
			}
			else if ( (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_RIGHT && keyrec.Event.KeyEvent.bKeyDown == true)
				|| (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_RETURN && keyrec.Event.KeyEvent.bKeyDown == true) )// 确定
			{
				return (currentPos.Y - _NONE_BUTTON_SPACE_LINES)/2;// 返回按钮的编号（第几个按钮<0~5>）
			}
			else if ( (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_LEFT && keyrec.Event.KeyEvent.bKeyDown == true)
				|| (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_BACK && keyrec.Event.KeyEvent.bKeyDown == true) )// 回退
			{
				return _RETURN_LAST_PAGE_WND;
			}
			else if (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE
				&& keyrec.Event.KeyEvent.bKeyDown == true)
			{
				exit(0);
			}
			else {}
		}
	}

	return -2;
}

int viewCalendar()
{
	COORD currentPos = {0, _FIRST_BUTTON_LINE};
	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	INPUT_RECORD keyrec;
	DWORD res;

	char selectButton[3][_MAX_BUTTONNAME] = {
		"\033[0;37;40m* 1. 查看年历                                    *",
		"\033[0;37;40m* 2. 查看月历                                    *",
		"\033[0;37;40m* 3. 返回上一页                                  *"
	};

	char selectButton_hover[3][_MAX_BUTTONNAME] = {
		"*\033[0;30;47m 1. 查看年历                                    \033[0;37;40m*",
		"*\033[0;30;47m 2. 查看月历                                    \033[0;37;40m*",
		"*\033[0;30;47m 3. 返回上一页                                  \033[0;37;40m*"
	};

	system("cls && title 万年历 - 查看日历 && mode con cols=50 lines=21");
	printf(	"**************************************************\n"
			"*             欢迎使用万年历查询系统             *\n"
			"**************************************************\n"
			"*                                                *\n"
			"%s\n"
			"*                                                *\n"
			"%s\n"
			"*                                                *\n"
			"%s\n"
			"*                                                *\n"
			"**************************************************\n"
			"*                                                *\n"
			"*              _                _                *\n"
			"*             | |              | |               *\n"
			"*     ___ __ _| | ___ _ __   __| | __ _ _ __     *\n"
			"*    / __/ _` | |/ _ \\ '_ \\ / _` |/ _` | '__|    *\n"
			"*   | (_| (_| | |  __/ | | | (_| | (_| | |       *\n"
			"*    \\___\\__,_|_|\\___|_| |_|\\__,_|\\__,_|_|       *\n"
			"*                                                *\n"
			"*                                                *\n"
			"**************************************************",
			selectButton_hover[0], selectButton[1], selectButton[2]);

	gotoxy(currentPos.X, currentPos.Y);

	while(true)
	{
		ReadConsoleInput(handle_in, &keyrec, 1, &res);
		if ( keyrec.EventType == KEY_EVENT )
		{
			if (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_UP
				&& keyrec.Event.KeyEvent.bKeyDown == true)
			{
				printf("%s", selectButton[(currentPos.Y - _NONE_BUTTON_SPACE_LINES)/2]);
				(currentPos.Y > _FIRST_BUTTON_LINE) ? (currentPos.Y -= _BUTTON_SPACE) : (currentPos.Y = _LAST_BUTTON_LINE_VIEWCALENDAR);
				gotoxy(currentPos.X, currentPos.Y);
				printf("%s", selectButton_hover[(currentPos.Y - _NONE_BUTTON_SPACE_LINES)/2]);
				gotoxy(currentPos.X, currentPos.Y);
			}
			else if (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_DOWN
				&& keyrec.Event.KeyEvent.bKeyDown == true)
			{
				printf("%s", selectButton[(currentPos.Y-_NONE_BUTTON_SPACE_LINES)/2]);
				(currentPos.Y < _LAST_BUTTON_LINE_VIEWCALENDAR) ? (currentPos.Y += _BUTTON_SPACE) : (currentPos.Y = _FIRST_BUTTON_LINE);
				gotoxy(currentPos.X, currentPos.Y);
				printf("%s", selectButton_hover[(currentPos.Y-_NONE_BUTTON_SPACE_LINES)/2]);
				gotoxy(currentPos.X, currentPos.Y);
			}
			else if ( (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_RIGHT && keyrec.Event.KeyEvent.bKeyDown == true)
				|| (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_RETURN && keyrec.Event.KeyEvent.bKeyDown == true) )// 确定
			{
				return (currentPos.Y-_NONE_BUTTON_SPACE_LINES)/2;
			}
			else if ( (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_LEFT && keyrec.Event.KeyEvent.bKeyDown == true)
				|| (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_BACK && keyrec.Event.KeyEvent.bKeyDown == true) )// 回退
			{
				return _RETURN_LAST_PAGE_WND;
			}
			else if (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE
				&& keyrec.Event.KeyEvent.bKeyDown == true)
			{
				exit(0);
			}
			else {}
		}
	}

	return -2;
}

int leapYearJudgment()
{
	unsigned int year;
	char leapYearName[2][40] = {"\033[1;35;40m平年\033[0;37;40m", "\033[1;35;40m闰年\033[0;37;40m"};// “闰年”与“平年”文字
	COORD currentPos = {2, 6};// 输入数据时光标默认位置
	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	INPUT_RECORD keyrec;
	DWORD res;

LeapYearJudgment_RESTART:

	system("cls && title 万年历 - 闰年判断 && mode con cols=50 lines=21");
	printf(	"**************************************************\n"
			"*             欢迎使用万年历查询系统             *\n"
			"**************************************************\n"
			"*                                                *\n"
			"* 请输入需要查询的年份（例如：2022）:            *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"**************************************************" );
	gotoxy(currentPos.X, currentPos.Y);

	// 一定要刷新输入缓冲区，不然遇到非法字符残留时会影响输入
	// 例如：第一次输入 2022wwww，2022会被读取，但 www 会残留在输入缓冲区，
	// 那么第二次输入时，www 会自动被读取从而报错！
	fflush(stdin);
	scanf("%u", &year);
	if (year < 1 || year > 10000)
	{
		system("cls");
		printf(	"对不起，您输入的年份（%u）有问题，\n"
				"请检查后再次输入！\n"
				"[\033[0;30;47m任意键返回\033[0;37;40m]",
				year);
		system("pause 1>nul 2>nul");
		goto LeapYearJudgment_RESTART;
	}

	system("cls");
	printf(	"**************************************************\n"
			"*             欢迎使用万年历查询系统             *\n"
			"**************************************************\n"
			"*                                                *\n"
			"* 您查询的\033[1;37;40m %4u \033[0;37;40m年是:                            *\n"
			"*                                                *\n"
			"*     %4s                                       *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"**************************************************\n"
			"[\033[0;30;47m←键返回\033[0;37;40m]                          [\033[0;30;47m→键重新查询\033[0;37;40m]",
			year, leapYearName[isLeapYear(year)]);

	while(true)
	{
		ReadConsoleInput(handle_in, &keyrec, 1, &res);
		if ( keyrec.EventType == KEY_EVENT )
		{
			if (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_LEFT
				&& keyrec.Event.KeyEvent.bKeyDown == true)
			{
				return _RETURN_LAST_PAGE_WND;
			}
			else if (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_RIGHT
				&& keyrec.Event.KeyEvent.bKeyDown == true)
			{
				goto LeapYearJudgment_RESTART;
			}
			else {}
		}
	}

	return -2;
}
int weeklyQuery()
{
	unsigned int year, month, day;
	char weekName[7][40] = {"\033[1;35;40m星期日\033[0;37;40m", "\033[1;35;40m星期一\033[0;37;40m", "\033[1;35;40m星期二\033[0;37;40m", "\033[1;35;40m星期三\033[0;37;40m", "\033[1;35;40m星期四\033[0;37;40m", "\033[1;35;40m星期五\033[0;37;40m", "\033[1;35;40m星期六\033[0;37;40m"};// 从“星期日”到“星期六”的文字
	COORD currentPos = {2, 6};
	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	INPUT_RECORD keyrec;
	DWORD res;

WeeklyQuery_RESTART:
	year = 1, month = 1, day = 1;

	system("cls && title 万年历 - 星期查询 && mode con cols=50 lines=21");
	printf(	"**************************************************\n"
			"*             欢迎使用万年历查询系统             *\n"
			"**************************************************\n"
			"*                                                *\n"
			"* 请输入需要查询的时间（例如：2022-6-9）:        *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"**************************************************" );
	gotoxy(currentPos.X, currentPos.Y);

	fflush(stdin);
	scanf("%u-%u-%u", &year, &month, &day);
	if ((year < 1 || year > 10000) || (month < 1 || month > 12) || (day < 1 || day > returnNum_DaysOfMonth(year, month)))
	{
		system("cls");
		printf("对不起，您输入的日期（%u-%u-%u）有问题，\n请检查后再次输入！\n[\033[0;30;47m任意键返回\033[0;37;40m]", year, month);
		system("pause 1>nul 2>nul");
		goto WeeklyQuery_RESTART;
	}

	system("cls");
	printf(	"**************************************************\n"
			"*             欢迎使用万年历查询系统             *\n"
			"**************************************************\n"
			"*                                                *\n"
			"* 您查询的日期\033[1;37;40m %4u \033[0;37;40m年\033[1;37;40m %2u \033[0;37;40m月\033[1;37;40m %2u \033[0;37;40m日为:            *\n"
			"*                                                *\n"
			"*     %6s                                     *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"**************************************************\n"
			"[\033[0;30;47m←键返回\033[0;37;40m]                          [\033[0;30;47m→键重新查询\033[0;37;40m]",
			year, month, day, weekName[weekdayOfDay(year, month, day)]);

	while(true)
	{
		ReadConsoleInput(handle_in, &keyrec, 1, &res);
		if ( keyrec.EventType == KEY_EVENT )
		{
			if (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_LEFT
				&& keyrec.Event.KeyEvent.bKeyDown == true)
			{
				return _RETURN_LAST_PAGE_WND;
			}
			else if (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_RIGHT
				&& keyrec.Event.KeyEvent.bKeyDown == true)
			{
				goto WeeklyQuery_RESTART;
			}
			else {}
		}
	}

	return -2;
}

int queryMaxDays()
{
	unsigned int year, month, max_DaysOfMonth;
	COORD currentPos = {2, 6};
	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	INPUT_RECORD keyrec;
	DWORD res;

QueryMaxDays_RESTART:
	year = 1, month = 1;

	system("cls && title 万年历 - 查询最大日数 && mode con cols=50 lines=21");
	printf(	"**************************************************\n"
			"*             欢迎使用万年历查询系统             *\n"
			"**************************************************\n"
			"*                                                *\n"
			"* 请输入需要查询的时间（例如：2022-6）:          *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"**************************************************" );
	gotoxy(currentPos.X, currentPos.Y);

	fflush(stdin);
	scanf("%u-%u", &year, &month);
	if ((year < 1 || year > 10000) || (month < 1 || month > 12))
	{
		system("cls");
		printf("对不起，您输入的年月（%u-%u）有问题，\n请检查后再次输入！\n[\033[0;30;47m任意键返回\033[0;37;40m]", year, month);
		system("pause 1>nul 2>nul");
		goto QueryMaxDays_RESTART;
	}

	// 获取一个这个月对应最大有多少天
	max_DaysOfMonth = returnNum_DaysOfMonth(year, month);

	system("cls");
	printf(	"**************************************************\n"
			"*             欢迎使用万年历查询系统             *\n"
			"**************************************************\n"
			"*                                                *\n"
			"* 您查询的日期\033[1;37;40m %4u \033[0;37;40m年\033[1;37;40m %2u \033[0;37;40m月的最大日数为:        *\n"
			"*                                                *\n"
			"*     \033[1;35;40m%2u\033[0;37;40m                                         *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"**************************************************\n"
			"[\033[0;30;47m←键返回\033[0;37;40m]                          [\033[0;30;47m→键重新查询\033[0;37;40m]",
			year, month, max_DaysOfMonth);

	while(true)
	{
		ReadConsoleInput(handle_in, &keyrec, 1, &res);
		if ( keyrec.EventType == KEY_EVENT )
		{
			if (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_LEFT
				&& keyrec.Event.KeyEvent.bKeyDown == true)
			{
				return _RETURN_LAST_PAGE_WND;
			}
			else if (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_RIGHT
				&& keyrec.Event.KeyEvent.bKeyDown == true)
			{
				goto QueryMaxDays_RESTART;
			}
			else {}
		}
	}

	return -2;
}

int viewAlmanac()
{
	unsigned int year, month;
	unsigned int i, j;// 临时变量

	//  input_lines 变量用于调整日历界面行数的统一（从头到尾共 21 行）
	// 原理：最上面 6 行和最下面 3 行是固定的，变化的主要是
	// 由于日历会输出 4 ~ 6 行不等，具体取决于当月天数和每月 1 号位置
	//（也就是当一个月历输出完毕后，其总共输出的行数是无法确定的，是会变化的）。
	// 所以，需要有一个变量来统计这个月历到底排列了几行。
	// input_lines 变量选择统计\n的个数来统计行数，从当月 1 号的输出开始计算 1 行，后面实时检测换行符的出现！
	unsigned int input_lines;
	unsigned int weekdayOfFirstDay_InMonth, num_DaysOfMonth;
	COORD currentPos = {2, 6};
	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	INPUT_RECORD keyrec;
	DWORD res;

ViewAlmanac_RESTART:
	year = 1, month = 1, i = 0, j = 0;

	system("cls && title 万年历 - 查看日历 - 查看月历 && mode con cols=50 lines=21");
	printf( "**************************************************\n"
			"*             欢迎使用万年历查询系统             *\n"
			"**************************************************\n"
			"*                                                *\n"
			"* 请输入需要查询的时间（例如：2022）:            *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"**************************************************" );
	gotoxy(currentPos.X, currentPos.Y);

	fflush(stdin);
	scanf("%u", &year);
	if ((year < 1 || year > 10000))
	{
		system("cls");
		printf("对不起，您输入的年份（%u）有问题，\n请检查后再次输入！\n[\033[0;30;47m任意键返回\033[0;37;40m]", year);
		system("pause 1>nul 2>nul");
		goto ViewAlmanac_RESTART;
	}

ViewAlmanac_NEXTPAGE:

	input_lines = 0;// 一定放在这里刷新，因为换页是回到此处。而换页本身就是输出一个新的月历，需要重新统计行数（每个月的情况是不一样的）

	system("cls");
	printf( "**************************************************\n"
			"*               公元\033[1;37;40m%5u \033[0;37;40m年\033[1;37;40m%3u \033[0;37;40m月               *\n"
			"**************************************************\n"
			"*                                                *\n"
			"*\033[0;33;40m星期日 星期一 星期二 星期三 星期四 星期五 星期六\033[0;37;40m*\n"
			"*                                                *\n",
			year, month);

	// 获取某年某月的第一天是星期几，用于后面输出打印月历
	weekdayOfFirstDay_InMonth = weekdayOfDay(year, month, 1);
	// 获取这个月有多少天，用于后面输出打印月历
	num_DaysOfMonth = returnNum_DaysOfMonth(year, month);


	printf("*\033[1;37;40m");
	for (i = 0; i < weekdayOfFirstDay_InMonth; i++)
	{
		printf("%7c", ' ');
	}

	for (j = 1; j <= num_DaysOfMonth; j++, i++)
	{
		printf("%4u  ", j);
		
		if (i % 7 == 6)
		{
			printf("\033[0;37;40m*\n*                                                *\n*\033[1;37;40m");
			input_lines += 2;// 出现两个\n，统计加 2
		}
		else
		{
			putchar(' ');
		}
	}

	while (i % 7 != 6)
	{
		printf("%7c", ' ');
		i++;
	}
	printf(	"      \033[0;37;40m*\n");
	input_lines++;// 出现一个\n，统计加 1

	// 月历部分掐头去尾（固定的那部分），应该要有 12 行，所以未达到的就补上
	while (input_lines < 12)
	{
		printf("*                                                *\n");
		input_lines++;
	}

	printf(	"*                                                *\n"
			"**************************************************\n"
			"[\033[0;30;47m←键返回\033[0;37;40m]      [\033[0;30;47m↑/↓键翻页\033[0;37;40m]       [\033[0;30;47m→键重新查询\033[0;37;40m]");

	while(true)
	{
		ReadConsoleInput(handle_in, &keyrec, 1, &res);
		if ( keyrec.EventType == KEY_EVENT )
		{
			if (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_LEFT
				&& keyrec.Event.KeyEvent.bKeyDown == true)
			{
				return _RETURN_LAST_PAGE_WND;
			}
			else if (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_RIGHT
				&& keyrec.Event.KeyEvent.bKeyDown == true)
			{
				goto ViewAlmanac_RESTART;
			}
			else if (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_UP
				&& keyrec.Event.KeyEvent.bKeyDown == true)// 月历上翻页
			{
				(month <= 1) ? (month = 12) : (month--);
				goto ViewAlmanac_NEXTPAGE;
			}
			else if (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_DOWN
				&& keyrec.Event.KeyEvent.bKeyDown == true)// 月历下翻页
			{
				(month >= 12) ? (month = 1) : (month++);
				goto ViewAlmanac_NEXTPAGE;
			}
			else {}
		}
	}

	return -2;
}

int viewMonthlyCalendar()
{
	unsigned int year, month;
	unsigned int i, j, input_lines;
	unsigned int weekdayOfFirstDay_InMonth, num_DaysOfMonth;
	COORD currentPos = {2, 6};
	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	INPUT_RECORD keyrec;
	DWORD res;

ViewMonthlyCalendar_RESTART:
	year = 1, month = 1, i = 0, j = 0, input_lines = 0;

	system("cls && title 万年历 - 查看日历 - 查看月历 && mode con cols=50 lines=21");
	printf( "**************************************************\n"
			"*             欢迎使用万年历查询系统             *\n"
			"**************************************************\n"
			"*                                                *\n"
			"* 请输入需要查询的时间（例如：2022-6）:          *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"*                                                *\n"
			"**************************************************" );
	gotoxy(currentPos.X, currentPos.Y);

	fflush(stdin);
	scanf("%u-%u", &year, &month);
	if ((year < 1 || year > 10000) || (month < 1 || month > 12))
	{
		system("cls");
		printf("对不起，您输入的年月（%u-%u）有问题，\n请检查后再次输入！\n[\033[0;30;47m任意键返回\033[0;37;40m]", year, month);
		system("pause 1>nul 2>nul");
		goto ViewMonthlyCalendar_RESTART;
	}


	system("cls");
	printf( "**************************************************\n"
			"*               公元\033[1;37;40m%5u \033[0;37;40m年\033[1;37;40m%3u \033[0;37;40m月               *\n"
			"**************************************************\n"
			"*                                                *\n"
			"*\033[0;33;40m星期日 星期一 星期二 星期三 星期四 星期五 星期六\033[0;37;40m*\n"
			"*                                                *\n",
			year, month);

	weekdayOfFirstDay_InMonth = weekdayOfDay(year, month, 1);
	num_DaysOfMonth = returnNum_DaysOfMonth(year, month);


	printf("*\033[1;37;40m");
	for (i = 0; i < weekdayOfFirstDay_InMonth; i++)
	{
		printf("%7c", ' ');
	}

	for (j = 1; j <= num_DaysOfMonth; j++, i++)
	{
		printf("%4u  ", j);
		
		if (i % 7 == 6)
		{
			printf("\033[0;37;40m*\n*                                                *\n*\033[1;37;40m");
			input_lines += 2;
		}
		else
		{
			putchar(' ');
		}
	}

	while (i % 7 != 6)
	{
		printf("%7c", ' ');
		i++;
	}
	printf(	"      \033[0;37;40m*\n");
	input_lines++;

	while (input_lines < 12)
	{
		printf("*                                                *\n");
		input_lines++;
	}

	printf(	"*                                                *\n"
			"**************************************************\n"
			"[\033[0;30;47m←键返回\033[0;37;40m]                          [\033[0;30;47m→键重新查询\033[0;37;40m]");

	while(true)
	{
		ReadConsoleInput(handle_in, &keyrec, 1, &res);
		if ( keyrec.EventType == KEY_EVENT )
		{
			if (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_LEFT
				&& keyrec.Event.KeyEvent.bKeyDown == true)
			{
				return _RETURN_LAST_PAGE_WND;
			}
			else if (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_RIGHT
				&& keyrec.Event.KeyEvent.bKeyDown == true)
			{
				goto ViewMonthlyCalendar_RESTART;
			}
			else {}
		}
	}

	return -2;
}