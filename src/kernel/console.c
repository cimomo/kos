/*
 * console.c : K-OS console
 *
 * ------------------------------------------------------------------
 *
 * @begin[license]
 * Copyright (C) 2003  Kai Chen, Caltech
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * @author: Kai Chen
 * @email{kchen@cs.caltech.edu}
 * @end[license]
 *
 */


#include <console.h>
#include <kio.h>
#include <vdesktop.h>
#include <kbd.h>
#include <kstring.h>
#include <util.h>
#include <login.h>


/*
 * post: returns the index of the start of the command
 */
int cmd_start(char *cmd, int start)
{
    int i=start;

    while(cmd[i]){
        if(cmd[i] == ' ') i++;
        else return i;
    }
    return i;
}


/*
 * post: returns the end of the command
 */
int cmd_end(char *cmd, int start)
{
    int i=start;

    while(1){
        if(cmd[i]!=0 && cmd[i]!=' ') i++;
        else return i;
    }
}


/*
 * post: runs the K-OS shell and interprets commands
 *       warning: the command buffer may overflow. funny things will happen.
 */
void shell()
{
    unsigned char c;
    vd_t *vd;
    int start, end;
    

    while(1){
        printk("[K-OS:%s]> ", get_curr_user()->usrname);
        while((c=kbd_buffer_dequeue())!='\n'){
            if((alt_pressed()) && (c==K_F1 || c==K_F2 || c==K_F3 || c==K_F4)){
                switch_vd(c-K_F1);
                vd = get_curr_vd();
                if(vd->buffer_index==0 && get_cursor_col(get_cursor())==0)
                    printk("[K-OS:%s]> ", get_curr_user()->usrname);
                continue;
            }
            
            vd = get_curr_vd();
            if(c=='\b' && vd->buffer_index>0){
                vd->buffer_index --;
                printk("\b");
            }
            else if(c=='\t'){
                vd->buffer_index = (vd->buffer_index+8) & ~(8-1);
                printk("\t");
            }
            else if(c!='\b' && c<128){
                vd->cmd_buffer[vd->buffer_index++] = c;
                printk("%c",c);
            }
        }
        vd = get_curr_vd();
        vd->cmd_buffer[vd->buffer_index] = 0;
        printk("\n");

        /* interpret the command */
        start = cmd_start(vd->cmd_buffer, 0);
        end = cmd_end(vd->cmd_buffer, start);

        vd->cmd_buffer[end] = 0;

        if(end>start){
            if(!(kstrcmp(vd->cmd_buffer+start, "clear"))){
                k_clear_screen_from(SCREEN_COLS*CHAR_BYTES);
            }
            
            else if(!(kstrcmp(vd->cmd_buffer+start, "useradd"))){
                unsigned char usrname[10], passwd[10], passwd2[10];
                int i=0;
                while(1){
                    printk("Username: ");
                    while((c=kbd_buffer_dequeue())!='\n'){
                        if(c==' ' || c=='\t') continue;
                        else if(c=='\b' && i>0){
                            i--;
                            printk("\b");
                        }
                        else if(i>=8) continue;
                        else if(c!='\b' && c<128){
                            usrname[i++] = c;
                            printk("%c", c);
                        }
                    }
                    printk("\n");
                    usrname[i] = 0;
                    i = 0;

                    /* checks if usrname nonempty */
                    if(!(kstrlen(usrname))){
                        printk("Username too short. Try again? (y/n)\n");
                        do{
                            c=kbd_buffer_dequeue();
                        }while(c!='y' && c!='n');
                        if(c=='y') continue;
                        break;
                    }
                    
                    printk("Password: ");

                    while((c=kbd_buffer_dequeue())!='\n'){
                        if(c==' ' || c=='\t') continue;
                        else if(c=='\b' && i>0) i--;
                        else if(i>=8) continue;
                        else if(c!='\b' && c<128) passwd[i++] = c;
                    }
                    printk("\n");
                    passwd[i] = 0;
                    i = 0;

                    /* checks to see if passwd too short */
                    if((kstrlen(passwd))<4){
                        printk("Password too short. Try again? (y/n)\n");
                        do{
                            c=kbd_buffer_dequeue();
                        }while(c!='y' && c!='n');
                        if(c=='y') continue;
                        break;
                    }

                    printk("Enter password again: ");

                    while((c=kbd_buffer_dequeue())!='\n'){
                        if(c==' ' || c=='\t') continue;
                        else if(c=='\b' && i>0) i--;
                        else if(i>=8) continue;
                        else if(c!='\b' && c<128) passwd2[i++] = c;
                    }
                    printk("\n");
                    passwd2[i] = 0;
                    i = 0;

                    /* passwd matches */
                    if(!kstrcmp(passwd, passwd2)){
                        /* adduser failed */
                        if((add_user(usrname, passwd))<0){
                            printk("Failure. Try again? (y/n)\n");
                            do{
                                c=kbd_buffer_dequeue();
                            }while(c!='y' && c!='n');
                            if(c=='y') continue;
                            break;
                        }
                        /* succeed */
                        printk("User %s added.\n", usrname);
                        break;
                    }
                    
                    /* passwd dont match */
                    printk("2 passwords are different. Try again? (y/n)\n");
                    do{
                        c=kbd_buffer_dequeue();
                    }while(c!='y' && c!='n');
                    if(c=='y') continue;
                    break;
                }
            }
                    
            else if(!(kstrcmp(vd->cmd_buffer+start, "userdel"))){
                unsigned char usrname[10];
                start = cmd_start(vd->cmd_buffer, end+1);
                end = cmd_end(vd->cmd_buffer, start);
                vd->cmd_buffer[end] = 0;
                kstrcpy(usrname, vd->cmd_buffer+start);
                if(del_user(usrname) < 0){
                    printk("Failure. Cannot delete user %s.\n",usrname);
                }
                else{
                    printk("User %s deleted.\n", usrname);
                }
            }   
                
            else if(!(kstrcmp(vd->cmd_buffer+start, "users"))){
                list_users();
            }

            else if(!(kstrcmp(vd->cmd_buffer+start, "logout"))){
                logout();
            }

            else if(!(kstrcmp(vd->cmd_buffer+start, "exit"))){
                logout();
            }
            
            else if(!(kstrcmp(vd->cmd_buffer+start, "reboot"))){
                _reboot();
            }
                
            else if(!(kstrcmp(vd->cmd_buffer+start, "help"))){
                printk("K-OS Help Menu: \n");
                printk("Alt + (F1 - F4)\t\t - Switch virtual desktops.\n");
                printk("clear\t\t\t - Clear the screen.\n");
                printk("useradd\t\t\t - Add a new user.\n");
                printk("userdel <usrname>\t - Delete a user.\n");
                printk("users\t\t\t - List all users.\n");
                printk("logout\t\t\t - Logout the system.\n");
                printk("exit\t\t\t - Exit the system (logout).\n");
                printk("reboot\t\t\t - Reboot the machine.\n");
                printk("help\t\t\t - Display this menu.\n");
            }
            

            else{
                printk("%s: Command not found.\n", vd->cmd_buffer);
            }
        }

        vd->buffer_index = 0;
    }
}
