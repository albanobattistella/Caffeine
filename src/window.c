#include "window.h"

#include <adwaita.h>

#include <glib/gi18n.h>
#include <locale.h>

#include <string.h>

#include "application.h"
#include "entry.h"

#include "ratio.h"

#include "config.h"

/**
 * This class handles all window logic.
 */
struct _CaffeineWindow {
    AdwApplicationWindow parent;

    AdwSpinRow *ground_entry;
    AdwSpinRow *extract_entry;
    double ground;
    double extract;

    GtkLabel *ratio_label;

    GtkListBox *log_listbox;
};

G_DEFINE_TYPE(CaffeineWindow, caffeine_window, ADW_TYPE_APPLICATION_WINDOW);

static void caffeine_window_ratio_init(CaffeineWindow * window);
static void caffeine_window_ratio_update(GtkEditable * self,
                                         CaffeineWindow * window);

static void caffeine_window_log_init(CaffeineWindow *window);

/**
 * This function initializes a CaffeineWindow object.
 *
 * @param window CaffeineWindow to be initialized
 */
static void caffeine_window_init(CaffeineWindow *window)
{
    gtk_widget_init_template(GTK_WIDGET(window));

    caffeine_window_ratio_init(window);
    caffeine_window_log_init(window);
}

/**
 * This function initializes the CaffeineWindow class.
 *
 * @param class CaffeineWindowClass to be initialized
 */
static void caffeine_window_class_init(CaffeineWindowClass *class)
{
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
                                                UI_RESOURCE("window.ui"));

    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class),
                                         CaffeineWindow, ground_entry);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class),
                                         CaffeineWindow, extract_entry);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class),
                                         CaffeineWindow, ratio_label);

    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class),
                                         CaffeineWindow, log_listbox);
}

/**
 * This function creates a new CaffeineWindow object.
 *
 * @param app CaffeineApplication to create the new CaffeineWindow for
 *
 * @return CaffeineWindow
 */
CaffeineWindow *caffeine_window_new(CaffeineApplication *app)
{
    return g_object_new(CAFFEINE_TYPE_WINDOW, "application", app, NULL);
}

/**
 * This function opens a CaffeineWindow window.
 *
 * @param window Window to be opened
 * @param file File to be processed with the window
 */

void caffeine_window_open(CaffeineWindow *window, GFile *file)
{
}

/**
 * This function initializes the ratio page of a CaffeineWindow.
 *
 * @param window CaffeineWindow to initialize the ratio page of
 */
static void caffeine_window_ratio_init(CaffeineWindow *window)
{
    g_signal_connect(window->ground_entry, "changed",
                     G_CALLBACK(caffeine_window_ratio_update), window);
    g_signal_connect(window->extract_entry, "changed",
                     G_CALLBACK(caffeine_window_ratio_update), window);

    caffeine_window_ratio_update(GTK_EDITABLE(window->ground_entry), window);
    caffeine_window_ratio_update(GTK_EDITABLE(window->extract_entry), window);
}

/**
 * This function updates ratio data displayed.
 *
 * @param self GtkEditable from which a changed signal was emitted
 * @param window CaffeineWindow from which the signal was emitted
 */
static void caffeine_window_ratio_update(GtkEditable *self,
                                         CaffeineWindow *window)
{
    AdwSpinRow *ground_entry = window->ground_entry;
    AdwSpinRow *extract_entry = window->extract_entry;

    double value = adw_spin_row_get_value(ADW_SPIN_ROW(self));

    if (ADW_SPIN_ROW(self) == ground_entry)
        window->ground = value;
    if (ADW_SPIN_ROW(self) == extract_entry)
        window->extract = value;

    if (!window->ground || !window->extract)
        return;

    double ratio_denominator = ratio(window->ground, window->extract);
    gchar *ratio_text = g_strdup_printf("1:%.2f", ratio_denominator);

    if (ratio_denominator == (int)ratio_denominator)    // No decimal places
    {
        ratio_text = g_strdup_printf("1:%.0f", ratio_denominator);
    } else if ((ratio_denominator * 10) == (int)(ratio_denominator * 10))       // One decimal place
    {
        ratio_text = g_strdup_printf("1:%.1f", ratio_denominator);
    }

    gtk_label_set_label(window->ratio_label, ratio_text);

    g_free(ratio_text);
}

/**
 * This function initializes the log page of a CaffeineWindow.
 *
 * @param window CaffeineWindow to initialize the log page of
 */
static void caffeine_window_log_init(CaffeineWindow *window)
{
    gtk_list_box_append(window->log_listbox, GTK_WIDGET(caffeine_entry_new()));
}
