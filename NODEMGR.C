#include "vardec.h"
#include "vars.h"
#include "fcns.h"
#include "fidoadr.h"
#include "fidoadr.c"
void node_setup(void)
{
    int done,keypress,i,j;
    unsigned int keycode;
    char ch;
    done=0;
    wopen(6,5,19,55,1,YELLOW|_BLUE,WHITE|_BLUE);
    DoShadow();
    wmessage(" F1 For Editing Help ",1,7,LIGHTRED|_BLUE);
    show_hint58();
    i=1;
    while (!done)
    {
        show_node_screen(i);
        keycode=getxch();
        keypress=keycode>>8;
        switch(keypress)
        {
            case 1 : /* ESC */
                done=1;
                break;
            case 28: /* Enter */
                current_node=i;
                edit_node(i);
                i=current_node;
                break;
            case 71: /* Home */
                i=1;
                break;
            case 75: /* Left Arrow */
                i--;
                if (i<1)
                    i=num_nodes;
                break;
            case 77: /* Right Arrow */
                i++;
                if (i>num_nodes)
            i=1;
                break;
            case 79: /* End */
                i=num_nodes;
                break;
            case 82: /* Insert */
                num_nodes++;
                if (num_nodes>MAX_NODES)
                {
                    wperror("Warning!  Maximum of 50 nodes!");
                    num_nodes=MAX_NODES-1;
                    break;
                }
                i=num_nodes;
                nodes[i].address[0]=0;
                nodes[i].sysop_name[0]=0;
                nodes[i].pkt_pw[0]=0;
                nodes[i].compression=0;
                nodes[i].groups=0;
                edit_node(i);
                break;
        case 83: /* Delete */
                wopen(7,25,10,55,1,WHITE|_RED,WHITE|_RED);
                DoShadow();
                wcenters(0,YELLOW|_RED,"Delete!  Are You Sure?");
                wgotoxy(0,26);
                showcur();
                ch=wgetc();
                wclose();
                hidecur();
                if (upcase(ch)=='Y')
                {
                    current_node=i;
                    num_nodes--;
                    for (j=i;j<=num_nodes;j++)
                        nodes[j]=nodes[j+1];
                    if (num_nodes<1)
                        num_nodes=1;
                    i=current_node;
                    if (i<1)
                        i=1;
                    if (i>num_nodes)
                        i=num_nodes;
                    break;
                }
        }
    }
    wclose();
    for (i=1;i<num_nodes+1;i++)
    {
        for (j=1;j<num_nodes+1;j++)
        {
            if (strcmp(nodes[j].address,nodes[i].address)>0)
            {
                nodes[0]=nodes[j];
                nodes[j]=nodes[i];
                nodes[i]=nodes[0];
        }

        }

    }
    clear_hint();
    show_hint4();
}
void show_node_screen(int nodenum)
{
    char s[161],s1[15];
    int i;
    for (i=0;i<15;i++)
    s1[i]=32;
    for (i=0;i<12;i++)
    {
    if (nodes[nodenum].groups & (1<< i))
        s1[i]='A'+i;
    else
        s1[i]=32;
    }
    s1[11]=0;
    sprintf(s,"Node # %d out of %d",nodenum,num_nodes);
    wclear();
    wcenters(0,WHITE|_BLUE,s);
    wprints(3,2,CYAN|_BLUE,"Node Address    : ");
    wprints(3,20,LIGHTCYAN|_BLUE,nodes[nodenum].address);
    wprints(4,2,CYAN|_BLUE,"Node SysOp      : ");
    wprints(4,20,LIGHTCYAN|_BLUE,nodes[nodenum].sysop_name);
    wprints(5,2,CYAN|_BLUE,"Sysop Voice #   : ");
    wprints(5,20,LIGHTCYAN|_BLUE,nodes[nodenum].sysop_phone);
    wprints(6,2,CYAN|_BLUE,"Packet Password : ");
    wprints(6,20,LIGHTCYAN|_BLUE,nodes[nodenum].pkt_pw);
    wprints(7,2,CYAN|_BLUE,"Areafix Password: ");
    wprints(7,20,LIGHTCYAN|_BLUE,nodes[nodenum].areafix_pw);
    wprints(8,2,CYAN|_BLUE,"Allowed Groups  : ");
    wprints(8,20,LIGHTCYAN|_BLUE,s1);
    wprints(9,2,CYAN|_BLUE,"Archive Status  : ");
    switch(nodes[nodenum].archive_status)
    {
        case 0:
            strcpy(s,"NONE");
            break;
        case 1:
            strcpy(s,"HOLD");
            break;
        case 2:
            strcpy(s,"CRASH");
            break;
        default:
            strcpy(s,"NONE");
            nodes[nodenum].archive_status=0;
            break;
    }
    wprints(9,20,LIGHTCYAN|_BLUE,s);
    wprints(10,2,CYAN|_BLUE,"Compression     : ");
    wprints(10,20,LIGHTCYAN|_BLUE,cfg.archive[nodes[nodenum].compression].name);

}
void edit_node(int nodenum)
{
    char s[80],s1[15];
    char ch,ch1;
    char temp[10];
    int esc,i,done;
    FIDOADR fadr;

    sprintf(s,"Edit node # %d",nodenum);
    wclear();
    showcur();
    current_node=nodenum;
    switch(nodes[nodenum].archive_status)
    {
        case 0:
            strcpy(temp_stat,"NONE   ");
            break;
        case 1:
            strcpy(temp_stat,"HOLD   ");
            break;
        case 2:
            strcpy(temp_stat,"CRASH  ");
            break;
        default:
            strcpy(temp_stat,"NONE");
            break;
    }
    wcenters(0,LIGHTRED|_BLUE,s);
    for (i=0;i<15;i++)
        s[i]=32;
        for (i=0;i<12;i++)
        {
            if (nodes[nodenum].groups & (1<< i))
                s1[i]='A'+i;
            else
                s1[i]=32;
        }
        s1[11]=0;
        wprints(8,2,CYAN|_BLUE,"Allowed Groups  : ");
        wprints(8,20,CYAN|_BLUE,s1);

    wprints(10,2,CYAN|_BLUE,"Compression     : ");
    wprints(10,20,CYAN|_BLUE,cfg.archive[nodes[nodenum].compression].name);
    wprints(9,2,CYAN|_BLUE,"Archive Status  : ");
    wprints(9,20,CYAN|_BLUE,temp_stat);
    winpbeg(CYAN|_BLUE,LIGHTCYAN|_BLUE);
    winpdef(3,2,nodes[nodenum].address,"'Node Address    : '??????????????????????????????",'U',2,NULL,0);
    winpfba(show_hint48,clear_hint);
    winpdef(4,2,nodes[nodenum].sysop_name,"'Node SysOp      : '????????????????????????????????????",'0',2,NULL,0);
    winpfba(show_hint49,clear_hint);
    winpdef(5,2,nodes[nodenum].sysop_phone,"'SysOp Voice #   : '????????????????????????????????????",'0',2,NULL,0);
    winpfba(show_hint101,clear_hint);
    winpdef(6,2,nodes[nodenum].pkt_pw, "'Packet Password : '????????????????????",'U',2,NULL,0);
    winpfba(show_hint50,clear_hint);
    winpdef(7,2,nodes[nodenum].areafix_pw, "'Areafix Password: '??????????",'U',2,NULL,0);
    winpfba(show_hint51,clear_hint);
    winpdef(8,2,s1,"'Allowed Groups  : '",'U',2,NULL,0);
    winpdef(9,2,temp_stat,"'Archive Status  : '",'U',2,NULL,0);
    winpdef(10,2,temp,"'Compression     : '",'U',2,NULL,0);
    esc=winpread();
    if (!esc)
    {
        for (i=0;i<12;i++)
        {
            if (nodes[nodenum].groups & (1<< i))
                s1[i]='A'+i;
            else
                s1[i]=32;
        }
        s1[11]=0;
        wprints(8,2,LIGHTCYAN|_BLUE,"Allowed Groups  : ");
        wprints(8,20,LIGHTCYAN|_BLUE,s1);
        done=0;
        do
        {
            wprints(8,32,LIGHTCYAN|_BLUE," ");
            show_hint82();
            wgotoxy(8,32);
            ch=wgetchf("AaBbCcDdEeFfGgHhIiJj\r",0);
            wprints(8,32,LIGHTCYAN|_BLUE," ");
            if (ch!=13)
            {
                ch1=(upcase(ch)-'A');
                nodes[nodenum].groups ^= (1 <<ch1);
                for (i=0;i<12;i++)
                {
                    if (nodes[nodenum].groups & (1<< i))
                        s1[i]='A'+i;
                    else
                        s1[i]=32;
                }
                s1[11]=0;
                wprints(8,20,LIGHTCYAN|_BLUE,s1);
            }
            else
                done=1;
            if (ch==0)
                done=1;
        } while (!done);
        wprints(8,2,CYAN|_BLUE,"Allowed Groups  : ");
        wprints(8,20,CYAN|_BLUE,s1);

        hidecur();
        show_hint52();
        wprints(9,2,LIGHTCYAN|_BLUE,"Archive Status  : ");
        wprints(9,20,LIGHTCYAN|_BLUE,temp_stat);
        choose_status();
        wprints(9,2,CYAN|_BLUE,"Archive Status  : ");
        wprints(9,20,CYAN|_BLUE,temp_stat);
        clear_hint();
        wprints(10,2,LIGHTCYAN|_BLUE,"Compression     : ");
        wprints(10,20,LIGHTCYAN|_BLUE,cfg.archive[nodes[nodenum].compression].name);
        show_hint53();
        choose_archiver();
        wprints(10,2,CYAN|_BLUE,"Compression     : ");
        wprints(20,20,CYAN|_BLUE,cfg.archive[nodes[nodenum].compression].name);
        clear_hint();
    }
    wprints(10,2,CYAN|_BLUE,"Compression     : ");
    wprints(10,20,CYAN|_BLUE,cfg.archive[nodes[nodenum].compression].name);
    fidoadr_split(nodes[nodenum].address,&fadr);
    sprintf(nodes[nodenum].address,"%d:%d/%d.%d",fadr.zone,fadr.net,fadr.node,fadr.point);
    strtrim(nodes[nodenum].sysop_name);
    strtrim(nodes[nodenum].pkt_pw);
    strtrim(nodes[nodenum].areafix_pw);
    strtrim(nodes[nodenum].sysop_phone);
    hidecur();
    show_node_screen(nodenum);
    show_hint58();
}

