/* Contains the callbacks for all of the stock non-numeric message types
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
#ifndef __MESSAGE_TYPES_H__
#define __MESSAGE_TYPES_H__
#include "irc_network.h"

extern void init_message_types();

extern void cap_msg_callback(struct irc_network * network,
                             char * hostmask,
                             short argc,
                             char * argv[]);
extern void join_msg_callback(struct irc_network * network,
                              char * hostmask,
                              short argc,
                              char * argv[]);
extern void part_msg_callback(struct irc_network * network,
                              char * hostmask,
                              short argc,
                              char * argv[]);
extern void privmsg_msg_callback(struct irc_network * network,
                                 char * hostmask,
                                 short argc,
                                 char * argv[]);
extern void notice_msg_callback(struct irc_network * network,
                                char * hostmask,
                                short argc,
                                char * argv[]);
extern void ping_msg_callback(struct irc_network * network,
                              char * hostmask,
                              short argc,
                              char * argv[]);
extern void nick_msg_callback(struct irc_network * network,
                              char * hostmask,
                              short argc,
                              char * argv[]);
extern void topic_msg_callback(struct irc_network * network,
                               char * hostmask,
                               short argc,
                               char * argv[]);
extern void mode_msg_callback(struct irc_network * network,
                              char * hostmask,
                              short argc,
                              char * argv[]);

#endif // __MESSAGE_TYPES_H__

// vim: expandtab:tw=80:tabstop=4:shiftwidth=4:softtabstop=4
