#include <stdio.h>
#include <time.h> //时间函数
#include <conio.h> //字符模式函数
#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h> //与多媒体有关 

#pragma comment(lib,"winmm.lib") //将 winmm.lib作为附加的库文件链接到目标可执行文件中

typedef struct date
{
    int hour;
    int min;
    int sec;
    int month;
    int day;
    int year;
}Date; //定义日期结构体 

void display_time();	//总体显示时间 
void start_timer();		//开始计时 
void pause_timer(); 	//暂停计时 
void reset_timer();		//重置计时器 
void set_alarm(); 		//设置闹钟 
void reset_alarm(); 	//重置闹钟 
void checking_time(int hour, int min , int sec); //检查闹钟设置时间 
void world_clock(); //显示世界时钟 
void cal_worldtime(Date country,int diff,int i); //计算世界时间 
void countdown_timer(); //设置倒计时 
void reset_countdown(); //重置倒计时 
void exit_program(); //退出程序 
void gotoxy(int x,int y); //光标定位
void dians(); //绘制点号 
void draw_numb(int x,int shu); //绘制数字 
void draws(char wei,int shu); //绘制时分秒 
void show_time(); //电子显示时间 

struct tm *curtime; 	//结构体tm，结构指针curtime，可指向当前日期,time.h中定义
time_t t;         		//时间类型变量t，time.h中定义

int timer_running = 0; //计时器运行标志 
int pause_timers = 0; //是否暂停的标志 
int countdown_running = 0; //倒计时是否设置成功 
int flag_setcountdown = 0; //设置倒计时时正确输入个数 
int start_time = 0;  //计时器开始时间 
int pause_time = 0; //暂停时的时间 
int total_time = 0; //总耗时 
int flag_wrongtime = 0; //是否错误设定闹钟时间 
int flag_setalarm = 0; //设定闹钟时正确输入个数 
int alarm_set = 0; //是否设置了闹钟 
int alarm_hours, alarm_minutes, alarm_seconds; //闹钟时间
int countdown_hour,countdown_min,countdown_sec; //倒计时设置的时分秒 
int endhour,endmin,endsec; //倒计时结束时的时间 
Date london,newyork,moscow,tokyo,paris,los,sydney,toronto; //世界各国日期结构体 
char coun[8][20]={"London","Newyork","Moscow","Tokyo","Paris","Los","Sydney","Toronto"}; //国家名称 

int Month[2][12]={{31,28,31,30,31,30,31,31,30,31,30,31},{31,29,31,30,31,30,31,31,30,31,30,31}}; //非闰年和闰年各月天数 


int main()
{
    SetConsoleOutputCP(CP_UTF8);

    while (1)
    {	//打印菜单
        printf("Clock Program\n");
        printf("Press 't' to start or continue timer (开始计时器)\n");
        printf("Press 'p' to pause timer (暂停计时器)\n");
        printf("Press 'r' to reset timer or alarm or countdown timer (重置计时器或闹钟或倒计时)\n");
        printf("Press 'a' to set alarm (设置闹钟)\n");
        printf("Press 's' to watch the world clock (查看世界时钟)\n");
        printf("Press 'd' to set countdown timer (设置倒计时)\n");
        printf("Press 'x' to exit program (退出)\n\n");
        display_time(); //电子显示当前时间
        if (kbhit()) //是否有键盘输入 
        {
            char input = getch();
            switch (input)
            {
                case 't':
                case 'T':
                    if (timer_running == 0)
                    {
                        printf("\nStart timer\n");
                        start_timer(); //启动计时器
                    }
                    break;
                case 'r':
                case 'R':
                    if(pause_timers == 1 || timer_running == 1)
                    {
                        printf("\nReset Timer\n");
                        reset_timer(); //重置计时器
                        pause_timers = 0;
                    }
                    else if(alarm_set == 1)
                    {
                        printf("\nReset the Alarm\n");
                        reset_alarm(); //重置闹钟
                    }
                    else if(countdown_running == 1)
                    {
                        printf("\nReset the Countdown Timer\n");
                        reset_countdown(); //重置倒计时
                    }
                    else
                    {
                        printf("\nAlarm or timer or countdown are not yet in use, cannot be reset\n"); //无法重置
                        Sleep(2000);
                    }
                    break;
                case 'p':
                case 'P':
                    if (timer_running == 1)
                    {
                        printf("Pause timer\n");
                        pause_timer(); //暂停计时器
                        pause_timers=1;
                        printf("Total timing:%ds \n",total_time);
                        printf("Press 't' to continue timer or 'r' to reset timer\n");
                        while(1)
                        {
                            if(kbhit())
                            {
                                break; //有键盘输入时才关闭页面，防止页面刷新
                            }
                        }
                    }
                    else
                    {
                        printf("\nThe timer did not start, cannot pause\n");
                        Sleep(1500);
                    }
                    break;
                case 'a':
                case 'A':
                    if (alarm_set == 0)
                    {
                        printf("\nSet alarm (24-hour clock, format: HH MM SS)\n");
                        set_alarm(); //设置闹钟 
                        checking_time(alarm_hours, alarm_minutes, alarm_seconds); //检查闹钟设置时间是否有错 
                        if(flag_wrongtime == 1) //闹钟设置时间有错的情况 
                        {
                            printf("Wrong!!! press 'a' again to set alarm\n");
                            alarm_set = 0;
                            reset_alarm(); //重置闹钟时间
                            fflush(stdin); //清空输入缓冲区，将上一次的输入读走
                            Sleep(1500); //暂停1.5s，方便用户看清
                        }
                        if(flag_wrongtime == 0) //正确输入的情况
                        {
                            printf("Alarm set for %02d:%02d:%02d\n", alarm_hours, alarm_minutes, alarm_seconds);
                            Sleep(1500);
                        }
                    }
                    else if(alarm_set == 1) //已经设置过闹钟 
                    {
                        printf("\nThe alarm has been set for %02d:%02d:%02d\n", alarm_hours, alarm_minutes, alarm_seconds);
                        printf("If you want to reset the alarm, press 'r'\n");
                        Sleep(2500);
                    }
                    break;
                case 's':
                case 'S':
                    world_clock(); //显示世界时间
                    printf("\nPress Enter Key to go back to the main page\n");
                    while(1)
                    {
                        if(kbhit())
                        {
                            break; //按回车键回到主页面
                        }
                    }
                    break;
                case 'd':
                case 'D':
                    if(countdown_running == 0)
                    {
                        countdown_timer(); //设置倒计时
                        countdown_running = 1;
                    }
                    else
                    {
                        printf("\nThe countdown has been set for %dHour %dMin %dSec\n",countdown_hour,countdown_min,countdown_sec);
                        printf("If you want to reset the countdown timer, press 'r'\n");
                        Sleep(2500);
                    }
                    break;
                case 'x':
                case 'X':
                    exit_program(); //退出程序 
                    break;
                default: //输入错误时 退出当次循环 
                    break;
            }
        }

        Sleep(100); //隔0.1s刷新一次 
        system("cls");//清屏
    }
}

void display_time() //总体显示时间并检查计时器、闹钟、倒计时 
{
    show_time(); //电子显示时间
    char key;
    if (alarm_set==1 && curtime->tm_hour==alarm_hours && curtime->tm_min==alarm_minutes && curtime->tm_sec==alarm_seconds)
    { //闹钟时间到 
        printf("\nAlarm! Alarm! Alarm!\n");
        //PlaySound(TEXT("alarm.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); //播放音乐 
        printf("Press Enter key to pause the alarm or Press 'c' to add 5 minutes\n"); //按回车停止闹钟或'c'键延长5分钟 
        key=getch();
        if(key=='c')
        {
            alarm_minutes=alarm_minutes+5;
            if(alarm_minutes>=60)
            {
                alarm_minutes=alarm_minutes%60; //分钟大于60时时钟加1
                alarm_hours=alarm_hours+1;
                alarm_hours=alarm_hours%24;
            }
            printf("The alarm has been delayed 5 minutes, will run at %02d:%02d:%02d\n",alarm_hours,alarm_minutes,alarm_seconds);
            Sleep(2500);
        }
        else
        {
            alarm_set=0;
            return;
        }
    }

    if (timer_running == 1)
    { //计时器在运行时 
        int elapsed_time = (clock() - start_time) / 1000;//将毫秒转换为秒
        printf("\n");
        printf("\nTimer: %02d:%02d:%02d\n", (elapsed_time + total_time) / 3600, ((elapsed_time + total_time) % 3600) / 60, (elapsed_time + total_time) % 60);
    }

    if (countdown_running==1 && curtime->tm_hour==endhour && curtime->tm_min==endmin && curtime->tm_sec==endsec)
    { //倒计时结束时 
        printf("\nThe time is up!!! The time is up!!!\n");
        //PlaySound(TEXT("countdown.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); //播放音乐
        printf("Press Enter key to pause the Countdown Timer\n"); //按回车退出计时器
        while(1)
        {
            if(kbhit())
            {
                break;
            }
        }
        countdown_running=0;
    }
}

void start_timer() //开始计时 
{
    timer_running = 1;
    start_time = clock();
}

void pause_timer() //暂停计时 
{
    timer_running = 0;
    pause_time = clock();
    total_time += (pause_time - start_time) / 1000;
}

void reset_timer() //重置计时器 
{
    timer_running = 0;
    start_time = 0;
    pause_time = 0;
    total_time = 0;
    Sleep(1000);
}

void set_alarm() //设置闹钟 
{
    flag_setalarm = scanf("%d %d %d", &alarm_hours, &alarm_minutes, &alarm_seconds);
    alarm_set = 1;
    return;
}

void reset_alarm() //重置闹钟 
{
    alarm_set = 0;
    alarm_hours = 0;
    alarm_minutes = 0;
    alarm_seconds = 0;
    Sleep(1000);
}

void checking_time(int hour, int min , int sec) //检查闹钟设置时间是否有误 
{
    if(hour<0 || hour>24 || min<0 || min>60 || sec<0 || sec>60)
    {
        flag_wrongtime = 1;
        return;
    }
    if(flag_setalarm!=3)
    {
        flag_wrongtime = 1;
        return;
    }
    flag_wrongtime = 0;
}

void world_clock()  //世界时钟london,newyork,moscow,tokyo,paris,los,sydney,toronto;
{
    cal_worldtime(london,-7,0);
    cal_worldtime(newyork,-12,1);
    cal_worldtime(moscow,-5,2);
    cal_worldtime(tokyo,+1,3);
    cal_worldtime(paris,-6,4);
    cal_worldtime(los,-15,5);
    cal_worldtime(sydney,+2,6);
    cal_worldtime(toronto,-12,7);
}

void cal_worldtime(Date country,int diff,int i) //计算每个地区的时间 
{
    int flag=0;
    if((curtime->tm_year%4==0 && curtime->tm_year%100!=0) || curtime->tm_year%400==0 )
    {//闰年的情况
        flag=1;
    }
    country.hour=curtime->tm_hour+diff;
    country.min=curtime->tm_min;
    country.sec=curtime->tm_sec;
    country.day=curtime->tm_mday;
    country.month=curtime->tm_mon+1;
    country.year=curtime->tm_year+1900;
    if(curtime->tm_hour+diff<0) //北京时间加时差小时数小于0
    {
        country.day=curtime->tm_mday-1; //天数减1
        country.hour=curtime->tm_hour+24+diff; //小时数补上24
        if(curtime->tm_mday-1<1) //天数小于1号
        {
            country.day=(flag==1) ? Month[0][curtime->tm_mon-1] : Month[1][curtime->tm_mon-1]; //天数为前一月最后一天
            country.month=curtime->tm_mon; //月数减1
            if(country.month<1) //月数不够减
            {
                country.month=12;
                country.day=Month[0][11];
                country.year=curtime->tm_year+1900-1; //年数减1
            }
        }
    }
    else if(curtime->tm_hour+diff>24) //北京时间加时差小时数大于24
    {
        country.day=curtime->tm_mday+1; //天数加1
        country.hour=curtime->tm_hour-24+diff; //小时数减24
        if(curtime->tm_mday+1 > ((flag==1) ? Month[0][curtime->tm_mon] : Month[1][curtime->tm_mon])) //天数超过当月天数
        {
            country.month=curtime->tm_mon+2; //月份数加1
            country.day=1;
            if(country.month>12) //月份数超过12
            {
                country.month=1;
                country.day=1;
                country.year=curtime->tm_year+1900+1; //年数加1
            }
        }
    }
    printf("\n%s time: %d/%d/%d %02d:%02d:%02d\n",coun[i],country.year,country.month,country.day,country.hour,country.min,country.sec);
}

void countdown_timer() //启动倒计时 
{
    printf("\nPlease enter the time you want to count down (format: HH MM SS)\n");
    while((flag_setcountdown = scanf("%d %d %d",&countdown_hour,&countdown_min,&countdown_sec))!=3) //输入有误时
    {
        printf("Wrong!!! Please set countdown time again\n");
        fflush(stdin); //清空输入缓冲区，将上一次的输入读走
    }
    printf("countdown %dHour %dMin %dSec\n",countdown_hour,countdown_min,countdown_sec);
    endsec=(curtime->tm_sec+countdown_sec)%60; //倒计时结束时秒数
    endmin=(curtime->tm_min+countdown_min+ (curtime->tm_sec+countdown_sec)/60 )%60; //倒计时结束时分数
    endhour=curtime->tm_hour+countdown_hour+(curtime->tm_min+countdown_min+ (curtime->tm_sec+countdown_sec)/60 )/60; //倒计时结束时小时数
    endhour=endhour%24;
    printf("the countdown timer will end at %02d:%02d:%02d\n",endhour,endmin,endsec);
    Sleep(3000);
}

void reset_countdown() //重置计时器 
{
    countdown_hour=0;
    countdown_min=0;
    countdown_sec=0;
    countdown_running=0;
    Sleep(1000);
}

void exit_program() //退出程序 
{
    //PlaySound(NULL, NULL, 0); //播放音乐 
    exit(0);
}

void gotoxy(int x,int y)	//光标定位函数
{
    COORD coord; //表示字符在控制台屏幕上的坐标的结构体 
    coord.X=x; //横坐标
    coord.Y=y; //纵坐标
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coord); //定位光标位置
    //参数1：句柄（通过GetStdHandle()函数获得句柄）参数2：位置（结构体类型）
}

void dians() //绘制时间之间的点号 
{
    int x=8;
    gotoxy(x*3,11);
    printf(" **");
    gotoxy(x*3,12);
    printf(" **");
    gotoxy(x*3,14);
    printf(" **");
    gotoxy(x*3,15);
    printf(" **");

    gotoxy(x*6,11);
    printf(" **");
    gotoxy(x*6,12);
    printf(" **");
    gotoxy(x*6,14);
    printf(" **");
    gotoxy(x*6,15);
    printf(" **");
}

void draw_numb(int x,int shu)
{ //判断0-9的数据,通过gotoxy显示出来
    if(shu==0){ //绘制数字0
        gotoxy(x,9);
        printf("*****");
        gotoxy(x,10);
        printf("*   *");
        gotoxy(x,11);
        printf("*   *");
        gotoxy(x,12);
        printf("*   *");
        gotoxy(x,13);
        printf("*   *");
        gotoxy(x,14);
        printf("*   *");
        gotoxy(x,15);
        printf("*   *");
        gotoxy(x,16);
        printf("*   *");
        gotoxy(x,17);
        printf("*****");
    }
    if(shu==1){ //绘制数字1
        gotoxy(x,9);
        printf("  *  ");
        gotoxy(x,10);
        printf("  *  ");
        gotoxy(x,11);
        printf("  *  ");
        gotoxy(x,12);
        printf("  *  ");
        gotoxy(x,13);
        printf("  *  ");
        gotoxy(x,14);
        printf("  *  ");
        gotoxy(x,15);
        printf("  *  ");
        gotoxy(x,16);
        printf("  *  ");
        gotoxy(x,17);
        printf("  *  ");
    }
    if(shu==2){ //绘制数字2
        gotoxy(x,9);
        printf("*****");
        gotoxy(x,10);
        printf("    *");
        gotoxy(x,11);
        printf("    *");
        gotoxy(x,12);
        printf("    *");
        gotoxy(x,13);
        printf("*****");
        gotoxy(x,14);
        printf("*    ");
        gotoxy(x,15);
        printf("*    ");
        gotoxy(x,16);
        printf("*    ");
        gotoxy(x,17);
        printf("*****");
    }
    if(shu==3){ //绘制数字3
        gotoxy(x,9);
        printf("*****");
        gotoxy(x,10);
        printf("    *");
        gotoxy(x,11);
        printf("    *");
        gotoxy(x,12);
        printf("    *");
        gotoxy(x,13);
        printf("*****");
        gotoxy(x,14);
        printf("    *");
        gotoxy(x,15);
        printf("    *");
        gotoxy(x,16);
        printf("    *");
        gotoxy(x,17);
        printf("*****");
    }
    if(shu==4){ //绘制数字4
        gotoxy(x,9);
        printf("*   *");
        gotoxy(x,10);
        printf("*   *");
        gotoxy(x,11);
        printf("*   *");
        gotoxy(x,12);
        printf("*   *");
        gotoxy(x,13);
        printf("*****");
        gotoxy(x,14);
        printf("    *");
        gotoxy(x,15);
        printf("    *");
        gotoxy(x,16);
        printf("    *");
        gotoxy(x,17);
        printf("    *");
    }
    if(shu==5){ //绘制数字5
        gotoxy(x,9);
        printf("*****");
        gotoxy(x,10);
        printf("*    ");
        gotoxy(x,11);
        printf("*    ");
        gotoxy(x,12);
        printf("*    ");
        gotoxy(x,13);
        printf("*****");
        gotoxy(x,14);
        printf("    *");
        gotoxy(x,15);
        printf("    *");
        gotoxy(x,16);
        printf("    *");
        gotoxy(x,17);
        printf("*****");
    }
    if(shu==6){ //绘制数字6
        gotoxy(x,9);
        printf("*****");
        gotoxy(x,10);
        printf("*    ");
        gotoxy(x,11);
        printf("*    ");
        gotoxy(x,12);
        printf("*    ");
        gotoxy(x,13);
        printf("*****");
        gotoxy(x,14);
        printf("*   *");
        gotoxy(x,15);
        printf("*   *");
        gotoxy(x,16);
        printf("*   *");
        gotoxy(x,17);
        printf("*****");
    }
    if(shu==7){ //绘制数字7
        gotoxy(x,9);
        printf("*****");
        gotoxy(x,10);
        printf("    *");
        gotoxy(x,11);
        printf("    *");
        gotoxy(x,12);
        printf("    *");
        gotoxy(x,13);
        printf("    *");
        gotoxy(x,14);
        printf("    *");
        gotoxy(x,15);
        printf("    *");
        gotoxy(x,16);
        printf("    *");
        gotoxy(x,17);
        printf("    *");
    }
    if(shu==8){ //绘制数字8
        gotoxy(x,9);
        printf("*****");
        gotoxy(x,10);
        printf("*   *");
        gotoxy(x,11);
        printf("*   *");
        gotoxy(x,12);
        printf("*   *");
        gotoxy(x,13);
        printf("*****");
        gotoxy(x,14);
        printf("*   *");
        gotoxy(x,15);
        printf("*   *");
        gotoxy(x,16);
        printf("*   *");
        gotoxy(x,17);
        printf("*****");
    }
    if(shu==9){ //绘制数字9
        gotoxy(x,9);
        printf("*****");
        gotoxy(x,10);
        printf("*   *");
        gotoxy(x,11);
        printf("*   *");
        gotoxy(x,12);
        printf("*   *");
        gotoxy(x,13);
        printf("*****");
        gotoxy(x,14);
        printf("    *");
        gotoxy(x,15);
        printf("    *");
        gotoxy(x,16);
        printf("    *");
        gotoxy(x,17);
        printf("*****");
    }
}

void draws(char wei,int shu)
{//定义了6个位置 分别是小时的个位十位，分钟的个位十位和秒钟的个位十位
    int x=8;
    if(wei=='1')
    {
        draw_numb(x*1,shu);	//调用draw_numb函数把x*1(表示第几个位置数)，shu(表示要显示的数字)调过去
    }						//x*1表示第一个位置 即小时的十位
    if(wei=='2')
    {
        draw_numb(x*2,shu); //绘制小时的个位
    }
    if(wei=='3')
    {
        draw_numb(x*4,shu); //绘制分钟的十位
    }
    if(wei=='4')
    {
        draw_numb(x*5,shu); //绘制分钟的个位
    }
    if(wei=='5')
    {
        draw_numb(x*7,shu); //绘制秒钟的十位
    }
    if(wei=='6')
    {
        draw_numb(x*8,shu); //绘制秒钟的个位
    }
}

void show_time()
{
    //system("color 9");  //可以改变颜色
    dians();							//将两个点绘制出来
    t=time(0);							//获取当前的秒数,参数为0则函数返回值即为结果
    curtime=localtime(&t);				//得到当前系统时间/
    if((double)curtime->tm_hour<=12)	//午前的处理/
    {
        gotoxy(1,13);					//绘制AM
        printf("AM ");
        if((double)curtime->tm_hour<10) draws('1',0);   //十点之前在小时数前加零
        draws('1',((int)curtime->tm_hour)/10);   	 //绘制小时的十位
        draws('2',((int)((double)curtime->tm_hour))%10);	//绘制小时的个位
    }
    else								//午后的处理
    {
        gotoxy(1,13);
        printf("PM ");					//绘制PM
        if((double)curtime->tm_hour-12<10) draws('1',0);//输入0
        draws('1',(int)curtime->tm_hour/10);		//绘制小时的十位
        draws('2',((int)((double)curtime->tm_hour))%10);	//绘制小时的个位
    }
    if((double)curtime->tm_min<10)			//时钟小于10添加0 
    {
        draws('3',0);
    }
    draws('3',(int)curtime->tm_min/10);		//绘制分钟的十位 
    draws('4',(int)curtime->tm_min%10);		//绘制分钟的个位 
    if((double)curtime->tm_sec<10)
    {
        draws('5',0);						//时钟小于10添加0
    }
    draws('5',(int)curtime->tm_sec/10);		//绘制秒钟的十位
    draws('6',(int)curtime->tm_sec%10);		//绘制秒钟的个位
}
