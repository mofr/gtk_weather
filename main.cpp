#include <gtk/gtk.h>
//#include <glade/glade.h>
#include <glib.h>
#include <string>
#include <stdlib.h>
#include "xml_parser.h"

#ifdef WIN32
#include <winsock.h>
#elif
#include <sock.h>
#endif

GtkWidget *window,*switch_button;
//GladeXML *xml;
GtkWidget *menu;
GtkStatusIcon *status_icon;

std::string tooltip;
int active=1;
char change;

int port_number = 80;
sockaddr_in server_info;
in_addr * addressptr;
std::string query("GET /data.xml HTTP/1.0\r\nHost: termo.tomsk.ru\r\n\r\n");

int load_file(std::string url,std::string &out)
{
int main_socket = socket(AF_INET, SOCK_STREAM, 0);
if (main_socket<0)
   {
   printf("error socket creation\n");
   return 0;
   }
   
int r = connect(main_socket, (sockaddr *) &server_info, sizeof(server_info));
if (r<0)
    {
    printf("connect error\n");
    return 0;
    }
    
send(main_socket,query.c_str(),query.length(),0);

char* result=new char[1025];
recv(main_socket,result,1024,0);
result[1024]='\0';
//printf("%s\n",result);

closesocket(main_socket);

out=result;
free(result);
return 1;
}

int update()
{
if(active)
    {
    std::string temper,day,week,month;
    if(!load_file("",temper))
        {
        gtk_status_icon_set_from_file(status_icon,"icon-off.ico");
        return 0;
        /*
        tooltip="Connecting...";
        gtk_status_icon_set_tooltip(status_icon,g_locale_to_utf8(tooltip.c_str(),tooltip.length(),0,0,0));
        gtk_status_icon_set_from_file(status_icon,"icon.ico");*/
        }

    int pos1,pos2;
    pos1=temper.find("<termo>",0);
    pos2=temper.find("</termo>",0);
    temper.assign(&temper.c_str()[pos1],pos2-pos1+8);
    temper=parseDoc(temper,&change);
     
    tooltip="Сегодня: "+temper;

    gtk_status_icon_set_tooltip(status_icon,g_locale_to_utf8(tooltip.c_str(),tooltip.length()+5,0,0,0));
    if(change=='-')
        gtk_status_icon_set_from_file(status_icon,"icon-down.ico");
    if(change=='+')
        gtk_status_icon_set_from_file(status_icon,"icon-up.ico");
    }
else
    {
    tooltip="Disabled";
    gtk_status_icon_set_tooltip(status_icon,g_locale_to_utf8(tooltip.c_str(),tooltip.length(),0,0,0));
    }
}

void* thread_update(void *data)
{
_sleep(500);
do
    {
    update();
    _sleep(1000*60*10);
    }
while(1);
}


void popup_menu()
{
gtk_menu_popup(GTK_MENU(menu),NULL,NULL,NULL,NULL,0,0);
}


void popdown_menu()
{
gtk_menu_popdown(GTK_MENU(menu));
}


void enable_switch()
{
active=!active;
if(active)
    {
    tooltip="Connecting...";
    gtk_status_icon_set_tooltip(status_icon,g_locale_to_utf8(tooltip.c_str(),tooltip.length(),0,0,0));
    gtk_status_icon_set_from_file(status_icon,"icon.ico");
    update();
    }
else
    {
    tooltip="Disabled";
    gtk_status_icon_set_tooltip(status_icon,g_locale_to_utf8(tooltip.c_str(),tooltip.length(),0,0,0));
    gtk_status_icon_set_from_file(status_icon,"icon-off.ico");
    }
}


void on_Exit()
{
gtk_status_icon_set_visible(status_icon,false);
gtk_main_quit();
}

int main(int argc, char *argv[])
{
atexit(on_Exit);
#ifdef WIN32
WSADATA wsaData;
WSAStartup(0x0101, &wsaData);
#endif

hostent * host = gethostbyname("termo.tomsk.ru");
if (host==NULL)
  {
  printf("gethostbyname failed\n");
  exit(1);
  }
addressptr = (in_addr *) host->h_addr;

server_info.sin_family = AF_INET;
server_info.sin_addr = * addressptr;
server_info.sin_port = htons(port_number);

g_thread_init(NULL);

gtk_init(&argc,&argv);

menu=gtk_menu_new();
GtkWidget *item;
item=gtk_check_menu_item_new_with_label("Disabled");
g_signal_connect(G_OBJECT(item),"activate",enable_switch,NULL);
gtk_menu_append(GTK_MENU(menu),item);
switch_button=item;

item=gtk_separator_menu_item_new();
gtk_menu_append(GTK_MENU(menu),item);

item=gtk_menu_item_new_with_label("Exit");
g_signal_connect(G_OBJECT(item),"activate",on_Exit,NULL);
gtk_menu_append(GTK_MENU(menu),item);
gtk_widget_show_all(menu);

status_icon=gtk_status_icon_new_from_file("icon.ico");
g_signal_connect(G_OBJECT(status_icon),"popup-menu",popup_menu,NULL);
g_signal_connect(G_OBJECT(status_icon),"activate",popdown_menu,NULL);
tooltip="Connecting...";
gtk_status_icon_set_tooltip(status_icon,g_locale_to_utf8(tooltip.c_str(),tooltip.length(),0,0,0));
    
g_thread_create(thread_update,NULL,true,NULL);

gtk_main ();

#ifdef WIN32
WSACleanup();
#endif
return 0;
}
