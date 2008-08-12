#include <adg/adg.h>
#include <gcontainer/gcontainer.h>
#include <gtk/gtk.h>


static void     draw_shape              (AdgPath        *path);
static void     drawing_expose          (GtkWidget      *widget,
                                         GdkEventExpose *event,
                                         AdgCanvas      *canvas);


int
main(gint argc, gchar ** argv)
{
    GtkWidget *window;
    GtkWidget *widget;
    AdgCanvas *canvas;
    AdgEntity *entity;

    gtk_init(&argc, &argv);

    /* Create the canvas */
    canvas = adg_canvas_new();

    /* Add the shape to quote */
    entity = adg_path_new(ADG_CALLBACK(draw_shape), NULL);
    g_containerable_add(G_CONTAINERABLE(canvas), G_CHILDABLE(entity));

    /* Add the north quotes */
    entity = adg_ldim_new_full_explicit(2., -10., 8., -10., CPML_DIR_UP,
                                        0.0, -10.);
    g_containerable_add(G_CONTAINERABLE(canvas), G_CHILDABLE(entity));

    entity = adg_ldim_new_full_explicit(0., -5., 10., -5., CPML_DIR_UP,
                                        0.0, -10.);
    adg_dim_set_level(ADG_DIM(entity), 2);
    g_containerable_add(G_CONTAINERABLE(canvas), G_CHILDABLE(entity));

    /* User interface stuff */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

    widget = gtk_drawing_area_new();
    g_signal_connect(widget, "expose-event", G_CALLBACK(drawing_expose),
		     canvas);
    gtk_container_add(GTK_CONTAINER(window), widget);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

static void
draw_shape(AdgPath *path)
{
    adg_path_move_to(path, 0., 0.);
    adg_path_line_to(path, 0., -5.);
    adg_path_line_to(path, 2., -5.);
    adg_path_line_to(path, 2., -10.);
    adg_path_line_to(path, 8., -10.);
    adg_path_line_to(path, 8., -5.);
    adg_path_line_to(path, 10., -5.);
    adg_path_line_to(path, 10., 0.);
    adg_path_close(path);
}

static void
drawing_expose(GtkWidget *widget, GdkEventExpose *event, AdgCanvas *canvas)
{
    cairo_t *cr;
    gint width, height;
    double scale;
    AdgMatrix matrix;

    cr = gdk_cairo_create(widget->window);
    width = widget->allocation.width;
    height = widget->allocation.height;
    scale = (double) (width - 10.) / 10.;

    cairo_matrix_init(&matrix, scale, 0., 0., scale, 5.,
		      75. + 10. * scale);
    adg_container_set_model_transformation(ADG_CONTAINER(canvas), &matrix);

    /* Rendering process */
    adg_entity_render(ADG_ENTITY(canvas), cr);

    cairo_destroy(cr);
}
