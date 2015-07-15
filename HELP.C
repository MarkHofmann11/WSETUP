#include "vardec.h"
#include "vars.h"
#include "fcns.h"
void clear_hint(void)
{
    int i;
    for (i=0;i<80;i++)
        wwprints(window_1,24,i,WHITE|_BLUE," ");
}
char *hint[] = {
    "Hint 0",
    "Setup Basic WWIVToss Options",
    "Miscellaneous Global Options",
    "Setup Areas",
    "Setup Nodes",
    "Configure Mail Routing",
    "Setup Site Info",
    "Network Addresses and AKAs",
    "Define Path Names",
    "Define Archivers and Options",
    "Define Area Group Names",
    "Define Properties of Internal AREAFIX",
    "BBS Name as in WWIV INIT",
    "SysOp Real Name",
    "City & State Location",
    "WSETUP Password (Leave blank for none) - Case Sensitive!",
    "Default Origin Line",
    "Path and Filename of Logfile.  Leave Blank For None",
    "Your Registration Key",
    "Path to Inbound Packets",
    "Path to Outbound Packets",
    "Temporary Outbound Path",
    "Path to NetMail Directory",
    "Temporary Directory",
    "Directory Where Bad or Unknown Messages Are To Be Placed",
    "Path and Filename of ARCHIVE Program",
    "Switches to Use With ARCHIVE Program",
    "Path and Filename of UNARCHIVE Program",
    "Switches to Use With UNARCHIVE Program",
    "Default Crash Mail Setting. 0=Route All, 1=Crash My Net Only, 2=Crash All",
    "Default Direct Mail Setting.  0=Not Direct, 1=Direct My Net Only, 2=Direct ALL",
    "Translate WWIV Centering Codes in Outgoing Messages",
    "Delete Mail That Has Been Marked As SENT",
    "Import Mail That Has Been Marked As Recieved",
    "Use Internal AREAFIX",
    "Skip Importing Netmail",
    "Cleanup Networks After Importing",
    "Mailer Type (Spacebar to Cycle)",
    "Area Name as in Fidonet ELIST",
    "Comment Regarding Area (Optional)",
    "Group Area Belongs in (A-J Only, SPACE=none)",
    "WWIV Subtype For This Area",
    "WWIV Network Number as Listed in INIT (MINUS 1!!!)",
    "Allow Use of Handles in Area?",
    "Allow Use of WWIV Color Codes In Area?",
    "Allow Use of High ASCII Codes (>127)?",
    "Origin Line For This Echo (Leave Blank for Default)",
    "Nodes to Export This Echo To",
    "Node Address",
    "Node's SysOp Real Name",
    "Password To Use On Packets To/From This Node (Blank To Disable)",
    "Password For This Node To Use With Areafix",
    "How Packets For This Node Will Be Sent (Spacebar to Cycle)",
    "How Packets For This Node Will Be Compressed",
    "Mail Route Manager",
    "Your Fido Technology Network Node Number (Zone:Net/Node.Point)",
    "Fake WWIV Node For This Network",
    "WWIV Network Number For This Network As Listed in INIT (MINUS 1!!!)",
    "Arrow Keys to Move, [ENTER] to Edit, ESC to Exit",
    "Arrow Keys to Move, [ENTER] to Edit, ESC to exit, [DEL] to delete",
    "About WWIVToss!",                          /* 60 */
    "Keep BBS Tagline on Messages?",
    "Export WWIVTOSS Setup Information",        /* 62 */
    "Export Node Manager Configuration",
    "Export Area Manager Configuration",
    "Export Group Names",
    "Export Echo Name Listing",
    "Export Echo Names Sorted by WWIV Subtype", /* 67 */
    "Globally Change Area Export Information",
    "Globally Change Area Groups",
    "Globally Change 'Use Alias'",
    "Globally Change 'Use WWIV Color'",
    "Globally Change 'Allow High ASCII'",       /* 72 */
    "Globally Change 'Keep BBS Tag'",
    "Globally Change WWIV Network Number",
    "Globally Change Area Origin Line",         /* 75 */
    "Domain Name for This Network (Network Name)",
    "Press The Key For the Character That You Wish To Change.  [ESC] Exits",
    "Edit Character Translation Tables",
    "Edit Incoming Character Translation Table",
    "Edit Outgoing Character Translation Table",    /* 80 */
    "Use Translation Tables On Incoming/Outgoing Messages For This Area",
    "List of Groups (A-J) That This Node Is Allowed To Areafix", /* 82 */
    "Set IMMEDIATE Flag on Outbound Messages?", /* 83 */
    "Automatically Add Unlisted Areas to Area Manager?",
    "Log Passthrough Areas to Logfile?", /*85 */
    "Add Hard Carriage Returns To Inbound Messages?",
    "Add Line Feeds To Inbound Messages?",
    "Add Soft Carriage Returns To Inbound Messages?",
    "Convert Soft Carriage Returns To Hard Carriage Returns on Inbound?",
    "Convert Hard Carriage Returns To Soft Carriage Returns on Inbound?",
    "Route Mail Via Route Manager?",
    "Pass High ASCII Codes In Outgoing Netmail?", /*92 */
    "Create Pseudo SUBS.LST Files In The Appropriate Network Directories",
    "Directory To Put .MSG Files In, If Area Type Is 2",
    "Type Of Area [0=Passthru, 1=Import To WWIV, 2=Write To .MSG Files", //95
    "Purge WWIVTOSS.LOG After How Many Days? (0 = Disable Purging)",
    "Colorize TO: Line on incoming messages?", //97
    "Add Author's Initials to '>' When Quoting?",
    "Validate Incoming Fidonet Messages For This Area?",    //99
    "Globally Change 'Valid. Incoming?'", //100
    "Node Sysop's Voice Phone Number",//101
    "Name of Uplink's Areafix Program",
    "Password to use with Uplink's Areafix Program",
    "Send Add Request Even If Area No Listed In Areas File?",
    "Use '+' When Adding Areas?",
    "Filename of Uplinks Area Listing",
    "Uplink Area Listing Is In What Format? ([SPACE] Cycles)",
    "Area Groups To Go To This Uplink (A-J Only, Space=NONE)",
    "Origin Address To Use For This Uplink",    //109
    "Bounce Back Bad Email To Source?",          //110
    "Packet Type To Send To This Feed (SPACEBAR Cycles)", //111
    "Check For and Delete Duplicate Inbound Messages?", //112
    "Check For and Use Existing Origin Lines, if present and valid?",//113
    NULL
};

void show_hint1(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[1]);
}
void show_hint2(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[2]);
}
void show_hint3(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[3]);
}
void show_hint4(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[4]);
}
void show_hint5(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[5]);
}
void show_hint6(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[6]);
}
void show_hint7(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[7]);
}
void show_hint8(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[8]);
}
void show_hint9(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[9]);
}
void show_hint10(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[10]);
}
void show_hint11(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[11]);
}
void show_hint12(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[12]);
}
void show_hint13(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[13]);
}
void show_hint14(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[14]);
}
void show_hint15(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[15]);
}
void show_hint16(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[16]);
}
void show_hint17(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[17]);
}
void show_hint18(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[18]);
}
void show_hint19(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[19]);
}
void show_hint20(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[20]);
}
void show_hint21(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[21]);
}
void show_hint22(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[22]);
}
void show_hint23(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[23]);
}
void show_hint24(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[24]);
}
void show_hint25(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[25]);
}
void show_hint26(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[26]);
}
void show_hint27(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[27]);
}
void show_hint28(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[28]);
}
void show_hint29(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[29]);
}
void show_hint30(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[30]);
}
void show_hint31(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[31]);
}
void show_hint32(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[32]);
}
void show_hint33(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[33]);
}
void show_hint34(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[34]);
}
void show_hint35(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[35]);
}
void show_hint36(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[36]);
}
void show_hint37(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[37]);
}
void show_hint38(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[38]);
}
void show_hint39(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[39]);
}
void show_hint40(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[40]);
}
void show_hint41(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[41]);
}
void show_hint42(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[42]);
}
void show_hint43(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[43]);
}
void show_hint44(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[44]);
}
void show_hint45(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[45]);
}
void show_hint46(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[46]);
}
void show_hint47(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[47]);
}
void show_hint48(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[48]);
}
void show_hint49(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[49]);
}
void show_hint50(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[50]);
}
void show_hint51(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[51]);
}
void show_hint52(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[52]);
}
void show_hint53(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[53]);
}
void show_hint54(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[54]);
}
void show_hint55(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[55]);
}
void show_hint56(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[56]);
}
void show_hint57(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[57]);
}
void show_hint58(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[58]);
}
void show_hint59(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[59]);
}
void show_hint60(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[60]);
}
void show_hint61(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[61]);
}
void show_hint62(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[62]);
}
void show_hint63(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[63]);
}
void show_hint64(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[64]);
}
void show_hint65(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[65]);
}
void show_hint66(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[66]);
}
void show_hint67(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[67]);
}
void show_hint68(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[68]);
}
void show_hint69(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[69]);
}
void show_hint70(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[70]);
}
void show_hint71(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[71]);
}
void show_hint72(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[72]);
}
void show_hint73(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[73]);
}
void show_hint74(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[74]);
}
void show_hint75(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[75]);
}
void show_hint76(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[76]);
}
void show_hint77(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[77]);
}
void show_hint78(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[78]);
}
void show_hint79(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[79]);
}
void show_hint80(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[80]);
}
void show_hint81(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[81]);
}
void show_hint82(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[82]);
}
void show_hint83(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[83]);
}
void show_hint84(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[84]);
}
void show_hint85(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[85]);
}
void show_hint86(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[86]);
}
void show_hint87(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[87]);
}
void show_hint88(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[88]);
}
void show_hint89(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[89]);
}
void show_hint90(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[90]);
}
void show_hint91(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[91]);
}
void show_hint92(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[92]);
}
void show_hint93(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[93]);
}
void show_hint94(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[94]);
}
void show_hint95(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[95]);
}
void show_hint96(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[96]);
}
void show_hint97(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[97]);
}
void show_hint98(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[98]);
}
void show_hint99(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[99]);
}
void show_hint100(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[100]);
}
void show_hint101(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[101]);
}
void show_hint102(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[102]);
}
void show_hint103(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[103]);
}
void show_hint104(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[104]);
}
void show_hint105(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[105]);
}
void show_hint106(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[106]);
}
void show_hint107(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[107]);
}
void show_hint108(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[108]);
}
void show_hint109(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[109]);
}
void show_hint110(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[110]);
}
void show_hint111(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[111]);
}
void show_hint112(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[112]);
}
void show_hint113(void)
{
    clear_hint();
    wwprints(window_1,24,1,LIGHTCYAN|_BLUE,hint[113]);
}

