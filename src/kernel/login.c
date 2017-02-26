/*
 * login.c : Dummy login screen
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

#include <login.h>
#include <kio.h>
#include <mm.h>
#include <kstring.h>
#include <vdesktop.h>
#include <kbd.h>
#include <console.h>
#include <sched.h>

user_t *root, *curr_user;

/*
 * post: creates users and passwds.
 *       returns negative value on error.
 */
int init_login()
{
    /* creates root user */
    if(!(root = (user_t *) kmalloc (sizeof(user_t)))){
        printk("init_login: out of memory.\n");
        return -1;
    }
    if(!(root->usrname = (char *) kmalloc (USRNAME_LEN+1))){
        printk("init_login: out of memory.\n");
        return -1;
    }
    if(!(root->passwd = (char *) kmalloc (PASSWD_LEN+1))){
        printk("init_login: out of memory.\n");
        return -1;
    }
    
    kstrcpy(root->usrname, "root");
    kstrcpy(root->passwd, "almighty");
    root->next = NULL;

    return 0;
}


/*
 * post: sets the current user.
 */
void set_curr_user(user_t *user)
{
    curr_user = user;
}


/*
 * post: returns the current user.
 */
user_t *get_curr_user()
{
    return curr_user;
}


/*
 * post: returns user_t* if the (usrname, passwd) pair is valid.
 *       or NULL otherwise
 */
user_t *auth(char *usrname, char *passwd)
{
    user_t *user;

    for(user=root; user; user=user->next){
        if(!(kstrcmp(user->usrname, usrname)) && 
           !(kstrcmp(user->passwd, passwd))){
            return user;
        }
    }

    return NULL;
}


/*
 * post: lists all users
 */
void list_users()
{
    user_t *user;
    for(user=root; user; user=user->next){
        printk("Username: %s \n", user->usrname);
    }
}


/*
 * post: adds a new user with (usrname, passwd)
 */
int add_user(char *usrname, char *passwd)
{
    user_t *new_user, *tmp;

    /* only root has the permission to add a new user */
    if(get_curr_user() != root){
        printk("add_user: Permission denied. (%s is not root.) \n",
                get_curr_user()->usrname);
        return -1;
    }
    
    /* check if the user already exists */
    for(new_user=root; new_user; new_user=new_user->next){
        if(!(kstrcmp(new_user->usrname, usrname))){
            printk("add_user: User %s already exits.\n", usrname);
            return -1;
        }
    }

    /* allocate memory for new user */
    if(!(new_user = (user_t *) kmalloc (sizeof(user_t)))){
        printk("add_user: out of memory.\n");
        return -1;
    }
    if(!(new_user->usrname = (char *) kmalloc (USRNAME_LEN+1))){
        printk("add_user: out of memory.\n");
        return -1;
    }
    if(!(new_user->passwd = (char *) kmalloc (PASSWD_LEN+1))){
        printk("add_user: out of memory.\n");
        return -1;
    }

    /* sets new user fields and link it to the users list */
    kstrcpy(new_user->usrname, usrname);
    kstrcpy(new_user->passwd, passwd);
    
    tmp = root->next;
    root->next = new_user;
    new_user->next = tmp;

    return 0;
}


/*
 * post: deletes a user with (username, passwd)
 */
int del_user(char *usrname)
{
    user_t *prev, *finger;
    int err=0;

    /* only root can delete a user */
    if(get_curr_user() != root){
        printk("del_user: Permission denied. (%s is not root.)\n",
                get_curr_user()->usrname);
        return -1;
    }

    /* we cannot delete root */
    if(!(kstrcmp(root->usrname, usrname))){
        printk("del_user: Permission denied. Cannot delete root.\n");
        return -1;
    }

    /* check if the user really exists, and we skip root */
    for(prev=root,finger=root->next; finger; prev=finger,finger=finger->next){
        if(!(kstrcmp(finger->usrname, usrname))){
            /* found. delete this user */
            prev->next = finger->next;
            if(kfree(finger->usrname)<0){
                printk("del_user: memory error.\n");
                err = 1;
            }
            if(kfree(finger->passwd)<0){
                printk("del_user: memory error.\n");
                err = 1;
            }
            if(kfree(finger)<0){
                printk("del_user: memory error.\n");
                err = 1;
            }
            if(err) return -1;
            return 0;
        }
    }

    /* not found */
    printk("del_user: User %s does not exist.\n", usrname);
    return -1;
}


/*
 * post: authenticate and login, returns true on success
 */
bool login(char *usrname, char *passwd)
{
    user_t *user = auth(usrname, passwd);

    if(user){
        set_curr_user(user);
        init_vd();
        start_vd();
        shell();
        return true;
    }

    return false;
}


/*
 * post: logout a user. destroys all virtual desktops, and promp for new login
 */
void logout()
{
    set_curr_user(NULL);
    destroy_vd();
    k_clear_screen();
    print_login();
}


/*
 * post: promp user for login
 */
void print_login()
{
    unsigned char usrname[10], passwd[10], c;
    int i=0;
    
    
    printk("          ");
    printk("************************************************************\n");

    printk("          ");
    printk("*             K-OS (Kai's Operating System)                *\n");
    
    printk("          ");
    printk("*       (C) 2003, Kai Chen <kchen@cs.caltech.edu>          *\n");

    printk("          ");
    printk("*                 CS134c, Spring 2003                      *\n");

    printk("          ");
    printk("************************************************************\n");
    printk("\n\n");

    while(1){
        printk("Login: ");

        while((c=kbd_buffer_dequeue())!='\n'){
            if(c==' ' || c=='\t')
                /* ignore */
                continue;
            else if(c=='\b' && i>0){
                i--;
                printk("\b");
            }
            else if(i>=8)
                /* ignore */
                continue;
            else if(c!='\b' && c<128){
                usrname[i++] = c;
                printk("%c", c);
            }
        }
        printk("\n");
        usrname[i] = NULL;
        i = 0;

        printk("Password: ");

        while((c=kbd_buffer_dequeue())!='\n'){
            if(c==' ' || c=='\t') continue;
            else if(c=='\b' && i>0) i--;
            else if(i>=8) continue;
            else if(c!='\b' && c<128) passwd[i++] = c;
        }
        printk("\n");
        passwd[i] = NULL;
        i = 0;

        if(login(usrname, passwd)) return;

        /* failed */
        printk("Permission denied. Please try again.\n");
    }
}
