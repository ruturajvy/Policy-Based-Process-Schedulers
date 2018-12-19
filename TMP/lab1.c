#include <lab1.h>
#include <stdio.h>

extern unsigned int class_flag;
void setschedclass(int sched_class)
{
if(sched_class==RANDOMSCHED)
{
class_flag = 1;
}

else if(sched_class==LINUXSCHED)
{
class_flag = 2;
}

else
{
kprintf("Invalid option");
}
}

int getschedclass()
{
if(class_flag==1)
return RANDOMSCHED;
if(class_flag==2)
return LINUXSCHED;
}

