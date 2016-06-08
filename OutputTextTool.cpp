#include "outputtexttool.h"

#define CHANGE_DOT_MS_TIME	500	// change when every 500ms
#define MAX_DOT_LEN			5

int OutputTextTool::m_nDotLen = 0;

void OutputTextTool::waitText(int sleep_time)
{
    int mod = MAX_DOT_LEN + 1;
    printf("\rWait");

    m_nDotLen += sleep_time;
    m_nDotLen %= CHANGE_DOT_MS_TIME * mod;

    int len = (m_nDotLen / CHANGE_DOT_MS_TIME) % mod;	// change when every 500ms
    for(int i = 0 ; i <= MAX_DOT_LEN ; i ++)
    {
        if(i < len)
            printf(".");
        else
            printf(" ");	// remove the orignial dot
    }
}

void OutputTextTool::showProgress(int percentage, std::string tailStr)
{
    int count = percentage / 10;
    printf("\rProgress : [");

    for(int i = 1; i <= 10 ; i++)
        printf("%c", i < count ? '>' : ( i > count ? ' ' : (i == 10) ? '>' : '>'));

    if(percentage >= 100)
        printf("] %3d %%\t\t\t\t\t\n", percentage);
    else
        printf("] %3d %%\t%s", percentage, tailStr.c_str());
}

void OutputTextTool::showTextWithVariableDots(std::string str, int count, int total)
{
    int mod = MAX_DOT_LEN + 1;

    printf("\r%s", str.c_str());

    if(total > 0)
        printf(" (%d/%d)", count, total);

    int len = (m_nDotLen++ % mod);

    for(int i = 0 ; i <= MAX_DOT_LEN ; i ++)
    {
        if(i < len)
            printf(".");
        else
            printf(" ");	// remove the orignial dot
    }
}
