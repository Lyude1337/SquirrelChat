/* Functions for creating and managing chat windows
 *
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

#include "chat_window.h"

#include "buffer.h"
#include "main_menu_bar.h"
#include "network_tree.h"
#include "command_box.h"
#include "buffer_view.h"
#include "user_list.h"

#include <stdlib.h>
#include <gtk/gtk.h>

struct sqchat_chat_window * sqchat_chat_window_new(struct sqchat_network * network) {
    struct sqchat_chat_window * new_window = malloc(sizeof(struct sqchat_chat_window));
    memset(new_window, '\0', sizeof(struct sqchat_chat_window));

    new_window->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(new_window->window), "SquirrelChat");
    gtk_window_set_position(GTK_WINDOW(new_window->window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(new_window->window), 640, 480);

    new_window->top_vertical_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(new_window->window),
                      new_window->top_vertical_container);

    sqchat_main_menu_bar_setup(new_window);
    gtk_box_pack_start(GTK_BOX(new_window->top_vertical_container),
                       new_window->main_menu_bar, FALSE, FALSE, 0);

    new_window->network_tree_and_buffer_pane =
        gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(new_window->top_vertical_container),
                       new_window->network_tree_and_buffer_pane,
                       TRUE, TRUE, 0);

    sqchat_network_tree_setup(new_window);
    new_window->scrolled_window_for_network_tree =
        gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_kinetic_scrolling(
            GTK_SCROLLED_WINDOW(new_window->scrolled_window_for_network_tree),
            TRUE);
    gtk_container_add(GTK_CONTAINER(new_window->scrolled_window_for_network_tree),
                      new_window->network_tree);
    gtk_paned_add1(GTK_PANED(new_window->network_tree_and_buffer_pane),
                   new_window->scrolled_window_for_network_tree);

    new_window->chat_viewer_and_user_list_pane
        = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_add2(GTK_PANED(new_window->network_tree_and_buffer_pane),
                   new_window->chat_viewer_and_user_list_pane);
    gtk_paned_set_position(GTK_PANED(new_window->chat_viewer_and_user_list_pane),
                           380);

    /* Set the default position of the divider between the network tree and
     * buffer
     */
    gtk_paned_set_position(GTK_PANED(new_window->network_tree_and_buffer_pane),
                           125);

    new_window->buffer_and_command_box_container =
        gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_paned_add1(GTK_PANED(new_window->chat_viewer_and_user_list_pane),
                   new_window->buffer_and_command_box_container);

    sqchat_user_list_setup(new_window);
    new_window->scrolled_window_for_user_list =
        gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_kinetic_scrolling(
            GTK_SCROLLED_WINDOW(new_window->scrolled_window_for_user_list),
            TRUE);
    gtk_scrolled_window_set_policy(
            GTK_SCROLLED_WINDOW(new_window->scrolled_window_for_user_list),
            GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(new_window->scrolled_window_for_user_list),
                      new_window->user_list);
    gtk_paned_add2(GTK_PANED(new_window->chat_viewer_and_user_list_pane),
                   new_window->scrolled_window_for_user_list);

    /* Create a new network to act as a place holder if a network wasn't
     * provided
     */
    if (network == NULL) {
        struct sqchat_network * placeholder = sqchat_network_new();
        sqchat_network_tree_network_add(new_window, placeholder);
        sqchat_chat_window_change_active_buffer(new_window, placeholder->buffer);
    }

    // Connect the signals
    sqchat_network_tree_connect_signals(new_window);
    sqchat_main_menu_bar_connect_signals(new_window);

    // TODO: Destroy sqchat_chat_window struct when windows are destroyed
    g_signal_connect(new_window->window, "destroy", G_CALLBACK(gtk_main_quit),
                     NULL);

    // Show the main window
    gtk_widget_show_all(new_window->window);

    return new_window;
}

void sqchat_chat_window_change_active_buffer(struct sqchat_chat_window * window,
                                             struct sqchat_buffer * new_buffer) {
    GtkTreePath * path_to_buffer = gtk_tree_row_reference_get_path(new_buffer->row);

    if (window->current_buffer != NULL) {
        /* Remove the active command box and buffer from the window's container
         * and replace them with the new ones
         */
        gtk_container_remove(
            GTK_CONTAINER(window->buffer_and_command_box_container),
            window->current_buffer->scrolled_container);
        gtk_container_remove(
            GTK_CONTAINER(window->buffer_and_command_box_container),
            window->current_buffer->command_box_entry);
    }
    gtk_box_pack_start(GTK_BOX(window->buffer_and_command_box_container),
                       new_buffer->scrolled_container, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(window->buffer_and_command_box_container),
                       new_buffer->command_box_entry, FALSE, FALSE, 0);
    gtk_widget_show_all(window->buffer_and_command_box_container);

    if (new_buffer->type == CHANNEL) {
        gtk_tree_view_set_model(GTK_TREE_VIEW(window->user_list),
                GTK_TREE_MODEL(new_buffer->chan_data->user_list_store));
        gtk_widget_show(window->scrolled_window_for_user_list);
    }
    else
        gtk_widget_hide(window->scrolled_window_for_user_list);

    window->current_buffer = new_buffer;

    // Make sure that the buffer is selected in the network tree
    gtk_tree_view_expand_to_path(GTK_TREE_VIEW(window->network_tree),
                                 path_to_buffer);
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(window->network_tree),
                             path_to_buffer,
                             NULL, false);
}
// vim: set expandtab tw=80 shiftwidth=4 softtabstop=4 cinoptions=(0,W4:
