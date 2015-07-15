  #ifdef _DEFINE_GLOBALS_
    #define __EXTRN__
  #else
    #define __EXTRN__ extern
  #endif

#include <conio.h>
#include <stdio.h>
#include <dos.h>
#include <process.h>
#include <stdlib.h>
#include <string.h>
#include <dir.h>
#include <alloc.h>
#include <io.h>
#include <fcntl.h>
#include <stdarg.h>
#include <time.h>
#include <ctype.h>
#include <sys\stat.h>
#include <bp.h>
#include "cxlkey.h"
#include "cxlvid.h"
#include "cxlwin.h"
#include "cxldef.h"
#include "cxlstr.h"
#include "net.h"


#define MT_DESQVIEW 0x01
#define MT_WINDOWS  0x02
#define MT_OS2      0x04
#define MT_NB       0x40

#define VERSION "1.40"
#define DATE "03/06/98"
#define PROGRAM "WWIVTOSS SETUP"
#define KEYCODE 1996
#define MAX_AREAS 130
#define MAX_NODES 20
#define NUM_ARCHIVERS 8 /* 1+Number of archivers (0=.PKT) */
/* ------------------------------------------------------------ */
/* Structure for nodes - Distribution, connects, etc.           */
/* ------------------------------------------------------------ */

typedef struct {
        int     zone,                   /* Zone of Node         */
                net,                    /* Net of Node          */
                node,                   /* Node of Node         */
                point;                  /* Point of Node        */
  //              used;
} netnode;

typedef struct {
        int     idx;                    /* Index Number         */
} feed_index;


/* ------------------------------------------------------------ */
/* Translation Table Structure.  Stored in XLAT.DAT             */
/* ------------------------------------------------------------ */

__EXTRN__ struct
{
        unsigned char   in,     /* Character to change to - Incoming    */
                        out;    /* Character to change to - Outgoing    */
} xlat[225];

/* ------------------------------------------------------------ */
/* Structure for Archivers                                      */
/* ------------------------------------------------------------ */

typedef struct {
        char    name[10],               /* Program ID           */
                archive_file[15],       /* Archive Program      */
                unarchive_file[15],     /* Unarchive Program    */
                archive_switches[30],   /* Archive Switches     */
		unarchive_switches[30]; /* Unarchive Switches   */
        char    reserved[50];           /* Reserved             */
} arc_dat;

/* ------------------------------------------------------------ */
/* Structure for determining where to route netmail             */
/*              Stored in WWIVTOSS.DAT                          */
/* ------------------------------------------------------------ */

typedef struct {
        int     zone,                   /* Zone of Node         */
                net,                    /* Net of Node          */
                node,                   /* Node of Node         */
                point;                  /* Point of Node        */
        int     route_zone;             /* Zone to route on     */
} route_mail;

/* ------------------------------------------------------------ */
/* Structure for areas                                          */
/*              Saved in AREAS.DAT                              */
/* ------------------------------------------------------------ */

typedef struct
{
	char            name[81],       /* Area Name                    */
			comment[81],    /* Comment on Area              */
                        origin[64],     /* Origin to use for this area  */
                        subtype[9],     /* WWIV Subtype To Use          */
                        directory[81];  /* Directory to .MSG section    */
        char            res_char[50];   /* Reserved Strings             */
        netnode         feed[MAX_NODES];/* Feed Structure               */
//        feed_index      feed[160];      /* Feed Index                   */
        int             type;           /* Passthru, local, regular     */
        int             alias_ok,       /* Ok to pass aliases?          */
                        pass_color,     /* Ok to pass WWIV Colors?      */
                        high_ascii,     /* Ok to pass High ASCII?       */
                        net_num,        /* WWIV Network Number          */
                        group,          /* Area Group #                 */
                        count,          /* Count of messages            */
                        active,         /* If 0, marks end of list      */
                        keep_tag,       /* Keep tagline?                */
                        def_origin,     /* Using Default Origin?        */
                        count_out,      /* Export Count                 */
                        translate,      /* Use Translation Tables       */
                        val_incoming;   /* Validate Incoming Messages?  */
        unsigned char   res_byte[18];   /* Reserved Bytes               */
} arearec;

/* ------------------------------------------------------------ */
/* Structure of nodes for connect/feed data                     */
/*              Saved in NODES.DAT                              */
/* ------------------------------------------------------------ */

__EXTRN__ struct
{
        char            address[40],    /* Network Address of Node      */
                        sysop_name[81], /* Sysop of Node                */
                        pkt_pw[30],     /* Packet Password to Use       */
                        sysop_phone[15],/* Sysop's Voice Phone          */
                        domain[10],     /* Domain                       */
                        res_char[75];   /* Reserved Bytes               */
        unsigned char   compression;    /* Compression to use           */
        unsigned int    daynum,         /* Day Of The Year              */
                        counter;        /* Counter of sent packets      */
        char            areafix_pw[12]; /* Areafix Password             */
        int             archive_status; /* Packet Archive Status        */
        int             direct;         /* Direct Flag                  */
        int             allow_areafix;  /* Can this node Areafix?       */
        unsigned short  groups;         /* Groups this node is allowed  */
        int             packet_type;    /* Packet Type To Send          */
        char            res_bytes[24];  /* Reserved bytes               */
} nodes [20];

/* ------------------------------------ */
/* Structure for the Uplink Manager     */
/* ------------------------------------ */
__EXTRN__ struct
{
        char            address[40],            /* Address of Uplink    */
                        areafix_prog[30],       /* Areafix Program      */
                        areafix_pw[30],         /* Areafix Password     */
                        areas_filename[81],     /* Areas Filename       */
                        origin_address[40],     /* Origin AKA To Use    */
			res_char[80];            /* Reserved Characters  */
        int             add_plus,               /* Use '+' to add areas */
                        areas_file_type,        /* Areas File Type      */
                        unconditional,          /* Send Unconditioanlly */
                        res_int[20];            /* Reserved Integers    */
        unsigned short  groups;                 /* Groups Useable       */
} uplink[20];

/* ------------------------------------------------------------ */
/* Structure for this system's AKA information.                 */
/*              Saved in WWIVTOSS.DAT as tosser_cfg.akas        */
/* ------------------------------------------------------------ */

typedef  struct {
        int     zone,                   /* Zone of Node         */
                net,                    /* Net of Node          */
                node,                   /* Node of Node         */
                point;                  /* Point of Node        */
        int     wwiv_node,              /* WWIV Network Node    */
                wwiv_netnum;            /* WWIV Network Number  */
} akas;

__EXTRN__ struct {
        int     zone,                   /* Zone of Node         */
                net,                    /* Net of Node          */
                node,                   /* Node of Node         */
                point,                  /* Point of Node        */
                wwiv_node,              /* WWIV Network Node    */
                wwiv_netnum;            /* WWIV Network Number  */
        char    network_name[20];       /* Network Name         */
} akas2[20];

__EXTRN__ struct {
        char    domain[20];             /* Domain Name          */
} domains[11];

/* ------------------------------------------------------------ */
/* Structure for area group names                               */
/*              Stored in tosser_cfg                            */
/* ------------------------------------------------------------ */
typedef struct {
        char    group_id[2],            /* Group I.D. (A-Z)     */
                group_name[21];         /* Group Name           */
} groups;

/* ------------------------------------------------------------ */
/* Structure for main WWIVTOSS Configuration Data               */
/*              Saved as WWIVTOSS.DAT                           */
/* ------------------------------------------------------------ */

typedef struct
{
        char            bbs_name[81],           /* BBS Name             */
                        sysop_name[81],         /* SysOp Name           */
                        password[20],           /* WTSETUP Password     */
                        system_address[20],     /* Main Network Address */
                        inbound_path[64],       /* Inbound Packet Dir   */
                        outbound_path[64],      /* Outbound Packet Dir  */
                        outbound_temp_path[64], /* Temp Outbound Dir    */
                        netmail_path[64],       /* NetMail Dir          */
                        temp_path[64],          /* Temporary Path       */
                        badecho_path[64],       /* Where Bad Echos Go   */
                        origin_line[64],        /* Default Origin Line  */
                        log_file[81],           /* Path of log file     */
                        registration[80],       /* Registration Key     */
                        location[40];           /* Site Location        */
        char            reserved[100];          /* Reserved Bytes       */
        akas            aka_list[11];           /* List of AKAs         */
        route_mail      route_to[11];           /* Where to Route Mail  */
        arc_dat         archive[10];            /* Archive Information  */
	groups          group[26];              /* Group Information    */
        unsigned char   crash,                  /* Crash Mail Flag      */
                        direct,                 /* Direct Mail Flag     */
                        center,                 /* Allow centering?     */
                        delete_sent,            /* Delete SENT Mail?    */
                        import_recd,            /* Import Rec'd Mail?   */
                        use_areafix,            /* Use AREAFIX?         */
                        skip_mail,              /* Skip Importing Mail  */
                        notify,                 /* Notify Author Flag   */
                        default_compression,    /* Default Compression  */
                        registered,             /* Registered Flag      */
                        mailer,                 /* Mailer Type          */
                        installed,              /* Installed Flag!      */
                        cleanup,                /* Cleanup Nets         */
                        kill_recd,              /* Delete Rec'd Mail?   */
                        num_areas,              /* Number of Areas      */
                        num_nodes,              /* Number of Nodes      */
                        set_immediate;          /* Use IMMEDIATE Flag?  */
        unsigned long   date_installed;         /* Date Installed       */
        int             auto_add,               /* Auto Add Areas?      */
                        log_feeds,              /* Log Feed Echos?      */
                        add_hard_cr,            /* Add Hard C/R         */
                        add_line_feed,          /* Add Line Feed        */
                        high_ascii,             /* Pass High ASCII Mail */
                        soft_to_hard,           /* Soft To Hard C/R     */
                        hard_to_soft,           /* Hard To Soft C/R     */
                        add_soft_cr,            /* Add Soft C/R         */
                        route_me,               /* Route Mail?          */
                        log_days,               /* Days To Keep Log     */
                        color_to,               /* Colorize TO: Line?   */
                        initial_quote,          /* Initial Quote?       */
                        num_uplinks,            /* Number of Uplinks    */
                        bounce_mail,            /* Bounce Bad Mail?     */
                        check_dupes,            /* Check Dupe Messages? */
                        pass_origin;            /* Use Existing Origin? */
        char            res_byte[46];           /* Reserved Bytes       */
} tosser_config;

/* ------------------------------------------------------------ */
/* Miscellaneous Global Options                                 */
/* ------------------------------------------------------------ */
__EXTRN__ tosser_config cfg;
__EXTRN__ configrec syscfg;
__EXTRN__ arearec thisarea;
__EXTRN__ netnode nettnode;
__EXTRN__ net_networks_rec *net_networks;
__EXTRN__ char bbs_name[80],temp_stat[5];
__EXTRN__ unsigned long reg_key,correct_key;
__EXTRN__ int registered,startup,num_areas,current_area,num_nodes,current_node,nodenum,num_uplink,uplink_num;
__EXTRN__ int window_1,window_2,window_3,window_4,window_5,window_6,window_7,window_8;
__EXTRN__ int items,enter_key,curr_area,value,areawin,spot,quiet,net_num_max,multitasker;
__EXTRN__ char *path_ptr,temp_ch[81];
