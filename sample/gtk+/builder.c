/* -*- c-file-style: "gnu" -*- */
/* buildertest.c
 * Copyright (C) 2006-2007 Async Open Source
 * Authors: Johan Dahlin
 *          Henrique Romano
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <cutter.h>

#include <string.h>
#include <libintl.h>
#include <locale.h>
#include <math.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

void test_parser (void);
void test_connect_signals (void);
void test_value_from_string (void);
void test_uimanager_simple (void);
void test_domain (void);
void test_sizegroup (void);
void test_list_store (void);
void test_tree_store (void);
void test_types (void);
void test_notebook (void);
void test_construct_only_property (void);
void test_object_properties (void);
void test_children (void);
void test_child_properties (void);
void test_treeview_column (void);
void test_icon_view (void);
void test_combo_box (void);
void test_cell_view (void);
void test_dialog (void);
void test_accelerators (void);
void test_widget (void);
void test_window (void);
void test_reference_counting (void);

void signal_normal (GtkWindow *window, GParamSpec spec);
void signal_after (GtkWindow *window, GParamSpec spec);
void signal_object (GtkButton *button, GParamSpec spec);
void signal_object_after (GtkButton *button, GParamSpec spec);
void signal_first (GtkButton *button, GParamSpec spec);
void signal_second (GtkButton *button, GParamSpec spec);
void signal_extra (GtkButton *button, GParamSpec spec);
void signal_extra2 (GtkButton *button, GParamSpec spec);

static GtkBuilder *builder = NULL;

void
setup (void)
{
  builder = NULL;
}

void
teardown (void)
{
  if (builder)
    g_object_unref (builder);
}

static GtkBuilder *
builder_new_from_string (const gchar *buffer,
                         gsize length,
                         gchar *domain)
{
  GtkBuilder *new_builder;
  new_builder = gtk_builder_new ();
  if (domain)
    gtk_builder_set_translation_domain (new_builder, domain);
  gtk_builder_add_from_string (new_builder, buffer, length, NULL);
  return new_builder;
}

void
test_parser (void)
{
  GError *error;
  
  builder = gtk_builder_new ();

  error = NULL;
  gtk_builder_add_from_string (builder, "<xxx/>", -1, &error);
  cut_assert (error);
  cut_assert_equal_int (GTK_BUILDER_ERROR, error->domain);
  cut_assert_equal_int (GTK_BUILDER_ERROR_UNHANDLED_TAG, error->code);
  g_error_free (error);
  
  error = NULL;
  gtk_builder_add_from_string (builder, "<interface invalid=\"X\"/>", -1, &error);
  cut_assert (error);
  cut_assert_equal_int (GTK_BUILDER_ERROR, error->domain);
  cut_assert_equal_int (GTK_BUILDER_ERROR_INVALID_ATTRIBUTE, error->code);
  g_error_free (error);

  error = NULL;
  gtk_builder_add_from_string (builder, "<interface><child/></interface>", -1, &error);
  cut_assert (error);
  cut_assert_equal_int (GTK_BUILDER_ERROR, error->domain);
  cut_assert_equal_int (GTK_BUILDER_ERROR_INVALID_TAG, error->code);

  error = NULL;
  gtk_builder_add_from_string (builder, "<interface><object class=\"GtkVBox\" id=\"a\"><object class=\"GtkHBox\" id=\"b\"/></object></interface>", -1, &error);
  cut_assert (error);
  cut_assert_equal_int (GTK_BUILDER_ERROR, error->domain);
  cut_assert_equal_int (GTK_BUILDER_ERROR_INVALID_TAG, error->code);
  g_error_free (error);

  g_object_unref (builder);
  builder = NULL;
}

static int normal;
static int after;
static int object;
static int object_after;

void
signal_normal (GtkWindow *window, GParamSpec spec)
{
  cut_assert (GTK_IS_WINDOW (window));
  cut_assert_equal_int (0, normal);
  cut_assert_equal_int (0, after);

  normal++;
}
                    
void
signal_after (GtkWindow *window, GParamSpec spec)
{
  cut_assert (GTK_IS_WINDOW (window));
  cut_assert_equal_int (1, normal);
  cut_assert_equal_int (0, after);
  
  after++;
}

void
signal_object (GtkButton *button, GParamSpec spec)
{
  cut_assert (GTK_IS_BUTTON (button));
  cut_assert_equal_int (0, object);
  cut_assert_equal_int (0, object_after);

  object++;
}

void
signal_object_after (GtkButton *button, GParamSpec spec)
{
  cut_assert (GTK_IS_BUTTON (button));
  cut_assert_equal_int (1, object);
  cut_assert_equal_int (0, object_after);

  object_after++;
}

void
signal_first (GtkButton *button, GParamSpec spec)
{
  cut_assert_equal_int (0, normal);
  normal = 10;
}

void
signal_second (GtkButton *button, GParamSpec spec)
{
  cut_assert_equal_int (10, normal);
  normal = 20;
}

void
signal_extra (GtkButton *button, GParamSpec spec)
{
  cut_assert_equal_int (20, normal);
  normal = 30;
}

void
signal_extra2 (GtkButton *button, GParamSpec spec)
{
  cut_assert_equal_int (30, normal);
  normal = 40;
}

static void
connect_signals (GtkBuilder *gtk_builder,
                 GObject *object,
		 const gchar *signal_name,
		 const gchar *handler_name,
		 GObject *connect_object,
		 GConnectFlags flags,
		 gpointer user_data)
{
  GCallback func;
  GModule *module;

  module = g_module_open ("test_builder.so", G_MODULE_BIND_LAZY);
  if (!g_module_symbol (module, handler_name, (gpointer)&func))
    {
       g_warning ("Could not find signal handler '%s'", handler_name);
       return;
    }

  if (connect_object)
    g_signal_connect_object (object, signal_name, func, connect_object, flags);
  else
    g_signal_connect_data (object, signal_name, func, user_data, NULL, flags);
  g_module_close (module);
}

void
test_connect_signals (void)
{
  GObject *window;
  const gchar buffer[] =
    "<interface>"
    "  <object class=\"GtkButton\" id=\"button\"/>"
    "  <object class=\"GtkWindow\" id=\"window1\">"
    "    <signal name=\"notify::title\" handler=\"signal_normal\"/>"
    "    <signal name=\"notify::title\" handler=\"signal_after\" after=\"yes\"/>"
    "    <signal name=\"notify::title\" handler=\"signal_object\""
    "            object=\"button\"/>"
    "    <signal name=\"notify::title\" handler=\"signal_object_after\""
    "            object=\"button\" after=\"yes\"/>"
    "  </object>"
    "</interface>";
  const gchar buffer_order[] =
    "<interface>"
    "  <object class=\"GtkWindow\" id=\"window1\">"
    "    <signal name=\"notify::title\" handler=\"signal_first\"/>"
    "    <signal name=\"notify::title\" handler=\"signal_second\"/>"
    "  </object>"
    "</interface>";
  const gchar buffer_extra[] =
    "<interface>"
    "  <object class=\"GtkWindow\" id=\"window2\">"
    "    <signal name=\"notify::title\" handler=\"signal_extra\"/>"
    "  </object>"
    "</interface>";
  const gchar buffer_extra2[] =
    "<interface>"
    "  <object class=\"GtkWindow\" id=\"window3\">"
    "    <signal name=\"notify::title\" handler=\"signal_extra2\"/>"
    "  </object>"
    "</interface>";
  const gchar buffer_after_child[] =
    "<interface>"
    "  <object class=\"GtkWindow\" id=\"window1\">"
    "    <child>"
    "      <object class=\"GtkButton\" id=\"button1\"/>"
    "    </child>"
    "    <signal name=\"notify::title\" handler=\"signal_normal\"/>"
    "  </object>"
    "</interface>";

  if (!GTK_CHECK_VERSION(2,12,2))
    cut_pending("The following tests fail on GTK+-2.12.1");

  builder = builder_new_from_string (buffer, -1, NULL);
  gtk_builder_connect_signals_full (builder, connect_signals, NULL);

  window = gtk_builder_get_object (builder, "window1");
  gtk_window_set_title (GTK_WINDOW (window), "test");

  cut_assert_equal_int (1, normal);
  cut_assert_equal_int (1, after);
  cut_assert_equal_int (1, object);
  cut_assert_equal_int (1, object_after);
  gtk_widget_destroy (GTK_WIDGET (window));
  g_object_unref (builder);
  
  builder = builder_new_from_string (buffer_order, -1, NULL);
  gtk_builder_connect_signals_full (builder, connect_signals, NULL);
  window = gtk_builder_get_object (builder, "window1");
  normal = 0;
  gtk_window_set_title (GTK_WINDOW (window), "test");
  cut_assert_equal_int (20, normal);

  gtk_widget_destroy (GTK_WIDGET (window));

  gtk_builder_add_from_string (builder, buffer_extra,
			       strlen (buffer_extra), NULL);
  gtk_builder_add_from_string (builder, buffer_extra2,
			       strlen (buffer_extra2), NULL);
  gtk_builder_connect_signals_full (builder, connect_signals, NULL);
  window = gtk_builder_get_object (builder, "window2");
  gtk_window_set_title (GTK_WINDOW (window), "test");
  cut_assert_equal_int (30, normal);

  gtk_widget_destroy (GTK_WIDGET (window));
  window = gtk_builder_get_object (builder, "window3");
  gtk_window_set_title (GTK_WINDOW (window), "test");
  cut_assert_equal_int (40, normal);
  gtk_widget_destroy (GTK_WIDGET (window));
  
  g_object_unref (builder);

  /* new test, reset globals */
  after = 0;
  normal = 0;

  builder = builder_new_from_string (buffer_after_child, -1, NULL);
  gtk_builder_connect_signals_full (builder, connect_signals, NULL);
  window = gtk_builder_get_object (builder, "window1");
  gtk_window_set_title (GTK_WINDOW (window), "test");

  cut_assert_equal_int (1, normal);
  gtk_widget_destroy (GTK_WIDGET (window));
  g_object_unref (builder);
  builder = NULL;
}

void
test_uimanager_simple (void)
{
  GObject *window, *uimgr, *menubar;
  GObject *menu, *label;
  GList *children;
  const gchar buffer[] =
    "<interface>"
    "  <object class=\"GtkUIManager\" id=\"uimgr1\"/>"
    "</interface>";
    
  const gchar buffer2[] =
    "<interface>"
    "  <object class=\"GtkUIManager\" id=\"uimgr1\">"
    "    <child>"
    "      <object class=\"GtkActionGroup\" id=\"ag1\">"
    "        <child>"
    "          <object class=\"GtkAction\" id=\"file\">"
    "            <property name=\"label\">_File</property>"
    "          </object>"
    "          <accelerator key=\"n\" modifiers=\"GDK_CONTROL_MASK\"/>"
    "        </child>"
    "      </object>"
    "    </child>"
    "    <ui>"
    "      <menubar name=\"menubar1\">"
    "        <menu action=\"file\">"
    "        </menu>"
    "      </menubar>"
    "    </ui>"
    "  </object>"
    "  <object class=\"GtkWindow\" id=\"window1\">"
    "    <child>"
    "      <object class=\"GtkMenuBar\" id=\"menubar1\" constructor=\"uimgr1\"/>"
    "    </child>"
    "  </object>"
    "</interface>";

  builder = builder_new_from_string (buffer, -1, NULL);

  uimgr = gtk_builder_get_object (builder, "uimgr1");
  cut_assert (uimgr);
  cut_assert (GTK_IS_UI_MANAGER (uimgr));
  g_object_unref (builder);
  builder = NULL;
  
  builder = builder_new_from_string (buffer2, -1, NULL);

  menubar = gtk_builder_get_object (builder, "menubar1");
  cut_assert (menubar != NULL);
  cut_assert (GTK_IS_MENU_BAR (menubar));

  children = gtk_container_get_children (GTK_CONTAINER (menubar));
  menu = children->data;
  cut_assert (menu != NULL);
  cut_assert (GTK_IS_MENU_ITEM (menu));
  cut_assert_equal_string ("file", GTK_WIDGET (menu)->name);
  g_list_free (children);
  
  label = G_OBJECT (GTK_BIN (menu)->child);
  cut_assert (label != NULL);
  cut_assert (GTK_IS_LABEL (label));
  cut_assert_equal_string
    ("File", gtk_label_get_text (GTK_LABEL (label)));

  window = gtk_builder_get_object (builder, "window1");
  gtk_widget_destroy (GTK_WIDGET (window));
  g_object_unref (builder);
  builder = NULL;
}

void
test_domain (void)
{
  const gchar buffer1[] = "<interface/>";
  const gchar buffer2[] = "<interface domain=\"domain\"/>";
  const gchar *domain;
  
  builder = builder_new_from_string (buffer1, -1, NULL);
  domain = gtk_builder_get_translation_domain (builder);
  cut_assert (domain == NULL);
  g_object_unref (builder);
  builder = NULL;
  
  builder = builder_new_from_string (buffer1, -1, "domain-1");
  domain = gtk_builder_get_translation_domain (builder);
  cut_assert (domain);
  cut_assert_equal_string ("domain-1", domain);
  g_object_unref (builder);
  builder = NULL;
  
  builder = builder_new_from_string (buffer2, -1, NULL);
  domain = gtk_builder_get_translation_domain (builder);
  cut_assert_equal_string (NULL, domain);
  g_object_unref (builder);
  builder = NULL;
}

#if 0
void test_translation (void)
{
  GtkBuilder *builder;
  const gchar buffer[] =
    "<interface>"
    "  <object class=\"GtkWindow\" id=\"window1\">"
    "    <child>"
    "      <object class=\"GtkLabel\" id=\"label\">"
    "        <property name=\"label\" translatable=\"yes\">File</property>"
    "      </object>"
    "    </child>"
    "  </object>"
    "</interface>";
  GtkLabel *window, *label;

  setlocale (LC_ALL, "sv_SE");
  textdomain ("builder");
  bindtextdomain ("builder", "tests");

  builder = builder_new_from_string (buffer, -1, NULL);
  label = GTK_LABEL (gtk_builder_get_object (builder, "label"));
  cut_assert (strcmp (gtk_label_get_text (label), "Arkiv") == 0);

  window = gtk_builder_get_object (builder, "window1");
  gtk_widget_destroy (GTK_WIDGET (window));
  g_object_unref (builder);
  
  builder = NULL;
}
#endif

void test_sizegroup (void)
{
  const gchar buffer1[] =
    "<interface domain=\"test\">"
    "  <object class=\"GtkSizeGroup\" id=\"sizegroup1\">"
    "    <property name=\"mode\">GTK_SIZE_GROUP_HORIZONTAL</property>"
    "    <widgets>"
    "      <widget name=\"radio1\"/>"
    "      <widget name=\"radio2\"/>"
    "    </widgets>"
    "  </object>"
    "  <object class=\"GtkWindow\" id=\"window1\">"
    "    <child>"
    "      <object class=\"GtkVBox\" id=\"vbox1\">"
    "        <child>"
    "          <object class=\"GtkRadioButton\" id=\"radio1\"/>"
    "        </child>"
    "        <child>"
    "          <object class=\"GtkRadioButton\" id=\"radio2\"/>"
    "        </child>"
    "      </object>"
    "    </child>"
    "  </object>"
    "</interface>";
  const gchar buffer2[] =
    "<interface domain=\"test\">"
    "  <object class=\"GtkSizeGroup\" id=\"sizegroup1\">"
    "    <property name=\"mode\">GTK_SIZE_GROUP_HORIZONTAL</property>"
    "    <widgets>"
    "    </widgets>"
    "   </object>"
    "</interface>";
  const gchar buffer3[] =
    "<interface domain=\"test\">"
    "  <object class=\"GtkSizeGroup\" id=\"sizegroup1\">"
    "    <property name=\"mode\">GTK_SIZE_GROUP_HORIZONTAL</property>"
    "    <widgets>"
    "      <widget name=\"radio1\"/>"
    "      <widget name=\"radio2\"/>"
    "    </widgets>"
    "  </object>"
    "  <object class=\"GtkSizeGroup\" id=\"sizegroup2\">"
    "    <property name=\"mode\">GTK_SIZE_GROUP_HORIZONTAL</property>"
    "    <widgets>"
    "      <widget name=\"radio1\"/>"
    "      <widget name=\"radio2\"/>"
    "    </widgets>"
    "  </object>"
    "  <object class=\"GtkWindow\" id=\"window1\">"
    "    <child>"
    "      <object class=\"GtkVBox\" id=\"vbox1\">"
    "        <child>"
    "          <object class=\"GtkRadioButton\" id=\"radio1\"/>"
    "        </child>"
    "        <child>"
    "          <object class=\"GtkRadioButton\" id=\"radio2\"/>"
    "        </child>"
    "      </object>"
    "    </child>"
    "  </object>"
    "</interface>";
  GObject *sizegroup;
  GSList *widgets;

  builder = builder_new_from_string (buffer1, -1, NULL);
  sizegroup = gtk_builder_get_object (builder, "sizegroup1");
  widgets = gtk_size_group_get_widgets (GTK_SIZE_GROUP (sizegroup));
  cut_assert_equal_int (2, g_slist_length (widgets));
  g_slist_free (widgets);
  g_object_unref (builder);

  builder = builder_new_from_string (buffer2, -1, NULL);
  sizegroup = gtk_builder_get_object (builder, "sizegroup1");
  widgets = gtk_size_group_get_widgets (GTK_SIZE_GROUP (sizegroup));
  cut_assert_equal_int (0, g_slist_length (widgets));
  g_slist_free (widgets);
  g_object_unref (builder);

  builder = builder_new_from_string (buffer3, -1, NULL);
  sizegroup = gtk_builder_get_object (builder, "sizegroup1");
  widgets = gtk_size_group_get_widgets (GTK_SIZE_GROUP (sizegroup));
  cut_assert_equal_int (2, g_slist_length (widgets));
  g_slist_free (widgets);
  sizegroup = gtk_builder_get_object (builder, "sizegroup2");
  widgets = gtk_size_group_get_widgets (GTK_SIZE_GROUP (sizegroup));
  cut_assert_equal_int (2, g_slist_length (widgets));
  g_slist_free (widgets);

#if 0
  {
    GObject *window;
    window = gtk_builder_get_object (builder, "window1");
    gtk_widget_destroy (GTK_WIDGET (window));
  }
#endif  
  g_object_unref (builder);
  builder = NULL;
}

void test_list_store (void)
{
  const gchar buffer1[] =
    "<interface>"
    "  <object class=\"GtkListStore\" id=\"liststore1\">"
    "    <columns>"
    "      <column type=\"gchararray\"/>"
    "      <column type=\"guint\"/>"
    "    </columns>"
    "  </object>"
    "</interface>";
  const char buffer2[] = 
    "<interface>"
    "  <object class=\"GtkListStore\" id=\"liststore1\">"
    "    <columns>"
    "      <column type=\"gchararray\"/>"
    "      <column type=\"gchararray\"/>"
    "      <column type=\"gint\"/>"
    "    </columns>"
    "    <data>"
    "      <row>"
    "        <col id=\"0\">John</col>"
    "        <col id=\"1\">Doe</col>"
    "        <col id=\"2\">25</col>"
    "      </row>"
    "      <row>"
    "        <col id=\"0\">Johan</col>"
    "        <col id=\"1\">Dole</col>"
    "        <col id=\"2\">50</col>"
    "      </row>"
    "    </data>"
    "  </object>"
    "</interface>";
  GObject *store;
  GtkTreeIter iter;
  gchar *surname, *lastname;
  int age;
  
  builder = builder_new_from_string (buffer1, -1, NULL);
  store = gtk_builder_get_object (builder, "liststore1");
  cut_assert (gtk_tree_model_get_n_columns (GTK_TREE_MODEL (store)) == 2);
  cut_assert (gtk_tree_model_get_column_type (GTK_TREE_MODEL (store), 0) == G_TYPE_STRING);
  cut_assert (gtk_tree_model_get_column_type (GTK_TREE_MODEL (store), 1) == G_TYPE_UINT);
  g_object_unref (builder);
  
  builder = builder_new_from_string (buffer2, -1, NULL);
  store = gtk_builder_get_object (builder, "liststore1");
  cut_assert (gtk_tree_model_get_n_columns (GTK_TREE_MODEL (store)) == 3);
  cut_assert (gtk_tree_model_get_column_type (GTK_TREE_MODEL (store), 0) == G_TYPE_STRING);
  cut_assert (gtk_tree_model_get_column_type (GTK_TREE_MODEL (store), 1) == G_TYPE_STRING);
  cut_assert (gtk_tree_model_get_column_type (GTK_TREE_MODEL (store), 2) == G_TYPE_INT);
  
  cut_assert (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (store), &iter) == TRUE);
  gtk_tree_model_get (GTK_TREE_MODEL (store), &iter,
                      0, &surname,
                      1, &lastname,
                      2, &age,
                      -1);
  cut_assert (surname != NULL);
  cut_assert_equal_string ("John", surname);
  g_free (surname);
  cut_assert (lastname != NULL);
  cut_assert_equal_string ("Doe", lastname);
  g_free (lastname);
  cut_assert_equal_int (25, age);
  cut_assert (gtk_tree_model_iter_next (GTK_TREE_MODEL (store), &iter) == TRUE);
  
  gtk_tree_model_get (GTK_TREE_MODEL (store), &iter,
                      0, &surname,
                      1, &lastname,
                      2, &age,
                      -1);
  cut_assert (surname != NULL);
  cut_assert_equal_string ("Johan", surname);
  g_free (surname);
  cut_assert (lastname != NULL);
  cut_assert_equal_string ("Dole", lastname);
  g_free (lastname);
  cut_assert_equal_int (50, age);
  cut_assert (gtk_tree_model_iter_next (GTK_TREE_MODEL (store), &iter) == FALSE);
  
  g_object_unref (builder);

  builder = NULL;
}

void test_tree_store (void)
{
  const gchar buffer[] =
    "<interface domain=\"test\">"
    "  <object class=\"GtkTreeStore\" id=\"treestore1\">"
    "    <columns>"
    "      <column type=\"gchararray\"/>"
    "      <column type=\"guint\"/>"
    "    </columns>"
    "  </object>"
    "</interface>";
  GObject *store;
  
  builder = builder_new_from_string (buffer, -1, NULL);
  store = gtk_builder_get_object (builder, "treestore1");
  cut_assert (gtk_tree_model_get_n_columns (GTK_TREE_MODEL (store)) == 2);
  cut_assert (gtk_tree_model_get_column_type (GTK_TREE_MODEL (store), 0) == G_TYPE_STRING);
  cut_assert (gtk_tree_model_get_column_type (GTK_TREE_MODEL (store), 1) == G_TYPE_UINT);
  
  g_object_unref (builder);

  builder = NULL;
}

void test_types (void)
{
  const gchar buffer[] = 
    "<interface>"
    "  <object class=\"GtkAction\" id=\"action\"/>"
    "  <object class=\"GtkActionGroup\" id=\"actiongroup\"/>"
    "  <object class=\"GtkAlignment\" id=\"alignment\"/>"
    "  <object class=\"GtkArrow\" id=\"arrow\"/>"
    "  <object class=\"GtkButton\" id=\"button\"/>"
    "  <object class=\"GtkCheckButton\" id=\"checkbutton\"/>"
    "  <object class=\"GtkDialog\" id=\"dialog\"/>"
    "  <object class=\"GtkDrawingArea\" id=\"drawingarea\"/>"
    "  <object class=\"GtkEventBox\" id=\"eventbox\"/>"
    "  <object class=\"GtkEntry\" id=\"entry\"/>"
    "  <object class=\"GtkFontButton\" id=\"fontbutton\"/>"
    "  <object class=\"GtkHButtonBox\" id=\"hbuttonbox\"/>"
    "  <object class=\"GtkHBox\" id=\"hbox\"/>"
    "  <object class=\"GtkHPaned\" id=\"hpaned\"/>"
    "  <object class=\"GtkHRuler\" id=\"hruler\"/>"
    "  <object class=\"GtkHScale\" id=\"hscale\"/>"
    "  <object class=\"GtkHScrollbar\" id=\"hscrollbar\"/>"
    "  <object class=\"GtkHSeparator\" id=\"hseparator\"/>"
    "  <object class=\"GtkImage\" id=\"image\"/>"
    "  <object class=\"GtkLabel\" id=\"label\"/>"
    "  <object class=\"GtkListStore\" id=\"liststore\"/>"
    "  <object class=\"GtkMenuBar\" id=\"menubar\"/>"
    "  <object class=\"GtkNotebook\" id=\"notebook\"/>"
    "  <object class=\"GtkProgressBar\" id=\"progressbar\"/>"
    "  <object class=\"GtkRadioButton\" id=\"radiobutton\"/>"
    "  <object class=\"GtkSizeGroup\" id=\"sizegroup\"/>"
    "  <object class=\"GtkScrolledWindow\" id=\"scrolledwindow\"/>"
    "  <object class=\"GtkSpinButton\" id=\"spinbutton\"/>"
    "  <object class=\"GtkStatusbar\" id=\"statusbar\"/>"
    "  <object class=\"GtkTextView\" id=\"textview\"/>"
    "  <object class=\"GtkToggleAction\" id=\"toggleaction\"/>"
    "  <object class=\"GtkToggleButton\" id=\"togglebutton\"/>"
    "  <object class=\"GtkToolbar\" id=\"toolbar\"/>"
    "  <object class=\"GtkTreeStore\" id=\"treestore\"/>"
    "  <object class=\"GtkTreeView\" id=\"treeview\"/>"
    "  <object class=\"GtkTable\" id=\"table\"/>"
    "  <object class=\"GtkVBox\" id=\"vbox\"/>"
    "  <object class=\"GtkVButtonBox\" id=\"vbuttonbox\"/>"
    "  <object class=\"GtkVScrollbar\" id=\"vscrollbar\"/>"
    "  <object class=\"GtkVSeparator\" id=\"vseparator\"/>"
    "  <object class=\"GtkViewport\" id=\"viewport\"/>"
    "  <object class=\"GtkVRuler\" id=\"vruler\"/>"
    "  <object class=\"GtkVPaned\" id=\"vpaned\"/>"
    "  <object class=\"GtkVScale\" id=\"vscale\"/>"
    "  <object class=\"GtkWindow\" id=\"window\"/>"
    "  <object class=\"GtkUIManager\" id=\"uimanager\"/>"
    "</interface>";
  const gchar buffer2[] = 
    "<interface>"
    "  <object type-func=\"gtk_window_get_type\" id=\"window\"/>"
    "</interface>";
  const gchar buffer3[] = 
    "<interface>"
    "  <object type-func=\"xxx_invalid_get_type_function\" id=\"window\"/>"
    "</interface>";
  GObject *window;
  GError *error;

  builder = builder_new_from_string (buffer, -1, NULL);
  gtk_widget_destroy (GTK_WIDGET (gtk_builder_get_object (builder, "dialog")));
  gtk_widget_destroy (GTK_WIDGET (gtk_builder_get_object (builder, "window")));
  g_object_unref (builder);

  builder = builder_new_from_string (buffer2, -1, NULL);
  window = gtk_builder_get_object (builder, "window");
  cut_assert (window != NULL);
  cut_assert (GTK_IS_WINDOW (window));
  gtk_widget_destroy (GTK_WIDGET (window));
  g_object_unref (builder);
  
  error = NULL;
  builder = gtk_builder_new ();
  gtk_builder_add_from_string (builder, buffer3, -1, &error);
  cut_assert (error != NULL);
  cut_assert (error->domain == GTK_BUILDER_ERROR);
  cut_assert (error->code == GTK_BUILDER_ERROR_INVALID_TYPE_FUNCTION);
  g_error_free (error);
  g_object_unref (builder);

  builder = NULL;
}

void test_notebook (void)
{
  const gchar buffer[] =
    "<interface>"
    "  <object class=\"GtkNotebook\" id=\"notebook1\">"
    "    <child>"
    "      <object class=\"GtkLabel\" id=\"label1\">"
    "        <property name=\"label\">label1</property>"
    "      </object>"
    "    </child>"
    "    <child type=\"tab\">"
    "      <object class=\"GtkLabel\" id=\"tablabel1\">"
    "        <property name=\"label\">tab_label1</property>"
    "      </object>"
    "    </child>"
    "    <child>"
    "      <object class=\"GtkLabel\" id=\"label2\">"
    "        <property name=\"label\">label2</property>"
    "      </object>"
    "    </child>"
    "    <child type=\"tab\">"
    "      <object class=\"GtkLabel\" id=\"tablabel2\">"
    "        <property name=\"label\">tab_label2</property>"
    "      </object>"
    "    </child>"
    "  </object>"
    "</interface>";
  GObject *notebook;
  GtkWidget *label;

  builder = builder_new_from_string (buffer, -1, NULL);
  notebook = gtk_builder_get_object (builder, "notebook1");
  cut_assert (notebook != NULL);
  cut_assert (gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook)) == 2);

  label = gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 0);
  cut_assert (GTK_IS_LABEL (label));
  cut_assert (strcmp (gtk_label_get_label (GTK_LABEL (label)),
                                "label1") == 0);
  label = gtk_notebook_get_tab_label (GTK_NOTEBOOK (notebook), label);
  cut_assert (GTK_IS_LABEL (label));
  cut_assert (strcmp (gtk_label_get_label (GTK_LABEL (label)),
				"tab_label1") == 0);

  label = gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 1);
  cut_assert (GTK_IS_LABEL (label));
  cut_assert (strcmp (gtk_label_get_label (GTK_LABEL (label)),
                                "label2") == 0);
  label = gtk_notebook_get_tab_label (GTK_NOTEBOOK (notebook), label);
  cut_assert (GTK_IS_LABEL (label));
  cut_assert (strcmp (gtk_label_get_label (GTK_LABEL (label)),
				"tab_label2") == 0);

  g_object_unref (builder);
  builder = NULL;
}

void test_construct_only_property (void)
{
  const gchar buffer[] =
    "<interface>"
    "  <object class=\"GtkWindow\" id=\"window1\">"
    "    <property name=\"type\">GTK_WINDOW_POPUP</property>"
    "  </object>"
    "</interface>";
  const gchar buffer2[] =
    "<interface>"
    "  <object class=\"GtkTextTagTable\" id=\"tagtable1\"/>"
    "  <object class=\"GtkTextBuffer\" id=\"textbuffer1\">"
    "    <property name=\"tag-table\">tagtable1</property>"
    "  </object>"
    "</interface>";
  GObject *widget, *tagtable, *textbuffer;
  GtkWindowType type;
  
  builder = builder_new_from_string (buffer, -1, NULL);
  widget = gtk_builder_get_object (builder, "window1");
  g_object_get (widget, "type", &type, NULL);
  cut_assert (type == GTK_WINDOW_POPUP);

  gtk_widget_destroy (GTK_WIDGET (widget));
  g_object_unref (builder);

  builder = builder_new_from_string (buffer2, -1, NULL);
  textbuffer = gtk_builder_get_object (builder, "textbuffer1");
  cut_assert (textbuffer != NULL);
  g_object_get (textbuffer, "tag-table", &tagtable, NULL);
  cut_assert (tagtable == gtk_builder_get_object (builder, "tagtable1"));
  g_object_unref (tagtable);
  g_object_unref (builder);

  builder = NULL;
}

void test_object_properties (void)
{
  const gchar buffer[] =
    "<interface>"
    "  <object class=\"GtkWindow\" id=\"window1\">"
    "    <child>"
    "      <object class=\"GtkVBox\" id=\"vbox\">"
    "        <property name=\"border-width\">10</property>"
    "        <child>"
    "          <object class=\"GtkLabel\" id=\"label1\">"
    "            <property name=\"mnemonic-widget\">spinbutton1</property>"
    "          </object>"
    "        </child>"
    "        <child>"
    "          <object class=\"GtkSpinButton\" id=\"spinbutton1\"/>"
    "        </child>"
    "      </object>"
    "    </child>"
    "  </object>"
    "</interface>";
  GObject *label, *spinbutton;
  
  builder = builder_new_from_string (buffer, -1, NULL);
  label = gtk_builder_get_object (builder, "label1");
  cut_assert (label != NULL);
  spinbutton = gtk_builder_get_object (builder, "spinbutton1");
  cut_assert (spinbutton != NULL);
  cut_assert (spinbutton == (GObject*)gtk_label_get_mnemonic_widget (GTK_LABEL (label)));
  
  g_object_unref (builder);

  builder = NULL;
}

void test_children (void)
{
  GList *children;
  const gchar buffer1[] =
    "<interface>"
    "  <object class=\"GtkWindow\" id=\"window1\">"
    "    <child>"
    "      <object class=\"GtkButton\" id=\"button1\">"
    "        <property name=\"label\">Hello</property>"
    "      </object>"
    "    </child>"
    "  </object>"
    "</interface>";
  const gchar buffer2[] =
    "<interface>"
    "  <object class=\"GtkDialog\" id=\"dialog1\">"
    "    <child internal-child=\"vbox\">"
    "      <object class=\"GtkVBox\" id=\"dialog1-vbox\">"
    "        <property name=\"border-width\">10</property>"
    "          <child internal-child=\"action_area\">"
    "            <object class=\"GtkHButtonBox\" id=\"dialog1-action_area\">"
    "              <property name=\"border-width\">20</property>"
    "            </object>"
    "          </child>"
    "      </object>"
    "    </child>"
    "  </object>"
    "</interface>";

  GObject *window, *button;
  GObject *dialog, *vbox, *action_area;
  
  builder = builder_new_from_string (buffer1, -1, NULL);
  window = gtk_builder_get_object (builder, "window1");
  cut_assert (window != NULL);
  cut_assert (GTK_IS_WINDOW (window));

  button = gtk_builder_get_object (builder, "button1");
  cut_assert (button != NULL);
  cut_assert (GTK_IS_BUTTON (button));
  cut_assert (strcmp (gtk_buildable_get_name (GTK_BUILDABLE (GTK_WIDGET (button)->parent)), "window1") == 0);

  gtk_widget_destroy (GTK_WIDGET (window));
  g_object_unref (builder);
  
  builder = builder_new_from_string (buffer2, -1, NULL);
  dialog = gtk_builder_get_object (builder, "dialog1");
  cut_assert (dialog != NULL);
  cut_assert (GTK_IS_DIALOG (dialog));
  children = gtk_container_get_children (GTK_CONTAINER (dialog));
  cut_assert_equal_int (1, g_list_length (children));
  g_list_free (children);
  
  vbox = gtk_builder_get_object (builder, "dialog1-vbox");
  cut_assert (vbox != NULL);
  cut_assert (GTK_IS_VBOX (vbox));
  cut_assert (GTK_WIDGET (vbox)->parent != NULL);
  cut_assert (strcmp (gtk_buildable_get_name (GTK_BUILDABLE (GTK_WIDGET (vbox)->parent)), "dialog1") == 0);
  cut_assert (GTK_CONTAINER (vbox)->border_width == 10);
  cut_assert (strcmp (gtk_buildable_get_name (GTK_BUILDABLE (GTK_DIALOG (dialog)->vbox)),
				"dialog1-vbox") == 0);

  action_area = gtk_builder_get_object (builder, "dialog1-action_area");
  cut_assert (action_area != NULL);
  cut_assert (GTK_IS_HBUTTON_BOX (action_area));
  cut_assert (GTK_WIDGET (action_area)->parent != NULL);
  cut_assert (GTK_CONTAINER (action_area)->border_width == 20);
  cut_assert (GTK_DIALOG (dialog)->action_area != NULL);
  cut_assert (gtk_buildable_get_name (GTK_BUILDABLE (GTK_DIALOG (dialog)->action_area)) != NULL);
  cut_assert (strcmp (gtk_buildable_get_name (GTK_BUILDABLE (GTK_DIALOG (dialog)->action_area)),
				"dialog1-action_area") == 0);
  gtk_widget_destroy (GTK_WIDGET (dialog));
  g_object_unref (builder);
  
  builder = NULL;
}

void test_child_properties (void)
{
  const gchar buffer1[] =
    "<interface>"
    "  <object class=\"GtkVBox\" id=\"vbox1\">"
    "    <child>"
    "      <object class=\"GtkLabel\" id=\"label1\"/>"
    "      <packing>"
    "        <property name=\"pack-type\">start</property>"
    "      </packing>"
    "    </child>"
    "    <child>"
    "      <object class=\"GtkLabel\" id=\"label2\"/>"
    "      <packing>"
    "        <property name=\"pack-type\">end</property>"
    "      </packing>"
    "    </child>"
    "  </object>"
    "</interface>";

  GObject *label, *vbox;
  GtkPackType pack_type;
  
  builder = builder_new_from_string (buffer1, -1, NULL);
  vbox = gtk_builder_get_object (builder, "vbox1");
  cut_assert (GTK_IS_VBOX (vbox));

  label = gtk_builder_get_object (builder, "label1");
  cut_assert (GTK_IS_LABEL (label));
  gtk_container_child_get (GTK_CONTAINER (vbox),
                           GTK_WIDGET (label),
                           "pack-type",
                           &pack_type,
                           NULL);
  cut_assert (pack_type == GTK_PACK_START);
  
  label = gtk_builder_get_object (builder, "label2");
  cut_assert (GTK_IS_LABEL (label));
  gtk_container_child_get (GTK_CONTAINER (vbox),
                           GTK_WIDGET (label),
                           "pack-type",
                           &pack_type,
                           NULL);
  cut_assert (pack_type == GTK_PACK_END);

  g_object_unref (builder);
  
  builder = NULL;
}

void test_treeview_column (void)
{
  const gchar buffer[] =
    "<interface>"
    "<object class=\"GtkListStore\" id=\"liststore1\">"
    "  <columns>"
    "    <column type=\"gchararray\"/>"
    "    <column type=\"guint\"/>"
    "  </columns>"
    "  <data>"
    "    <row>"
    "      <col id=\"0\">John</col>"
    "      <col id=\"1\">25</col>"
    "    </row>"
    "  </data>"
    "</object>"
    "<object class=\"GtkWindow\" id=\"window1\">"
    "  <child>"
    "    <object class=\"GtkTreeView\" id=\"treeview1\">"
    "      <property name=\"visible\">True</property>"
    "      <property name=\"model\">liststore1</property>"
    "      <child>"
    "        <object class=\"GtkTreeViewColumn\" id=\"column1\">"
    "          <property name=\"title\">Test</property>"
    "          <child>"
    "            <object class=\"GtkCellRendererText\" id=\"renderer1\"/>"
    "            <attributes>"
    "              <attribute name=\"text\">1</attribute>"
    "            </attributes>"
    "          </child>"
    "        </object>"
    "      </child>"
    "      <child>"
    "        <object class=\"GtkTreeViewColumn\" id=\"column2\">"
    "          <property name=\"title\">Number</property>"
    "          <child>"
    "            <object class=\"GtkCellRendererText\" id=\"renderer2\"/>"
    "            <attributes>"
    "              <attribute name=\"text\">0</attribute>"
    "            </attributes>"
    "          </child>"
    "        </object>"
    "      </child>"
    "    </object>"
    "  </child>"
    "</object>"
    "</interface>";
  GObject *window, *treeview;
  GtkTreeViewColumn *column;
  GList *renderers;
  GObject *renderer;
  gchar *text;

  builder = builder_new_from_string (buffer, -1, NULL);
  treeview = gtk_builder_get_object (builder, "treeview1");
  cut_assert (treeview);
  cut_assert (GTK_IS_TREE_VIEW (treeview));
  column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), 0);
  cut_assert (GTK_IS_TREE_VIEW_COLUMN (column));
  cut_assert_equal_string ("Test", gtk_tree_view_column_get_title (column));

  renderers = gtk_tree_view_column_get_cell_renderers (column);
  cut_assert_equal_int (1, g_list_length (renderers));
  renderer = g_list_nth_data (renderers, 0);
  cut_assert (renderer);
  cut_assert (GTK_IS_CELL_RENDERER_TEXT (renderer));
  g_list_free (renderers);

  gtk_widget_realize (GTK_WIDGET (treeview));

  renderer = gtk_builder_get_object (builder, "renderer1");
  g_object_get (renderer, "text", &text, NULL);
  cut_assert (text);
  cut_assert_equal_string ("25", text);
  g_free (text);
  
  renderer = gtk_builder_get_object (builder, "renderer2");
  g_object_get (renderer, "text", &text, NULL);
  cut_assert (text);
  cut_assert_equal_string ("John", text);
  g_free (text);

  gtk_widget_unrealize (GTK_WIDGET (treeview));

  window = gtk_builder_get_object (builder, "window1");
  gtk_widget_destroy (GTK_WIDGET (window));
  
  g_object_unref (builder);
  builder = NULL;
}

void test_icon_view (void)
{
  const gchar buffer[] =
    "<interface>"
    "  <object class=\"GtkListStore\" id=\"liststore1\">"
    "    <columns>"
    "      <column type=\"gchararray\"/>"
    "      <column type=\"GdkPixbuf\"/>"
    "    </columns>"
    "    <data>"
    "      <row>"
    "        <col id=\"0\">test</col>"
    "      </row>"
    "    </data>"
    "  </object>"
    "  <object class=\"GtkWindow\" id=\"window1\">"
    "    <child>"
    "      <object class=\"GtkIconView\" id=\"iconview1\">"
    "        <property name=\"model\">liststore1</property>"
    "        <property name=\"text-column\">0</property>"
    "        <property name=\"pixbuf-column\">1</property>"
    "        <property name=\"visible\">True</property>"
    "        <child>"
    "          <object class=\"GtkCellRendererText\" id=\"renderer1\"/>"
    "          <attributes>"
    "            <attribute name=\"text\">0</attribute>"
    "          </attributes>"
    "        </child>"
    "      </object>"
    "    </child>"
    "  </object>"
    "</interface>";
  GObject *window, *iconview, *renderer;
  gchar *text;
  
  builder = builder_new_from_string (buffer, -1, NULL);
  iconview = gtk_builder_get_object (builder, "iconview1");
  cut_assert (iconview);
  cut_assert (GTK_IS_ICON_VIEW (iconview));

  gtk_widget_realize (GTK_WIDGET (iconview));

  renderer = gtk_builder_get_object (builder, "renderer1");
  g_object_get (renderer, "text", &text, NULL);
  cut_assert (text);
  cut_assert_equal_string ("test", text);
  g_free (text);
    
  window = gtk_builder_get_object (builder, "window1");
  gtk_widget_destroy (GTK_WIDGET (window));
  g_object_unref (builder);
  builder = NULL;
}

void test_combo_box (void)
{
  const gchar buffer[] =
    "<interface>"
    "  <object class=\"GtkListStore\" id=\"liststore1\">"
    "    <columns>"
    "      <column type=\"guint\"/>"
    "      <column type=\"gchararray\"/>"
    "    </columns>"
    "    <data>"
    "      <row>"
    "        <col id=\"0\">1</col>"
    "        <col id=\"1\">Foo</col>"
    "      </row>"
    "      <row>"
    "        <col id=\"0\">2</col>"
    "        <col id=\"1\">Bar</col>"
    "      </row>"
    "    </data>"
    "  </object>"
    "  <object class=\"GtkWindow\" id=\"window1\">"
    "    <child>"
    "      <object class=\"GtkComboBox\" id=\"combobox1\">"
    "        <property name=\"model\">liststore1</property>"
    "        <property name=\"visible\">True</property>"
    "        <child>"
    "          <object class=\"GtkCellRendererText\" id=\"renderer1\"/>"
    "          <attributes>"
    "            <attribute name=\"text\">0</attribute>"
    "          </attributes>"
    "        </child>"
    "        <child>"
    "          <object class=\"GtkCellRendererText\" id=\"renderer2\"/>"
    "          <attributes>"
    "            <attribute name=\"text\">1</attribute>"
    "          </attributes>"
    "        </child>"
    "      </object>"
    "    </child>"
    "  </object>"
    "</interface>";
  GObject *window, *combobox, *renderer;
  gchar *text;

  builder = builder_new_from_string (buffer, -1, NULL);
  combobox = gtk_builder_get_object (builder, "combobox1");
  cut_assert (combobox);
  gtk_widget_realize (GTK_WIDGET (combobox));

  renderer = gtk_builder_get_object (builder, "renderer2");
  cut_assert (renderer);
  g_object_get (renderer, "text", &text, NULL);
  cut_assert (text);
  cut_assert_equal_string ("Bar", text);
  g_free (text);

  renderer = gtk_builder_get_object (builder, "renderer1");
  cut_assert (renderer);
  g_object_get (renderer, "text", &text, NULL);
  cut_assert (text);
  cut_assert_equal_string ("2", text);
  g_free (text);

  window = gtk_builder_get_object (builder, "window1");
  gtk_widget_destroy (GTK_WIDGET (window));

  g_object_unref (builder);
  builder = NULL;
}

void
test_cell_view (void)
{
  gchar *buffer =
    "<interface>"
    "  <object class=\"GtkListStore\" id=\"liststore1\">"
    "    <columns>"
    "      <column type=\"gchararray\"/>"
    "    </columns>"
    "    <data>"
    "      <row>"
    "        <col id=\"0\">test</col>"
    "      </row>"
    "    </data>"
    "  </object>"
    "  <object class=\"GtkWindow\" id=\"window1\">"
    "    <child>"
    "      <object class=\"GtkCellView\" id=\"cellview1\">"
    "        <property name=\"visible\">True</property>"
    "        <property name=\"model\">liststore1</property>"
    "        <child>"
    "          <object class=\"GtkCellRendererText\" id=\"renderer1\"/>"
    "          <attributes>"
    "            <attribute name=\"text\">0</attribute>"
    "          </attributes>"
    "        </child>"
    "      </object>"
    "    </child>"
    "  </object>"
    "</interface>";
  GObject *cellview;
  GObject *model, *window;
  GtkTreePath *path;
  GList *renderers;
  GObject *renderer;
  gchar *text;
  
  builder = builder_new_from_string (buffer, -1, NULL);
  cellview = gtk_builder_get_object (builder, "cellview1");
  cut_assert (builder);
  cut_assert (cellview);
  cut_assert (GTK_IS_CELL_VIEW (cellview));
  g_object_get (cellview, "model", &model, NULL);
  cut_assert (model);
  cut_assert (GTK_IS_TREE_MODEL (model));
  g_object_unref (model);
  path = gtk_tree_path_new_first ();
  gtk_cell_view_set_displayed_row (GTK_CELL_VIEW (cellview), path);
  
  renderers = gtk_cell_view_get_cell_renderers (GTK_CELL_VIEW (cellview));
  cut_assert (renderers);
  cut_assert_equal_int (1, g_list_length (renderers));
  
  gtk_widget_realize (GTK_WIDGET (cellview));

  renderer = g_list_nth_data (renderers, 0);
  g_list_free (renderers);
  cut_assert (renderer);
  g_object_get (renderer, "text", &text, NULL);
  cut_assert_equal_string ("test", text);
  g_free (text);
  gtk_tree_path_free (path);

  window = gtk_builder_get_object (builder, "window1");
  gtk_widget_destroy (GTK_WIDGET (window));
  
  g_object_unref (builder);
  builder = NULL;
}

void
test_dialog (void)
{
  const gchar buffer1[] =
    "<interface>"
    "  <object class=\"GtkDialog\" id=\"dialog1\">"
    "    <child internal-child=\"vbox\">"
    "      <object class=\"GtkVBox\" id=\"dialog1-vbox\">"
    "          <child internal-child=\"action_area\">"
    "            <object class=\"GtkHButtonBox\" id=\"dialog1-action_area\">"
    "              <child>"
    "                <object class=\"GtkButton\" id=\"button_cancel\"/>"
    "              </child>"
    "              <child>"
    "                <object class=\"GtkButton\" id=\"button_ok\"/>"
    "              </child>"
    "            </object>"
    "          </child>"
    "      </object>"
    "    </child>"
    "    <action-widgets>"
    "      <action-widget response=\"3\">button_ok</action-widget>"
    "      <action-widget response=\"-5\">button_cancel</action-widget>"
    "    </action-widgets>"
    "  </object>"
    "</interface>";

  GObject *dialog1;
  GObject *button_ok;
  GObject *button_cancel;
  
  builder = builder_new_from_string (buffer1, -1, NULL);
  dialog1 = gtk_builder_get_object (builder, "dialog1");
  button_ok = gtk_builder_get_object (builder, "button_ok");
  cut_assert_equal_int (3,
                        gtk_dialog_get_response_for_widget
                        (GTK_DIALOG (dialog1),
                         GTK_WIDGET (button_ok)));
  button_cancel = gtk_builder_get_object (builder, "button_cancel");
  cut_assert_equal_int (-5,
                        gtk_dialog_get_response_for_widget
                        (GTK_DIALOG (dialog1),
                         GTK_WIDGET (button_cancel)));
  
  gtk_widget_destroy (GTK_WIDGET (dialog1));
  g_object_unref (builder);
  builder = NULL;
}

void
test_accelerators (void)
{
  gchar *buffer =
    "<interface>"
    "  <object class=\"GtkWindow\" id=\"window1\">"
    "    <child>"
    "      <object class=\"GtkButton\" id=\"button1\">"
    "        <accelerator key=\"q\" modifiers=\"GDK_CONTROL_MASK\" signal=\"clicked\"/>"
    "      </object>"
    "    </child>"
    "  </object>"
    "</interface>";
  gchar *buffer2 =
    "<interface>"
    "  <object class=\"GtkWindow\" id=\"window1\">"
    "    <child>"
    "      <object class=\"GtkTreeView\" id=\"treeview1\">"
    "        <signal name=\"cursor-changed\" handler=\"gtk_main_quit\"/>"
    "        <accelerator key=\"f\" modifiers=\"GDK_CONTROL_MASK\" signal=\"grab_focus\"/>"
    "      </object>"
    "    </child>"
    "  </object>"
    "</interface>";
  GObject *window1;
  GSList *accel_groups;
  GObject *accel_group;
  
  builder = builder_new_from_string (buffer, -1, NULL);
  window1 = gtk_builder_get_object (builder, "window1");
  cut_assert (window1);
  cut_assert (GTK_IS_WINDOW (window1));

  accel_groups = gtk_accel_groups_from_object (window1);
  cut_assert_equal_int  (1, g_slist_length (accel_groups));
  accel_group = g_slist_nth_data (accel_groups, 0);
  cut_assert (accel_group);

  gtk_widget_destroy (GTK_WIDGET (window1));
  g_object_unref (builder);

  builder = builder_new_from_string (buffer2, -1, NULL);
  window1 = gtk_builder_get_object (builder, "window1");
  cut_assert (window1);
  cut_assert (GTK_IS_WINDOW (window1));

  accel_groups = gtk_accel_groups_from_object (window1);
  cut_assert_equal_int  (1, g_slist_length (accel_groups));
  accel_group = g_slist_nth_data (accel_groups, 0);
  cut_assert (accel_group);

  gtk_widget_destroy (GTK_WIDGET (window1));
  g_object_unref (builder);
  builder = NULL;
}

void
test_widget (void)
{
  gchar *buffer =
    "<interface>"
    "  <object class=\"GtkWindow\" id=\"window1\">"
    "    <child>"
    "      <object class=\"GtkButton\" id=\"button1\">"
    "         <property name=\"can-focus\">True</property>"
    "         <property name=\"has-focus\">True</property>"
    "      </object>"
    "    </child>"
    "  </object>"
   "</interface>";
  gchar *buffer2 =
    "<interface>"
    "  <object class=\"GtkWindow\" id=\"window1\">"
    "    <child>"
    "      <object class=\"GtkButton\" id=\"button1\">"
    "         <property name=\"can-default\">True</property>"
    "         <property name=\"has-default\">True</property>"
    "      </object>"
    "    </child>"
    "  </object>"
   "</interface>";
  GObject *window1, *button1;
  
  builder = builder_new_from_string (buffer, -1, NULL);
  button1 = gtk_builder_get_object (builder, "button1");

#if 0
  cut_assert (GTK_WIDGET_HAS_FOCUS (GTK_WIDGET (button1)));
#endif
  window1 = gtk_builder_get_object (builder, "window1");
  gtk_widget_destroy (GTK_WIDGET (window1));
  
  g_object_unref (builder);
  
  builder = builder_new_from_string (buffer2, -1, NULL);
  button1 = gtk_builder_get_object (builder, "button1");

  cut_assert (GTK_WIDGET_RECEIVES_DEFAULT (GTK_WIDGET (button1)));
  
  window1 = gtk_builder_get_object (builder, "window1");
  gtk_widget_destroy (GTK_WIDGET (window1));
  g_object_unref (builder);

  builder = NULL;
}

void
test_window (void)
{
  gchar *buffer1 =
    "<interface>"
    "  <object class=\"GtkWindow\" id=\"window1\">"
    "     <property name=\"title\"></property>"
    "  </object>"
   "</interface>";
  gchar *buffer2 =
    "<interface>"
    "  <object class=\"GtkWindow\" id=\"window1\">"
    "  </object>"
   "</interface>";
  GObject *window1;
  gchar *title;
  
  builder = builder_new_from_string (buffer1, -1, NULL);
  window1 = gtk_builder_get_object (builder, "window1");
  g_object_get (window1, "title", &title, NULL);
  cut_assert_equal_string ("", title);
  g_free (title);
  gtk_widget_destroy (GTK_WIDGET (window1));
  g_object_unref (builder);

  builder = builder_new_from_string (buffer2, -1, NULL);
  window1 = gtk_builder_get_object (builder, "window1");
  g_object_get (window1, "title", &title, NULL);
  cut_assert_null (title);
  gtk_widget_destroy (GTK_WIDGET (window1));
  g_object_unref (builder);
  builder = NULL;
}

void
test_value_from_string (void)
{
  GValue value = { 0 };
  GError *error = NULL;

  cut_assert (gtk_builder_value_from_string_type (builder, G_TYPE_STRING, "test", &value, &error));
  cut_assert (G_VALUE_HOLDS_STRING (&value));
  cut_assert_equal_string ("test", g_value_get_string (&value));
  g_value_unset (&value);

  cut_assert (gtk_builder_value_from_string_type (builder, G_TYPE_BOOLEAN, "true", &value, &error));
  cut_assert (G_VALUE_HOLDS_BOOLEAN (&value));
  cut_assert (g_value_get_boolean (&value) == TRUE);
  g_value_unset (&value);

  cut_assert (gtk_builder_value_from_string_type (builder, G_TYPE_BOOLEAN, "false", &value, &error));
  cut_assert (G_VALUE_HOLDS_BOOLEAN (&value));
  cut_assert (g_value_get_boolean (&value) == FALSE);
  g_value_unset (&value);

  cut_assert (gtk_builder_value_from_string_type (builder, G_TYPE_BOOLEAN, "yes", &value, &error));
  cut_assert (G_VALUE_HOLDS_BOOLEAN (&value));
  cut_assert (g_value_get_boolean (&value) == TRUE);
  g_value_unset (&value);

  cut_assert (gtk_builder_value_from_string_type (builder, G_TYPE_BOOLEAN, "no", &value, &error));
  cut_assert (G_VALUE_HOLDS_BOOLEAN (&value));
  cut_assert (g_value_get_boolean (&value) == FALSE);
  g_value_unset (&value);

  cut_assert (gtk_builder_value_from_string_type (builder, G_TYPE_BOOLEAN, "0", &value, &error));
  cut_assert (G_VALUE_HOLDS_BOOLEAN (&value));
  cut_assert (g_value_get_boolean (&value) == FALSE);
  g_value_unset (&value);

  cut_assert (gtk_builder_value_from_string_type (builder, G_TYPE_BOOLEAN, "1", &value, &error));
  cut_assert (G_VALUE_HOLDS_BOOLEAN (&value));
  cut_assert (g_value_get_boolean (&value) == TRUE);
  g_value_unset (&value);

  cut_assert (gtk_builder_value_from_string_type (builder, G_TYPE_BOOLEAN, "tRuE", &value, &error));
  cut_assert (G_VALUE_HOLDS_BOOLEAN (&value));
  cut_assert (g_value_get_boolean (&value) == TRUE);
  g_value_unset (&value);
  
  cut_assert (gtk_builder_value_from_string_type (builder, G_TYPE_BOOLEAN, "blaurgh", &value, &error) == FALSE);
  cut_assert (error != NULL);
  g_value_unset (&value);
  cut_assert (error->domain == GTK_BUILDER_ERROR);
  cut_assert (error->code == GTK_BUILDER_ERROR_INVALID_VALUE);
  g_error_free (error);
  error = NULL;

  cut_assert (gtk_builder_value_from_string_type (builder, G_TYPE_BOOLEAN, "yess", &value, &error) == FALSE);
  g_value_unset (&value);
  cut_assert (error->domain == GTK_BUILDER_ERROR);
  cut_assert (error->code == GTK_BUILDER_ERROR_INVALID_VALUE);
  g_error_free (error);
  error = NULL;
  
  cut_assert (gtk_builder_value_from_string_type (builder, G_TYPE_BOOLEAN, "trueee", &value, &error) == FALSE);
  g_value_unset (&value);
  cut_assert (error->domain == GTK_BUILDER_ERROR);
  cut_assert (error->code == GTK_BUILDER_ERROR_INVALID_VALUE);
  g_error_free (error);
  error = NULL;
  
  cut_assert (gtk_builder_value_from_string_type (builder, G_TYPE_BOOLEAN, "", &value, &error) == FALSE);
  g_value_unset (&value);
  cut_assert (error->domain == GTK_BUILDER_ERROR);
  cut_assert (error->code == GTK_BUILDER_ERROR_INVALID_VALUE);
  g_error_free (error);
  error = NULL;
  
  cut_assert (gtk_builder_value_from_string_type (builder, G_TYPE_INT, "12345", &value, &error));
  cut_assert (G_VALUE_HOLDS_INT (&value));
  cut_assert (g_value_get_int (&value) == 12345);
  g_value_unset (&value);

  cut_assert (gtk_builder_value_from_string_type (builder, G_TYPE_LONG, "9912345", &value, &error));
  cut_assert (G_VALUE_HOLDS_LONG (&value));
  cut_assert (g_value_get_long (&value) == 9912345);
  g_value_unset (&value);

  cut_assert (gtk_builder_value_from_string_type (builder, G_TYPE_UINT, "2345", &value, &error));
  cut_assert (G_VALUE_HOLDS_UINT (&value));
  cut_assert (g_value_get_uint (&value) == 2345);
  g_value_unset (&value);

  cut_assert (gtk_builder_value_from_string_type (builder, G_TYPE_FLOAT, "1.454", &value, &error));
  cut_assert (G_VALUE_HOLDS_FLOAT (&value));
  cut_assert (fabs (g_value_get_float (&value) - 1.454) < 0.00001);
  g_value_unset (&value);

  cut_assert (gtk_builder_value_from_string_type (builder, G_TYPE_FLOAT, "abc", &value, &error) == FALSE);
  g_value_unset (&value);
  cut_assert (error->domain == GTK_BUILDER_ERROR);
  cut_assert (error->code == GTK_BUILDER_ERROR_INVALID_VALUE);
  g_error_free (error);
  error = NULL;

  cut_assert (gtk_builder_value_from_string_type (builder, G_TYPE_INT, "/-+,abc", &value, &error) == FALSE);
  g_value_unset (&value);
  cut_assert (error->domain == GTK_BUILDER_ERROR);
  cut_assert (error->code == GTK_BUILDER_ERROR_INVALID_VALUE);
  g_error_free (error);
  error = NULL;

  cut_assert (gtk_builder_value_from_string_type (builder, GTK_TYPE_WINDOW_TYPE, "toplevel", &value, &error) == TRUE);
  cut_assert (G_VALUE_HOLDS_ENUM (&value));
  cut_assert (g_value_get_enum (&value) == GTK_WINDOW_TOPLEVEL);
  g_value_unset (&value);

  cut_assert (gtk_builder_value_from_string_type (builder, GTK_TYPE_WINDOW_TYPE, "sliff", &value, &error) == FALSE);
  g_value_unset (&value);
  cut_assert (error->domain == GTK_BUILDER_ERROR);
  cut_assert (error->code == GTK_BUILDER_ERROR_INVALID_VALUE);
  g_error_free (error);
  error = NULL;
  
  cut_assert (gtk_builder_value_from_string_type (builder, GTK_TYPE_WIDGET_FLAGS, "mapped", &value, &error) == TRUE);
  cut_assert (G_VALUE_HOLDS_FLAGS (&value));
  cut_assert (g_value_get_flags (&value) == GTK_MAPPED);
  g_value_unset (&value);

  cut_assert (gtk_builder_value_from_string_type (builder, GTK_TYPE_WIDGET_FLAGS, "GTK_VISIBLE | GTK_REALIZED", &value, &error) == TRUE);
  cut_assert (G_VALUE_HOLDS_FLAGS (&value));
  cut_assert (g_value_get_flags (&value) == (GTK_VISIBLE | GTK_REALIZED));
  g_value_unset (&value);
  
  cut_assert (gtk_builder_value_from_string_type (builder, GTK_TYPE_WINDOW_TYPE, "foobar", &value, &error) == FALSE);
  g_value_unset (&value);
  cut_assert (error->domain == GTK_BUILDER_ERROR);
  cut_assert (error->code == GTK_BUILDER_ERROR_INVALID_VALUE);
  g_error_free (error);
  error = NULL;
}

static gboolean model_freed = FALSE;

static void model_weakref (gpointer data, GObject *model)
{
  model_freed = TRUE;
}

void
test_reference_counting (void)
{
  const gchar buffer1[] =
    "<interface>"
    "  <object class=\"GtkListStore\" id=\"liststore1\"/>"
    "  <object class=\"GtkListStore\" id=\"liststore2\"/>"
    "  <object class=\"GtkWindow\" id=\"window1\">"
    "    <child>"
    "      <object class=\"GtkTreeView\" id=\"treeview1\">"
    "        <property name=\"model\">liststore1</property>"
    "      </object>"
    "    </child>"
    "  </object>"
    "</interface>";
  const gchar buffer2[] =
    "<interface>"
    "  <object class=\"GtkVBox\" id=\"vbox1\">"
    "    <child>"
    "      <object class=\"GtkLabel\" id=\"label1\"/>"
    "      <packing>"
    "        <property name=\"pack-type\">start</property>"
    "      </packing>"
    "    </child>"
    "  </object>"
    "</interface>";
  GObject *window, *treeview, *model;
  
  builder = builder_new_from_string (buffer1, -1, NULL);
  window = gtk_builder_get_object (builder, "window1");
  treeview = gtk_builder_get_object (builder, "treeview1");
  model = gtk_builder_get_object (builder, "liststore1");
  g_object_unref (builder);

  g_object_weak_ref (model, (GWeakNotify)model_weakref, NULL);

  cut_assert (model_freed == FALSE);
  gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), NULL);
  cut_assert (model_freed == TRUE);
  
  gtk_widget_destroy (GTK_WIDGET (window));

  builder = builder_new_from_string (buffer2, -1, NULL);
  g_object_unref (builder);
  builder = NULL;
}

