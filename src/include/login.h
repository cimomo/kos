/*
 * login.h : dummy login screen header
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

#include "kernel.h"

#define MAX_USERS           100
#define USRNAME_LEN         8
#define PASSWD_LEN          8

typedef struct _user {
    char *usrname;
    char *passwd;
    struct _user *next;
} user_t;


int init_login();
void set_curr_user(user_t *user);
user_t *get_curr_user();
user_t *auth(char *usrname, char *passwd);
void list_users();
int add_user(char *usrname, char *passwd);
int del_user(char *usrname);
bool login(char *usrname, char *passwd);
void logout();
void print_login();
