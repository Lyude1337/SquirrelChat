/*
 * Copyright (C) 2013 Stephen Chandler Paul
 *
 * This file is free software: you may copy it, redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of this License or (at your option) any
 * later version.
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "../irc_network.h"
#include "chat_window.h"
#include "../trie.h"

#include <pthread.h>
#include <gtk/gtk.h>

/* TODO: Make a typeless buffer object, and make children buffer objects for
 * all other types of buffers
 */
enum buffer_type {
    NETWORK,
    CHANNEL,
    QUERY
};

struct __channel_data {
    GtkListStore * user_list_store;
    trie * users;
};

struct __query_data {
    char * away_msg;
    bool received_away;
};

struct __queued_output {
    char * msg;
    size_t msg_len;
    struct __queued_output * next;
};

struct buffer_info {
    enum buffer_type type;
    char * buffer_name;
    GtkTreeRowReference * row;

    pthread_mutex_t output_mutex;
    struct __queued_output * out_queue;
    struct __queued_output * out_queue_end;
    size_t out_queue_size;

    struct irc_network * network;
    struct chat_window * window;
    GtkTextBuffer * buffer;
    double buffer_scroll_pos;
    GtkEntryBuffer * command_box_buffer;

    union {
        struct __channel_data * chan_data;
        struct __query_data * query_data;
        void * extra_data;
    };
};

extern struct buffer_info * new_buffer(const char * buffer_name,
                                       enum buffer_type type,
                                       struct irc_network * network);
extern void destroy_buffer(struct buffer_info * buffer)
    _nonnull(1);

extern void print_to_buffer(struct buffer_info * buffer,
                            const char * msg, ...)
    _nonnull(1, 2) _format(printf, 2, 3);

#endif /* __BUFFER_H__ */
// vim: expandtab:tw=80:tabstop=4:shiftwidth=4:softtabstop=4
