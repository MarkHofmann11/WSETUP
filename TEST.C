#define MAX_NODES 20
/* ------------------------------------------------------------ */
/* Structure for nodes - Distribution, connects, etc.           */
/* ------------------------------------------------------------ */

typedef struct {
        int     zone,                   /* Zone of Node         */
                net,                    /* Net of Node          */
                node,                   /* Node of Node         */
                point;                  /* Point of Node        */
                used;
} netnode;

typedef struct {
        int     idx;                    /* Index Number         */
} feed_index;


typedef struct
{
	char            name[81],       /* Area Name                    */
			comment[81],    /* Comment on Area              */
                        origin[64],     /* Origin to use for this area  */
                        subtype[9],     /* WWIV Subtype To Use          */
                        directory[81];  /* Directory to .MSG section    */
        char            res_char[50];   /* Reserved Strings             */
//        netnode         feed[MAX_NODES];/* Feed Structure               */
        feed_index      feed[80];      /* Feed Index                   */
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
void main(void)
{
    int i;

    printf("netnode size = %d\r\n",sizeof(netnode));
    printf("netnode * MAX_NODES = %d\r\n",(sizeof(netnode)*MAX_NODES));
    i=sizeof(arearec);
    printf("Size = %d\r\n",i);
}
