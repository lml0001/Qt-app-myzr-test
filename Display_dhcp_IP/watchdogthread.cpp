#include "watchdogthread.h"
extern int wdgflag;
watchdogthread::watchdogthread(QObject *parent,QApplication *a):QThread(parent)
{
    app = a;
    cnt = 0;
}
void watchdogthread::run()
{
    while(1){
        sleep(1);
        cnt++;
        if(cnt==20){
            std::cout<<"cnt: "<<cnt<<" wdgflag: "<<wdgflag<<std::endl;
            if(wdgflag != 0){
                app->quit();
            }
            else{
                cnt = 0;
            }
        }
        else if(cnt == 5)
        {
            wdgflag = -1;
        }
    }
}
