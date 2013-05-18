#include "buffer.h"

#include <gtk/gtk.h>
#include <stdlib.h>

struct buffer_info * new_buffer(enum buffer_type type) {
    struct buffer_info * buffer = malloc(sizeof(struct buffer_info));
    buffer->type = type;

    buffer->chat_viewer = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(buffer->chat_viewer), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(buffer->chat_viewer), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(buffer->chat_viewer),
                                GTK_WRAP_WORD_CHAR);
    
    buffer->scrolled_window_for_chat_viewer = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_kinetic_scrolling(
            GTK_SCROLLED_WINDOW(buffer->scrolled_window_for_chat_viewer), TRUE);
    gtk_container_add(GTK_CONTAINER(buffer->scrolled_window_for_chat_viewer),
                      buffer->chat_viewer);

    buffer->command_box = gtk_entry_new();

    buffer->chat_and_command_box_container =
        gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(buffer->chat_and_command_box_container),
                       buffer->scrolled_window_for_chat_viewer, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(buffer->chat_and_command_box_container),
                       buffer->command_box, FALSE, FALSE, 0);

    // Add a userlist if the buffer is a channel buffer
    if (type == CHANNEL) {
        buffer->user_list_store =
            gtk_list_store_new(2, G_TYPE_CHAR, G_TYPE_STRING);
        buffer->user_list =
            gtk_tree_view_new_with_model(GTK_TREE_MODEL(buffer->user_list_store));

        buffer->chat_viewer_and_user_list_pane =
            gtk_paned_new(GTK_ORIENTATION_VERTICAL);
        gtk_paned_add1(GTK_PANED(buffer->chat_viewer_and_user_list_pane),
                       buffer->chat_and_command_box_container);
        gtk_paned_add2(GTK_PANED(buffer->chat_viewer_and_user_list_pane),
                       buffer->user_list);
    }
    return buffer;
}

void destroy_buffer(struct buffer_info * buffer) {
    if (buffer->type == CHANNEL) {
        g_object_unref(buffer->user_list_store);
        gtk_widget_destroy(buffer->chat_viewer_and_user_list_pane);
    }
    else
        gtk_widget_destroy(buffer->chat_and_command_box_container);
    
    free(buffer);
}

void print_to_buffer(struct buffer_info * buffer,
                     char * message, ...) {
    va_list args;
    char * parsed_message;
    gchar * parsed_message_utf8;
    size_t parsed_message_len;
    size_t parsed_message_utf8_len;
    GtkTextIter end_of_buffer;
    GtkTextBuffer * text_buffer =
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(buffer->chat_viewer));

    // Parse the message passed to this function
    va_start(args, message);
    parsed_message_len = vsnprintf(NULL, 0, message, args);

    va_start(args, message);
    parsed_message = malloc(parsed_message_len);
    vsnprintf(parsed_message, parsed_message_len + 1, message, args);
    va_end(args);

    /* FIXME (maybe): String is already supposed to be in utf8, but glib doesn't
     * think so, so we convert it to glib's liking
     */
    parsed_message_utf8 = g_locale_to_utf8(parsed_message,
                                           parsed_message_len,
                                           &parsed_message_utf8_len,
                                           NULL, NULL);
    free(parsed_message);
    
    // Figure out where the end of the buffer is
    gtk_text_buffer_get_end_iter(text_buffer, &end_of_buffer);

    /* TODO: Add in code to maintain the line limit for the buffer whenever
     * anything is printed to the buffer
     */

    // Print the message
    gtk_text_buffer_insert(text_buffer, &end_of_buffer, parsed_message_utf8,
                           parsed_message_utf8_len);
    free(parsed_message_utf8);
}

// vim: expandtab:tw=80:tabstop=4:shiftwidth=4:softtabstop=4