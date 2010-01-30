/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009 Nicola Fontana <ntd at entidi.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */


/**
 * SECTION:adg-table-style
 * @short_description: Customization of table rendering
 *
 * Contains parameters on how to build tables such as the lines to
 * use for frames and grids and the font dresses for titles or values.
 */

/**
 * AdgTableStyle:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-internal.h"
#include "adg-table-style.h"
#include "adg-table-style-private.h"
#include "adg-dress-builtins.h"
#include "adg-font-style.h"
#include "adg-line-style.h"
#include "adg-util.h"


enum {
    PROP_0,
    PROP_COLOR_DRESS,
    PROP_GRID_DRESS,
    PROP_FRAME_DRESS,
    PROP_TITLE_DRESS,
    PROP_VALUE_DRESS,
    PROP_ROW_HEIGHT,
    PROP_CELL_PADDING,
    PROP_CELL_SPACING
};


static void             get_property            (GObject        *object,
                                                 guint           prop_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             set_property            (GObject        *object,
                                                 guint           prop_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             apply                   (AdgStyle       *style,
                                                 AdgEntity      *entity,
                                                 cairo_t        *cr);


G_DEFINE_TYPE(AdgTableStyle, adg_table_style, ADG_TYPE_STYLE);


static void
adg_table_style_class_init(AdgTableStyleClass *klass)
{
    GObjectClass *gobject_class;
    AdgStyleClass *style_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    style_class = (AdgStyleClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgTableStylePrivate));

    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;

    style_class->apply = apply;

    param = adg_param_spec_dress("color-dress",
                                  P_("Color Dress"),
                                  P_("Color dress for the whole tableension"),
                                  ADG_DRESS_COLOR,
                                  G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_COLOR_DRESS, param);

    param = adg_param_spec_dress("grid-dress",
                                  P_("Grid Dress"),
                                  P_("Line dress to use while rendering the grid of the table"),
                                  ADG_DRESS_LINE_GRID,
                                  G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_GRID_DRESS, param);

    param = adg_param_spec_dress("frame-dress",
                                  P_("Frame Dress"),
                                  P_("Line dress to use while drawing the table frame"),
                                  ADG_DRESS_LINE_FRAME,
                                  G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_FRAME_DRESS, param);

    param = adg_param_spec_dress("title-dress",
                                  P_("Title Dress"),
                                  P_("Font dress to use for titles"),
                                  ADG_DRESS_TEXT_LIMIT,
                                  G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TITLE_DRESS, param);

    param = adg_param_spec_dress("value-dress",
                                  P_("Value Dress"),
                                  P_("Font dress to use for values inside the cells"),
                                  ADG_DRESS_TEXT_VALUE,
                                  G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_VALUE_DRESS, param);

    param = g_param_spec_double("row-height",
                                P_("Row Height"),
                                P_("The fallback row height when not explicitely specified while creating a new row"),
                                0, G_MAXDOUBLE, 30,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_ROW_HEIGHT, param);

    param = g_param_spec_boxed("cell-padding",
                               P_("Cell Padding"),
                               P_("How much space from the bounding box must left inside every cell"),
                               ADG_TYPE_PAIR,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_CELL_PADDING, param);

    param = g_param_spec_boxed("cell-spacing",
                               P_("Cell Spacing"),
                               P_("How much space to left between the cells"),
                               ADG_TYPE_PAIR,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_CELL_SPACING, param);
}

static void
adg_table_style_init(AdgTableStyle *table_style)
{
    AdgTableStylePrivate *data;

    data = G_TYPE_INSTANCE_GET_PRIVATE(table_style, ADG_TYPE_TABLE_STYLE,
                                       AdgTableStylePrivate);

    data->color_dress = ADG_DRESS_COLOR;
    data->grid_dress = ADG_DRESS_LINE_GRID;
    data->frame_dress = ADG_DRESS_LINE_FRAME;
    data->title_dress = ADG_DRESS_TEXT_LIMIT;
    data->value_dress = ADG_DRESS_TEXT_VALUE;
    data->row_height = 30;
    data->cell_padding.x = 5;
    data->cell_padding.y = 5;
    data->cell_spacing.x = 0;
    data->cell_spacing.y = 0;

    table_style->data = data;
}

static void
get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgTableStylePrivate *data = ((AdgTableStyle *) object)->data;

    switch (prop_id) {
    case PROP_COLOR_DRESS:
        g_value_set_int(value, data->color_dress);
        break;
    case PROP_GRID_DRESS:
        g_value_set_int(value, data->grid_dress);
        break;
    case PROP_FRAME_DRESS:
        g_value_set_int(value, data->frame_dress);
        break;
    case PROP_TITLE_DRESS:
        g_value_set_int(value, data->title_dress);
        break;
    case PROP_VALUE_DRESS:
        g_value_set_int(value, data->value_dress);
        break;
    case PROP_ROW_HEIGHT:
        g_value_set_double(value, data->row_height);
        break;
    case PROP_CELL_PADDING:
        g_value_set_boxed(value, &data->cell_padding);
        break;
    case PROP_CELL_SPACING:
        g_value_set_boxed(value, &data->cell_spacing);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
set_property(GObject *object,
             guint prop_id, const GValue *value, GParamSpec *pspec)
{
    AdgTableStylePrivate *data = ((AdgTableStyle *) object)->data;

    switch (prop_id) {
    case PROP_COLOR_DRESS:
        adg_dress_set(&data->color_dress, g_value_get_int(value));
        break;
    case PROP_GRID_DRESS:
        adg_dress_set(&data->grid_dress, g_value_get_int(value));
        break;
    case PROP_FRAME_DRESS:
        adg_dress_set(&data->frame_dress, g_value_get_int(value));
        break;
    case PROP_TITLE_DRESS:
        adg_dress_set(&data->title_dress, g_value_get_int(value));
        break;
    case PROP_VALUE_DRESS:
        adg_dress_set(&data->value_dress, g_value_get_int(value));
        break;
    case PROP_ROW_HEIGHT:
        data->row_height = g_value_get_double(value);
        break;
    case PROP_CELL_PADDING:
        cpml_pair_copy(&data->cell_padding, g_value_get_boxed(value));
        break;
    case PROP_CELL_SPACING:
        cpml_pair_copy(&data->cell_spacing, g_value_get_boxed(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_table_style_new:
 *
 * Constructs a new tableension style initialized with default params.
 *
 * Returns: a new tableension style
 **/
AdgStyle *
adg_table_style_new(void)
{
    return g_object_new(ADG_TYPE_TABLE_STYLE, NULL);
}

/**
 * adg_table_style_set_color_dress:
 * @table_style: an #AdgTableStyle object
 * @dress: the new color dress
 *
 * Sets a new color dress on @table_style.
 **/
void
adg_table_style_set_color_dress(AdgTableStyle *table_style, AdgDress dress)
{
    AdgTableStylePrivate *data;

    g_return_if_fail(ADG_IS_TABLE_STYLE(table_style));

    data = table_style->data;

    if (adg_dress_set(&data->color_dress, dress))
        g_object_notify((GObject *) table_style, "color-dress");
}

/**
 * adg_table_style_get_color_dress:
 * @table_style: an #AdgTableStyle object
 *
 * Gets the @table_style color dress to be used. This dress should be
 * intended as a fallback color as it could be overriden by more
 * specific dresses, such as a color explicitely specified on the
 * #AdgTableStyle:value-dress.
 *
 * Returns: the color dress
 **/
AdgDress
adg_table_style_get_color_dress(AdgTableStyle *table_style)
{
    AdgTableStylePrivate *data;

    g_return_val_if_fail(ADG_IS_TABLE_STYLE(table_style), ADG_DRESS_UNDEFINED);

    data = table_style->data;

    return data->color_dress;
}

/**
 * adg_table_style_set_frame_dress:
 * @table_style: an #AdgTableStyle object
 * @dress: the new line dress
 *
 * Sets a new line dress on @table_style for rendering the frames.
 **/
void
adg_table_style_set_frame_dress(AdgTableStyle *table_style, AdgDress dress)
{
    AdgTableStylePrivate *data;

    g_return_if_fail(ADG_IS_TABLE_STYLE(table_style));

    data = table_style->data;

    if (adg_dress_set(&data->frame_dress, dress))
        g_object_notify((GObject *) table_style, "frame-dress");
}

/**
 * adg_table_style_get_frame_dress:
 * @table_style: an #AdgTableStyle object
 *
 * Gets the line dress to be used for rendering the frames with
 * @table_style.
 *
 * Returns: the line dress
 **/
AdgDress
adg_table_style_get_frame_dress(AdgTableStyle *table_style)
{
    AdgTableStylePrivate *data;

    g_return_val_if_fail(ADG_IS_TABLE_STYLE(table_style), ADG_DRESS_UNDEFINED);

    data = table_style->data;

    return data->frame_dress;
}

/**
 * adg_table_style_set_grid_dress:
 * @table_style: an #AdgTableStyle object
 * @dress: the new line dress
 *
 * Sets a new line dress on @table_style for rendering the grids.
 **/
void
adg_table_style_set_grid_dress(AdgTableStyle *table_style, AdgDress dress)
{
    AdgTableStylePrivate *data;

    g_return_if_fail(ADG_IS_TABLE_STYLE(table_style));

    data = table_style->data;

    if (adg_dress_set(&data->grid_dress, dress))
        g_object_notify((GObject *) table_style, "grid-dress");
}

/**
 * adg_table_style_get_grid_dress:
 * @table_style: an #AdgTableStyle object
 *
 * Gets the line dress to be used for rendering the grids with
 * @table_style.
 *
 * Returns: the line dress
 **/
AdgDress
adg_table_style_get_grid_dress(AdgTableStyle *table_style)
{
    AdgTableStylePrivate *data;

    g_return_val_if_fail(ADG_IS_TABLE_STYLE(table_style), ADG_DRESS_UNDEFINED);

    data = table_style->data;

    return data->grid_dress;
}

/**
 * adg_table_style_set_title_dress:
 * @table_style: an #AdgTableStyle object
 * @dress: the new font dress
 *
 * Sets a new font dress on @table_style for rendering cell titles.
 **/
void
adg_table_style_set_title_dress(AdgTableStyle *table_style, AdgDress dress)
{
    AdgTableStylePrivate *data;

    g_return_if_fail(ADG_IS_TABLE_STYLE(table_style));

    data = table_style->data;

    if (adg_dress_set(&data->title_dress, dress))
        g_object_notify((GObject *) table_style, "title-dress");
}

/**
 * adg_table_style_get_title_dress:
 * @table_style: an #AdgTableStyle object
 *
 * Gets the font dress to be used for rendering cell titles
 * with @table_style.
 *
 * Returns: the font dress
 **/
AdgDress
adg_table_style_get_title_dress(AdgTableStyle *table_style)
{
    AdgTableStylePrivate *data;

    g_return_val_if_fail(ADG_IS_TABLE_STYLE(table_style), ADG_DRESS_UNDEFINED);

    data = table_style->data;

    return data->title_dress;
}

/**
 * adg_table_style_set_value_dress:
 * @table_style: an #AdgTableStyle object
 * @dress: the new font dress
 *
 * Sets a new font dress on @table_style for rendering cell values.
 **/
void
adg_table_style_set_value_dress(AdgTableStyle *table_style, AdgDress dress)
{
    AdgTableStylePrivate *data;

    g_return_if_fail(ADG_IS_TABLE_STYLE(table_style));

    data = table_style->data;

    if (adg_dress_set(&data->value_dress, dress))
        g_object_notify((GObject *) table_style, "value-dress");
}

/**
 * adg_table_style_get_value_dress:
 * @table_style: an #AdgTableStyle object
 *
 * Gets the font dress to be used for rendering cell values
 * with @table_style.
 *
 * Returns: the font dress
 **/
AdgDress
adg_table_style_get_value_dress(AdgTableStyle *table_style)
{
    AdgTableStylePrivate *data;

    g_return_val_if_fail(ADG_IS_TABLE_STYLE(table_style), ADG_DRESS_UNDEFINED);

    data = table_style->data;

    return data->value_dress;
}

/**
 * adg_table_style_set_row_height:
 * @table_style: an #AdgTableStyle object
 * @row_heigth: the new row heigth fallback
 *
 * Sets a new #AdgTableStyle:row-height fallback. @row_height must
 * be a valid row height greather than %0 or a warning will be
 * raised and this function will fail.
 **/
void
adg_table_style_set_row_height(AdgTableStyle *table_style, gdouble row_height)
{
    AdgTableStylePrivate *data;

    g_return_if_fail(ADG_IS_TABLE_STYLE(table_style));
    g_return_if_fail(row_height > 0);

    data = table_style->data;

    if (row_height == data->row_height)
        return;

    data->row_height = row_height;

    g_object_notify((GObject *) table_style, "row-height");
}

/**
 * adg_table_style_get_row_height:
 * @table_style: an #AdgTableStyle object
 *
 * Gets the row height fallback value.
 *
 * Returns: the fallback row height or %0 on errors
 **/
gdouble
adg_table_style_get_row_height(AdgTableStyle *table_style)
{
    AdgTableStylePrivate *data;

    g_return_val_if_fail(ADG_IS_TABLE_STYLE(table_style), 0);

    data = table_style->data;

    return data->row_height;
}

/**
 * adg_table_style_set_cell_padding:
 * @table_style: an #AdgTableStyle object
 * @cell_padding: the new padding values
 *
 * Sets new #AdgTableStyle:cell-padding values.
 **/
void
adg_table_style_set_cell_padding(AdgTableStyle *table_style,
                                 const AdgPair *cell_padding)
{
    AdgTableStylePrivate *data;

    g_return_if_fail(ADG_IS_TABLE_STYLE(table_style));
    g_return_if_fail(cell_padding != NULL);

    data = table_style->data;
    cpml_pair_copy(&data->cell_padding, cell_padding);

    g_object_notify((GObject *) table_style, "cell-padding");
}

/**
 * adg_table_style_get_cell_padding:
 * @table_style: an #AdgTableStyle object
 *
 * Gets the padding values in x and y to be left clear inside the cells.
 * The returned pointer refers to an internal allocated struct and
 * must not be modified or freed.
 *
 * The cell padding is a symmetric value, that is the padding on the
 * left will always be equal to the padding on the right and the top
 * will always be equal to the bottom.
 *
 * Returns: the cell padding values or %NULL on errors
 **/
const AdgPair *
adg_table_style_get_cell_padding(AdgTableStyle *table_style)
{
    AdgTableStylePrivate *data;

    g_return_val_if_fail(ADG_IS_TABLE_STYLE(table_style), NULL);

    data = table_style->data;

    return &data->cell_padding;
}

/**
 * adg_table_style_set_cell_spacing:
 * @table_style: an #AdgTableStyle object
 * @cell_spacing: the new spacing values
 *
 * Sets new #AdgTableStyle:cell-spacing values.
 **/
void
adg_table_style_set_cell_spacing(AdgTableStyle *table_style,
                                 const AdgPair *cell_spacing)
{
    AdgTableStylePrivate *data;

    g_return_if_fail(ADG_IS_TABLE_STYLE(table_style));
    g_return_if_fail(cell_spacing != NULL);

    data = table_style->data;
    cpml_pair_copy(&data->cell_spacing, cell_spacing);

    g_object_notify((GObject *) table_style, "cell-spacing");
}

/**
 * adg_table_style_get_cell_spacing:
 * @table_style: an #AdgTableStyle object
 *
 * Gets the spacing values in x and y to be left between the cell
 * boundary boxes. The returned pointer refers to an internal
 * allocated struct and must not be modified or freed.
 *
 * The cell spacing is a symmetric value, that is the spacing on the
 * left will always be equal to the spacing on the right and the top
 * will always be equal to the bottom.
 *
 * Returns: the cell spacing values or %NULL on errors
 **/
const AdgPair *
adg_table_style_get_cell_spacing(AdgTableStyle *table_style)
{
    AdgTableStylePrivate *data;

    g_return_val_if_fail(ADG_IS_TABLE_STYLE(table_style), NULL);

    data = table_style->data;

    return &data->cell_spacing;
}


static void
apply(AdgStyle *style, AdgEntity *entity, cairo_t *cr)
{
    AdgTableStylePrivate *data = ((AdgTableStyle *) style)->data;

    adg_entity_apply_dress(entity, data->color_dress, cr);
}
