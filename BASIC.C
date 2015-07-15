#define _DEFINE_GLOBALS_
#include "vardec.h"
#include "vars.h"
#include "fcns.h"
#include "fidoadr.h"
#include "fidoadr.c"
#define SETREC(f,i)  sh_lseek(f,((long) (i))*((long)sizeof(arearec)),SEEK_SET);



/* MMH mod begins */

void dv_pause(void)
{
  __emit__(0xb8, 0x1a, 0x10, 0xcd, 0x15);
  __emit__(0xb8, 0x00, 0x10, 0xcd, 0x15);
  __emit__(0xb8, 0x25, 0x10, 0xcd, 0x15);
}

void win_pause(void)
{
  __emit__(0x55, 0xb8, 0x80, 0x16, 0xcd, 0x2f, 0x5d);
}

int get_dos_version(void)
{
  _AX = 0x3000;
  geninterrupt(0x21);
  if (_AX % 256 >= 10) {
    multitasker |= MT_OS2;
  }
  return (_AX);
}

int get_dv_version(void)
{
  int v;

  if (multitasker & MT_OS2)
    return 0;
  _AX = 0x2b01;
  _CX = 0x4445;
  _DX = 0x5351;
  geninterrupt(0x21);
  if (_AL == 0xff) {
    return 0;
  } else {
    v = _BX;
    multitasker |= MT_DESQVIEW;
    return v;
  }
}

int get_win_version(void)
{
  int v = 0;

  __emit__(0x55, 0x06, 0x53);
  _AX = 0x352f;
  geninterrupt(0x21);
  _AX = _ES;
  if (_AX | _BX) {
    _AX = 0x1600;
    geninterrupt(0x2f);
    v = _AX;
    if (v % 256 <= 1)
      v = 0;
  }
  __emit__(0x5b, 0x07, 0x5d);
  if (v != 0)
    multitasker |= MT_WINDOWS;
  return (v);
}

int get_nb_version(void)
{
  _AX = 0;
  geninterrupt(0x2A);
  return (_AH);
}

void detect_multitask(void)
{
  get_dos_version();
  get_win_version();
  get_dv_version();
  if (multitasker < 2)
    if (get_nb_version())
      multitasker = MT_NB;
}

void giveup_timeslice(void)
{
  if (multitasker) {
    switch (multitasker) {
 case 1:
 case 3:
        dv_pause();
        break;
      case 2:
      case 4:
      case 5:
      case 6:
      case 7:
        win_pause();
        break;
      default:
        break;
    }
  }
}

/* MMH mod end */






char upcase(char ch)
{
    if ((ch>96) && (ch<123))
        ch=ch-32;
    return(ch);
}

void read_domain_config(void)
{
    int handle;


    handle=open("DOMAINS.DAT",O_RDONLY | O_BINARY);
    if (handle<0)
        exit(1);

    read(handle,(void *) (&domains), sizeof(domains));
    close(handle);
}

void write_domain_config(void)
{
    FILE *fp;

    if ((fp = fopen("DOMAINS.DAT","w"))==NULL)
        exit(1);

    fwrite(domains, sizeof domains,1,fp);
    fclose(fp);
}

void initialize_areas_config(void)
{
    int i;

    strcpy(thisarea.name,"");
    strcpy(thisarea.comment,"");
    thisarea.group=0;
    strcpy(thisarea.origin,"");
    strcpy(thisarea.subtype,"");
    strcpy(thisarea.comment,"");
    strcpy(thisarea.directory,"");
    strcpy(thisarea.res_char,"");
    thisarea.type=1;
    thisarea.alias_ok=0;
    thisarea.pass_color=0;
    thisarea.high_ascii=0;
    thisarea.keep_tag=0;
    thisarea.net_num=0;
    thisarea.group=0;
    thisarea.count=0;
    thisarea.active=1;
    thisarea.translate=0;
    thisarea.val_incoming=0;
    for (i=0;i<MAX_NODES;i++)
    {
        thisarea.feed[i].zone=999;
    }
    i=0;
    write_area(i,&thisarea);
    strcpy(thisarea.name,"AREA_1");
    strcpy(thisarea.comment,"First Area");
    thisarea.group=0;
    strcpy(thisarea.origin,cfg.origin_line);
    strcpy(thisarea.subtype,"1");
    i=1;
    write_area(i,&thisarea);
    cfg.num_areas=1;
}

void write_area(unsigned int un, arearec *a)
{
  char s[81];
  long pos;
  int f, j;

  for (j=0;j<MAX_NODES;j++)
  {
    if (thisarea.feed[j].zone==10)
        thisarea.feed[j].zone=32765;
    if (thisarea.feed[j].net==10)
        thisarea.feed[j].net=32765;
    if (thisarea.feed[j].node==10)
        thisarea.feed[j].node=32765;
    if (thisarea.feed[j].point==10)
        thisarea.feed[j].point=32765;
  }

  strcpy(s,"AREAS.DAT");
  f=sh_open(s,O_RDWR | O_BINARY | O_CREAT, S_IREAD | S_IWRITE);
  if (f>=0) {
    pos=((long) sizeof(arearec)) * ((long) un);
    sh_lseek(f,pos,SEEK_SET);
    sh_write(f, (void *)a, sizeof(arearec));
    sh_close(f);
  }
}
void read_area(unsigned int un, arearec *a)
{
  char s[81];
  long pos;
  int f, nu, j;

    strcpy(s,"AREAS.DAT");
  f=sh_open1(s,O_RDONLY | O_BINARY);
  if (f<0) {
    return;
  }

  nu=((int) (filelength(f)/sizeof(arearec))-1);

  if (un>nu) {
    sh_close(f);
    return;
  }
  pos=((long) sizeof(arearec)) * ((long) un);
  sh_lseek(f,pos,SEEK_SET);
  sh_read(f, (void *)a, sizeof(arearec));
  sh_close(f);
  for (j=0;j<MAX_NODES;j++)
  {
      if (thisarea.feed[j].zone==32765)
          thisarea.feed[j].zone=10;
      if (thisarea.feed[j].net==32765)
          thisarea.feed[j].net=10;
      if (thisarea.feed[j].node==32765)
          thisarea.feed[j].node=10;
      if (thisarea.feed[j].point==32765)
          thisarea.feed[j].point=10;
  }

}


void regd(void)
{
	char s[80];
    strcpy(s,"Registered!  Thank You!!");
    wopen(5,26,7,54,1,YELLOW|_RED,YELLOW|_RED);
	wshadow(LGREY|_BLACK);
	wcenters(0,BLINK|YELLOW|_RED,s);
    if (!quiet)
        beep();
    delay(500);
	wclose();

}
void setup_archivers(void)
{
    strcpy(cfg.archive[0].name,"PKT");
    strcpy(cfg.archive[1].name,"ARC");
    strcpy(cfg.archive[2].name,"ARJ");
    strcpy(cfg.archive[3].name,"LZH");
    strcpy(cfg.archive[4].name,"PAK");
    strcpy(cfg.archive[5].name,"ZIP");
    strcpy(cfg.archive[6].name,"ZOO");
    strcpy(cfg.archive[7].name,"SQZ");
    strcpy(cfg.archive[8].name,"RAR");
    strcpy(cfg.archive[0].archive_file,"");
    strcpy(cfg.archive[0].unarchive_file,"");
    strcpy(cfg.archive[1].archive_file,"PKPAK ");
    strcpy(cfg.archive[1].unarchive_file,"ARCE ");
    strcpy(cfg.archive[2].archive_file,"ARJ ");
    strcpy(cfg.archive[2].unarchive_file,"ARJ ");
    strcpy(cfg.archive[3].archive_file,"LHA ");
    strcpy(cfg.archive[3].unarchive_file,"LHA ");
    strcpy(cfg.archive[4].archive_file,"PAK ");
    strcpy(cfg.archive[4].unarchive_file,"PAK");
    strcpy(cfg.archive[5].archive_file,"PKZIP");
    strcpy(cfg.archive[5].unarchive_file,"PKUNZIP");
    strcpy(cfg.archive[6].archive_file,"ZOO ");
    strcpy(cfg.archive[6].unarchive_file,"ZOO ");
    strcpy(cfg.archive[7].archive_file,"SQZ ");
    strcpy(cfg.archive[7].unarchive_file,"SQZ ");
    strcpy(cfg.archive[8].archive_file,"RAR ");
    strcpy(cfg.archive[8].unarchive_file,"RAR ");
    strcpy(cfg.archive[1].archive_switches,"-a ");
    strcpy(cfg.archive[1].unarchive_switches,"");
    strcpy(cfg.archive[2].archive_switches,"a -e -m1 -s -t0 -y ");
    strcpy(cfg.archive[2].unarchive_switches,"e -c -y ");
    strcpy(cfg.archive[3].archive_switches,"a /mt ");
    strcpy(cfg.archive[3].unarchive_switches,"e /cm ");
    strcpy(cfg.archive[4].archive_switches,"a /l /st ");
    strcpy(cfg.archive[4].unarchive_switches,"e /wa");
    strcpy(cfg.archive[5].archive_switches,"-ao ");
    strcpy(cfg.archive[5].unarchive_switches,"-o -ed ");
    strcpy(cfg.archive[6].archive_switches,"a: ");
    strcpy(cfg.archive[6].unarchive_switches,"eO ");
    strcpy(cfg.archive[7].archive_switches,"a /q0 /z3 ");
    strcpy(cfg.archive[7].unarchive_switches,"e /o1 ");
    strcpy(cfg.archive[8].archive_switches,"a -y ");
    strcpy(cfg.archive[8].unarchive_switches,"e -o+ ");
}
void check_reg(void)
{
    strtrim(cfg.registration);
    strtrim(cfg.bbs_name);
    reg_key=atol(cfg.registration);
    correct_key=bp(cfg.bbs_name,KEYCODE);
    if (correct_key==reg_key)
    {
        cfg.registered=1;
        regd();
    }
    else
    {
        cfg.registered=0;
        non_reg();
    }
    registered=cfg.registered;
}
void initialize_nodes_config(void)
{
    FILE *fp;

    strcpy(nodes[1].address,"1:376/126.0");
    strcpy(nodes[1].sysop_name,"Craig Dooley");
    strcpy(nodes[1].pkt_pw,"");
    nodes[1].compression=0;
    nodes[1].groups=0;
    nodes[1].allow_areafix=0;
    strcpy(nodes[1].areafix_pw,"");
    nodes[1].archive_status=0;
    nodes[1].direct=0;

    if ((fp = fopen("NODES.DAT","w"))==NULL)
        exit(1);

    fwrite(nodes, sizeof nodes,1,fp);
    fclose(fp);
    cfg.num_nodes=1;
}
void initialize_uplink_config(void)
{
    FILE *fp;

    strcpy(uplink[1].address,"");
    strcpy(uplink[1].areafix_prog,"");
    strcpy(uplink[1].areafix_pw,"");
    strcpy(uplink[1].areas_filename,"");
    strcpy(uplink[1].origin_address,"");
    uplink[1].add_plus=0;
    uplink[1].unconditional=0;
    uplink[1].areas_file_type=0;

    if ((fp = fopen("UPLINKS.DAT","w"))==NULL)
        exit(1);

    fwrite(uplink, sizeof uplink,1,fp);
    fclose(fp);
    cfg.num_uplinks=1;
    num_uplink=1;
}

void initialize_domains_config(void)
{
    FILE *fp;

    strcpy(domains[0].domain,"Fidonet");
    if ((fp = fopen("DOMAINS.DAT","w"))==NULL)
        exit(1);

    fwrite(domains, sizeof domains,1,fp);
    fclose(fp);
}
void initialize_xlat_dat(void)
{
    unsigned int i;
    FILE *fp;

    for (i=0;i<222;i++)
    {
        xlat[i].in=i+33;
        xlat[i].out=i+33;
    }

    if ((fp = fopen("XLAT.DAT","w"))==NULL)
        exit(1);

    fwrite(xlat, sizeof xlat,1,fp);
    fclose(fp);
}
void initialize_wwivtoss_config(void)
{
    int i;
    FILE *fp;
    time_t t;

    t=time(NULL);
    for (i=0;i<12;i++)
    {
        cfg.aka_list[i].zone=0;
        cfg.aka_list[i].net=0;
        cfg.aka_list[i].node=0;
        cfg.aka_list[i].point=0;
        cfg.aka_list[i].wwiv_node=0;
        cfg.aka_list[i].wwiv_netnum=0;
    }
    setup_archivers();
    sprintf(cfg.origin_line,"WWIVTOSS V. %s Default Origin Line",VERSION);
    cfg.notify=0;
    cfg.installed=0;
    cfg.date_installed=t;
    cfg.crash=cfg.direct=cfg.center=cfg.delete_sent=cfg.import_recd=cfg.use_areafix=cfg.skip_mail=cfg.default_compression=cfg.registered=cfg.mailer=cfg.installed=cfg.cleanup=cfg.kill_recd=cfg.auto_add=cfg.log_feeds=0;
    cfg.add_hard_cr=cfg.add_line_feed=1;
    cfg.high_ascii=1;
    cfg.soft_to_hard=cfg.hard_to_soft=cfg.add_soft_cr=0;
    cfg.route_me=1;
    cfg.log_days=0;
    cfg.color_to=0;
    cfg.initial_quote=0;
    if ((fp = fopen("WWIVTOSS.DAT","w"))==NULL)
        exit(1);

    fwrite((void *) (&cfg), sizeof (tosser_config),1,fp);
    fclose(fp);
    strcpy(domains[0].domain,"Fidonet");
    if ((fp = fopen("DOMAINS.DAT","w"))==NULL)
        exit(1);

    fwrite(domains, sizeof domains,1,fp);
    fclose(fp);

}

void read_config(void)
{
    int configfile,f,nu;
    char s[80];
    time_t t;

    t=time(NULL);

    configfile=open("WWIVTOSS.DAT",O_RDWR | O_BINARY | O_CREAT);
    if (configfile<0)
    {
          configfile=open("WWIVTOSS.DAT",O_WRONLY | O_BINARY | O_CREAT);
          if (configfile<0)
          {
              printf("Error opening WWIVTOSS.DAT!\r\n");
              exit(1);
          }
          write(configfile,(void *) (&cfg), sizeof (tosser_config));
          close(configfile);
          configfile=open("WWIVTOSS.DAT",O_RDWR | O_BINARY | O_CREAT);

    }
    read(configfile,(void *) (&cfg), sizeof (tosser_config));
    close(configfile);
    if (!cfg.date_installed)
        cfg.date_installed=t;
    strcpy(s,"AREAS.DAT");
      f=sh_open1(s,O_RDONLY | O_BINARY);
      if (f<0) {
        return;
      }

      nu=((int) (filelength(f)/sizeof(arearec))-1);
      sh_close(f);
      cfg.num_areas=num_areas=nu;

}
void write_config(void)
{
    int configfile;

    configfile=open("WWIVTOSS.DAT",O_RDWR | O_BINARY | O_CREAT);
    if (configfile<0)
    {
        printf("Error opening WWIVTOSS.DAT!\r\n");
        exit(1);
    }
    write(configfile,(void *) (&cfg), sizeof (tosser_config));
    close(configfile);

}


void main_screen(void)
{
    char s1[80];

	clrscrn();
	window_1=wopen(0,0,24,80,5,BLUE|_BLUE,WHITE|_BLUE);
	wfill(1,0,23,79,'²',WHITE|_BLUE);
    window_2=wopen(0,0,4,79,1,LIGHTCYAN|_BLUE,LIGHTGREEN|_BLUE);

    sprintf(s1,"%s! Version %s - Written by Craig Dooley - (c) 1996-1997",PROGRAM,VERSION);
    wcenters(0,WHITE|_BLUE,s1);

    sprintf(s1,"%s",cfg.bbs_name);
    wcenters(1,LIGHTRED|_BLUE,s1);
    wcenters(2,LIGHTGREEN|_BLUE,cfg.system_address);
}

void update(void)
{
    if (registered)
    {
        wwprints(window_2,2,1,LIGHTCYAN|_BLUE,"REGISTERED  ");
        wwprints(window_2,2,65,LIGHTCYAN|_BLUE,"  THANK YOU!");
    }
    if (!registered)
    {
        wwprints(window_2,2,1,BLINK|LIGHTRED|_BLUE,"UNREGISTERED");
        wwprints(window_2,2,65,BLINK|LIGHTRED|_BLUE,"UNREGISTERED");
    }

}
void non_reg(void)
{
    char s[80];

	strcpy(s,"This Version is UNREGISTERED!");
    wopen(5,20,7,60,1,YELLOW|_RED,YELLOW|_RED);
	wshadow(LGREY|_BLACK);
	wcenters(0,BLINK|YELLOW|_RED,s);
    if (!quiet)
        beep();
	sleep(1);
    if (!quiet)
        beep();
	wclose();
}
void DoShadow(void)
{
    wshadow(LGREY|_BLACK);
}

void main (int argc, char *argv[])
{
    char s[80];
    FILE *fp;

    path_ptr=getenv("PATH");
    setonkey(0x3B00,help_message,0);
    setonkey(0x5D00,dos_shell,0);
//    whelpdef("WWIVTOSS.HLP",0x3B00,BLACK|_LGREY,WHITE|_LGREY,LIGHTRED|_LGREY,BLACK|_GREEN,DoShadow);
    quiet=0;
    if (argc>1)
        if (stricmp(argv[1],"/Q")==0)
            quiet=1;

    if ((fp = fopen("AREAS.DAT","r"))==NULL)
        initialize_areas_config();
    else
        fclose(fp);
    if ((fp = fopen("XLAT.DAT","r"))==NULL)
        initialize_xlat_dat();
    else
        fclose(fp);
    if ((fp = fopen("WWIVTOSS.DAT","r"))==NULL)
        initialize_wwivtoss_config();
    else
        fclose(fp);
    if ((fp = fopen("NODES.DAT","r"))==NULL)
        initialize_nodes_config();
    else
        fclose(fp);
    if ((fp = fopen("DOMAINS.DAT","r"))==NULL)
        initialize_domains_config();
    else
        fclose(fp);
    if ((fp = fopen("UPLINKS.DAT","r"))==NULL)
        initialize_uplink_config();
    else
        fclose(fp);

    read_config();

//    read_areas_config();
    read_nodes_config();
    read_domain_config();
    read_uplink_config();
    if (cfg.password[0]!=0)
    {
        wopen(8,20,12,60,1,WHITE|_GREEN,WHITE|_GREEN);
        wcenters(0,WHITE|_GREEN,"Please Enter WSETUP Password");
        wgotoxy(1,10);
        wgets(s);
        if (strcmp(s,cfg.password)!=0)
        {
            wperror("Error!  Incorrect Password!");
            wcloseall();
            clrscr();
            exit(1);
        }
        wclose();
    }
    clrscr();
	giveup_timeslice();
    hidecur();
	giveup_timeslice();
    main_screen();
    check_reg();
    update();
    main_menu();
	giveup_timeslice();
    write_nodes_config();
    write_domain_config();
    write_uplink_config();
    write_config();
    wcloseall();
	giveup_timeslice();
    showcur();
    exit(0);
}
void read_nodes_config(void)
{
    FILE *fp;

    if ((fp = fopen("NODES.DAT","r"))==NULL)
        exit(1);

    fread(nodes, sizeof nodes,1,fp);
    fclose(fp);
    num_nodes=cfg.num_nodes;
}
void read_uplink_config(void)
{
    FILE *fp;

    if ((fp = fopen("UPLINKS.DAT","r"))==NULL)
        exit(1);

    fread(uplink, sizeof uplink,1,fp);
    fclose(fp);
    num_uplink=cfg.num_uplinks;
    if (num_uplink==0)
        cfg.num_uplinks=num_uplink=1;
}
void write_nodes_config(void)
{
    FILE *fp;

    if ((fp = fopen("NODES.DAT","w"))==NULL)
	exit(1);

    fwrite(nodes, sizeof nodes,1,fp);
    fclose(fp);
    cfg.num_nodes=num_nodes;
}
void write_uplink_config(void)
{
    FILE *fp;

    if ((fp = fopen("UPLINKS.DAT","w"))==NULL)
        exit(1);

    fwrite(uplink, sizeof uplink,1,fp);
    fclose(fp);
    cfg.num_uplinks=num_uplink;
}
void site_info(void)
{
    int i;
    char temp_origin[80];

    strcpy(temp_origin,cfg.origin_line);
    wopen(6,3,15,76,1,YELLOW|_BLUE,WHITE|_BLUE);
    DoShadow();
    wmessage(" F1 For Editing Help ",1,7,LIGHTRED|_BLUE);
    wcenters(0,LIGHTRED|_BLUE,"System Data");
    showcur();
    winpbeg(CYAN|_BLUE,LIGHTCYAN|_BLUE);
    winpdef(1,2,cfg.bbs_name,"'BBS Name     : '????????????????????????????????????",'0',2,NULL,0);
    winpfba(show_hint12,clear_hint);
    winpdef(2,2,cfg.sysop_name,"'Sysop Name   : '????????????????????????????????????",'M',2,NULL,0);
    winpfba(show_hint13,clear_hint);
    winpdef(3,2,cfg.location,"'Location     : '???????????????????????????????????",'0',2,NULL,0);
    winpfba(show_hint14,clear_hint);
    winpdef(4,2,cfg.password,"'Password     : '???????????????",'0',2,NULL,0);
    winpfba(show_hint15,clear_hint);
    winpdef(5,2,cfg.origin_line,"'Origin Line  : '?????????????????????????????????????????????????",'0',2,NULL,0);
    winpfba(show_hint16,clear_hint);
    winpdef(6,2,cfg.log_file,"'LogFile      : '?????????????????????????????????????????????",'U',2,NULL,0);
    winpfba(show_hint17,clear_hint);
    winpdef(7,2,cfg.registration,"'Regis. Key   : '??????????????????????????????",'0',2,NULL,0);
    winpfba(show_hint18,clear_hint);
    winpread();
    hidecur();
    wclose();
    strtrim(cfg.origin_line);
    strtrim(cfg.bbs_name);
    strtrim(cfg.sysop_name);
    strtrim(cfg.location);
    strtrim(cfg.password);
    strtrim(cfg.log_file);
    strtrim(cfg.registration);
    if (strcmp(temp_origin,cfg.origin_line)!=0)
    {
        for (i=0;i<=num_areas;i++)
        {
            read_area(i,&thisarea);

            if (thisarea.def_origin)
                strcpy(thisarea.origin,cfg.origin_line);
            write_area(i,&thisarea);
        }
    }
    check_reg();
    if (!registered)
    {
        wprints(7,17,CYAN|_BLUE,"UNREGISTERED");
        strcpy(cfg.registration,"UNREGISTERED");
    }
    update();
    show_hint6();
}
void show_archivers2(void)
{
    int i,pos;
    char s[15];
    wclear();
    wcenters(0,WHITE|_BLUE,"Archivers");
    for (i=0;i<=NUM_ARCHIVERS;i++)
    {
        sprintf(s," %-2d   %-4s",i,cfg.archive[i].name);
        pos=i+2;
        wprints(pos,5,LIGHTCYAN|_BLUE,s);
    }
    wprints(cfg.default_compression+2,4,LIGHTRED|_BLUE,"þ");
}
void edit_archiver(int i)
{
    char s[80];
    if (i==0)
    {
        wperror("Hey!  .PKT Files aren't compressed, silly!");
        return;
    }
    wopen(8,12,17,65,1,LIGHTGREEN|_BLUE,WHITE|_BLUE);
    DoShadow();
    wmessage(" F1 For Editing Help ",1,7,LIGHTRED|_BLUE);
    sprintf(s,"%s Set-up",cfg.archive[i].name);
    wcenters(0,LIGHTGREEN|_BLUE,s);
    showcur();
    winpbeg(CYAN|_BLUE,LIGHTCYAN|_BLUE);
    winpdef(2,2,cfg.archive[i].archive_file,"'Archive Filename   : '?????????????",'U',2,NULL,0);
    winpfba(show_hint25,clear_hint);
    winpdef(3,2,cfg.archive[i].archive_switches,"'Archive Switches   : '?????????????????????????",'0',2,NULL,0);
    winpfba(show_hint26,clear_hint);
    winpdef(5,2,cfg.archive[i].unarchive_file,"'Unarchive Filename : '?????????????",'U',2,NULL,0);
    winpfba(show_hint27,clear_hint);
    winpdef(6,2,cfg.archive[i].unarchive_switches,"'Unarchive Switches : '?????????????????????????",'0',2,NULL,0);
    winpfba(show_hint28,clear_hint);
    winpread();
    strtrim(cfg.archive[i].archive_file);
    strtrim(cfg.archive[i].archive_switches);
    strtrim(cfg.archive[i].unarchive_file);
    strtrim(cfg.archive[i].unarchive_switches);
    hidecur();
    wclose();
    clear_hint();
    show_hint58();

}

void archiver_setup(void)
{
    int done,keypress,i,current_arc,last_default;
    int last_pos,curr_pos,last_num;
    char s[35];
    unsigned int keycode;
    done=i=0;
    wopen(6,35,20,55,1,YELLOW|_BLUE,WHITE|_BLUE);
    DoShadow();
    show_hint58();

    show_archivers2();
    wcenters(12,LIGHTRED|_BLUE,"þ = Default");
    i=0;
    last_pos=2;
    curr_pos=2;
    last_num=1;
    do
    {
        last_pos=curr_pos;
        curr_pos=i+2;
        sprintf(s," %-2d   %-4s",last_num,cfg.archive[last_num].name);

        last_num=i;
        wprints(last_pos,5,LIGHTCYAN|_BLUE,s);
        sprintf(s," %-2d   %-4s",i,cfg.archive[i].name);

        wprints(curr_pos,5,WHITE|_GREEN,s);
        wprints(last_default+2,4,LIGHTRED|_BLUE," ");
        wprints(cfg.default_compression+2,4,LIGHTRED|_BLUE,"þ");

        keycode=getxch();
        keypress=keycode>>8;
        switch (keypress)
        {
            case 1: /* ESC */
                done=1;
                break;
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
                i=keypress-1;
            case 28: /* Enter */
                current_arc=i;
                edit_archiver(i);
                i=current_arc;
                break;
            case 57: /* Space */
                last_default=cfg.default_compression;
                cfg.default_compression=i;
                break;
            case 71: /* Home */
                i=0;
                break;
            case 72: /* Up Arrow */
                i--;
                if (i<0)
                    i=NUM_ARCHIVERS;
                break;
            case 79: /* End */
                i=NUM_ARCHIVERS;
                break;
            case 80: /* Down Arrow */
                i++;
                if (i>NUM_ARCHIVERS)
                    i=0;
                break;
        }

    } while (!done);

    wclose();
    clear_hint();
    show_hint9();
}

void show_groups(void)
{
    int j;
    char s[80];

    wclear();
    wcenters(0,WHITE|_BLUE,"Area Group Names");
    for (j=0;j<10;j++)
    {
        sprintf(cfg.group[j].group_id,"%c",j+65);
        sprintf(s,"%-2s - %s",cfg.group[j].group_id,cfg.group[j].group_name);
        wprints(j+2,5,YELLOW|_BLUE,s);
    }
}
void edit_group(int i)
{
    char s[80];
    sprintf(s,"%-2s - °°°°°°°°°°°°",cfg.group[i].group_id);
    showcur();
    wprints(i+2,5,LIGHTCYAN|_BLUE,s);
    winpbeg(CYAN|_BLUE,LIGHTCYAN|_BLUE);
    winpdef(i+2,10,cfg.group[i].group_name,"???????????????",'0',2,NULL,0);
    winpread();
    strtrim(cfg.group[i].group_name);
    hidecur();

}
void group_setup(void)
{
    int done,keypress,i,current_num;
    int last_pos,curr_pos,last_num;
    char s[85];
    unsigned int keycode;
    done=i=0;
    last_pos=2;
    curr_pos=2;
    last_num=0;

    hidecur();
    wopen(6,15,20,48,1,YELLOW|_BLUE,WHITE|_BLUE);
    DoShadow();
    show_hint58();
    show_groups();
    i=0;
    do
    {
        last_pos=curr_pos;
        curr_pos=i+2;
        sprintf(s,"%-2s - %-10s",cfg.group[last_num].group_id,cfg.group[last_num].group_name);
        last_num=i;
        wprints(last_pos,5,YELLOW|_BLUE,s);
        sprintf(s,"%-2s - %-10s",cfg.group[i].group_id,cfg.group[i].group_name);
        last_num=i;
        wprints(curr_pos,5,WHITE|_GREEN,s);
        keycode=getxch();
        keypress=keycode>>8;
        switch (keypress)
        {
            case 1: /* ESC */
                done=1;
                break;
            case 28: /* Enter */
                current_num=i;
                edit_group(i);
                show_groups();
                i=current_num;
                break;
            case 71: /* Home */
                i=0;
                break;
            case 72: /* Up Arrow */
                i--;
                if (i<0)
                    i=9;
                break;
            case 79: /* End */
                i=10;
                break;
            case 80: /* Down Arrow */
                i++;
                if (i>=10)
                    i=0;
                break;
        }

    } while (!done);

    wclose();
    show_hint10();
}
void uplink_mgr(void)
{
    uplink_setup();
}
void about(void)
{
    char s[80];

    wopen(6,20,17,60,2,WHITE|_LGREY,LIGHTGREEN|_LGREY);
    wshadow(LGREY|_BLACK);
    wtitle(" About WWIVTOSS ",TCENTER,BLACK|_LGREY);
    wcenters(1,BLUE|_LGREY,"WWIVTOSS");
    sprintf(s,"Version: %s",VERSION);
    wcenters(2,BLUE|_LGREY,s);
    wcenters(3,BLUE|_LGREY,"Written By Craig Dooley");
    wcenters(4,BLUE|_LGREY,"(c) 1996-97 Craig Dooley");
    wcenters(5,BLUE|_LGREY,"All Rights Reserved");
    wcenters(6,BLUE|_LGREY,"Compiled with Borland C++ 3.1");
    sprintf(s,"Using TCXL Library Version  %s",cxlver());
    wcenters(7,BLUE|_LGREY,s);
    sprintf(s,"Compiled On %s",__DATE__);
    wcenters(8,BLUE|_LGREY,s);
    wmessage(" Press a key to continue! ", 1, 7,LIGHTCYAN|_LGREY);
	getch();
    wclose();

}
void xlat_in(void)
{
    edit_xlat(1);
}
void xlat_out(void)
{
    edit_xlat(2);
}
void main_menu(void)
{

    wmenubeg(7,32,17,49,1,LIGHTRED|_BLUE,LCYAN|_BLUE,DoShadow);
    wmenuitem(0,0,"  System  Data  ",'S',1,0,NULL,0,1);
    wmenuiba(show_hint1,clear_hint);
        wmenubeg(9,37,16,56,1,CYAN|_BLUE,WHITE|_BLUE,DoShadow);
            wmenuitem(0,0,"     Site Info     ",'S',6,0,site_info,0,6);
            wmenuiba(show_hint6,clear_hint);
            wmenuitem(1,0," Network Addresses ",'N',7,0,aka_choose,0,7);
            wmenuiba(show_hint7,clear_hint);
            wmenuitem(2,0,"    Path  Names    ",'P',8,0,path_setup,0,8);
            wmenuiba(show_hint8,clear_hint);
            wmenuitem(3,0,"  Archiver Set-Up  ",'A',9,0,archiver_setup,0,9);
            wmenuiba(show_hint9,clear_hint);
            wmenuitem(4,0,"    Area Groups    ",'G',10,0,group_setup,0,10);
            wmenuiba(show_hint10,clear_hint);
            wmenuitem(5,0,"  Area Uplink Mgr  ",'U',11,0,uplink_mgr,0,11);
            wmenuiba(show_hint11,clear_hint);
        wmenuend(6,M_VERT,0,0,YELLOW|_BLUE,LIGHTRED|_BLUE,LCYAN|_BLUE,WHITE|_RED);
		giveup_timeslice();
	wmenuget();
	giveup_timeslice();
    clear_hint();
	show_hint1();
    wmenuitem(1,0," Misc.  Options ",'M',2,0,misc_setup,0,2);
    wmenuiba(show_hint2,clear_hint);
    wmenuitem(2,0,"  Area Manager  ",'A',3,0,area_setup,0,3);
    wmenuiba(show_hint3,clear_hint);
    wmenuitem(3,0,"  Node Manager  ",'N',4,0,node_setup,0,4);
    wmenuiba(show_hint4,clear_hint);
    wmenuitem(4,0," Route  Manager ",'R',5,0,route_setup,0,5);
    wmenuiba(show_hint5,clear_hint);
    wmenuitem(5,0," Transl. Tables ",'T',34,0,NULL,0,34);
    wmenuiba(show_hint78,clear_hint);
        wmenubeg(13,37,16,53,1,CYAN|_BLUE,WHITE|_BLUE,DoShadow);
            wmenuitem(0,0," Translate  In ",'I',35,0,xlat_in,0,0);
            wmenuiba(show_hint79,clear_hint);
            wmenuitem(1,0," Translate Out ",'O',36,0,xlat_out,0,0);
            wmenuiba(show_hint80,clear_hint);
        wmenuend(35,M_VERT,0,0,YELLOW|_BLUE,LIGHTRED|_BLUE,LCYAN|_BLUE,WHITE|_RED);
    wmenuget();
    wmenuitem(6,0," Export Manager ",'E',17,0,export_mgr,0,17);
    wmenuiba(show_hint62,clear_hint);
    wmenuitem(7,0," Make SUBS.LSTs ",'L',37,0,create_subs_list,0,37);
    wmenuiba(show_hint93,clear_hint);
    wmenuitem(8,0," About WWIVTOSS ",'W',16,0,about,0,16);
    wmenuiba(show_hint60,clear_hint);
    wmenuend(1,M_VERT,0,0,YELLOW|_BLUE,LIGHTRED|_BLUE,LCYAN|_BLUE,WHITE|_RED);
    giveup_timeslice();
	wmenuget();
	giveup_timeslice();
}
void path_setup(void)
{
    char temp[80],ch,s[80];
    char current_dir[161];
    int length;
    getcwd(current_dir,160);

    wopen(6,5,14,74,1,YELLOW|_BLUE,WHITE|_BLUE);
    DoShadow();
    wmessage(" F1 For Editing Help ",1,7,LIGHTRED|_BLUE);
    showcur();
    wcenters(0,LIGHTRED|_BLUE,"Path Setup");
    winpbeg(CYAN|_BLUE,LIGHTCYAN|_BLUE);
    winpdef(1,2,cfg.inbound_path,"'Inbound Pkts  : '?????????????????????????????????????????????",'U',2,NULL,0);
    winpfba(show_hint19,clear_hint);
    winpdef(2,2,cfg.outbound_path,"'Outbound Pkts : '?????????????????????????????????????????????",'U',2,NULL,0);
    winpfba(show_hint20,clear_hint);
    winpdef(3,2,cfg.outbound_temp_path,"'Temp Outbound : '?????????????????????????????????????????????",'U',2,NULL,0);
    winpfba(show_hint21,clear_hint);
    winpdef(4,2,cfg.netmail_path,"'NetMail Path  : '?????????????????????????????????????????????",'U',2,NULL,0);
    winpfba(show_hint22,clear_hint);
    winpdef(5,2,cfg.temp_path,"'Temp Path     : '?????????????????????????????????????????????",'U',2,NULL,0);
    winpfba(show_hint23,clear_hint);
    winpdef(6,2,cfg.badecho_path,"'BadEcho Path  : '?????????????????????????????????????????????",'U',2,NULL,0);
    winpfba(show_hint24,clear_hint);

    winpread();
    hidecur();
    wclose();
    strtrim(cfg.inbound_path);
    strtrim(cfg.outbound_path);
    strtrim(cfg.outbound_temp_path);
    strtrim(cfg.netmail_path);
    strtrim(cfg.temp_path);
    strtrim(cfg.badecho_path);
    length=strlen(cfg.inbound_path);
    if (cfg.inbound_path[length-1]!='\\')
        strcat(cfg.inbound_path,"\\");
    length=strlen(cfg.outbound_path);
    if (cfg.outbound_path[length-1]!='\\')
        strcat(cfg.outbound_path,"\\");
    length=strlen(cfg.outbound_temp_path);
    if (cfg.outbound_temp_path[length-1]!='\\')
        strcat(cfg.outbound_temp_path,"\\");
    length=strlen(cfg.netmail_path);
    if (cfg.netmail_path[length-1]!='\\')
        strcat(cfg.netmail_path,"\\");
    length=strlen(cfg.temp_path);
    if (cfg.temp_path[length-1]!='\\')
        strcat(cfg.temp_path,"\\");
    length=strlen(cfg.badecho_path);
    if (cfg.badecho_path[length-1]!='\\')
        strcat(cfg.badecho_path,"\\");
    strcpy(temp,cfg.inbound_path);
    length=strlen(temp);
    temp[length-1]=0;
    if (chdir(temp))
    {
        wopen(10,10,12,70,1,YELLOW|_RED,WHITE|_RED);
        DoShadow();
        if (!quiet)
            beep();
        wcenters(0,WHITE|_RED,"Inbound Path Does Not Exist!  Shall I Create It?");
        wgotoxy(0,58);
        ch=wgetchf("YyNn",'N');
        if (upcase(ch)=='Y')
        {
            if (mkdir(temp))
            {
                sprintf(s,"Couldn't Create [%s]",temp);
                wperror(s);
            }
        }
        wclose();
    }
    strcpy(temp,cfg.outbound_path);
    length=strlen(temp);
    temp[length-1]=0;
    if (chdir(temp))
    {
        wopen(10,10,12,70,1,YELLOW|_RED,WHITE|_RED);
        DoShadow();
        if (!quiet)
           beep();
        wcenters(0,WHITE|_RED,"Outbound Path Does Not Exist!  Shall I Create It?");
        wgotoxy(0,58);
        ch=wgetchf("YyNn",'N');
        if (upcase(ch)=='Y')
        {
            if (mkdir(temp))
                wperror("Error:  Couldn't Create Outbound Path!");
        }
        wclose();
    }
    strcpy(temp,cfg.outbound_temp_path);
    length=strlen(temp);
    temp[length-1]=0;
    if (chdir(temp))
    {
        wopen(10,10,12,70,1,YELLOW|_RED,WHITE|_RED);
        DoShadow();
        if (!quiet)
            beep();
        wcenters(0,WHITE|_RED,"Outbound Temp Path Does Not Exist!  Shall I Create It?");
        wgotoxy(0,58);
        ch=wgetchf("YyNn",'N');
        if (upcase(ch)=='Y')
        {
            if (mkdir(temp))
                wperror("Error:  Couldn't Create Outbound Temp Path!");
        }
        wclose();
    }
    strcpy(temp,cfg.netmail_path);
    length=strlen(temp);
    temp[length-1]=0;
    if (chdir(temp))
    {
        wopen(10,10,12,70,1,YELLOW|_RED,WHITE|_RED);
        DoShadow();
        wcenters(0,WHITE|_RED,"NetMail Path Does Not Exist!  Shall I Create It?");
        wgotoxy(0,58);
        ch=wgetchf("YyNn",'N');
        if (upcase(ch)=='Y')
        {
            if (mkdir(temp))
                wperror("Error:  Couldn't Create NetMail Path!");
        }
        wclose();
    }
    strcpy(temp,cfg.temp_path);
    length=strlen(temp);
    temp[length-1]=0;
    if (chdir(temp))
    {
        wopen(10,10,12,70,1,YELLOW|_RED,WHITE|_RED);
        DoShadow();
        if (!quiet)
            beep();
        wcenters(0,WHITE|_RED,"Temporary Path Does Not Exist!  Shall I Create It?");
        wgotoxy(0,58);
        ch=wgetchf("YyNn",'N');
        if (upcase(ch)=='Y')
        {
            if (mkdir(temp))
                wperror("Error:  Couldn't Create Temporary Path!");
        }
        wclose();
    }
    strcpy(temp,cfg.badecho_path);
    length=strlen(temp);
    temp[length-1]=0;
    if (chdir(temp))
    {
        wopen(10,10,12,70,1,YELLOW|_RED,WHITE|_RED);
        DoShadow();
        if (!quiet)
            beep();
        wcenters(0,WHITE|_RED,"Bad Echo Path Does Not Exist!  Shall I Create It?");
        wgotoxy(0,58);
        ch=wgetchf("YyNn",'N');
        if (upcase(ch)=='Y')
        {
            if (mkdir(temp))
                wperror("Error:  Couldn't Create Bad Echo Path!");
        }
        wclose();
    }
    chdir(current_dir);
    show_hint8();
}
void show_mailer(void)
{
    char s[80];
    wprints(11,2,LIGHTCYAN|_BLUE,"Mailer Type  : ");
    switch(cfg.mailer)
    {
        case 0:
            strcpy(s,"FrontDoor  ");
            break;
        case 1:
            strcpy(s,"Intermail  ");
            break;
        case 2:
            strcpy(s,"BinkleyTerm");
            break;
	case 3:
	    strcpy(s,"Portal     ");
	    break;
	case 4:
	    strcpy(s,"D'Bridge   ");
	    break;
	default:
	    strcpy(s,"FrontDoor  ");
	    cfg.mailer=0;
            break;
    }
    wprints(11,17,LIGHTCYAN|_BLUE,s);
}
void choose_mailer(void)
{
    char s[80];
    char ch;
    int i,i1;

    show_mailer();
    i=i1=cfg.mailer;
    ch=0;
    while ((ch!=13) && (ch!=27))
    {
        show_mailer();
        ch=getch();
        if (ch==32)
        {
            i++;
            if (i>4)
               i=0;
            cfg.mailer=i;
        }
    }
    if (ch==27)
        cfg.mailer=i1;
    show_mailer();
    wprints(11,2,CYAN|_BLUE,"Mailer Type  : ");
    wprints(11,17,CYAN|_BLUE,s);

}
void misc_setup(void)
{
    int esc;
    char s[80];
    char temp[20],temp1[20],temp2[20],temp3[20],temp4[20],temp5[20],temp6[20],temp7[20];
    char temp8[20],temp9[20],temp10[20],temp18[20],temp19[20],temp20[10],temp21[10],temp22[10];
    char temp11[20],temp12[20],temp13[20],temp14[20],temp15[20],temp16[20],temp17[20],temp23[10];
    wopen(6,11,19,68,1,YELLOW|_BLUE,WHITE|_BLUE);
    DoShadow();
    wmessage(" F1 For Editing Help ",1,7,LIGHTRED|_BLUE);
    wcenters(0,LIGHTRED|_BLUE,"Miscellaneous Options");
    showcur();
    sprintf(temp,"%d",cfg.crash);
    sprintf(temp1,"%d",cfg.direct);
    sprintf(temp2,"%s",cfg.center?"Y":"N");
    sprintf(temp3,"%s",cfg.delete_sent?"Y":"N");
    sprintf(temp4,"%s",cfg.import_recd?"Y":"N");
    sprintf(temp5,"%s",cfg.use_areafix?"Y":"N");
    sprintf(temp6,"%s",cfg.skip_mail?"Y":"N");
    sprintf(temp7,"%s",cfg.cleanup?"Y":"N");
    sprintf(temp8,"%s",cfg.set_immediate?"Y":"N");
    sprintf(temp9,"%s",cfg.auto_add?"Y":"N");
    sprintf(temp10,"%s",cfg.log_feeds?"Y":"N");
    sprintf(temp11,"%s",cfg.add_hard_cr?"Y":"N");
    sprintf(temp12,"%s",cfg.add_line_feed?"Y":"N");
    sprintf(temp13,"%s",cfg.add_soft_cr?"Y":"N");
    sprintf(temp14,"%s",cfg.soft_to_hard?"Y":"N");
    sprintf(temp15,"%s",cfg.hard_to_soft?"Y":"N");
    sprintf(temp17,"%s",cfg.high_ascii?"Y":"N");
    sprintf(temp16,"%s",cfg.route_me?"Y":"N");
    sprintf(temp18,"%d",cfg.log_days);
    sprintf(temp19,"%s",cfg.color_to?"Y":"N");
    sprintf(temp20,"%s",cfg.initial_quote?"Y":"N");
    sprintf(temp21,"%s",cfg.bounce_mail?"Y":"N");
    sprintf(temp22,"%s",cfg.check_dupes?"Y":"N");
    sprintf(temp23,"%s",cfg.pass_origin?"Y":"N");
    switch(cfg.mailer)
    {
        case 0:
            strcpy(s,"FrontDoor  ");
            break;
        case 1:
            strcpy(s,"Intermail  ");
            break;
        case 2:
            strcpy(s,"BinkleyTerm");
            break;
        case 3:
            strcpy(s,"Portal     ");
            break;
        case 4:
            strcpy(s,"D'Bridge   ");
            break;
        default:
            strcpy(s,"FrontDoor  ");
            cfg.mailer=0;
            break;
    }
    wprints(11,2,CYAN|_BLUE,"Mailer Type  : ");
    wprints(11,17,CYAN|_BLUE,s);
    winpbeg(CYAN|_BLUE,LIGHTCYAN|_BLUE);
    winpdef(1,2,temp, "'Crash Mode   : '????",'0',1,NULL,0);
    winpfba(show_hint29,clear_hint);
    winpdef(1,30,temp8, "'Immed. Mode      : 'Y",'U',1,NULL,0);
    winpfba(show_hint83,clear_hint);
    winpdef(2,2,temp1,"'Direct Mode  : '??",'0',1,NULL,0);
    winpfba(show_hint30,clear_hint);
    winpdef(2,30,temp9, "'Auto Add Area    : 'Y",'U',1,NULL,0);
    winpfba(show_hint84,clear_hint);
    winpdef(3,2,temp2,"'Center       : 'Y",'U',1,NULL,0);
    winpfba(show_hint31,clear_hint);
    winpdef(3,30,temp10, "'Log Passthrus    : 'Y",'U',1,NULL,0);
    winpfba(show_hint85,clear_hint);
    winpdef(4,2,temp3,"'Delete Sent  : 'Y",'U',1,NULL,0);
    winpfba(show_hint32,clear_hint);
    winpdef(5,2,temp4,"'Import Recd  : 'Y",'U',1,NULL,0);
    winpfba(show_hint33,clear_hint);
//    winpdef(5,30,temp12, "'Add Line Feed    : 'Y",'U',1,NULL,0);
//    winpfba(show_hint87,clear_hint);
    winpdef(6,2,temp5,"'Use Areafix  : 'Y",'U',1,NULL,0);
    winpfba(show_hint34,clear_hint);
//    winpdef(6,30,temp13, "'Add Soft C/R     : 'Y",'U',1,NULL,0);
//    winpfba(show_hint88,clear_hint);
    winpdef(7,2,temp6,"'Skip Mail    : 'Y",'U',1,NULL,0);
    winpfba(show_hint35,clear_hint);
//    winpdef(7,30,temp14, "'Soft To Hard C/R : 'Y",'U',1,NULL,0);
//    winpfba(show_hint89,clear_hint);
    winpdef(8,2,temp7,"'Cleanup Nets : 'Y",'U',1,NULL,0);
    winpfba(show_hint36,clear_hint);
//    winpdef(8,30,temp15, "'Hard To Soft C/R : 'Y",'U',1,NULL,0);
//    winpfba(show_hint90,clear_hint);
    winpdef(9,2,temp16,"'Route Mail?  : 'Y",'U',1,NULL,0);
    winpfba(show_hint91,clear_hint);
    winpdef(4,30,temp17, "'Pass High ASCII  : 'Y",'U',1,NULL,0);
    winpfba(show_hint92,clear_hint);
    winpdef(5,30,temp18, "'Keep Log X Days? : '???",'0',1,NULL,0);
    winpfba(show_hint96,clear_hint);
    winpdef(6,30,temp19, "'Colorize TO: Line: 'Y",'U',1,NULL,0);
    winpfba(show_hint97,clear_hint);
    winpdef(7,30,temp20, "'Initial Quotes?  : 'Y",'U',1,NULL,0);
    winpfba(show_hint98,clear_hint);
    winpdef(8,30,temp21, "'Bounce Bad Email?: 'Y",'U',1,NULL,0);
    winpfba(show_hint110,clear_hint);
    winpdef(9,30,temp22, "'Check For Dupes? : 'Y",'U',1,NULL,0);
    winpfba(show_hint112,clear_hint);
 winpdef(10,2,temp23, "'Check Origin?: 'Y",'U',1,NULL,0);
    winpfba(show_hint113,clear_hint);
    esc=winpread();
    hidecur();
    if (!esc)
    {
        show_hint37();
        choose_mailer();
    }
    wclose();
    cfg.crash=atoi(temp);
    if ((cfg.crash>2))
	cfg.crash=0;
    cfg.direct=atoi(temp1);
    if ((cfg.direct>2))
        cfg.direct=0;
    if (temp2[0]=='Y')
        cfg.center=1;
    else
        cfg.center=0;
    if (temp3[0]=='Y')
        cfg.delete_sent=1;
    else
        cfg.delete_sent=0;
    if (temp4[0]=='Y')
        cfg.import_recd=1;
    else
        cfg.import_recd=0;
    if (temp5[0]=='Y')
        cfg.use_areafix=1;
    else
        cfg.use_areafix=0;
    if (temp6[0]=='Y')
        cfg.skip_mail=1;
    else
        cfg.skip_mail=0;
    if (temp7[0]=='Y')
        cfg.cleanup=1;
    else
        cfg.cleanup=0;
    if (temp8[0]=='Y')
        cfg.set_immediate=1;
    else
        cfg.set_immediate=0;
    if (temp9[0]=='Y')
        cfg.auto_add=1;
    else
        cfg.auto_add=0;
    if (temp10[0]=='Y')
        cfg.log_feeds=1;
    else
        cfg.log_feeds=0;
    if (temp11[0]=='Y')
        cfg.add_hard_cr=1;
    else
        cfg.add_hard_cr=0;
    if (temp12[0]=='Y')
        cfg.add_line_feed=1;
    else
        cfg.add_line_feed=0;
    if (temp13[0]=='Y')
        cfg.add_soft_cr=1;
    else
        cfg.add_soft_cr=0;
    if (temp14[0]=='Y')
        cfg.soft_to_hard=1;
    else
        cfg.soft_to_hard=0;
    if (temp15[0]=='Y')
        cfg.hard_to_soft=1;
    else
        cfg.hard_to_soft=0;
    if (temp17[0]=='Y')
        cfg.high_ascii=1;
    else
        cfg.high_ascii=0;
    if (temp16[0]=='Y')
        cfg.route_me=1;
    else
        cfg.route_me=0;
    cfg.log_days=atoi(temp18);
    if (temp19[0]=='Y')
        cfg.color_to=1;
    else
        cfg.color_to=0;
    if (temp20[0]=='Y')
        cfg.initial_quote=1;
    else
        cfg.initial_quote=0;
    if (temp21[0]=='Y')
        cfg.bounce_mail=1;
    else
        cfg.bounce_mail=0;
    if (temp22[0]=='Y')
        cfg.check_dupes=1;
    else
        cfg.check_dupes=0;
    if (temp23[0]=='Y')
        cfg.pass_origin=1;
    else
        cfg.pass_origin=0;
    clear_hint();
    show_hint2();

}
void show_akas(void)
{
    int i;
    char s[180];
    char temp[50];

    wclear();
    wcenters(0,LIGHTRED|_BLUE,"AKA Configuration");
    wprints(1,2,WHITE|_BLUE,"#");
    wprints(1,6,WHITE|_BLUE,"Address");
    wprints(1,25,WHITE|_BLUE,"Fake WWIV Node");
    wprints(1,44,WHITE|_BLUE,"WWIV Net #");
    wprints(1,58,WHITE|_BLUE,"Domain");
    for (i=0;i<11;i++)
    {
        sprintf(temp,"%d:%d/%d.%d",cfg.aka_list[i].zone,cfg.aka_list[i].net,cfg.aka_list[i].node,cfg.aka_list[i].point);
        sprintf(s,"%-2d  %-18s      %-5u             %-2u        %s",i,temp,cfg.aka_list[i].wwiv_node,cfg.aka_list[i].wwiv_netnum,domains[i].domain);
        wprints(i+2,2,YELLOW|_BLUE,s);
    }
}
void aka_choose(void)
{
    int done,keypress,i,current_num;
    int last_pos,curr_pos,last_num;
    char s[5];
    unsigned int keycode;
    done=i=0;
    last_pos=2;
    curr_pos=2;
    last_num=1;
    hidecur();
    wopen(6,1,21,77,1,YELLOW|_BLUE,WHITE|_BLUE);
    DoShadow();
    show_hint58();
    show_akas();
    do
    {
        last_pos=curr_pos;
        curr_pos=i+2;
        sprintf(s,"%-2d",last_num);
        last_num=i;
        wprints(last_pos,2,YELLOW|_BLUE,s);
        sprintf(s,"%-2d",i);
        last_num=i;
        wprints(curr_pos,2,WHITE|_GREEN,s);
        keycode=getxch();
        keypress=keycode>>8;
        switch (keypress)
        {
            case 1: /* ESC */
                done=1;
                break;
            case 28: /* Enter */
                current_num=i;
                edit_aka(i);
                show_akas();
                i=current_num;
                break;
            case 71: /* Home */
                i=0;
                break;
            case 72: /* Up Arrow */
                i--;
                if (i<0)
                    i=10;
                break;
            case 79: /* End */
                i=10;
                break;
            case 80: /* Down Arrow */
                i++;
                if (i>10)
                    i=0;
                break;
        }

    } while (!done);

    wclose();
    sprintf(cfg.system_address,"%d:%d/%d.%d",cfg.aka_list[0].zone,cfg.aka_list[0].net,cfg.aka_list[0].node,cfg.aka_list[0].point);
    show_hint7();
}
void edit_aka(int aka_num)
{
    char temp[50],temp1[20];
    char tnode[50];
    char tnet[50];
    int i;
    FIDOADR fadr;
    i=aka_num+2;
    showcur();
    temp[0]=0;
    tnode[0]=0;
    tnet[0]=0;
    sprintf(temp,"%d:%d/%d.%d",cfg.aka_list[aka_num].zone,cfg.aka_list[aka_num].net,cfg.aka_list[aka_num].node,cfg.aka_list[aka_num].point);
    sprintf(tnode,"%d",cfg.aka_list[aka_num].wwiv_node);
    sprintf(tnet,"%d",cfg.aka_list[aka_num].wwiv_netnum);
    sprintf(temp1,"%s",domains[aka_num].domain);
    winpbeg(YELLOW|_BLUE,LIGHTRED|_BLUE);
    winpdef(i,6,temp,"??????????????????",'0',2,NULL,0);
    winpfba(show_hint55,clear_hint);
    winpdef(i,30,tnode,"????????",'0',2,NULL,0);
    winpfba(show_hint56,clear_hint);
    winpdef(i,48,tnet,"???",'0',2,NULL,0);
    winpfba(show_hint57,clear_hint);
    winpdef(i,58,temp1,"??????????????",'0',2,NULL,0);
    winpfba(show_hint76,clear_hint);
    winpread();
    strtrim(temp);
    strtrim(tnet);
    strtrim(tnode);
    strtrim(temp1);
    fidoadr_split(temp,&fadr);
    cfg.aka_list[aka_num].zone=fadr.zone;
    cfg.aka_list[aka_num].net=fadr.net;
    cfg.aka_list[aka_num].node=fadr.node;
    cfg.aka_list[aka_num].point=fadr.point;
    cfg.aka_list[aka_num].wwiv_node=atoi(tnode);
    cfg.aka_list[aka_num].wwiv_netnum=atoi(tnet);
    strcpy(domains[aka_num].domain,temp1);
    hidecur();
    clear_hint();
    show_hint58();
}
void show_routes(void)
{
    int i;
    char s[80];
    char temp[50];
    wclear();
    wcenters(0,LIGHTRED|_BLUE,"Routing Configuration");
    wprints(1,5,WHITE|_BLUE,"#");
    wprints(1,12,WHITE|_BLUE,"Route To");
    wprints(1,34,WHITE|_BLUE,"Zone");

    for (i=0;i<11;i++)
    {
        sprintf(temp,"%d:%d/%d.%d",cfg.route_to[i].zone,cfg.route_to[i].net,cfg.route_to[i].node,cfg.route_to[i].point);
        if ((cfg.route_to[i].zone==999) || (cfg.route_to[i].zone==0))
            strcpy(temp," ");
        sprintf(s,"%-2d     %-18s      %-5d",i,temp,cfg.route_to[i].route_zone);
        wprints(i+2,5,YELLOW|_BLUE,s);
    }
}

void route_setup(void)
{
    int done,keypress,i,current_num;
    int last_pos,curr_pos,last_num;
    char s[5];
    unsigned int keycode;
    done=i=0;
    last_pos=2;
    curr_pos=2;
    last_num=1;
    wopen(6,15,21,64,1,YELLOW|_BLUE,WHITE|_BLUE);
    DoShadow();
    show_hint58();
    show_routes();
    do
    {
        last_pos=curr_pos;
        curr_pos=i+2;
        sprintf(s,"%-2d",last_num);
        last_num=i;
        wprints(last_pos,5,YELLOW|_BLUE,s);
        sprintf(s,"%-2d",i);
        last_num=i;
        wprints(curr_pos,5,WHITE|_GREEN,s);
        keycode=getxch();
        keypress=keycode>>8;
        switch (keypress)
        {
            case 1: /* ESC */
                done=1;
                break;
            case 28: /* Enter */
                current_num=i;
                edit_route(i);
                show_routes();
                i=current_num;
                break;
            case 71: /* Home */
                i=0;
                break;
            case 72: /* Up Arrow */
                i--;
                if (i<0)
                    i=10;
                break;
            case 79: /* End */
                i=10;
                break;
            case 80: /* Down Arrow */
                i++;
                if (i>10)
                    i=0;
                break;
        }

    } while (!done);

    wclose();
    show_hint5();
}
void edit_route(int route_num)
{
    char temp[50];
    char tnode[50];
    char tnet[50];
    int i;
    FIDOADR fadr;
    i=route_num+2;
    showcur();
    temp[0]=0;
    tnode[0]=0;
    tnet[0]=0;
    sprintf(temp,"%d:%d/%d.%d",cfg.route_to[route_num].zone,cfg.route_to[route_num].net,cfg.route_to[route_num].node,cfg.route_to[route_num].point);
    sprintf(tnode,"%d",cfg.route_to[route_num].route_zone);
    winpbeg(YELLOW|_BLUE,LIGHTRED|_BLUE);
    winpdef(i,12,temp,"??????????????????",'0',2,NULL,0);
    winpdef(i,36,tnode,"????????",'0',2,NULL,0);
    winpread();
    strtrim(temp);
    strtrim(tnode);
    fidoadr_split(temp,&fadr);
    cfg.route_to[route_num].zone=fadr.zone;
    cfg.route_to[route_num].net=fadr.net;
    cfg.route_to[route_num].node=fadr.node;
    cfg.route_to[route_num].point=fadr.point;
    cfg.route_to[route_num].route_zone=atoi(tnode);
    hidecur();

}
void show_exports(void)
{
    int i;
    char s[80];
    char temp[50],temp1[50];
    wclear();
    wcenters(0,LIGHTRED|_BLUE,"Area Export Configuration");
    wprints(1,5,WHITE|_BLUE,"#");
    wprints(1,12,WHITE|_BLUE,"Export To");
    wprints(1,29,WHITE|_BLUE,"#");
    wprints(1,36,WHITE|_BLUE,"Export To");
    for (i=0;i<10;i++)
    {
        sprintf(temp,"%d:%d/%d.%d",thisarea.feed[i].zone,thisarea.feed[i].net,thisarea.feed[i].node,thisarea.feed[i].point);
        sprintf(temp1,"%d:%d/%d.%d",thisarea.feed[i+10].zone,thisarea.feed[i+10].net,thisarea.feed[i+10].node,thisarea.feed[i+10].point);
        if ((thisarea.feed[i].zone==999) || (thisarea.feed[i].zone==0))
            strcpy(temp," ");
        if ((thisarea.feed[i+10].zone==999) || (thisarea.feed[i+10].zone==0))
            strcpy(temp1," ");
        sprintf(s,"%-2d  %-18s  %-2d  %-18s",i+1,temp,i+11,temp1);
        wprints(i+2,5,YELLOW|_BLUE,s);
    }
}

void export_setup(int areanum)
{
    int done,keypress,i,current_num,j;
    int last_pos,curr_pos,last_num,column,last_column;
    char s[5];
    unsigned int keycode;
    done=i=0;
    last_pos=2;
    column=last_column=5;
    curr_pos=2;
    last_num=1;
    curr_area=areanum;
    hidecur();
    wopen(6,10,20,64,1,YELLOW|_BLUE,WHITE|_BLUE);
    DoShadow();
    show_exports();
    show_hint59();
    do
    {
        last_pos=curr_pos;
        last_column=column;
        if (i>9)
            column=29;
        else
            column=5;
        curr_pos=i+2;
        if (i>9)
            curr_pos=i+2-10;
        sprintf(s,"%-2d",last_num+1);
        last_num=i;
        wprints(last_pos,last_column,YELLOW|_BLUE,s);
        sprintf(s,"%-2d",i+1);
        last_num=i;
        wprints(curr_pos,column,WHITE|_GREEN,s);
        keycode=getxch();
        keypress=keycode>>8;
        switch (keypress)
        {
            case 1: /* ESC */
                done=1;
                break;
            case 28: /* Enter */
                current_num=i;
                edit_export(i);
                show_exports();
                i=current_num;
                break;
            case 71: /* Home */
                i=0;
                break;
            case 72: /* Up Arrow */
                i--;
                if (i<0)
                {
                    i=MAX_NODES-1;
                }
                break;

            case 73: /* PgUp */
                for (j=0;j<=MAX_NODES;j++)
                {
                    thisarea.feed[j].zone=0;
                    thisarea.feed[j].net=0;
                    thisarea.feed[j].node=0;
                    thisarea.feed[j].point=0;
                }
                break;

            case 75: /* Left Arrow */
                if (i>9)
                    i-=10;
                else
                    i+=10;
                break;
            case 77: /* Right Arrow */
                if (i<10)
                    i+=10;
                else
                    i-=10;
                break;
            case 79: /* End */
                i=MAX_NODES-1;
                break;
            case 80: /* Down Arrow */
                i++;
                if (i>MAX_NODES-1)
                    i=0;
                break;
            case 83: /* Delete */
                current_num=i;
                thisarea.feed[i].zone=999;
                thisarea.feed[i].net=0;
                thisarea.feed[i].node=0;
                thisarea.feed[i].point=0;
                i=current_num;
                sort_export();
                show_exports();
                break;
        }

    } while (!done);

    wclose();
    show_hint47();
}

void edit_export(int export_num)
{
    char temp[50];
    char tnode[50];
    char tnet[50];
    int column,duplicate;
    int i,j,k;
    FIDOADR fadr;
    i=export_num;
    j=export_num+2;
    column=9;
    if (i>9)
    {
        j=export_num+2-10;
        column=33;
    }
    showcur();
    temp[0]=0;
    tnode[0]=0;
    tnet[0]=0;
    sprintf(temp,"%d:%d/%d.%d",thisarea.feed[i].zone,thisarea.feed[i].net,thisarea.feed[i].node,thisarea.feed[i].point);
    winpbeg(YELLOW|_BLUE,LIGHTRED|_BLUE);
    winpdef(j,column,temp,"??????????????????",'0',2,NULL,0);
    winpread();
    strtrim(temp);
    fidoadr_split(temp,&fadr);
    if ((fadr.zone==0) || (fadr.zone==999) || (fadr.net==0))
    {
        thisarea.feed[i].zone=999;
        thisarea.feed[i].net=999;
        thisarea.feed[i].node=999;
        thisarea.feed[i].point=999;
    } else
    {
        duplicate=0;
        for (k=0;k<MAX_NODES;k++)
        {
            if ((thisarea.feed[k].zone==fadr.zone)  &&
                (thisarea.feed[k].net==fadr.net)    &&
                (thisarea.feed[k].node==fadr.node)  &&
                (thisarea.feed[k].point==fadr.point)&&
                (k!=i))
                {
                    duplicate=1;
//                    wperror("Error!  Duplicate Node!");
//                    k=MAX_NODES;
                }
            if (!duplicate)
            {
                thisarea.feed[i].zone=fadr.zone;
                thisarea.feed[i].net=fadr.net;
                thisarea.feed[i].node=fadr.node;
                thisarea.feed[i].point=fadr.point;
            }
            else
            if (duplicate)
            {
                thisarea.feed[k].zone=0;
                thisarea.feed[k].net=0;
                thisarea.feed[k].node=0;
                thisarea.feed[k].point=0;
            }
        }
    }
    for (k=0;k<MAX_NODES;k++)
    {
        if (thisarea.feed[k].net==0)
        {
            thisarea.feed[k].zone=999;
            thisarea.feed[k].net=999;
            thisarea.feed[k].node=999;
            thisarea.feed[k].point=999;
        }
    }

    hidecur();
    show_hint59();
    sort_export();

}
void area_setup(void)
{
    int done,keypress,i,j,k;
    unsigned int keycode;
    char ch;
    arearec b;
    done=0;
    setonkey(0x3C00,global_options,0);
    areawin=wopen(5,0,23,79,1,YELLOW|_BLUE,WHITE|_BLUE);
//    DoShadow();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,"Arrow Keys to Move, [ENTER] to Edit, ESC to Exit.  F2 for Global Options");
    i=1;
    while (!done)
    {
        current_area=i;
        read_area(i,&thisarea);
        show_area_screen(i);
        keycode=getxch();
        keypress=keycode>>8;
        switch(keypress)
        {
            case 1 : /* ESC */
                done=1;
                break;
            case 28: /* Enter */
                current_area=i;
                edit_area(i);
                i=current_area;
                break;
            case 71: /* Home */
                write_area(i,&thisarea);
                i=1;
                read_area(i,&thisarea);
                break;
            case 75: /* Left Arrow */
                write_area(i,&thisarea);
                i--;
                if (i<1)
                    i=num_areas;
                read_area(i,&thisarea);
                break;
            case 77: /* Right Arrow */
                write_area(i,&thisarea);
                i++;
                if (i>num_areas)
                    i=1;
                read_area(i,&thisarea);
                break;
            case 79: /* End */
                write_area(i,&thisarea);
                i=num_areas;
                read_area(i,&thisarea);
                break;
            case 82: /* Insert */
                num_areas++;
                write_area(i,&thisarea);
                i=num_areas;

                thisarea.name[0]=0;
                thisarea.comment[0]=0;
                thisarea.group=0;
                thisarea.origin[0]=0;
                thisarea.subtype[0]=0;
                thisarea.comment[0]=0;
                thisarea.alias_ok=0;
                thisarea.pass_color=0;
                thisarea.high_ascii=0;
                thisarea.keep_tag=0;
                thisarea.net_num=0;

                for (j=0;j<MAX_NODES;j++)
                    thisarea.feed[j].zone=999;
                write_area(i,&thisarea);

                edit_area(i);
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
                    current_area=i;
                    for (j=0;j<MAX_NODES;j++)
                    {
                        thisarea.feed[j].zone=999;
                        thisarea.feed[j].net=0;
                        thisarea.feed[j].node=0;
                        thisarea.feed[j].point=0;
                    }
                    thisarea.active=0;
                    write_area(i,&thisarea);
                    if (num_areas<1)
                        num_areas=1;
                    cfg.num_areas=num_areas;
                    write_config();
                    cleanup();
                    i=current_area;
                    if (i<1)
                        i=1;
                    if (i>num_areas)
                        i=num_areas;
                    }
                    break;
            default:
                if (num_areas==1)
                    break;
                read_area(i+1,&b);

                if (b.name[0]==upcase(keycode))
                {
                    if (i+1<num_areas+1)
                        i++;
                }
                else
                for (k=1;k<=num_areas;k++)
                {
                    read_area(k,&b);
                    if (b.name[0]==upcase(keycode))
                    {
                        i=k;
                        k=num_areas+1;
                        break;
                    }
                }

                break;

        }
    }
    wclose();
    quicksort(1,num_areas,0);
    setonkey(0x3C00,NULL,0);
    show_hint3();

}

void up_addr(void)
{
    wprints(4,17,CYAN|_BLUE,cfg.system_address);
}

void show_area_screen(int areanum)
{
    char temp[80];
    char s[161];
    int j,prev_zone,prev_net,prev_node,temp_length,line;
    sprintf(s,"Area # %d out of %d",areanum,num_areas);
    wclear();

    wcenters(0,WHITE|_BLUE,s);
    wprints(1,2,CYAN|_BLUE,"Area Name       : ");
    wprints(1,20,LIGHTCYAN|_BLUE,thisarea.name);
    wprints(2,2,CYAN|_BLUE,"Area Comment    : ");
    wprints(2,20,LIGHTCYAN|_BLUE,thisarea.comment);
    wprints(3,2,CYAN|_BLUE,"Area Directory  : ");
    wprints(3,20,LIGHTCYAN|_BLUE,thisarea.directory);
    wprints(4,2,CYAN|_BLUE,"Area Type       : ");
    switch (thisarea.type)
    {
        case 0:
            strcpy(temp,"Passthru            ");
            break;
        case 1:
            strcpy(temp,"Import To WWIV      ");
            break;
        case 2:
            strcpy(temp,"Write As *.MSG Files");
            break;
    }
    wprints(4,25,LIGHTCYAN|_BLUE,temp);
    sprintf(temp,"%d",thisarea.type);
    wprints(4,20,LIGHTCYAN|_BLUE,temp);

    wprints(6,2,CYAN|_BLUE,"Area Group      : ");
    if (thisarea.group)
        sprintf(temp,"%c - %s",thisarea.group,cfg.group[thisarea.group-65].group_name);
    else
        strcpy(temp," ");
    wprints(6,20,LIGHTCYAN|_BLUE,temp);
    wprints(7,2,CYAN|_BLUE,"WWIV Subtype    : ");
    wprints(7,20,LIGHTCYAN|_BLUE,thisarea.subtype);
    wprints(7,40,CYAN|_BLUE,"WWIV Net Number : ");
    sprintf(temp,"%d",thisarea.net_num);
    wprints(7,58,LIGHTCYAN|_BLUE,temp);
    wprints(8,2,CYAN|_BLUE,"Use Trans. Table: ");
    sprintf(temp,"%s",thisarea.translate?"Y":"N");
    wprints(8,20,LIGHTCYAN|_BLUE,temp);
    wprints(8,40,CYAN|_BLUE,"Use Alias?      : ");
    sprintf(temp,"%s",thisarea.alias_ok?"Y":"N");
    wprints(8,58,LIGHTCYAN|_BLUE,temp);
    wprints(9,2,CYAN|_BLUE,"Use WWIV Color? : ");
    sprintf(temp,"%s",thisarea.pass_color?"Y":"N");
    wprints(9,20,LIGHTCYAN|_BLUE,temp);
    wprints(9,40,CYAN|_BLUE,"Use High ASCII? : ");
    sprintf(temp,"%s",thisarea.high_ascii?"Y":"N");
    wprints(9,58,LIGHTCYAN|_BLUE,temp);
    wprints(10,2,CYAN|_BLUE,"Keep BBS Tag?   : ");
    sprintf(temp,"%s",thisarea.keep_tag?"Y":"N");
    wprints(10,20,LIGHTCYAN|_BLUE,temp);
    wprints(10,40,CYAN|_BLUE,"Valid. Incoming?: ");
    sprintf(temp,"%s",thisarea.val_incoming?"Y":"N");
    wprints(10,58,LIGHTCYAN|_BLUE,temp);
    wprints(12,2,CYAN|_BLUE,"Export To Nodes : ");
    temp_length=0;
    line=12;
    if ((thisarea.feed[0].zone!=999) && (thisarea.feed[0].zone!=0))
    {
        if (thisarea.feed[0].point==0)
            sprintf(s,"%d:%d/%d ",thisarea.feed[0].zone,thisarea.feed[0].net,thisarea.feed[0].node);
        else
            sprintf(s,"%d:%d/%d.%d ",thisarea.feed[0].zone,thisarea.feed[0].net,thisarea.feed[0].node,thisarea.feed[0].point);
        prev_zone=thisarea.feed[0].zone;
        prev_net=thisarea.feed[0].net;
        prev_node=thisarea.feed[0].node;
        for (j=1;j<MAX_NODES;j++)
        {
            if (thisarea.feed[j].zone!=999)
            {
                if (thisarea.feed[j].zone!=prev_zone)
                {
                    if (thisarea.feed[j].point==0)
                        sprintf(temp,"%d:%d/%d ",thisarea.feed[j].zone,thisarea.feed[j].net,thisarea.feed[j].node);
                    else
                        sprintf(temp,"%d:%d/%d.%d ",thisarea.feed[j].zone,thisarea.feed[j].net,thisarea.feed[j].node,thisarea.feed[j].point);
                    prev_zone=thisarea.feed[j].zone;
                    prev_net=thisarea.feed[j].net;
                    prev_node=thisarea.feed[j].node;
                }
                else
                if (thisarea.feed[j].net!=prev_net)
                {
                    if (thisarea.feed[j].point==0)
                        sprintf(temp,"%d/%d ",thisarea.feed[j].net,thisarea.feed[j].node);
                    else
                        sprintf(temp,"%d/%d.%d ",thisarea.feed[j].net,thisarea.feed[j].node,thisarea.feed[j].point);
                    prev_net=thisarea.feed[j].net;
                    prev_node=thisarea.feed[j].node;
                }
                else
                if (thisarea.feed[j].node!=prev_node)
                {
                    if (thisarea.feed[j].point!=0)
                        sprintf(temp,"%d.%d ",thisarea.feed[j].node,thisarea.feed[j].point);
                    else
                        sprintf(temp,"%d ",thisarea.feed[j].node);
                    prev_node=thisarea.feed[j].node;
                }
                else
                    sprintf(temp,".%d ",thisarea.feed[j].point);
            temp_length+=strlen(temp);
            if (temp_length>=35)
            {
                wprints(line,20,LIGHTCYAN|_BLUE,s);
                line++;
                temp_length=0;
                strcpy(s,"");
            }
            strcat(s,temp);

            }

        }
    wprints(line,20,LIGHTCYAN|_BLUE,s);
    }
    wprints(16,2,CYAN|_BLUE,"Origin Line     : ");
    if (thisarea.origin[0]==0)
        strcpy(thisarea.origin,cfg.origin_line);
    wprints(16,20,LIGHTCYAN|_BLUE,thisarea.origin);

}
void update_area_screen(int areanum)
{
    char temp[80];
    char s[161];
    int j,prev_zone,prev_net,prev_node,temp_length,line;
    read_area(areanum,&thisarea);
    wwprints(areawin,1,20,LIGHTCYAN|_BLUE,thisarea.name);
    wwprints(areawin,2,20,LIGHTCYAN|_BLUE,thisarea.comment);
    wwprints(areawin,3,20,LIGHTCYAN|_BLUE,thisarea.directory);
    switch (thisarea.type)
    {
        case 0:
            strcpy(temp,"Passthru            ");
            break;
        case 1:
            strcpy(temp,"Import To WWIV      ");
            break;
        case 2:
            strcpy(temp,"Write As *.MSG Files");
            break;
    }
    wwprints(areawin,4,25,LIGHTCYAN|_BLUE,temp);
    sprintf(temp,"%d",thisarea.type);
    wwprints(areawin,4,20,LIGHTCYAN|_BLUE,temp);
    if (thisarea.group)
        sprintf(temp,"%c - %s",thisarea.group,cfg.group[thisarea.group-65].group_name);
    else
        strcpy(temp," ");
    wwprints(areawin,6,20,LIGHTCYAN|_BLUE,temp);
    wwprints(areawin,7,20,LIGHTCYAN|_BLUE,thisarea.subtype);
    sprintf(temp,"%d",thisarea.net_num);
    wwprints(areawin,7,58,LIGHTCYAN|_BLUE,temp);
    sprintf(temp,"%s",thisarea.alias_ok?"Y":"N");
    wwprints(areawin,8,20,LIGHTCYAN|_BLUE,temp);
    sprintf(temp,"%s",thisarea.translate?"Y":"N");
    wwprints(areawin,8,58,LIGHTCYAN|_BLUE,temp);
    sprintf(temp,"%s",thisarea.pass_color?"Y":"N");
    wwprints(areawin,9,20,LIGHTCYAN|_BLUE,temp);
    sprintf(temp,"%s",thisarea.high_ascii?"Y":"N");
    wwprints(areawin,9,58,LIGHTCYAN|_BLUE,temp);
    sprintf(temp,"%s",thisarea.keep_tag?"Y":"N");
    wwprints(areawin,10,20,LIGHTCYAN|_BLUE,temp);
    sprintf(temp,"%s",thisarea.val_incoming?"Y":"N");
    wwprints(areawin,10,58,LIGHTCYAN|_BLUE,temp);
   temp_length=0;
   line=12;
    if ((thisarea.feed[0].zone!=999) && (thisarea.feed[0].zone!=0))
    {
        if (thisarea.feed[0].point==0)
            sprintf(s,"%d:%d/%d ",thisarea.feed[0].zone,thisarea.feed[0].net,thisarea.feed[0].node);
        else
            sprintf(s,"%d:%d/%d.%d ",thisarea.feed[0].zone,thisarea.feed[0].net,thisarea.feed[0].node,thisarea.feed[0].point);
        prev_zone=thisarea.feed[0].zone;
        prev_net=thisarea.feed[0].net;
        prev_node=thisarea.feed[0].node;
        for (j=1;j<MAX_NODES;j++)
        {
            if (thisarea.feed[j].zone!=999)
            {
                if (thisarea.feed[j].zone!=prev_zone)
                {
                    if (thisarea.feed[j].point==0)
                        sprintf(temp,"%d:%d/%d ",thisarea.feed[j].zone,thisarea.feed[j].net,thisarea.feed[j].node);
                    else
                        sprintf(temp,"%d:%d/%d.%d ",thisarea.feed[j].zone,thisarea.feed[j].net,thisarea.feed[j].node,thisarea.feed[j].point);
                    prev_zone=thisarea.feed[j].zone;
                    prev_net=thisarea.feed[j].net;
                    prev_node=thisarea.feed[j].node;
                }
                else
                if (thisarea.feed[j].net!=prev_net)
                {
                    if (thisarea.feed[j].point==0)
                        sprintf(temp,"%d/%d ",thisarea.feed[j].net,thisarea.feed[j].node);
                    else
                        sprintf(temp,"%d/%d.%d ",thisarea.feed[j].net,thisarea.feed[j].node,thisarea.feed[j].point);
                    prev_net=thisarea.feed[j].net;
                    prev_node=thisarea.feed[j].node;
                }
                else
                if (thisarea.feed[j].node!=prev_node)
                {
                    if (thisarea.feed[j].point!=0)
                        sprintf(temp,"%d.%d ",thisarea.feed[j].node,thisarea.feed[j].point);
                    else
                        sprintf(temp,"%d ",thisarea.feed[j].node);
                    prev_node=thisarea.feed[j].node;
                }
                else
                    sprintf(temp,".%d ",thisarea.feed[j].point);
            temp_length+=strlen(temp);
            if (temp_length>=35)
            {
                wwprints(areawin,line,20,LIGHTCYAN|_BLUE,s);
                line++;
                temp_length=0;
                strcpy(s,"");
            }
            strcat(s,temp);

            }

        }
    while (strlen(s)<36)
        strcat(s," ");
    wwprints(areawin,line,20,LIGHTCYAN|_BLUE,s);
    }
    if (thisarea.origin[0]==0)
        strcpy(thisarea.origin,cfg.origin_line);
    strcpy(s,thisarea.origin);
    while (strlen(s)<60)
        strcat(s," ");
    wwprints(areawin,14,20,LIGHTCYAN|_BLUE,s);

}

void update_group(void)
{
   wprints(6,20,LIGHTCYAN|_BLUE,cfg.group[thisarea.group].group_id);
 }
void choose_type(void)
{
    char temp[80];
    char ch;
    int i,i1;

    wgotoxy(4,20);
    i=i1=thisarea.type;
    ch=0;
    while ((ch!=13) && (ch!=27))
    {
        switch (thisarea.type)
        {
            case 0:
                strcpy(temp,"Passthru            ");
                break;
            case 1:
                strcpy(temp,"Import To WWIV      ");
                break;
            case 2:
                strcpy(temp,"Write As *.MSG Files");
                break;
        }

        ch=getch();
        if (ch==32)
        {
            i++;
            if (i>2)
               i=0;
            thisarea.type=i;
        }
        switch (thisarea.type)
        {
            case 0:
                strcpy(temp,"Passthru            ");
                break;
            case 1:
                strcpy(temp,"Import To WWIV      ");
                break;
            case 2:
                strcpy(temp,"Write As *.MSG Files");
                break;
        }
    wprints(4,2,LIGHTCYAN|_BLUE,"Area Type       : ");
    wprints(4,20,LIGHTCYAN|_BLUE,temp);
    }
    if (ch==27)
        thisarea.type=i1;
        switch (thisarea.type)
        {
            case 0:
                strcpy(temp_ch,"Passthru            ");
                break;
            case 1:
                strcpy(temp_ch,"Import To WWIV      ");
                break;
            case 2:
                strcpy(temp_ch,"Write As *.MSG Files");
                break;
        }
    wprints(4,2,CYAN|_BLUE,"Area Type       : ");
    wprints(4,20,CYAN|_BLUE,temp_ch);
}
int show_type(char *str)
{
    int i;
    char temp[10];
    i=atoi(str);
    switch (i)
    {
        case 0:
            strcpy(temp_ch,"Passthru            ");
            break;
        case 1:
            strcpy(temp_ch,"Import To WWIV      ");
            break;
        case 2:
            strcpy(temp_ch,"Write As *.MSG Files");
            break;
        default:
            strcpy(temp_ch,"Import To WWIV      ");
            i=1;
            thisarea.type=1;
            break;
    }
    wprints(4,2,CYAN|_BLUE,"Area Type       : ");
    sprintf(temp,"%d  ",i);
    wprints(4,25,CYAN|_BLUE,temp);
    wprints(4,25,CYAN|_BLUE,temp_ch);
	return(0);
}
int check_dir(char *chek_dir)
{
    char temp[80],ch;
    char current_dir[161];
    int length;
    getcwd(current_dir,160);
    strtrim(chek_dir);
    if (chek_dir[0]==0)
        return (0);
    strcpy(temp,chek_dir);
    length=strlen(temp);
    if (temp[length-1]!='\\')
        strcat(temp,"\\");
    strcpy(chek_dir,temp);
    strcpy(thisarea.directory,temp);
    length=strlen(temp);
    temp[length-1]=0;
    if (chdir(temp))
    {
        wopen(10,10,12,70,1,YELLOW|_RED,WHITE|_RED);
        DoShadow();
        if (!quiet)
           beep();
        wcenters(0,WHITE|_RED,"Directory Does Not Exist!  Shall I Create It?");
	wgotoxy(0,58);
        ch=wgetchf("YyNn",'N');
        if (upcase(ch)=='Y')
        {
            if (mkdir(temp))
            {
                wperror("Error:  Couldn't Create Outbound Path!");
                wclose();
                chdir(current_dir);
                return(length);
            }
        }
        else
	{
            wclose();
            chdir(current_dir);
            return(length);
        }

	wclose();
    }
    chdir(current_dir);
    return(0);
}
void edit_area(int areanum)
{
    char temp[5],temp1[10],temp2[10],temp3[10],temp4[10],temp5[10],temp6[10],temp7[10],temp8[10];
    char s[80];
    int esc;
    sprintf(s,"Edit Area # %d",areanum);
    read_area(areanum,&thisarea);
    wclear();
    showcur();
    wcenters(0,WHITE|_BLUE,s);

    wmessage(" F1 For Editing Help ",1,7,LIGHTRED|_BLUE);
    itoa(thisarea.net_num,temp,10);
    itoa(thisarea.type,temp7,10);
    sprintf(temp1,"%s",thisarea.alias_ok?"Y":"N");
    sprintf(temp2,"%s",thisarea.pass_color?"Y":"N");
    sprintf(temp3,"%s",thisarea.high_ascii?"Y":"N");
    if (thisarea.group)
        sprintf(temp4,"%c",thisarea.group);
    else
        strcpy(temp4," ");
    sprintf(temp5,"%s",thisarea.keep_tag?"Y":"N");
    sprintf(temp6,"%s",thisarea.translate?"Y":"N");
    sprintf(temp8,"%s",thisarea.val_incoming?"Y":"N");
    switch (thisarea.type)
    {
        case 0:
            strcpy(temp_ch,"Passthru            ");
            break;
        case 1:
            strcpy(temp_ch,"Import To WWIV      ");
            break;
        case 2:
            strcpy(temp_ch,"Write As *.MSG Files");
            break;
    }
    wprints(4,25,CYAN|_BLUE,temp_ch);
    winpbeg(CYAN|_BLUE,LIGHTCYAN|_BLUE);
    winpdef(1,2,thisarea.name,"'Area Name       : '????????????????????????????????????????",'U',2,NULL,0);
    winpfba(show_hint38,clear_hint);
    winpdef(2,2,thisarea.comment,"'Area Comment    : '????????????????????????????????????",'0',2,NULL,0);
    winpfba(show_hint39,clear_hint);
    winpdef(3,2,thisarea.directory,"'Area Directory  : '????????????????????????????????????????????????????",'U',2,check_dir,0);
    winpfba(show_hint94,clear_hint);
    winpdef(4,2,temp7,"'Area Type       : '<012>?",0,2,show_type,0);
    winpfba(show_hint95,clear_hint);
    winpdef(6,2,temp4,"'Area Group      : '<ABCDEFGHIJ>?",'U',2,NULL,0);
    winpfba(show_hint40,clear_hint);
    winpdef(7,2,thisarea.subtype,"'WWIV Subtype    : '???????",'U',2,NULL,0);
    winpfba(show_hint41,clear_hint);
    winpdef(7,40,temp,"'WWIV Net Number : '???",'U',2,NULL,0);
    winpfba(show_hint42,clear_hint);
    winpdef(8,2,temp6,"'Use Trans. Table: '???",'U',2,NULL,0);
    winpfba(show_hint81,clear_hint);
    winpdef(8,40,temp1,"'Use Alias?      : 'Y",'U',2,NULL,0);
    winpfba(show_hint43,clear_hint);
    winpdef(9,2,temp2,"'Use WWIV Color? : 'Y",'U',2,NULL,0);
    winpfba(show_hint44,clear_hint);
    winpdef(9,40,temp3,"'Use High ASCII? : 'Y",'U',2,NULL,0);
    winpfba(show_hint45,clear_hint);
    winpdef(10,2,temp5,"'Keep BBS Tag?   : 'Y",'U',2,NULL,0);
    winpfba(show_hint61,clear_hint);
    winpdef(10,40,temp8,"'Valid. Incoming?: 'Y",'U',2,NULL,0);
    winpfba(show_hint99,clear_hint);
    winpdef(16,2,thisarea.origin,"'Origin Line     : '?????????????????????????????????????????????????????????",'0',2,NULL,0);
    winpfba(show_hint46,clear_hint);
    esc=winpread();
    if (!esc)
    {
        show_hint47();
        export_setup(areanum);
    }
    strtrim(temp);
    strtrim(temp1);
    strtrim(temp2);
    strtrim(temp3);
    strtrim(temp4);
    strtrim(temp5);
    strtrim(temp6);
    strtrim(temp7);
    strtrim(temp8);
    strtrim(temp_ch);
    thisarea.group=temp4[0];
    thisarea.net_num=atoi(temp);
    thisarea.type=atoi(temp7);
    if (temp1[0]=='Y')
    thisarea.alias_ok=1;
    else
    thisarea.alias_ok=0;
    if (temp2[0]=='Y')
    thisarea.pass_color=1;
    else
    thisarea.pass_color=0;
    if (temp3[0]=='Y')
    thisarea.high_ascii=1;
    else
        thisarea.high_ascii=0;
    if (temp5[0]=='Y')
        thisarea.keep_tag=1;
    else
        thisarea.keep_tag=0;
    if (temp6[0]=='Y')
        thisarea.translate=1;
    else
        thisarea.translate=0;
    if (temp8[0]=='Y')
        thisarea.val_incoming=1;
    else
        thisarea.val_incoming=0;

    strtrim(thisarea.name);
    strtrim(thisarea.comment);
    strtrim(thisarea.subtype);
    if (strlen(thisarea.subtype)>8)
        thisarea.subtype[8]=0;
    strtrim(thisarea.origin);
    strtrim(thisarea.directory);
    if (thisarea.type!=2)
        strcpy(thisarea.directory,"");
    if (thisarea.type>2)
        thisarea.type=0;
    hidecur();
    show_area_screen(areanum);
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,"Arrow Keys to Move, [ENTER] to Edit, ESC to Exit.  F2 for Global Options");
    write_area(areanum,&thisarea);
}

void show_archivers(void)
{
    int i,pos;
    char s[15];
    wclear();
    wcenters(0,WHITE|_BLUE,"Archivers");
    for (i=0;i<=NUM_ARCHIVERS;i++)
    {
        sprintf(s,"%-2d   %-5s",i,cfg.archive[i].name);
        pos=i+2;
        wprints(pos,5,LIGHTCYAN|_BLUE,s);
    }
    wprints(nodes[current_node].compression+2,4,LIGHTRED|_BLUE,"þ");
}
void choose_archiver(void)
{
    int done,keypress,i;
    int last_pos,curr_pos,last_num;
    char s[35];
    unsigned int keycode;
    done=i=0;
    wopen(6,25,19,45,1,YELLOW|_BLUE,WHITE|_BLUE);
    DoShadow();
    show_archivers();
    nodenum=current_node;
    i=0;
    last_pos=2;
    curr_pos=2;
    last_num=1;
    do
    {
        last_pos=curr_pos;
        curr_pos=i+2;
        sprintf(s,"%-2d   %-5s",last_num,cfg.archive[last_num].name);

        last_num=i;
        wprints(last_pos,5,LIGHTCYAN|_BLUE,s);
        sprintf(s,"%-2d   %-5s",i,cfg.archive[i].name);

        wprints(curr_pos,5,WHITE|_GREEN,s);
        keycode=getxch();
        keypress=keycode>>8;
        switch (keypress)
        {
            case 1: /* ESC */
                done=1;
                break;
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
                i=keypress-1;
            case 28: /* Enter */
                nodes[nodenum].compression=i;
                wclose();
                wprints(11,2,CYAN|_BLUE,"Compression     : ");
                wprints(11,20,CYAN|_BLUE,cfg.archive[nodes[nodenum].compression].name);
                done=1;
                return;
                break;
            case 71: /* Home */
                i=0;
                break;
            case 72: /* Up Arrow */
                i--;
                if (i<0)
                    i=NUM_ARCHIVERS;
                break;
            case 79: /* End */
                i=NUM_ARCHIVERS;
                break;
            case 80: /* Down Arrow */
                i++;
                if (i>NUM_ARCHIVERS)
                    i=0;
                break;
        }

    } while (!done);

    wclose();

}

void show_status(void)
{
    char s[80];
    wprints(10,2,LIGHTCYAN|_BLUE,"Archive Status  : ");
    switch(nodes[current_node].archive_status)
    {
        case 0:
            strcpy(s,"NONE  ");
            break;
        case 1:
            strcpy(s,"HOLD  ");
            break;
        case 2:
            strcpy(s,"CRASH ");
            break;
        default:
            strcpy(s,"NONE ");
            nodes[current_node].archive_status=0;
            break;
    }
    wprints(10,20,LIGHTCYAN|_BLUE,s);
}
void choose_status(void)
{
    char ch;
    int i,i1;

    show_status();
    i=i1=nodes[current_node].archive_status;
    ch=0;
    while ((ch!=13) && (ch!=27))
    {
        show_status();
        ch=getch();
        if (ch==32)
        {
            i++;
            if (i>2)
                i=0;
            nodes[current_node].archive_status=i;
        }
    }
    if (ch==27)
        nodes[current_node].archive_status=i1;
    show_status();

}
void show_packet(void)
{
    char s[80];
    wprints(12,2,LIGHTCYAN|_BLUE,"Packet Type     : ");
    switch(nodes[current_node].packet_type)
    {
        case 0:
            strcpy(s,"2.0 (Stoneage)");
            break;
        case 1:
            strcpy(s,"2.+ (FSC-0039)");
            break;
        case 2:
            strcpy(s,"2.N (FSC-0048)");
            break;
        case 3:
            strcpy(s,"2.2 (FSC-0045)");
            break;
        default:
            strcpy(s,"2.0 (Stoneage)");
            nodes[nodenum].packet_type=0;
            break;
    }
    wprints(12,20,LIGHTCYAN|_BLUE,s);
}
void choose_packet_type(void)
{
    char ch;
    int i,i1;

    show_packet();
    i=i1=nodes[current_node].packet_type;
    ch=0;
    while ((ch!=13) && (ch!=27))
    {
        show_packet();
        ch=getch();
        if (ch==32)
        {
            i++;
            if (i>3)
                i=0;
            nodes[current_node].packet_type=i;
        }
    }
    if (ch==27)
        nodes[current_node].packet_type=i1;
    show_packet();

}
void choose_group(void)
{
    int done,keypress,i;
    int last_pos,curr_pos,last_num;
    char s[85];
    unsigned int keycode;
    done=i=0;
    last_pos=2;
    curr_pos=2;
    last_num=0;
         hidecur();
    wopen(6,25,20,58,1,YELLOW|_BLUE,WHITE|_BLUE);
    DoShadow();
    show_groups();
    i=0;
    do
    {
        last_pos=curr_pos;
        curr_pos=i+2;
        sprintf(s,"%-2s - %-10s",cfg.group[last_num].group_id,cfg.group[last_num].group_name);
        last_num=i;
        wprints(last_pos,5,YELLOW|_BLUE,s);
        sprintf(s,"%-2s - %-10s",cfg.group[i].group_id,cfg.group[i].group_name);
        last_num=i;
        wprints(curr_pos,5,WHITE|_GREEN,s);
        keycode=getxch();
        keypress=keycode>>8;
        switch (keypress)
        {
            case 1: /* ESC */
                done=1;
                break;
            case 28: /* Enter */
               thisarea.group=i+65;
               sprintf(s,"%c",thisarea.group);
               wprints(4,20,LIGHTCYAN|_BLUE,s);
                done=1;
                break;
            case 71: /* Home */
                i=0;
                break;
            case 72: /* Up Arrow */
                i--;
                if (i<0)
                    i=9;
                break;
            case 79: /* End */
                i=10;
                break;
            case 80: /* Down Arrow */
                i++;
                if (i>=10)
                    i=0;
                break;
        }

    } while (!done);

    wclose();
}

void sort_export(void)
{
    int tzone,tnet,tnode,tpoint;
    int i,j,k;

    for (k=0;k<MAX_NODES;k++)
    {
        for (i=0;i<MAX_NODES;i++)
        {
            if ((thisarea.feed[i].zone==999) || (thisarea.feed[i].net==0))
            {
                for (j=i;j<MAX_NODES-1;j++)
                {
                    thisarea.feed[j].zone=thisarea.feed[j+1].zone;
                    thisarea.feed[j].net=thisarea.feed[j+1].net;
                    thisarea.feed[j].node=thisarea.feed[j+1].node;
                    thisarea.feed[j].point=thisarea.feed[j+1].point;
                }
            }
        }
    }
    for (k=0;k<MAX_NODES;k++)
        if (thisarea.feed[k].zone==0)
            thisarea.feed[k].zone=999;
    for (k=0;k<MAX_NODES;k++)
    {
        for (j=0;j<MAX_NODES;j++)
        {
            if (thisarea.feed[k].zone<thisarea.feed[j].zone)
            {
                tzone=thisarea.feed[j].zone;
                tnet=thisarea.feed[j].net;
                tnode=thisarea.feed[j].node;
                tpoint=thisarea.feed[j].point;
                thisarea.feed[j].zone=thisarea.feed[k].zone;
                thisarea.feed[j].net=thisarea.feed[k].net;
                thisarea.feed[j].node=thisarea.feed[k].node;
                thisarea.feed[j].point=thisarea.feed[k].point;
                thisarea.feed[k].zone=tzone;
                thisarea.feed[k].net=tnet;
                thisarea.feed[k].node=tnode;
                thisarea.feed[k].point=tpoint;
            }
            if (thisarea.feed[k].zone==thisarea.feed[j].zone)
            {
                if (thisarea.feed[k].net<thisarea.feed[j].net)
                {
                    tzone=thisarea.feed[j].zone;
                    tnet=thisarea.feed[j].net;
                    tnode=thisarea.feed[j].node;
                    tpoint=thisarea.feed[j].point;
                    thisarea.feed[j].zone=thisarea.feed[k].zone;
                    thisarea.feed[j].net=thisarea.feed[k].net;
                    thisarea.feed[j].node=thisarea.feed[k].node;
                    thisarea.feed[j].point=thisarea.feed[k].point;
                    thisarea.feed[k].zone=tzone;
                    thisarea.feed[k].net=tnet;
                    thisarea.feed[k].node=tnode;
                    thisarea.feed[k].point=tpoint;
                }
                if (thisarea.feed[k].net==thisarea.feed[j].net)
                {
                    if (thisarea.feed[k].node<thisarea.feed[j].node)
                    {
                        tzone=thisarea.feed[j].zone;
                        tnet=thisarea.feed[j].net;
                        tnode=thisarea.feed[j].node;
                        tpoint=thisarea.feed[j].point;
                        thisarea.feed[j].zone=thisarea.feed[k].zone;
                        thisarea.feed[j].net=thisarea.feed[k].net;
                        thisarea.feed[j].node=thisarea.feed[k].node;
                        thisarea.feed[j].point=thisarea.feed[k].point;
                        thisarea.feed[k].zone=tzone;
                        thisarea.feed[k].net=tnet;
                        thisarea.feed[k].node=tnode;
                        thisarea.feed[k].point=tpoint;
                    }
                    if (thisarea.feed[k].node==thisarea.feed[j].node)
                        {
                        if (thisarea.feed[k].point<thisarea.feed[j].point)
                        {
                            tzone=thisarea.feed[j].zone;
                            tnet=thisarea.feed[j].net;
                            tnode=thisarea.feed[j].node;
                            tpoint=thisarea.feed[j].point;
                            thisarea.feed[j].zone=thisarea.feed[k].zone;
                            thisarea.feed[j].net=thisarea.feed[k].net;
                            thisarea.feed[j].node=thisarea.feed[k].node;
                            thisarea.feed[j].point=thisarea.feed[k].point;
                            thisarea.feed[k].zone=tzone;
                            thisarea.feed[k].net=tnet;
                            thisarea.feed[k].node=tnode;
                            thisarea.feed[k].point=tpoint;
                        }
                    }
                }
            }
        }
    }
}

void exp_node(void)
{
    int i,length,j,ok;
    char s[80],fn[80],ch,s1[80];

    FILE *fp;

    ok=0;

    do
    {
        wopen(8,5,11,75,1,YELLOW|_RED,YELLOW|_RED);
        wcenters(0,WHITE|_RED,"File Name To Write To");
        DoShadow();
        wfill(1,5,1,63,'±',LCYAN|_RED);
        wwprints(window_1,24,1,LIGHTCYAN|_BLUE,"Name of File to Export Node List To");
        showcur();
        wgotoxy(1,5);
        winputsf(fn,"!U!????????????????????????????????????????");
        hidecur();
        wclose();
        if (fn[0]==0)
            return;
        if ( (stricmp(fn,"AREAS.DAT")==0) ||
             (stricmp(fn,"WWIVTOSS.DAT")==0) ||
             (stricmp(fn,"NODES.DAT")==0) ||
             (stricmp(fn,"XLAT.DAT")==0) ||
             (stricmp(fn,"DOMAINS.DAT")==0))
            {
                wperror("Error!  That filename is a data file of WWIVTOSS!");
                return;
            }
        if (exist(fn))
        {
            if (!quiet)
                beep();
            wopen(7,10,10,70,1,WHITE|_RED,YELLOW|_RED);
            DoShadow();
            wcenters(0,BLINK|YELLOW|_RED,"Warning!  File Already Exists!  Overwrite?");
            wgotoxy(1,30);
            showcur();
            ch=wgetc();
            hidecur();
            if (upcase(ch)==89)
            {
                ok=1;
                wclose();
            }
            else
            {
                ok=0;
                wclose();
            }

        } else
            ok=1;
    } while (!ok);

    if ((fp = fopen(fn,"w+"))==NULL)
    {
        wperror("Couldn't Open File!");
        return;
    }
    wopen(14,24,16,46,1,WHITE|_RED,YELLOW|_RED);
    DoShadow();
    wcenters(0,BLINK|WHITE|_RED,"  Writing...  ");

    sprintf(s1,"Node Manager Configuration For %s\n",cfg.bbs_name);
    length=strlen(s1);
    j=40-(length/2);
    strcpy(s,"");
    for (i=0;i<j;i++)
	strcat(s," ");
    strcat(s,s1);
    fputs(s,fp);
    sprintf(s1,"Created by WWIVTOSS v.%s\n",VERSION);
    length=strlen(s1);
    j=40-(length/2);
    strcpy(s,"");
    for (i=0;i<j;i++)
	strcat(s," ");
    strcat(s,s1);
    fputs(s,fp);
    sprintf(s1,"%d Nodes Total\n",num_nodes);
    length=strlen(s1);
    j=40-(length/2);
    strcpy(s,"");
    for (i=0;i<j;i++)
	strcat(s," ");
    strcat(s,s1);
    fputs(s,fp);

    for (i=0;i<79;i++)
        fputs("=",fp);
    fputs("\n",fp);
    for (i=1;i<=num_nodes;i++)
    {
        nodenum=i;
        sprintf(s,"Node # %d\n",nodenum);
        fputs(s,fp);
        sprintf(s,"Node Address     : %s\n",nodes[nodenum].address);
        fputs(s,fp);
        sprintf(s,"Node SysOp       : %s\n",nodes[nodenum].sysop_name);
        fputs(s,fp);
        sprintf(s,"Packet Password  : %s\n",nodes[nodenum].pkt_pw);
        fputs(s,fp);
        sprintf(s,"Areafix Password : %s\n",nodes[nodenum].areafix_pw);
        fputs(s,fp);
        sprintf(s,"Archive Status   : ");
        switch(nodes[nodenum].archive_status)
        {
            case 0:
                strcat(s,"NONE\n");
                break;
            case 1:
                strcat(s,"HOLD\n");
                break;
            case 2:
                strcat(s,"CRASH\n");
                break;
            default:
                strcat(s,"NONE\n");
                break;
        }
        fputs(s,fp);
        sprintf(s,"Compression      : %s\n",cfg.archive[nodes[nodenum].compression].name);
        fputs(s,fp);
        for (j=0;j<40;j++)
            fputs("=",fp);
        fputs("\n\n",fp);

	}
    sprintf(s,"\n  Listed %d Nodes!\n",num_nodes);
    fputs(s,fp);

    fclose(fp);
    wclear();
    wcenters(0,BLINK|WHITE|_RED,"Finished!");
    if (!quiet)
        beep();
    waitkeyt(10);
    wclose();
    show_hint63();
}
void exp_area(void)
{
    int i,length,j,ok,temp_length,areanum,prev_zone,prev_node,prev_net,count;
    char s[80],fn[80],ch,s1[80],temp[50];
    FILE *fp;

    ok=count=0;

    do
    {
        wopen(8,5,11,75,1,YELLOW|_RED,YELLOW|_RED);
        wcenters(0,WHITE|_RED,"File Name To Write To");
        DoShadow();
        wfill(1,5,1,63,'±',LCYAN|_RED);
        wwprints(window_1,24,1,LIGHTCYAN|_BLUE,"Name of File to Export Node List To");
        showcur();
        wgotoxy(1,5);
        winputsf(fn,"!U!????????????????????????????????????????");
        hidecur();
        wclose();
        if (fn[0]==0)
            return;
        if ( (stricmp(fn,"AREAS.DAT")==0) ||
             (stricmp(fn,"WWIVTOSS.DAT")==0) ||
             (stricmp(fn,"NODES.DAT")==0) ||
             (stricmp(fn,"XLAT.DAT")==0) ||
             (stricmp(fn,"DOMAINS.DAT")==0))
            {
                wperror("Error!  That filename is a data file of WWIVTOSS!");
                return;
            }
        if (exist(fn))
        {
            if (!quiet)
                beep();
            wopen(7,10,10,70,1,WHITE|_RED,YELLOW|_RED);
            DoShadow();
            wcenters(0,BLINK|YELLOW|_RED,"Warning!  File Already Exists!  Overwrite?");
            wgotoxy(1,30);
            showcur();
            ch=wgetc();
            hidecur();
            if (upcase(ch)==89)
            {
                ok=1;
                wclose();
            }
            else
            {
                ok=0;
                wclose();
            }

        } else
            ok=1;
    } while (!ok);
    wopen(7,20,10,60,1,WHITE|_RED,YELLOW|_RED);
            DoShadow();
    wcenters(0,YELLOW|_RED,"What Group? ");
    wcenters(1,WHITE|_RED,"<SPACE> Selects ALL Groups!");
    wgotoxy(0,25);
    showcur();
    ch=wgetc();
    hidecur();
    wclose();
    if ((fp = fopen(fn,"w+"))==NULL)
    {
        wperror("Couldn't Open File!");
        return;
    }
    wopen(14,24,16,46,1,WHITE|_RED,YELLOW|_RED);
    DoShadow();
    wcenters(0,BLINK|WHITE|_RED,"  Writing...  ");

    sprintf(s1,"Area Manager Configuration For %s\n",cfg.bbs_name);
    length=strlen(s1);
    j=40-(length/2);
    strcpy(s,"");
    for (i=0;i<j;i++)
	strcat(s," ");
    strcat(s,s1);
    fputs(s,fp);
    sprintf(s1,"Created by WWIVTOSS v.%s\n",VERSION);
    length=strlen(s1);
    j=40-(length/2);
    strcpy(s,"");
    for (i=0;i<j;i++)
	strcat(s," ");
    strcat(s,s1);
    fputs(s,fp);
    if (ch==32)
        sprintf(s1,"ALL Areas\n");
    else
        sprintf(s1,"Areas In Group '%c'\n",upcase(ch));
    length=strlen(s1);
    j=40-(length/2);
    strcpy(s,"");
    for (i=0;i<j;i++)
	strcat(s," ");
    strcat(s,s1);
    fputs(s,fp);

    for (i=0;i<79;i++)
        fputs("=",fp);
    fputs("\n",fp);
    for (i=1;i<=num_areas;i++)
    {
        read_area(i,&thisarea);
        if ((thisarea.group==upcase(ch)) || (ch==32))
        {
            count++;
            areanum=i;
            sprintf(s,"Area # %d\n\n",areanum);
            fputs(s,fp);
            sprintf(s,"Area Name       : %s\n",thisarea.name);
            fputs(s,fp);
            sprintf(s,"Area Comment    : %s\n",thisarea.comment);
            fputs(s,fp);
            if (thisarea.group)
                sprintf(temp,"%c - %s",thisarea.group,cfg.group[thisarea.group-65].group_name);
            else
                strcpy(temp," ");
            sprintf(s,"Area Group      : %s\n",temp);
            fputs(s,fp);
            sprintf(s,"WWIV Subtype    : %s\n",thisarea.subtype);
            fputs(s,fp);
            sprintf(s,"WWIV Net Number : %d\n",thisarea.net_num);
            fputs(s,fp);
            sprintf(s,"Use Alias?      : %s\n",thisarea.alias_ok?"Y":"N");
            fputs(s,fp);
            sprintf(s,"Use WWIV Color? : %s\n",thisarea.pass_color?"Y":"N");
            fputs(s,fp);
            sprintf(s,"Use High ASCII? : %s\n",thisarea.high_ascii?"Y":"N");
            fputs(s,fp);
            sprintf(s,"Keep BBS Tag?   : %s\n",thisarea.keep_tag?"Y":"N");
            fputs(s,fp);
            sprintf(s,"Use Trans. Table: %s\n",thisarea.translate?"Y":"N");
            fputs(s,fp);
            sprintf(s,"Origin Line     : %s\n",thisarea.origin);
            fputs(s,fp);
            sprintf(s,"Export To Nodes : ");
            temp_length=0;
            if ((thisarea.feed[0].zone!=999) && (thisarea.feed[0].zone!=0))
            {
                if (thisarea.feed[0].point==0)
                    sprintf(s1,"%d:%d/%d ",thisarea.feed[0].zone,thisarea.feed[0].net,thisarea.feed[0].node);
                else
                    sprintf(s1,"%d:%d/%d.%d ",thisarea.feed[0].zone,thisarea.feed[0].net,thisarea.feed[0].node,thisarea.feed[0].point);
                prev_zone=thisarea.feed[0].zone;
                prev_net=thisarea.feed[0].net;
                prev_node=thisarea.feed[0].node;
                for (j=1;j<MAX_NODES;j++)
                {
                    if (thisarea.feed[j].zone!=999)
                    {
                        if (thisarea.feed[j].zone!=prev_zone)
                        {
                            if (thisarea.feed[j].point==0)
                                sprintf(temp,"%d:%d/%d ",thisarea.feed[j].zone,thisarea.feed[j].net,thisarea.feed[j].node);
                            else
                                sprintf(temp,"%d:%d/%d.%d ",thisarea.feed[j].zone,thisarea.feed[j].net,thisarea.feed[j].node,thisarea.feed[j].point);
                            prev_zone=thisarea.feed[j].zone;
                            prev_net=thisarea.feed[j].net;
                            prev_node=thisarea.feed[j].node;
                        }
                        else
                        if (thisarea.feed[j].net!=prev_net)
                        {
                            if (thisarea.feed[j].point==0)
                                sprintf(temp,"%d/%d ",thisarea.feed[j].net,thisarea.feed[j].node);
                            else
                                sprintf(temp,"%d/%d.%d ",thisarea.feed[j].net,thisarea.feed[j].node,thisarea.feed[j].point);
                            prev_net=thisarea.feed[j].net;
                            prev_node=thisarea.feed[j].node;
                        }
                        else
                        if (thisarea.feed[j].node!=prev_node)
                        {
                            if (thisarea.feed[j].point!=0)
                                sprintf(temp,"%d.%d ",thisarea.feed[j].node,thisarea.feed[j].point);
                            else
                                sprintf(temp,"%d ",thisarea.feed[j].node);
                            prev_node=thisarea.feed[j].node;
                        }
                        else
                            sprintf(temp,".%d ",thisarea.feed[j].point);
                        temp_length+=strlen(temp);
                        if (temp_length>=35)
                        {
                            strcat(s,s1);
                            strcat(s,"\n");
                            fputs(s,fp);
                            sprintf(s,"                  ");
                            temp_length=0;
			    strcpy(s1,"");
                        }
                        strcat(s1,temp);
                    }
                }
                strcat(s,s1);
                strcat(s,"\n");
                fputs(s,fp);

            }

            for (j=0;j<40;j++)
                fputs("=",fp);
            fputs("\n\n",fp);
        }

	}
    sprintf(s,"\n  Listed %d out of %d Areas!\n",count,num_areas);
    fputs(s,fp);

    fclose(fp);
    wclear();
    wcenters(0,BLINK|WHITE|_RED,"Finished!");
    if (!quiet)
        beep();
    waitkeyt(10);
    wclose();
    show_hint64();
}
void exp_group(void)
{
    int i,length,j,ok;
    char s[80],fn[80],ch,s1[80];
    FILE *fp;

    ok=0;

    do
    {
        wopen(8,5,11,75,1,YELLOW|_RED,YELLOW|_RED);
        wcenters(0,WHITE|_RED,"File Name To Write To");
        DoShadow();
        wfill(1,5,1,63,'±',LCYAN|_RED);
        wwprints(window_1,24,1,LIGHTCYAN|_BLUE,"Name of File to Export Group List To");
        showcur();
        wgotoxy(1,5);
        winputsf(fn,"!U!????????????????????????????????????????");
        hidecur();
        wclose();
             if (fn[0]==0)
            return;
        if ( (stricmp(fn,"AREAS.DAT")==0) ||
             (stricmp(fn,"WWIVTOSS.DAT")==0) ||
             (stricmp(fn,"NODES.DAT")==0) ||
             (stricmp(fn,"XLAT.DAT")==0) ||
             (stricmp(fn,"DOMAINS.DAT")==0))
            {
                wperror("Error!  That filename is a data file of WWIVTOSS!");
                return;
            }
       if (exist(fn))
        {
            if (!quiet)
                beep();
            wopen(7,10,10,70,1,WHITE|_RED,YELLOW|_RED);
            DoShadow();
            wcenters(0,BLINK|YELLOW|_RED,"Warning!  File Already Exists!  Overwrite?");
            wgotoxy(1,30);
            showcur();
            ch=wgetc();
            hidecur();
            if (upcase(ch)==89)
            {
                ok=1;
                wclose();
            }
            else
            {
                ok=0;
                wclose();
            }

        } else
            ok=1;
    } while (!ok);

    if ((fp = fopen(fn,"w+"))==NULL)
    {
        wperror("Couldn't Open File!");
        return;
    }
    wopen(14,24,16,46,1,WHITE|_RED,YELLOW|_RED);
    DoShadow();
    wcenters(0,BLINK|WHITE|_RED,"  Writing...  ");

    sprintf(s1,"Group Listing for %s\n",cfg.bbs_name);
    length=strlen(s1);
    j=40-(length/2);
    strcpy(s,"");
    for (i=0;i<j;i++)
	strcat(s," ");
    strcat(s,s1);
    fputs(s,fp);
    sprintf(s1,"Created by WWIVTOSS v.%s\n",VERSION);
    length=strlen(s1);
    j=40-(length/2);
    strcpy(s,"");
    for (i=0;i<j;i++)
	strcat(s," ");
    strcat(s,s1);
    fputs(s,fp);
    sprintf(s1,"Group Listing\n");
    length=strlen(s1);
    j=40-(length/2);
    strcpy(s,"");
    for (i=0;i<j;i++)
	strcat(s," ");
    strcat(s,s1);
    fputs(s,fp);
    for (i=0;i<79;i++)
        fputs("=",fp);

    fputs("\n",fp);
    for (i=0;i<10;i++)
    {
        sprintf(s,"  %-2s - %s\n",cfg.group[i].group_id,cfg.group[i].group_name);
        fputs(s,fp);
	}

    fclose(fp);
    wclear();
    wcenters(0,BLINK|WHITE|_RED,"Finished!");
    if (!quiet)
        beep();
    waitkeyt(10);
    wclose();
    show_hint65();

}
void exp_list(void)
{
    int i,length,j,ok,include_subtype,count;
    char s[80],fn[80],ch,s1[80],ch1;

    FILE *fp;

    ok=count=0;
    include_subtype=0;

    do
    {
        wopen(8,5,11,75,1,YELLOW|_RED,YELLOW|_RED);
        wcenters(0,WHITE|_RED,"File Name To Write To");
        DoShadow();
        wfill(1,5,1,63,'±',LCYAN|_RED);
        wwprints(window_1,24,1,LIGHTCYAN|_BLUE,"Name of File to Export Area List To");
        showcur();
        wgotoxy(1,5);
        winputsf(fn,"!U!????????????????????????????????????????");
        hidecur();
        wclose();
        if (fn[0]==0)
            return;
        if ( (stricmp(fn,"AREAS.DAT")==0) ||
             (stricmp(fn,"WWIVTOSS.DAT")==0) ||
             (stricmp(fn,"NODES.DAT")==0) ||
             (stricmp(fn,"XLAT.DAT")==0) ||
             (stricmp(fn,"DOMAINS.DAT")==0))
            {
                wperror("Error!  That filename is a data file of WWIVTOSS!");
                return;
            }
        if (exist(fn))
        {
            if (!quiet)
                beep();
            wopen(7,10,10,70,1,WHITE|_RED,YELLOW|_RED);
            DoShadow();
            wcenters(0,BLINK|YELLOW|_RED,"Warning!  File Already Exists!  Overwrite?");
            wgotoxy(1,30);
            showcur();
            ch=wgetc();
            hidecur();
            if (upcase(ch)==89)
            {
                ok=1;
                wclose();
            }
            else
            {
                ok=0;
                wclose();
            }

        } else
            ok=1;
    } while (!ok);
    wopen(7,20,10,60,1,WHITE|_RED,YELLOW|_RED);
    DoShadow();
    wcenters(0,YELLOW|_RED,"What Group? ");
    wcenters(1,WHITE|_RED,"<SPACE> Selects ALL Groups!");
    wgotoxy(0,25);
    showcur();
    ch=wgetc();
    hidecur();
    wclose();
    wopen(7,20,9,60,1,WHITE|_RED,YELLOW|_RED);
    DoShadow();
    wcenters(0,YELLOW|_RED,"Include Subtype? ");
    wgotoxy(0,30);
    showcur();
    ch1=wgetc();
    hidecur();
    wclose();
    if (upcase(ch1)=='Y')
        include_subtype=1;

    if ((fp = fopen(fn,"w+"))==NULL)
    {
        wperror("Couldn't Open File!");
        return;
    }
    wopen(14,24,16,46,1,WHITE|_RED,YELLOW|_RED);
    DoShadow();
    wcenters(0,BLINK|WHITE|_RED,"  Writing...  ");

    sprintf(s1,"Area Listing for %s\n",cfg.bbs_name);
    length=strlen(s1);
    j=40-(length/2);
    strcpy(s,"");
    for (i=0;i<j;i++)
	strcat(s," ");
    strcat(s,s1);
    fputs(s,fp);
    sprintf(s1,"Created by WWIVTOSS v.%s\n",VERSION);
    length=strlen(s1);
    j=40-(length/2);
    strcpy(s,"");
    for (i=0;i<j;i++)
	strcat(s," ");
    strcat(s,s1);
    fputs(s,fp);
    if (ch==32)
        sprintf(s1,"ALL Areas");
    else
        sprintf(s1,"Areas In Group '%c'",upcase(ch));

    if (include_subtype)
        strcat(s1," With Subtypes\n");
    else
        strcat(s1,"\n");
    length=strlen(s1);
    j=40-(length/2);
    strcpy(s,"");
    for (i=0;i<j;i++)
	strcat(s," ");
    strcat(s,s1);
    fputs(s,fp);
    for (i=0;i<79;i++)
        fputs("=",fp);

    fputs("\n",fp);
    if (include_subtype)
    {
        sprintf(s,"  %-50s   %s\n","Area Name","Area Subtype");
        fputs(s,fp);
        sprintf(s,"  %-50s   %s\n","=========","============");
        fputs(s,fp);

    }
    for (i=1;i<=num_areas;i++)
    {
        read_area(i,&thisarea);
        if ((thisarea.group==upcase(ch)) || (ch==32))
        {
            if (include_subtype)
           sprintf(s,"  %-50s   %-10s\n",thisarea.name,thisarea.subtype);
            else
                sprintf(s,"  %s\n",thisarea.name);
            fputs(s,fp);
            count++;
        }
    }
    sprintf(s,"\n  Listed %d out of %d Areas!\n",count,num_areas);
    fputs(s,fp);

    fclose(fp);
    wclear();
    wcenters(0,BLINK|WHITE|_RED,"Finished!");
    if (!quiet)
        beep();
    waitkeyt(10);
    wclose();
    show_hint66();
}
void exp_subtype(void)
{
    int i,length,j,ok,count;
    char s[80],fn[80],ch,s1[80];
    FILE *fp;

    ok=count=0;

    do
    {
        wopen(8,5,11,75,1,YELLOW|_RED,YELLOW|_RED);
        wcenters(0,WHITE|_RED,"File Name To Write To");
        DoShadow();
        wfill(1,5,1,63,'±',LCYAN|_RED);
        wwprints(window_1,24,1,LIGHTCYAN|_BLUE,"Name of File to Export Area List (sorted by WWIV Subtype) To");
        showcur();
        wgotoxy(1,5);
        winputsf(fn,"!U!????????????????????????????????????????");
        hidecur();
        wclose();
        if (fn[0]==0)
            return;
        if ( (stricmp(fn,"AREAS.DAT")==0) ||
             (stricmp(fn,"WWIVTOSS.DAT")==0) ||
             (stricmp(fn,"NODES.DAT")==0) ||
             (stricmp(fn,"XLAT.DAT")==0) ||
             (stricmp(fn,"DOMAINS.DAT")==0))
            {
                wperror("Error!  That filename is a data file of WWIVTOSS!");
                return;
            }
        if (exist(fn))
        {
            if (!quiet)
                beep();
            wopen(7,10,10,70,1,WHITE|_RED,YELLOW|_RED);
            DoShadow();
            wcenters(0,BLINK|YELLOW|_RED,"Warning!  File Already Exists!  Overwrite?");
            wgotoxy(1,30);
            showcur();
            ch=wgetc();
            hidecur();
            if (upcase(ch)==89)
            {
                ok=1;
                wclose();
            }
            else
            {
                ok=0;
                wclose();
            }

        } else
            ok=1;
    } while (!ok);
    wopen(7,20,10,60,1,WHITE|_RED,YELLOW|_RED);
    DoShadow();
    wcenters(0,YELLOW|_RED,"What Group? ");
    wcenters(1,WHITE|_RED,"<SPACE> Selects ALL Groups!");
    wgotoxy(0,25);
    showcur();
    ch=wgetc();
    hidecur();
    wclose();

    if ((fp = fopen(fn,"w+"))==NULL)
    {
        wperror("Couldn't Open File!");
        return;
    }
    wopen(14,24,16,46,1,WHITE|_RED,YELLOW|_RED);
    DoShadow();
    wcenters(0,BLINK|WHITE|_RED,"  Writing...  ");

    sprintf(s1,"Area Listing for %s\n",cfg.bbs_name);
    length=strlen(s1);
    j=40-(length/2);
    strcpy(s,"");
    for (i=0;i<j;i++)
	strcat(s," ");
    strcat(s,s1);
    fputs(s,fp);
    sprintf(s1,"Sorted by WWIV Subtype\n");
    length=strlen(s1);
    j=40-(length/2);
    strcpy(s,"");
    for (i=0;i<j;i++)
	strcat(s," ");
    strcat(s,s1);
    fputs(s,fp);
    sprintf(s1,"Created by WWIVTOSS v.%s\n",VERSION);
    length=strlen(s1);
    j=40-(length/2);
    strcpy(s,"");
    for (i=0;i<j;i++)
	strcat(s," ");
    strcat(s,s1);
    fputs(s,fp);
    if (ch==32)
        sprintf(s1,"ALL Areas\n");
    else
        sprintf(s1,"Areas In Group '%c'\n",upcase(ch));
    length=strlen(s1);
    j=40-(length/2);
    strcpy(s,"");
    for (i=0;i<j;i++)
	strcat(s," ");
    strcat(s,s1);
    fputs(s,fp);
    for (i=0;i<79;i++)
        fputs("=",fp);

    fputs("\n",fp);
    sprintf(s,"  %-10s   %s\n","SubType","Area Name");
    fputs(s,fp);
    sprintf(s,"  %-10s   %s\n","=======","=========");
    fputs(s,fp);
    /*
    for (i=1;i<num_areas+1;i++)
    {
	for (j=1;j<num_areas+1;j++)
	{
	    if (strcmp(areas[j].subtype,thisarea.subtype)>0)
	    {
		areas[0]=areas[j];
		areas[j]=thisarea;
		thisarea=areas[0];
	    }

	}
    }
    */
    for (i=1;i<=num_areas;i++)
    {
        read_area(i,&thisarea);
        if ((thisarea.group==upcase(ch)) || (ch==32))
        {
            sprintf(s,"  %-10s   %-50s\n",thisarea.subtype,thisarea.name);
            fputs(s,fp);
            count++;
        }
    }
    sprintf(s,"\n  Listed %d out of %d Areas!\n",count,num_areas);
    fputs(s,fp);
    fclose(fp);
    wclear();
    wcenters(0,BLINK|WHITE|_RED,"Finished!");
    if (!quiet)
        beep();
    waitkeyt(10);
    wclose();
    show_hint67();
/*
    for (i=1;i<num_areas+1;i++)
    {
	for (j=1;j<num_areas+1;j++)
	{
	    if (strcmp(areas[j].name,thisarea.name)>0)
	    {
		areas[0]=areas[j];
		areas[j]=thisarea;
		thisarea=areas[0];
	    }

	}
    }
*/
}

void export_mgr(void)
{
    wmenubeg(9,38,15,54,1,CYAN|_BLUE,WHITE|_BLUE,DoShadow);
        wmenuitem(0,0,"  Node Config  ",'N',18,0,exp_node,0,18);
        wmenuiba(show_hint63,clear_hint);
        wmenuitem(1,0,"  Area Config  ",'A',19,0,exp_area,0,19);
        wmenuiba(show_hint64,clear_hint);
        wmenuitem(2,0,"  Area Groups  ",'G',20,0,exp_group,0,20);
        wmenuiba(show_hint65,clear_hint);
        wmenuitem(3,0,"   Echo List   ",'E',21,0,exp_list,0,21);
        wmenuiba(show_hint66,clear_hint);
        wmenuitem(4,0," WWIV Subtypes ",'W',22,0,exp_subtype,0,22);
        wmenuiba(show_hint67,clear_hint);
    wmenuend(18,M_VERT,0,0,YELLOW|_BLUE,LIGHTRED|_BLUE,LCYAN|_BLUE,WHITE|_RED);
    wmenuget();
    show_hint62();
}
void global_group(void)
{
    int i;
    char ch,ch2,ch3;
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,"Select Group To Modify.  Space Selects All");

    wopen(7,30,10,50,1,WHITE|_RED,YELLOW|_RED);
    DoShadow();
    wcenters(0,YELLOW|_RED,"What Group? ");
    wgotoxy(0,15);
    showcur();
    ch=wgetc();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,"Select New Group.  [ESC] Aborts.");
    wcenters(1,YELLOW|_RED,"New Group? ");
    wgotoxy(1,15);
    ch2=wgetc();
    if (ch2==27)
    {
        wclose();
        hidecur();
        show_hint69();
        return;
    }
    hidecur();
    if ( (upcase(ch2)<65) || (upcase(ch2)>74))
        ch3=32;
    else
        ch3=upcase(ch2);
    for (i=0;i<=num_areas;i++)
    {
        read_area(i,&thisarea);
        if ((thisarea.group==upcase(ch)) || (ch==32))
        {
            thisarea.group=ch3;
            write_area(i,&thisarea);
        }
    }
    wclose();
    show_hint69();
    update_area_screen(current_area);
}
void global_alias(void)
{
    int i,new_net;
    char ch,ch2;
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,"Select Group To Modify.  Space Selects All");
    wopen(7,30,10,50,1,WHITE|_RED,YELLOW|_RED);
    DoShadow();
    wcenters(0,YELLOW|_RED,"What Group? ");
    wgotoxy(0,16);
    showcur();
    ch=wgetc();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,"'Y' or 'y' To Allow Handles.  Any other choice sets to 'No'.  [ESC] Aborts.");
    wcenters(1,YELLOW|_RED,"Allow Aliases?");
    wgotoxy(1,17);
    ch2=wgetc();
    hidecur();
    if (ch2==27)
    {
        wclose();
        show_hint70();
        return;
    }
    if (upcase(ch2)==89)
        new_net=1;
    else
        new_net=0;
    for (i=0;i<=num_areas;i++)
    {
        read_area(i,&thisarea);
        if ((thisarea.group==upcase(ch)) || (ch==32))
        {
            thisarea.alias_ok=new_net;
            write_area(i,&thisarea);
        }
    }
    wclose();
    show_hint70();
    update_area_screen(current_area);
}
void global_color(void)
{
    int i,new_net;
    char ch,ch2;
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,"Select Group To Modify.  Space Selects All");
    wopen(7,30,10,50,1,WHITE|_RED,YELLOW|_RED);
    DoShadow();
    wcenters(0,YELLOW|_RED,"What Group? ");
    wgotoxy(0,15);
    showcur();
    ch=wgetc();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,"'Y' or 'y' To Allow WWIV Color.  Any other choice sets to 'No'.  [ESC] Aborts.");
    wcenters(1,YELLOW|_RED,"Pass WWIV Color?");
    wgotoxy(1,18);
    ch2=wgetc();
    hidecur();
    if (ch2==27)
    {
        wclose();
        show_hint71();
        return;
    }
    if (upcase(ch2)==89)
        new_net=1;
    else
	new_net=0;
    for (i=0;i<=num_areas;i++)
    {
        read_area(i,&thisarea);
        if ((thisarea.group==upcase(ch)) || (ch==32))
        {
            thisarea.pass_color=new_net;
            write_area(i,&thisarea);
        }
    }
    wclose();
    show_hint71();
    update_area_screen(current_area);
}
void global_ascii(void)
{
    int i,new_net;
    char ch,ch2;
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,"Select Group To Modify.  Space Selects All");
    wopen(7,30,10,50,1,WHITE|_RED,YELLOW|_RED);
    DoShadow();
    wcenters(0,YELLOW|_RED,"What Group? ");
    wgotoxy(0,15);
    showcur();
    ch=wgetc();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,"'Y' or 'y' To Allow High ASCII.  Any other choice sets to 'No'.  [ESC] Aborts.");
    wcenters(1,YELLOW|_RED,"Pass High ASCII?");
    wgotoxy(1,18);
    ch2=wgetc();
    hidecur();
    if (ch2==27)
    {
        wclose();
        show_hint72();
        return;
    }
    if (upcase(ch2)==89)
        new_net=1;
    else
    new_net=0;
    for (i=0;i<=num_areas;i++)
    {
        read_area(i,&thisarea);
        if ((thisarea.group==upcase(ch)) || (ch==32))
        {
            thisarea.high_ascii=new_net;
            write_area(i,&thisarea);
        }
    }

    wclose();
    show_hint72();
    update_area_screen(current_area);
}
void global_valid(void)
{
    int i,new_net;
    char ch,ch2;
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,"Select Group To Modify.  Space Selects All");
    wopen(7,30,10,50,1,WHITE|_RED,YELLOW|_RED);
    DoShadow();
    wcenters(0,YELLOW|_RED,"What Group? ");
    wgotoxy(0,15);
    showcur();
    ch=wgetc();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,"'Y' or 'y' To Validate Incoming Messages.  Any other choice sets to 'No'.  [ESC] Aborts.");
    wcenters(1,YELLOW|_RED,"Validate Incoming?");
    wgotoxy(1,18);
    ch2=wgetc();
    hidecur();
    if (ch2==27)
    {
        wclose();
        show_hint72();
        return;
    }
    if (upcase(ch2)==89)
        new_net=1;
    else
    new_net=0;
    for (i=0;i<=num_areas;i++)
    {
        read_area(i,&thisarea);
        if ((thisarea.group==upcase(ch)) || (ch==32))
        {
            thisarea.val_incoming=new_net;
            write_area(i,&thisarea);
        }
    }

    wclose();
    show_hint100();
    update_area_screen(current_area);
}
void global_tag(void)
{
    int i,new_net;
    char ch,ch2;
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,"Select Group To Modify.  Space Selects All");
    wopen(7,30,10,50,1,WHITE|_RED,YELLOW|_RED);
    DoShadow();
    wcenters(0,YELLOW|_RED,"What Group? ");
    wgotoxy(0,15);
    showcur();
    ch=wgetc();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,"'Y' or 'y' To Keep BBS Tags.  Any other choice sets to 'No'.  [ESC] Aborts.");
    wcenters(1,YELLOW|_RED,"Keep BBS Tags?");
    wgotoxy(1,17);
    ch2=wgetc();
    hidecur();
    if (ch2==27)
    {
        wclose();
        show_hint73();
        return;
    }
    if (upcase(ch2)==89)
        new_net=1;
    else
        new_net=0;
    for (i=0;i<=num_areas;i++)
    {
        read_area(i,&thisarea);
        if ((thisarea.group==upcase(ch)) || (ch==32))
        {
            thisarea.keep_tag=new_net;
            write_area(i,&thisarea);
        }
    }
    wclose();
    show_hint73();
    update_area_screen(current_area);
}
void global_netnum(void)
{
    int i,new_net;
    char s[80],ch;
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,"Select Group To Modify.  Space Selects All");
    wopen(7,30,10,50,1,WHITE|_RED,YELLOW|_RED);
    DoShadow();
    wcenters(0,YELLOW|_RED,"What Group? ");
    wgotoxy(0,16);
    showcur();
    ch=wgetc();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,"Enter New WWIV Network Number.  [ENTER] Aborts.");
    wcenters(1,YELLOW|_RED,"What Net Num? ");
    wgotoxy(1,16);
    wgets(s);
    hidecur();
    if (s[0]==0)
    {
        wclose();
        show_hint74();
        return;
    }
    new_net=atoi(s);
    if ((new_net>20) || (new_net<1))
	new_net=1;
    for (i=0;i<=num_areas;i++)
    {
	read_area(i,&thisarea);
        if ((thisarea.group==upcase(ch)) || (ch==32))
        {
            thisarea.net_num=new_net;
            write_area(i,&thisarea);
        }
    }
    wclose();
    show_hint74();
    update_area_screen(current_area);
}
void global_origin(void)
{
    int i;
    char s[80],ch;
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,"Select Group To Modify.  Space Selects All");
    wopen(7,30,10,50,1,WHITE|_RED,YELLOW|_RED);
    DoShadow();
    wcenters(0,YELLOW|_RED,"What Group? ");
    wgotoxy(0,15);
    showcur();
    ch=wgetc();
    wclose();
    wopen(7,5,10,75,1,WHITE|_RED,YELLOW|_RED);
    DoShadow();
    wfill(1,5,1,60,'±',LCYAN|_BLUE);
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,"Enter New Origin Line.  [ENTER] Chooses Default Origin.  [ESC] Aborts.");
    wcenters(0,YELLOW|_RED,"New Origin?");
    wgotoxy(1,5);
    wtextattr(LCYAN|_BLUE);
    if (wgetns(s,60))
    {
        hidecur();
        wclose();
        show_hint75();
        return;
    }
    hidecur();
    if (s[0]==0)
        strcpy(s,cfg.origin_line);
    for (i=0;i<=num_areas;i++)
    {
        read_area(i,&thisarea);
        if ((thisarea.group==upcase(ch)) || (ch==32))
        {
            strcpy(thisarea.origin,s);
	    write_area(i,&thisarea);
        }
    }
    wclose();
    show_hint75();
    update_area_screen(current_area);
}
void global_add(void)
{
    int i,j;
    char s[80],ch;
    FIDOADR fadr;
    wopen(7,25,10,55,1,WHITE|_RED,WHITE|_RED);
    wcenters(0,YELLOW|_RED,"What Group? ");
    wgotoxy(0,20);
    showcur();
    ch=wgetc();
    wclear();
    wcenters(0,YELLOW|_RED,"Add What Connect? ");
    wgotoxy(1,5);

//    printf("\nWhat connect? ");
    lgcursor();
    wgets(s);
    smcursor();
    hidecur();
    if (s[0]==0)
    {
        hidecur();
        smcursor();
        wclose();
        return;
    }
    fidoadr_split(s,&fadr);
    for (i=0;i<=num_areas;i++)
    {
        read_area(i,&thisarea);
        if ((thisarea.group==upcase(ch)) || (ch==32))
        {
            if (!(exist_export_node(s,i)))
            {
                for (j=0;j<MAX_NODES;j++)
                {
                    if ((thisarea.feed[j].zone==999) ||
                        (thisarea.feed[j].zone==0))
                        {
                            spot=j;
                            j=MAX_NODES;
                        }
                }

                thisarea.feed[spot].zone=fadr.zone;
                thisarea.feed[spot].net=fadr.net;
                thisarea.feed[spot].node=fadr.node;
                thisarea.feed[spot].point=fadr.point;
            }
        write_area(i,&thisarea);
        }
    }
    wclose();
    for (i=0;i<num_areas;i++)
    {
        read_area(i,&thisarea);
        curr_area=i;
        sort_export();
        write_area(i,&thisarea);
    }
    update_area_screen(current_area);
}
void global_delete(void)
{
    int i;
    char s[80],ch;
    FIDOADR fadr;
    wopen(7,25,10,55,1,WHITE|_RED,WHITE|_RED);
    wcenters(0,YELLOW|_RED,"What Group? ");
    wgotoxy(0,20);
    showcur();
    ch=wgetc();
    wclear();
    wcenters(0,YELLOW|_RED,"Delete What Connect? ");
    wgotoxy(1,5);

    lgcursor();
    wgets(s);
    smcursor();
    hidecur();
    if (s[0]==0)
    {
        hidecur();
        smcursor();
        wclose();
        return;
    }
    fidoadr_split(s,&fadr);
    for (i=0;i<=num_areas;i++)
    {
        read_area(i,&thisarea);
        if ((thisarea.group==upcase(ch)) || (ch==32))
        {
            if ((exist_export_node(s,i)))
            {
                thisarea.feed[spot].zone=999;
                thisarea.feed[spot].net=999;
                thisarea.feed[spot].node=999;
                thisarea.feed[spot].point=999;
            }
        write_area(i,&thisarea);
        }
    }
    wclose();
    for (i=0;i<num_areas;i++)
    {
        read_area(i,&thisarea);
        curr_area=i;
        sort_export();
        write_area(i,&thisarea);
    }
    update_area_screen(current_area);

}
void global_edit(void)
{
    int i,esc;
    char s[80],ch,s1[80];
    FIDOADR fadr,fadr2;
//    wperror("Not Implemented Yet!");
//    return;

    wopen(7,25,10,55,1,WHITE|_RED,WHITE|_RED);
    wcenters(0,YELLOW|_RED,"What Group? ");
    wgotoxy(0,20);
    showcur();
    ch=wgetc();
    wclear();
    wcenters(0,YELLOW|_RED,"Find What Connect? ");
    wgotoxy(1,5);
    lgcursor();
    wgets(s);
    if (s[0]==0)
    {
        hidecur();
        smcursor();
        wclose();
        return;
    }
    strcpy(s1,s);
    wclear();
    wcenters(0,YELLOW|_RED,"Edit Connect");
    lgcursor();
    winpbeg(YELLOW|_BLUE,LIGHTCYAN|_BLUE);
    winpdef(1,5,s1,"????????????????",'0',2,NULL,0);
    esc=winpread();
    if (esc)
    {
        hidecur();
        smcursor();
        wclose();
        return;
    }
    smcursor();
    hidecur();
    fidoadr_split(s,&fadr);
    fidoadr_split(s1,&fadr2);
    for (i=0;i<=num_areas;i++)
    {
        read_area(i,&thisarea);
        if ((thisarea.group==upcase(ch)) || (ch==32))
        {
            if ((exist_export_node(s,i)))
            {
                thisarea.feed[spot].zone=fadr2.zone;
                thisarea.feed[spot].net=fadr2.net;
                thisarea.feed[spot].node=fadr2.node;
                thisarea.feed[spot].point=fadr2.point;
            }
            write_area(i,&thisarea);
        }
    }
    wclose();
    for (i=0;i<num_areas;i++)
    {
        read_area(i,&thisarea);
        curr_area=i;
        sort_export();
        write_area(i,&thisarea);
    }
    update_area_screen(current_area);
}



void global_options(void)
{
    wmenubeg(9,38,19,59,1,YELLOW|_RED,WHITE|_RED,DoShadow);
        wmenuitem(0,0,"   Export Options   ",'E',23,0,NULL,0,23);
        wmenuiba(show_hint68,clear_hint);
            wmenubeg(11,40,15,54,1,CYAN|_BLUE,WHITE|_BLUE,DoShadow);
                wmenuitem(0,0,"  Add  Node  ",'A',31,0,global_add,0,31);
                wmenuiba(clear_hint,clear_hint);
                wmenuitem(1,0," Delete Node ",'D',32,0,global_delete,0,32);
                wmenuiba(clear_hint,clear_hint);
                wmenuitem(2,0,"  Edit Node  ",'E',33,0,global_edit,0,33);
                wmenuiba(clear_hint,clear_hint);
            wmenuend(31,M_VERT,0,0,YELLOW|_BLUE,LIGHTRED|_BLUE,LCYAN|_BLUE,WHITE|_RED);
            wmenuget();
        wmenuitem(1,0,"    Change Group    ",'G',24,0,global_group,0,24);
        wmenuiba(show_hint69,clear_hint);
        wmenuitem(2,0,"  Set  'Use Alias'  ",'A',25,0,global_alias,0,25);
        wmenuiba(show_hint70,clear_hint);
        wmenuitem(3,0,"  Set 'WWIV Color'  ",'W',26,0,global_color,0,26);
        wmenuiba(show_hint71,clear_hint);
        wmenuitem(4,0,"  Set 'High ASCII'  ",'H',27,0,global_ascii,0,27);
        wmenuiba(show_hint72,clear_hint);
        wmenuitem(5,0,"   Set 'Keep Tag'   ",'K',28,0,global_tag,0,28);
        wmenuiba(show_hint73,clear_hint);
        wmenuitem(6,0," Change WWIV Netnum ",'N',29,0,global_netnum,0,29);
        wmenuiba(show_hint74,clear_hint);
        wmenuitem(7,0,"   Change  Origin   ",'O',30,0,global_origin,0,30);
        wmenuiba(show_hint75,clear_hint);
        wmenuitem(8,0,"Set 'Val.  Incoming'",'V',30,0,global_valid,0,30);
        wmenuiba(show_hint100,clear_hint);
    wmenuend(23,M_VERT,0,0,YELLOW|_RED,LCYAN|_RED,WHITE|_RED,WHITE|_GREEN);
//    wmenuend(18,M_VERT,0,0,YELLOW|_BLUE,LIGHTRED|_BLUE,LCYAN|_BLUE,WHITE|_RED);
    wmenuget();
    show_hint59();
    show_area_screen(current_area);
}
/* ------------------------------------------------------------------------ */
/* Checks to see if file (s) exists.  Returns 1 if exists, 0 if not         */
/* ------------------------------------------------------------------------ */

int exist(char *s)
{
  int i;
  struct ffblk ff;

  i=findfirst(s,&ff,FA_HIDDEN);
  if (i)
    return(0);
  else
    return(1);
}

int exist_export_node(char *address,int area)
{
    int j;
    FIDOADR fadr;

    spot=0;
    fidoadr_split(address, &fadr);
    read_area(area,&thisarea);
    for (j=0;j<MAX_NODES;j++)
    {
        if ((thisarea.feed[j].zone==fadr.zone) &&
            (thisarea.feed[j].net==fadr.net) &&
            (thisarea.feed[j].node==fadr.node) &&
            (thisarea.feed[j].point==fadr.point))
            {
                spot=j;
                return (1);
            }
    }
    return (0);
}

void dos_shell(void)
{
    char curdir[81],prmpt[181],fastprmpt[181];
    char *path;
    char *comspec=getenv("COMSPEC");
    int dr;
    if (comspec == NULL)
        comspec = "COMMAND.COM" ; /* default program to run is COMMAND.COM */
    showcur();
    dr=getdisk();
    wopen(0,0,24,79,5,WHITE|_BLACK,WHITE|_BLACK);
    strcpy(curdir,"\\");
    getcurdir(0,curdir+1);
    putenv("WWIVTOSS=SHELL");
    path=(char *) malloc (strlen(path_ptr)+15);
    strcpy(path,"PATH=");
    strcat(path,path_ptr);
    strcpy(prmpt,getenv("PROMPT"));
    strcpy(prmpt,"$p$g");
    strcpy(fastprmpt,"PROMPT=$_Type EXIT to return to WSETUP!");
    strcat(fastprmpt,"$_");
    putenv(fastprmpt);
    spawnl(P_WAIT,comspec,comspec,NULL);
    setdisk(dr);
    chdir(curdir);
    fastprmpt[0]=0;
    prmpt[0]=0;
    hidecur();
    wclose();


}
void load_xlat(void)
{
    FILE *fp;

    if ((fp = fopen("XLAT.DAT","r"))==NULL)
        exit(1);

    fread(xlat, sizeof xlat,1,fp);
    fclose(fp);
}
void save_xlat(void)
{
    FILE *fp;

    if ((fp = fopen("XLAT.DAT","w"))==NULL)
        exit(1);

    fwrite(xlat, sizeof xlat,1,fp);
    fclose(fp);
}



/* Mode
    1 = inbound
    2 = outbound
    */

void show_xlat(int mode)
{
    int column,i,row;
    char s[5],line[15];

    row=2;
    column=2;
    wclear();
    if (mode==1)
        wcenters(0,LIGHTGREEN|_BLACK,"Incoming Character Translation Table");
    else
        wcenters(0,LIGHTGREEN|_BLACK,"Outgoing Character Translation Table");
    for (i=0;i<222;i++)
    {
        if (mode==1)
            sprintf(s,"%c",xlat[i].in);
        else
            sprintf(s,"%c",xlat[i].out);
        sprintf(line,"%c",i+33);
        wprints(row,column,LIGHTCYAN|_BLACK,line);
        if (strcmp(line,s)!=0)
            wprints(row,column+2,LIGHTRED|_BLACK,s);
        row++;
        if (row>20)
        {
            row=2;
        column+=6;
        }
    }

}
void edit_xlat(int mode)
{
    unsigned char ch;
    int i;
    char s[50],s1[50];

    load_xlat();

    wopen(1,1,23,76,1,YELLOW|_BLACK,LIGHTCYAN|_BLACK);
    show_hint77();
    do
    {
        show_xlat(mode);
        wtextattr(BLACK|_BLACK);
        ch=wgetc();
        wtextattr(LIGHTCYAN|_BLACK);
        if (ch>32)
        {
            i=ch-33;
            wopen(8,28,12,52,1,WHITE|_RED,WHITE|_RED);
            DoShadow();
            if (mode==1)
                sprintf(s,"Current Character : %c",xlat[i].in);
            else
                sprintf(s,"Current Character : %c",xlat[i].out);
            sprintf(s1,"Change  Character : ");
            wprints(0,1,WHITE|_RED,s);
            wprints(1,1,WHITE|_RED,s1);
            wgotoxy(1,21);
            ch=wgetc();
            if (ch>32)
            {
                if (mode==1)
                    xlat[i].in=ch;
                else
                    xlat[i].out=ch;
            }
            wclose();
        }
    } while (ch!=27);

    wopen(8,25,12,55,1,WHITE|_GREEN,WHITE|_GREEN);
    DoShadow();
    wcenters(1,WHITE|_GREEN,"Save Changes?");
    ch=wgetc();
    if (upcase(ch)=='Y')
        save_xlat();
    else
        load_xlat();
    wclose();
    wclose();
}
void help_message(void)
{
    wopen(1,5,22,74,1,WHITE|_GREEN,WHITE|_GREEN);
    DoShadow();
    setonkey(0x3B00,NULL,0);
    hidecur();
    wprints(0,4,WHITE|_GREEN,"Data entry fields are navigated by either the arrow keys or");
    wprints(1,4,WHITE|_GREEN,"(preferably) the enter key.  All fields are assumed to be in");
    wprints(2,4,WHITE|_GREEN,"*update* mode.  You can use these keys to edit the fields:");
    wprints(4,2,WHITE|_GREEN,"          Key                  Action");
    wprints(5,2,WHITE|_GREEN,"          ---                  ------");
    wprints(6,2,WHITE|_GREEN,"        LeftArrow             cursor left");
    wprints(7,2,WHITE|_GREEN,"        RightArrow            cursor right");
    wprints(8,2,WHITE|_GREEN,"        UpArrow               cursor up");
    wprints(9,2,WHITE|_GREEN,"        DownArrow             cursor down");
    wprints(10,2,WHITE|_GREEN,"        Enter                 process field");
    wprints(11,2,WHITE|_GREEN,"        Ctrl-Enter            process all fields");
    wprints(12,2,WHITE|_GREEN,"        Home                  beginning of field");
    wprints(13,2,WHITE|_GREEN,"        End                   end of field line / end of field");
    wprints(14,2,WHITE|_GREEN,"        Ins                   toggle field insert mode");
    wprints(15,2,WHITE|_GREEN,"        Del                   delete character at cursor");
    wprints(16,2,WHITE|_GREEN,"        BackSpace             delete character left");
    wprints(17,2,WHITE|_GREEN,"        Ctrl-R                restore field to original contents");
    wprints(18,2,WHITE|_GREEN,"        Esc                   abort data entry");
    wmessage(" Press Any Key to Return ",1,20,WHITE|_GREEN);
    waitkey();
    wclose();
    showcur();
    setonkey(0x3B00,help_message,0);
}
void sort_areas(void)
{
    char s[80];
    int i,j;
    long pos;
    int f;

    arearec a,b,c;

    strcpy(s,"AREAS.DAT");
    f=sh_open1(s,O_RDONLY | O_BINARY);

    for (i=1;i<num_areas+1;i++)
    {
        pos=((long) sizeof(arearec)) * ((long) i);
        sh_lseek(f,pos,SEEK_SET);
        sh_read(f, (void *)&a, sizeof(arearec));

//        read_area(i,&a);
        for (j=1;j<num_areas+1;j++)
        {
            pos=((long) sizeof(arearec)) * ((long) j);
            sh_lseek(f,pos,SEEK_SET);
            sh_read(f, (void *)&b, sizeof(arearec));
//            read_area(j,&b);
            if (strcmp(b.name,a.name)>0)
            {
                c=b;
                b=a;
                a=c;
                pos=((long) sizeof(arearec)) * ((long) i);
                sh_lseek(f,pos,SEEK_SET);
                sh_write(f, (void *)&a, sizeof(arearec));
                pos=((long) sizeof(arearec)) * ((long) j);
                sh_lseek(f,pos,SEEK_SET);
                sh_write(f, (void *)&b, sizeof(arearec));

//                write_area(i,&a);
//                write_area(j,&b);
            }
        }
    }
    sh_close(f);
}
void write_area2(unsigned int un, arearec *a)
{
  char s[81];
  long pos;
  int f;

  strcpy(s,"AREAS.$$$");
  f=sh_open(s,O_RDWR | O_BINARY | O_CREAT, S_IREAD | S_IWRITE);
  if (f>=0) {
    pos=((long) sizeof(arearec)) * ((long) un);
    sh_lseek(f,pos,SEEK_SET);
    sh_write(f, (void *)a, sizeof(arearec));
    sh_close(f);
  }
}
void cleanup(void)
{
    char s[80],s1[81];
    int i,j;
    arearec a;

    unlink("AREAS.$$$");
    j=0;
    strcpy(s,"AREAS.$$$");
    strcpy(s1,"AREAS.DAT");
    for (i=0;i<=num_areas;i++)
    {
        read_area(i,&a);
        if (a.active)
        {
            write_area2(j,&a);
            j++;
        }
    }
    num_areas=j-1;
    unlink("AREAS.DAT");
    rename("AREAS.$$$","AREAS.DAT");
    unlink("AREAS.$$$");
}

void read_config_dat(void)
{
    char s[80];
    int f;


    f=sh_open1("CONFIG.DAT",O_RDONLY | O_BINARY);
    if (f<0)
    {
        printf("\r\n\r\n\7Error:  WSETUP Must be run from the main WWIV directory!\r\n\r\n");
        exit(1);
    }
    sh_read(f,(void *) (&syscfg), sizeof(configrec));
    sh_close(f);

    if(net_networks)
        farfree(net_networks);
    net_networks=NULL;
    sprintf(s,"%sNETWORKS.DAT", syscfg.datadir);
    f=sh_open1(s,O_RDONLY|O_BINARY);
    if (f>0)
    {
        net_num_max=filelength(f)/sizeof(net_networks_rec);
        if (net_num_max)
        {
            net_networks=mallocx(net_num_max*sizeof(net_networks_rec),"networks.dat");
            sh_read(f, net_networks, net_num_max*sizeof(net_networks_rec));
        }
        else
        {
            printf("þ\7\7 Error!  No networks defined in INIT!  Cannot Continue!\r\n");
	    sh_close(f);
	    exit(1);
	}
	sh_close(f);
	}
}

void far *mallocx(unsigned long l, char *where)
{
    void *x;
    char huge *xx;

    if (!l)
        l=1;
    x=farmalloc(l);
    if (!x)
    {
        printf("Insufficient memory (%ld bytes) for %s.\n",l,where);
        exit(2);
    }

    xx=(char huge *)x;
    while (l>0)
    {
        if (l>32768)
        {
            memset((void *)xx,0,32768);
            l-=32768;
        } else
        {
            memset((void *)xx,0,l);
            break;
        }
    }

    return(x);
}
void create_subs_list(void)
{
    char s[81],s1[181],s2[181],ch,temp[81],temp1[81];
    int i,j,ok,k,length;
    FILE *fp;

    wopen(5,25,7,55,1,WHITE|_RED,YELLOW|_RED);
    DoShadow();
    wcenters(0,BLINK|WHITE|_RED," Creating SUBS.LST... ");

    read_config_dat();
    for (j=0;j<11;j++)
    {
        if (cfg.aka_list[j].zone!=0)
        {
            sprintf(s,"%sSUBS.LST",net_networks[cfg.aka_list[j].wwiv_netnum].dir);
            ok=1;
            if (exist(s))
            {
                wopen(9,1,12,79,1,WHITE|_RED,YELLOW|_RED);
                DoShadow();
                if (!quiet)
                    beep();
                sprintf(temp,"Warning! %s Already Exists!  Overwrite?",s);
                wcenters(0,YELLOW|_RED,temp);
                wgotoxy(1,(strlen(temp)/2)+4);
                showcur();
                ch=wgetc();
                hidecur();
                if (upcase(ch)==89)
                {
                    ok=1;
                    wclose();
                }
                else
                {
                    ok=0;
                    wclose();
                }
            }
            if (ok)
            {
                if ((fp = fopen(s,"w+"))==NULL)
                {
                    wperror("Couldn't Open File!");
                }
                else
                {
                    sprintf(temp,"%s Pseudo Subs Listing\n",net_networks[cfg.aka_list[j].wwiv_netnum].name);
                    length=40-(strlen(temp)/2);
                    sprintf(temp1,"");
                    for (i=0;i<length;i++)
                        strcat(temp1," ");
                    strcat(temp1,temp);
                    fputs(temp1,fp);
                    sprintf(temp,"Created on %s by %s v. %s\n",sysdate(3),PROGRAM,VERSION);
                    length=40-(strlen(temp)/2);
                    sprintf(temp1,"");
                    for (i=0;i<length;i++)
                        strcat(temp1," ");
                    strcat(temp1,temp);
                    fputs(temp1,fp);
                    fputs("\n",fp);
                    strcpy(temp1," Type   Host  Note  Name\n");
                    fputs(temp1,fp);
                    strcpy(temp1,"======= ===== ====  =======================================================\n");
                    fputs(temp1,fp);
                    for (i=1;i<=num_areas;i++)
                    {
                        read_area(i,&thisarea);
                        if (thisarea.net_num==cfg.aka_list[j].wwiv_netnum)
                        {
                            strcpy(s2,"1");
                            for (k=0;k<11;k++)
                                if (cfg.aka_list[k].wwiv_netnum==thisarea.net_num)
                                    sprintf(s2,"%d",cfg.aka_list[k].wwiv_node);
                            sprintf(s1,"%-7s %5s R     %s\n",thisarea.subtype,s2,thisarea.name);
                            fputs(s1,fp);
                        }
                    }
                }
                fclose(fp);
            }
        }
    }
    wclose();
}
int comparedl(arearec *x, arearec *y, int type)
{
  switch(type) {
    case 0:
      return(strcmp(x->name,y->name));
    case 1:
      return(strcmp(x->subtype,y->subtype));
  }
  return(0);
}

void quicksort(int l,int r,int type)
{
  register int i,j,f;
  arearec a,a2,x;

  i=l; j=r;
  f=sh_open("AREAS.DAT",O_RDWR | O_BINARY | O_CREAT, S_IREAD | S_IWRITE);
  SETREC(f,((l+r)/2));
  sh_read(f, (void *)&x,sizeof(arearec));
  do {
    SETREC(f,i);
    sh_read(f, (void *)&a,sizeof(arearec));
    while (comparedl(&a,&x,type)<0) {
      SETREC(f,++i);
      sh_read(f, (void *)&a,sizeof(arearec));
    }
    SETREC(f,j);
    sh_read(f, (void *)&a2,sizeof(arearec));
    while (comparedl(&a2,&x,type)>0) {
      SETREC(f,--j);
      sh_read(f, (void *)&a2,sizeof(arearec));
    }
    if (i<=j) {
      if (i!=j) {
        SETREC(f,i);
        sh_write(f,(void *)&a2,sizeof(arearec));
        SETREC(f,j);
        sh_write(f,(void *)&a,sizeof(arearec));
      }
      i++;
      j--;
    }
  } while (i<j);
  f=sh_close(f);
  if (l<j)

    quicksort(l,j,type);
  if (i<r)
    quicksort(i,r,type);
}
void node_setup(void)
{
    int done,keypress,i,j;
    unsigned int keycode;
    char ch;
    done=0;
    wopen(6,5,20,55,1,YELLOW|_BLUE,WHITE|_BLUE);
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
    wprints(4,2,CYAN|_BLUE,"Node Network    : ");
    wprints(4,20,LIGHTCYAN|_BLUE,nodes[nodenum].domain);
    wprints(5,2,CYAN|_BLUE,"Node SysOp      : ");
    wprints(5,20,LIGHTCYAN|_BLUE,nodes[nodenum].sysop_name);
    wprints(6,2,CYAN|_BLUE,"SysOp Voice #   : ");
    wprints(6,20,LIGHTCYAN|_BLUE,nodes[nodenum].sysop_phone);
    wprints(7,2,CYAN|_BLUE,"Packet Password : ");
    wprints(7,20,LIGHTCYAN|_BLUE,nodes[nodenum].pkt_pw);
    wprints(8,2,CYAN|_BLUE,"Areafix Password: ");
    wprints(8,20,LIGHTCYAN|_BLUE,nodes[nodenum].areafix_pw);
    wprints(9,2,CYAN|_BLUE,"Allowed Groups  : ");
    wprints(9,20,LIGHTCYAN|_BLUE,s1);
    wprints(10,2,CYAN|_BLUE,"Archive Status  : ");
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
    wprints(10,20,LIGHTCYAN|_BLUE,s);
    wprints(11,2,CYAN|_BLUE,"Compression     : ");
    wprints(11,20,LIGHTCYAN|_BLUE,cfg.archive[nodes[nodenum].compression].name);
    wprints(12,2,CYAN|_BLUE,"Packet Type     : ");
    switch(nodes[nodenum].packet_type)
    {
        case 0:
            strcpy(s,"2.0 (Stoneage)");
            break;
        case 1:
            strcpy(s,"2.+ (FSC-0039)");
            break;
        case 2:
            strcpy(s,"2.N (FSC-0048)");
            break;
        case 3:
            strcpy(s,"2.2 (FSC-0045)");
            break;
        default:
            strcpy(s,"2.0 (Stoneage)");
            nodes[nodenum].packet_type=0;
            break;
    }
    wprints(12,20,LIGHTCYAN|_BLUE,s);

}
void edit_node(int nodenum)
{
    char s[80],s1[15],s2[81];
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
    switch(nodes[nodenum].packet_type)
    {
        case 0:
            strcpy(s2,"2.0 (Stoneage)");
            break;
        case 1:
            strcpy(s2,"2.+ (FSC-0039)");
            break;
        case 2:
            strcpy(s2,"2.N (FSC-0048)");
            break;
        case 3:
            strcpy(s2,"2.2 (FSC-0045)");
            break;
        default:
            strcpy(s2,"2.0 (Stoneage)");
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
        wprints(9,2,CYAN|_BLUE,"Allowed Groups  : ");
        wprints(9,20,CYAN|_BLUE,s1);

    wprints(11,2,CYAN|_BLUE,"Compression     : ");
    wprints(11,20,CYAN|_BLUE,cfg.archive[nodes[nodenum].compression].name);
    wprints(10,2,CYAN|_BLUE,"Archive Status  : ");
    wprints(10,20,CYAN|_BLUE,temp_stat);
    wprints(12,2,CYAN|_BLUE,"Packet Type     : ");
    wprints(12,20,CYAN|_BLUE,s2);
    winpbeg(CYAN|_BLUE,LIGHTCYAN|_BLUE);
    winpdef(3,2,nodes[nodenum].address,"'Node Address    : '??????????????????????????????",'U',2,NULL,0);
    winpfba(show_hint48,clear_hint);
    winpdef(4,2,nodes[nodenum].domain, "'Node Network    : '??????????",'0',2,NULL,0);
    winpfba(show_hint76,clear_hint);
    winpdef(5,2,nodes[nodenum].sysop_name,"'Node SysOp      : '????????????????????????????????????",'0',2,NULL,0);
    winpfba(show_hint49,clear_hint);
    winpdef(6,2,nodes[nodenum].sysop_phone,"'SysOp Voice #   : '????????????????????????????????????",'0',2,NULL,0);
    winpfba(show_hint101,clear_hint);
    winpdef(7,2,nodes[nodenum].pkt_pw, "'Packet Password : '????????????????????",'U',2,NULL,0);
    winpfba(show_hint50,clear_hint);
    winpdef(8,2,nodes[nodenum].areafix_pw, "'Areafix Password: '??????????",'U',2,NULL,0);
    winpfba(show_hint51,clear_hint);
    winpdef(9,2,s1,"'Allowed Groups  : '",'U',2,NULL,0);
    winpdef(10,2,temp_stat,"'Archive Status  : '",'U',2,NULL,0);
    winpdef(11,2,temp,"'Compression     : '",'U',2,NULL,0);
    winpdef(12,2,s2,  "'Packet Type     : '",'U',2,NULL,0);
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
        wprints(9,2,LIGHTCYAN|_BLUE,"Allowed Groups  : ");
        wprints(9,20,LIGHTCYAN|_BLUE,s1);
        done=0;
        do
        {
            wprints(9,32,LIGHTCYAN|_BLUE," ");
            show_hint82();
            wgotoxy(9,32);
            ch=wgetchf("AaBbCcDdEeFfGgHhIiJj\r",0);
            wprints(9,32,LIGHTCYAN|_BLUE," ");
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
                wprints(9,20,LIGHTCYAN|_BLUE,s1);
            }
            else
                done=1;
            if (ch==0)
                done=1;
        } while (!done);
        wprints(9,2,CYAN|_BLUE,"Allowed Groups  : ");
        wprints(9,20,CYAN|_BLUE,s1);

        hidecur();
        show_hint52();
        wprints(10,2,LIGHTCYAN|_BLUE,"Archive Status  : ");
        wprints(10,20,LIGHTCYAN|_BLUE,temp_stat);
        choose_status();
        wprints(10,2,CYAN|_BLUE,"Archive Status  : ");
        wprints(10,20,CYAN|_BLUE,temp_stat);
        clear_hint();
        wprints(11,2,LIGHTCYAN|_BLUE,"Compression     : ");
        wprints(11,20,LIGHTCYAN|_BLUE,cfg.archive[nodes[nodenum].compression].name);
        show_hint53();
        choose_archiver();
        wprints(11,2,CYAN|_BLUE,"Compression     : ");
        wprints(11,20,CYAN|_BLUE,cfg.archive[nodes[nodenum].compression].name);
        clear_hint();
        wprints(12,2,LIGHTCYAN|_BLUE,"Packet Type     : ");
        wprints(12,20,LIGHTCYAN|_BLUE,s2);
        show_hint111();
        choose_packet_type();
    switch(nodes[nodenum].packet_type)
    {
        case 0:
            strcpy(s2,"2.0 (Stoneage)");
            break;
        case 1:
            strcpy(s2,"2.+ (FSC-0039)");
            break;
        case 2:
            strcpy(s2,"2.N (FSC-0048)");
            break;
        case 3:
            strcpy(s2,"2.2 (FSC-0045)");
            break;
        default:
            strcpy(s2,"2.0 (Stoneage)");
            break;
    }

        wprints(12,2,CYAN|_BLUE,"Packet Type     : ");
        wprints(12,20,CYAN|_BLUE,s2);
        clear_hint();
    }
//    wprints(10,2,CYAN|_BLUE,"Compression     : ");
//    wprints(10,20,CYAN|_BLUE,cfg.archive[nodes[nodenum].compression].name);
    fidoadr_split(nodes[nodenum].address,&fadr);
    sprintf(nodes[nodenum].address,"%d:%d/%d.%d",fadr.zone,fadr.net,fadr.node,fadr.point);
    strtrim(nodes[nodenum].sysop_name);
    strtrim(nodes[nodenum].pkt_pw);
    strtrim(nodes[nodenum].areafix_pw);
    strtrim(nodes[nodenum].sysop_phone);
    strtrim(nodes[nodenum].domain);
    hidecur();
    show_node_screen(nodenum);
    show_hint58();
}
void uplink_setup(void)
{
    int done,keypress,i,j;
    unsigned int keycode;
    char ch;
    done=0;
    wopen(6,5,22,75,1,YELLOW|_BLUE,WHITE|_BLUE);
    DoShadow();
    wmessage(" F1 For Editing Help ",1,7,LIGHTRED|_BLUE);
    show_hint58();
    i=1;
    while (!done)
    {
        show_uplink_screen(i);
        keycode=getxch();
        keypress=keycode>>8;
        switch(keypress)
        {
            case 1 : /* ESC */
                done=1;
                break;
            case 28: /* Enter */
                current_node=i;
                edit_uplink(i);
                i=current_node;
                break;
            case 71: /* Home */
                i=1;
                break;
            case 75: /* Left Arrow */
                i--;
                if (i<1)
                    i=num_uplink;
                break;
            case 77: /* Right Arrow */
                i++;
                if (i>num_uplink)
                    i=1;
                break;
            case 79: /* End */
                i=num_uplink;
                break;
            case 82: /* Insert */
                num_uplink++;
                if (num_uplink>MAX_NODES)
                {
                    wperror("Warning!  Maximum of 50 nodes!");
                    num_uplink=MAX_NODES-1;
                    break;
                }
                i=num_uplink;
                strcpy(uplink[i].address,"");
                strcpy(uplink[i].areafix_prog,"");
                strcpy(uplink[i].areafix_pw,"");
                strcpy(uplink[i].areas_filename,"");
                strcpy(uplink[i].origin_address,"");
                uplink[i].add_plus=0;
                uplink[i].unconditional=0;
                uplink[i].areas_file_type=0;
                edit_uplink(i);
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
                    strcpy(uplink[i].address,"");
                    strcpy(uplink[i].areafix_prog,"");
                    strcpy(uplink[i].areafix_pw,"");
                    strcpy(uplink[i].areas_filename,"");
                    strcpy(uplink[i].origin_address,"");
                    uplink[i].add_plus=0;
                    uplink[i].unconditional=0;
                    uplink[i].areas_file_type=0;
                    current_node=i;
                    num_uplink--;
                    for (j=i;j<=num_uplink;j++)
                        uplink[j]=uplink[j+1];
                    if (num_uplink<1)
                        num_uplink=1;
                    i=current_node;
                    if (i<1)
                        i=1;
                    if (i>num_uplink)
                        i=num_uplink;
                    break;
                }
        }
    }
    wclose();
    for (i=1;i<num_uplink+1;i++)
    {
        for (j=1;j<num_uplink+1;j++)
        {
            if (strcmp(uplink[j].address,uplink[i].address)>0)
            {
                uplink[0]=uplink[j];
                uplink[j]=uplink[i];
                uplink[i]=uplink[0];
            }
        }
    }
    clear_hint();
    show_hint4();
}

void show_uplink_screen(int nodenum)
{
    char s[161],s1[15];
    int i;
    for (i=0;i<15;i++)
    s1[i]=32;
    for (i=0;i<12;i++)
    {
        if (uplink[nodenum].groups & (1<< i))
            s1[i]='A'+i;
        else
            s1[i]=32;
    }
    s1[11]=0;
    sprintf(s,"Uplink # %d out of %d",nodenum,num_uplink);
    wclear();
    wcenters(0,WHITE|_BLUE,s);
    wprints(2,2,CYAN|_BLUE,"Node Address      : ");
    wprints(2,22,LIGHTCYAN|_BLUE,uplink[nodenum].address);
    wprints(4,2,CYAN|_BLUE,"Areafix Program   : ");
    wprints(4,22,LIGHTCYAN|_BLUE,uplink[nodenum].areafix_prog);
    wprints(5,2,CYAN|_BLUE,"Areafix Password  : ");
    wprints(5,22,LIGHTCYAN|_BLUE,uplink[nodenum].areafix_pw);
    wprints(6,2,CYAN|_BLUE,"Unconditional Add : ");
    sprintf(s,"%s",uplink[nodenum].unconditional?"Yes":"No");
    wprints(6,22,LIGHTCYAN|_BLUE,s);
    wprints(7,2,CYAN|_BLUE,"Use '+' To Add?   : ");
    sprintf(s,"%s",uplink[nodenum].add_plus?"Yes":"No");
    wprints(7,22,LIGHTCYAN|_BLUE,s);
    wprints(9,2,CYAN|_BLUE,"Areas Filename    : ");
    wprints(9,22,LIGHTCYAN|_BLUE,uplink[nodenum].areas_filename);
    wprints(10,2,CYAN|_BLUE,"Areas File Type   : ");
    sprintf(s,"%s",uplink[nodenum].areas_file_type?"<AREA> <DESCRIP.>":"Random");
    wprints(10,22,LIGHTCYAN|_BLUE,s);
    wprints(12,2,CYAN|_BLUE,"Allowed Groups    : ");
    wprints(12,22,LIGHTCYAN|_BLUE,s1);
    wprints(13,2,CYAN|_BLUE,"Origin Address    : ");
    wprints(13,22,LIGHTCYAN|_BLUE,uplink[nodenum].origin_address);

}
void show_filetype(int nodenum)
{
    char s[80];

    wprints(10,2,LIGHTCYAN|_BLUE,"Areas File Type   : ");
    switch(uplink[nodenum].areas_file_type)
    {
        case 0:
            strcpy(s,"Random           ");
            break;
        case 1:
            strcpy(s,"<AREA> <DESCRIP.>");
            break;
        default:
            strcpy(s,"Random           ");
            uplink[nodenum].areas_file_type=0;;
            break;
    }
    wprints(10,22,LIGHTCYAN|_BLUE,s);
}
void choose_filetype(int nodenum)
{
    char s[80];
    char ch;
    int i,i1;
    wgotoxy(10,22);
    show_hint107();
    show_filetype(nodenum);
    i=i1=uplink[nodenum].areas_file_type;
    ch=0;
    while ((ch!=13) && (ch!=27))
    {
        show_filetype(nodenum);
        ch=getch();
        if (ch==32)
        {
            if (i==1)
                i=0;
            else
                i=1;
            uplink[nodenum].areas_file_type=i;
        }
    }
    if (ch==27)
        uplink[nodenum].areas_file_type=i1;
    show_filetype(nodenum);
    wprints(10,2,CYAN|_BLUE,"Areas File Type   : ");
    sprintf(s,"%s",uplink[nodenum].areas_file_type?"<AREA> <DESCRIP.>":"Random");
    wprints(10,22,CYAN|_BLUE,s);

}

void edit_uplink(int nodenum)
{
    char s[80],s1[15];
    char ch,ch1;
 //   char temp[10];
    char temp2[20],temp3[20],temp4[20];//,temp5[20],temp6[20],temp7[20];
    int esc,i,done;
    FIDOADR fadr;

    sprintf(s,"Edit node # %d",nodenum);
    wclear();
    showcur();
    current_node=nodenum;
    wcenters(0,LIGHTRED|_BLUE,s);
    for (i=0;i<15;i++)
        s[i]=32;
    for (i=0;i<12;i++)
    {
	if (uplink[nodenum].groups & (1<< i))
            s1[i]='A'+i;
        else
            s1[i]=32;
    }
    s1[11]=0;
    wprints(12,2,CYAN|_BLUE,"Allowed Groups    : ");
    wprints(12,22,CYAN|_BLUE,s1);
    wprints(13,2,CYAN|_BLUE,"Origin Address    : ");
    wprints(13,22,CYAN|_BLUE,uplink[nodenum].origin_address);
    wprints(10,2,CYAN|_BLUE,"Areas File Type   : ");
    sprintf(s,"%s",uplink[nodenum].areas_file_type?"<AREA> <DESCRIP.>":"Random");
    wprints(10,22,CYAN|_BLUE,s);
    sprintf(temp2,"%s",uplink[nodenum].unconditional?"Yes":"No ");
    sprintf(temp3,"%s",uplink[nodenum].add_plus?"Yes":"No ");
    sprintf(temp4,"%s",uplink[nodenum].areas_file_type?"<AREA> <DESCRIP.>":"Random");

    winpbeg(CYAN|_BLUE,LIGHTCYAN|_BLUE);
    winpdef(2,2,uplink[nodenum].address,"'Node Address      : '???????????????????",'U',2,NULL,0);
    winpfba(show_hint48,clear_hint);
    winpdef(4,2,uplink[nodenum].areafix_prog,"'Areafix Program   : '??????????????????",'0',2,NULL,0);
    winpfba(show_hint102,clear_hint);
    winpdef(5,2,uplink[nodenum].areafix_pw,"'Areafix Password  : '???????????????",'0',2,NULL,0);
    winpfba(show_hint103,clear_hint);
    winpdef(6,2,temp2, "'Unconditional Add : 'Y",'U',1,NULL,0);
    winpfba(show_hint104,clear_hint);
    winpdef(7,2,temp3, "'Use `+` To Add?   : 'Y",'U',1,NULL,0);
    winpfba(show_hint105,clear_hint);
    winpdef(9,2,uplink[nodenum].areas_filename, "'Areas Filename    : '????????????????????????????????????????????????",'U',2,NULL,0);
    winpfba(show_hint106,clear_hint);
    esc=winpread();
    if (!esc)
    {
        choose_filetype(nodenum);
        for (i=0;i<12;i++)
        {
            if (uplink[nodenum].groups & (1<< i))
                s1[i]='A'+i;
            else
                s1[i]=32;
        }
        s1[11]=0;
        wprints(12,2,LIGHTCYAN|_BLUE,"Allowed Groups    : ");
        wprints(12,22,LIGHTCYAN|_BLUE,s1);
        done=0;
        do
        {
            wprints(12,32,LIGHTCYAN|_BLUE," ");
            show_hint108();
            wgotoxy(12,32);
            ch=wgetchf("AaBbCcDdEeFfGgHhIiJj\r",0);
            wprints(12,32,LIGHTCYAN|_BLUE," ");
            if (ch!=13)
            {
                ch1=(upcase(ch)-'A');
                uplink[nodenum].groups ^= (1 <<ch1);
                for (i=0;i<12;i++)
                {
                    if (uplink[nodenum].groups & (1<< i))
                        s1[i]='A'+i;
                    else
                        s1[i]=32;
                }
                s1[11]=0;
                wprints(12,22,LIGHTCYAN|_BLUE,s1);
            }
            else
                done=1;
            if (ch==0)
                done=1;
        } while (!done);
        wprints(12,2,CYAN|_BLUE,"Allowed Groups    : ");
        wprints(12,22,CYAN|_BLUE,s1);
	choose_origin(nodenum);
        wprints(13,2,CYAN|_BLUE,"Origin Address    : ");
        wprints(13,22,CYAN|_BLUE,uplink[nodenum].origin_address);

        hidecur();
        clear_hint();
    }
    else
    {
        hidecur();
        show_node_screen(nodenum);
        return;
    }
    if (uplink[nodenum].address[0]!=NULL)
    {
        fidoadr_split(uplink[nodenum].address,&fadr);
        sprintf(uplink[nodenum].address,"%d:%d/%d.%d",fadr.zone,fadr.net,fadr.node,fadr.point);
    }
    strtrim(uplink[nodenum].areafix_prog);
    strtrim(uplink[nodenum].areafix_pw);
    strtrim(uplink[nodenum].areas_filename);
    strtrim(uplink[nodenum].origin_address);
    if (temp2[0]=='Y')
        uplink[nodenum].unconditional=1;
    else
        uplink[nodenum].unconditional=0;
    if (temp3[0]=='Y')
        uplink[nodenum].add_plus=1;
    else
        uplink[nodenum].add_plus=0;

    hidecur();
    show_node_screen(nodenum);
    show_hint58();
}
void choose_origin(int nodenum)
{
    int done,keypress,i;
    int last_pos,curr_pos,last_num;
    char s[5],temp[20];
    unsigned int keycode;
    done=i=0;
    last_pos=2;
    curr_pos=2;
    last_num=1;
    hidecur();
    wopen(6,1,21,77,1,YELLOW|_BLUE,WHITE|_BLUE);
    DoShadow();
    show_hint58();
    show_akas();
    do
    {
        last_pos=curr_pos;
        curr_pos=i+2;
        sprintf(s,"%-2d",last_num);
        last_num=i;
        wprints(last_pos,2,YELLOW|_BLUE,s);
        sprintf(s,"%-2d",i);
        last_num=i;
        wprints(curr_pos,2,WHITE|_GREEN,s);
        keycode=getxch();
        keypress=keycode>>8;
        switch (keypress)
        {
            case 1: /* ESC */
                done=1;
                break;
            case 28: /* Enter */
                sprintf(temp,"%d:%d/%d.%d",cfg.aka_list[i].zone,cfg.aka_list[i].net,cfg.aka_list[i].node,cfg.aka_list[i].point);
                strcpy(uplink[nodenum].origin_address,temp);
                done=1;
                break;
            case 71: /* Home */
                i=0;
                break;
            case 72: /* Up Arrow */
                i--;
                if (i<0)
                    i=10;
                break;
            case 79: /* End */
                i=10;
                break;
            case 80: /* Down Arrow */
                i++;
                if (i>10)
                    i=0;
                break;
        }

    } while (!done);

    wclose();
}



