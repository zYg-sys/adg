/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2008, Nicola Fontana <ntd at entidi.it>
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
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the 
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 */


/**
 * SECTION:adgldim
 * @title: AdgLDim
 * @short_description: Linear dimensions
 *
 * The #AdgLDim entity represents a linear dimension.
 */

#include "adg-ldim.h"
#include "adg-dim-private.h"
#include "adg-container.h"
#include "adg-util.h"
#include "adg-intl.h"

#define PARENT_CLASS ((AdgDimClass *) adg_ldim_parent_class)


enum
{
  PROP_0,
  PROP_DIRECTION
};


static void	finalize	(GObject	*object);
static void	get_property	(GObject	*object,
				 guint		 param_id,
				 GValue		*value,
				 GParamSpec	*pspec);
static void	set_property	(GObject	*object,
				 guint		 param_id,
				 const GValue	*value,
				 GParamSpec	*pspec);
static void	ctm_changed	(AdgEntity	*entity,
				 AdgMatrix	*old_matrix);
static void	update		(AdgEntity	*entity,
				 gboolean	 recursive);
static void	outdate		(AdgEntity	*entity,
				 gboolean	 recursive);
static void	render		(AdgEntity	*entity,
				 cairo_t	*cr);
static gchar *	default_label	(AdgDim		*dim);


G_DEFINE_TYPE (AdgLDim, adg_ldim, ADG_TYPE_DIM);


static void
adg_ldim_class_init (AdgLDimClass *klass)
{
  GObjectClass   *gobject_class;
  AdgEntityClass *entity_class;
  AdgDimClass    *dim_class;
  GParamSpec     *param;

  gobject_class = (GObjectClass *) klass;
  entity_class = (AdgEntityClass *) klass;
  dim_class = (AdgDimClass *) klass;

  gobject_class->finalize = finalize;
  gobject_class->get_property = get_property;
  gobject_class->set_property = set_property;

  entity_class->ctm_changed = ctm_changed;
  entity_class->update = update;
  entity_class->outdate = outdate;
  entity_class->render = render;

  g_type_class_add_private (klass, sizeof (_AdgLDimPrivate));
  dim_class->default_label = default_label;

  param = g_param_spec_double ("direction",
			       P_("Direction"),
			       P_("The inclination angle of the extension lines"),
			       -G_MAXDOUBLE, G_MAXDOUBLE, ADG_DIR_RIGHT,
			       G_PARAM_READWRITE|G_PARAM_CONSTRUCT);
  g_object_class_install_property (gobject_class, PROP_DIRECTION, param);

}

static void
adg_ldim_init (AdgLDim *ldim)
{
  _AdgLDimPrivate *cache = _ADG_LDIM_GET_PRIVATE (ldim);

  ldim->direction = ADG_DIR_RIGHT;

  cache->extension1.status = CAIRO_STATUS_SUCCESS;
  cache->extension1.data = NULL;
  cache->extension1.num_data = 4;

  cache->extension2.status = CAIRO_STATUS_SUCCESS;
  cache->extension2.data = NULL;
  cache->extension2.num_data = 4;

  cache->arrow_path.status = CAIRO_STATUS_SUCCESS;
  cache->arrow_path.data = NULL;
  cache->arrow_path.num_data = 4;

  cache->baseline.status = CAIRO_STATUS_SUCCESS;
  cache->baseline.data = NULL;
  cache->baseline.num_data = 4;
}

static void
finalize (GObject *object)
{
  _AdgLDimPrivate *cache = _ADG_LDIM_GET_PRIVATE (object);

  g_free (cache->extension1.data);
  g_free (cache->extension2.data);
  g_free (cache->arrow_path.data);
  g_free (cache->baseline.data);
}

static void
get_property (GObject    *object,
	      guint       prop_id,
	      GValue     *value,
	      GParamSpec *pspec)
{
  AdgLDim *ldim = ADG_LDIM (object);

  switch (prop_id)
    {
    case PROP_DIRECTION:
      g_value_set_double (value, ldim->direction);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
set_property (GObject      *object,
	      guint         prop_id,
	      const GValue *value,
	      GParamSpec   *pspec)
{
  AdgLDim *ldim = ADG_LDIM (object);
  
  switch (prop_id)
    {
    case PROP_DIRECTION:
      ldim->direction = g_value_get_double (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
ctm_changed (AdgEntity *entity,
	     AdgMatrix *old_matrix)
{
  AdgContainer *container;
  AdgMatrix    *matrix;

  /* entity is yet outdated, no needs for further checks */
  if (! ADG_ENTITY_UPTODATE (entity))
    return;

  container = (AdgContainer *) entity->parent;
  g_return_if_fail (ADG_IS_CONTAINER (container));

  matrix = &container->ctm;

  if (old_matrix == NULL || old_matrix->xx != matrix->xx || old_matrix->yy != matrix->yy)
    adg_entity_outdate (entity);
}

static void
update (AdgEntity *entity,
	gboolean   recursive)
{
  AdgDim            *dim;
  _AdgDimPrivate    *dim_cache;
  AdgLDim           *ldim;
  _AdgLDimPrivate   *cache;
  AdgMatrix          device2user;
  AdgVector          vector;
  AdgPair            offset;
  AdgPair            from1, to1;
  AdgPair            from2, to2;
  AdgPair            arrow1, arrow2;
  AdgPair            baseline1, baseline2;
  cairo_path_data_t *path_data;

  dim = (AdgDim *) entity;
  dim_cache = _ADG_DIM_GET_PRIVATE (dim);
  ldim = (AdgLDim *) entity;
  cache = _ADG_LDIM_GET_PRIVATE (ldim);

  g_assert (dim_cache != NULL);
  g_assert (cache != NULL);
  g_return_if_fail (dim->dim_style != NULL);
  g_return_if_fail (adg_pair_is_set (&dim->ref1));
  g_return_if_fail (adg_pair_is_set (&dim->ref2));
  g_return_if_fail (adg_pair_is_set (&dim->pos1));
  g_return_if_fail (! adg_isnan (ldim->direction));

  /* Get the inverted transformation matrix */
  adg_matrix_set (&device2user, adg_entity_get_ctm (entity));
  g_return_if_fail (cairo_matrix_invert (&device2user) == CAIRO_STATUS_SUCCESS);

  /* Set vector to the direction where ldim will extend */
  adg_vector_set_with_angle (&vector, ldim->direction);

  /* Calculate from1 and from2*/
  adg_pair_scale_and_transform (adg_pair_set (&offset, &vector),
                                dim->dim_style->from_offset,
                                &device2user);
  /* Checkpoint */
  g_return_if_fail (adg_pair_is_set (&offset));
  adg_pair_add (adg_pair_set (&from1, &offset), &dim->ref1);
  adg_pair_add (adg_pair_set (&from2, &offset), &dim->ref2);

  /* Calculate arrow1 and arrow2 */
  adg_pair_scale_and_transform (adg_pair_set (&offset, &vector),
                                dim->level * dim->dim_style->baseline_spacing,
                                &device2user);
  adg_pair_add (adg_pair_set (&arrow1, &dim->pos1), &offset);
  adg_pair_add (adg_pair_set (&arrow2, &dim->pos2), &offset);

  /* Calculate to1 and to2 */
  adg_pair_scale_and_transform (adg_pair_set (&offset, &vector),
                                dim->dim_style->to_offset,
                                &device2user);
  adg_pair_add (adg_pair_set (&to1, &arrow1), &offset);
  adg_pair_add (adg_pair_set (&to2, &arrow2), &offset);

  /* Set vector to the director of the baseline */
  adg_pair_sub (adg_pair_set (&offset, &arrow2), &arrow1);
  adg_vector_set_with_pair (&vector, &offset);

  /* Update the dim_cache contents */
  adg_pair_mid (adg_pair_set (&dim_cache->quote_org, &arrow1), &arrow2);
  dim_cache->quote_angle = adg_pair_get_angle (&vector);

  /* Calculate baseline1 and baseline2 */
  adg_pair_scale_and_transform (adg_pair_set (&offset, &vector),
                                dim->dim_style->arrow_style->margin,
                                &device2user);
  adg_pair_add (adg_pair_set (&baseline1, &arrow1), &offset);
  adg_pair_sub (adg_pair_set (&baseline2, &arrow2), &offset);

  /* Set extension1 */
  if (cache->extension1.data == NULL)
    cache->extension1.data = g_new (cairo_path_data_t, 4);

  path_data = cache->extension1.data;
  path_data[0].header.type = CAIRO_PATH_MOVE_TO;
  path_data[0].header.length = 2;
  path_data[1].point.x = from1.x;
  path_data[1].point.y = from1.y;
  path_data[2].header.type = CAIRO_PATH_LINE_TO;
  path_data[2].header.length = 2;
  path_data[3].point.x = to1.x;
  path_data[3].point.y = to1.y;

  /* Set extension2 */
  if (cache->extension2.data == NULL)
    cache->extension2.data = g_new (cairo_path_data_t, 4);

  path_data = cache->extension2.data;
  path_data[0].header.type = CAIRO_PATH_MOVE_TO;
  path_data[0].header.length = 2;
  path_data[1].point.x = from2.x;
  path_data[1].point.y = from2.y;
  path_data[2].header.type = CAIRO_PATH_LINE_TO;
  path_data[2].header.length = 2;
  path_data[3].point.x = to2.x;
  path_data[3].point.y = to2.y;

  /* Set arrow_path */
  if (cache->arrow_path.data == NULL)
    cache->arrow_path.data = g_new (cairo_path_data_t, 4);

  path_data = cache->arrow_path.data;
  path_data[0].header.type = CAIRO_PATH_MOVE_TO;
  path_data[0].header.length = 2;
  path_data[1].point.x = arrow1.x;
  path_data[1].point.y = arrow1.y;
  path_data[2].header.type = CAIRO_PATH_LINE_TO;
  path_data[2].header.length = 2;
  path_data[3].point.x = arrow2.x;
  path_data[3].point.y = arrow2.y;

  /* Set baseline */
  if (cache->baseline.data == NULL)
    cache->baseline.data = g_new (cairo_path_data_t, 4);

  path_data = cache->baseline.data;
  path_data[0].header.type = CAIRO_PATH_MOVE_TO;
  path_data[0].header.length = 2;
  path_data[1].point.x = baseline1.x;
  path_data[1].point.y = baseline1.y;
  path_data[2].header.type = CAIRO_PATH_LINE_TO;
  path_data[2].header.length = 2;
  path_data[3].point.x = baseline2.x;
  path_data[3].point.y = baseline2.y;

  ((AdgEntityClass *) PARENT_CLASS)->update (entity, recursive);
}

static void
outdate (AdgEntity *entity,
	 gboolean   recursive)
{
  ((AdgEntityClass *) PARENT_CLASS)->outdate (entity, recursive);
}

static void
render (AdgEntity *entity,
	cairo_t   *cr)
{
  AdgDim          *dim;
  AdgLDim         *ldim;
  _AdgLDimPrivate *cache;

  dim = (AdgDim *) entity;
  ldim = (AdgLDim *) entity;
  cache = _ADG_LDIM_GET_PRIVATE (ldim);

  g_assert (cache != NULL);
  g_return_if_fail (dim->dim_style != NULL);

  cairo_save (cr);
  cairo_set_source (cr, dim->dim_style->pattern);

  /* Arrows */
  adg_arrow_render (cr, dim->dim_style->arrow_style, &cache->arrow_path, ADG_PATH_POINT_START);
  adg_arrow_render (cr, dim->dim_style->arrow_style, &cache->arrow_path, ADG_PATH_POINT_END);

  /* Lines */
  adg_line_style_apply (dim->dim_style->line_style, cr);

  cairo_append_path (cr, &cache->extension1);
  cairo_append_path (cr, &cache->extension2);
  cairo_append_path (cr, &cache->baseline);

  cairo_stroke (cr);

  /* Quote */
  _adg_dim_render_quote (dim, cr);

  cairo_restore (cr);
}

static gchar *
default_label (AdgDim *dim)
{
  AdgPair delta;
  double  measure;

  measure = adg_pair_get_length (adg_pair_sub (adg_pair_set (&delta, &dim->pos2), &dim->pos1));

  return g_strdup_printf (dim->dim_style->measure_format, measure);
}


/**
 * adg_ldim_new:
 *
 * Creates a new - unreferenced - linear dimension. You must, at least, define
 * the reference points with adg_dim_set_ref(), the dimension direction with
 * adg_ldim_set_direction() and the position reference using adg_dim_set_pos()
 * or, better, adg_ldim_set_pos().
 *
 * Return value: the new entity
 */
AdgEntity *
adg_ldim_new (void)
{
  return (AdgEntity *) g_object_new (ADG_TYPE_LDIM, NULL);
}

/**
 * adg_ldim_new_full:
 * @ref1: the first reference point
 * @ref2: the second reference point
 * @direction: angle where to extend the dimension
 * @pos: the position reference
 *
 * Creates a new linear dimension, specifing all the needed properties in
 * one shot.
 *
 * Return value: the new entity
 */
AdgEntity *
adg_ldim_new_full (const AdgPair *ref1,
                   const AdgPair *ref2,
                   double         direction,
                   const AdgPair *pos)
{
  AdgEntity *entity = (AdgEntity *) g_object_new (ADG_TYPE_LDIM,
                                                  "ref1", ref1,
                                                  "ref2", ref2,
                                                  "direction", direction,
                                                  NULL);
  adg_ldim_set_pos ((AdgLDim *) entity, pos);
  return entity;
}

/**
 * adg_ldim_new_full_explicit:
 * @ref1_x: the x coordinate of the first reference point
 * @ref1_y: the y coordinate of the first reference point
 * @ref2_x: the x coordinate of the second reference point
 * @ref2_y: the y coordinate of the second reference point
 * @direction: angle where to extend the dimension
 * @pos_x: the x coordinate of the position reference
 * @pos_y: the y coordinate of the position reference
 *
 * Wrappes adg_ldim_new_full() with explicit quotes.
 *
 * Return value: the new entity
 */
AdgEntity *
adg_ldim_new_full_explicit (double ref1_x,
                            double ref1_y,
                            double ref2_x,
                            double ref2_y,
                            double direction,
                            double pos_x,
                            double pos_y)
{
  AdgPair ref1;
  AdgPair ref2;
  AdgPair pos;

  ref1.x = ref1_x;
  ref1.y = ref1_y;
  ref2.x = ref2_x;
  ref2.y = ref2_y;
  pos.x = pos_x;
  pos.y = pos_y;

  return adg_ldim_new_full (&ref1, &ref2, direction, &pos);
}



/**
 * adg_ldim_set_pos:
 * @ldim: an #AdgLDim entity
 * @pos: an #AdgPair structure
 *
 * Sets the position references (pos1 and pos2 properties) of @ldim using a
 * single @pos point. Before this call, @ldim MUST HAVE defined the reference
 * points and the direction. If these conditions are not met, an error message
 * is logged and the position references will not be set.
 */
void
adg_ldim_set_pos (AdgLDim       *ldim,
                  const AdgPair *pos)
{
  AdgDim   *dim;
  GObject  *object;
  AdgVector extension_vector;
  AdgVector baseline_vector;

  g_return_if_fail (ADG_IS_LDIM (ldim));
  g_return_if_fail (adg_pair_is_set (pos));
  g_return_if_fail (! adg_isnan (ldim->direction));

  dim = (AdgDim *) ldim;
  object = (GObject *) ldim;

  g_return_if_fail (adg_pair_is_set (&dim->ref1));
  g_return_if_fail (adg_pair_is_set (&dim->ref2));

  adg_pair_set (&dim->pos1, &dim->ref1);
  adg_pair_set (&dim->pos2, &dim->ref2);
  adg_vector_set_with_angle (&extension_vector, ldim->direction);
  adg_vector_normal (adg_pair_set (&baseline_vector, &extension_vector));

  adg_pair_intersection (&dim->pos1, &extension_vector, pos, &baseline_vector);
  adg_pair_intersection (&dim->pos2, &extension_vector, pos, &baseline_vector);

  g_object_freeze_notify (object);
  g_object_notify (object, "pos1");
  g_object_notify (object, "pos2");
  g_object_thaw_notify (object);
}

/**
 * adg_ldim_set_pos_explicit:
 * @ldim: an #AdgLDim entity
 * @pos_x: the new x coordinate position reference
 * @pos_y: the new y coordinate position reference
 *
 * Wrappers adg_ldim_set_pos() with explicit coordinates.
 */
void
adg_ldim_set_pos_explicit (AdgLDim *ldim,
                           double   pos_x,
                           double   pos_y)
{
  AdgPair pos;

  pos.x = pos_x;
  pos.y = pos_y;

  adg_ldim_set_pos (ldim, &pos);
}

/**
 * adg_ldim_get_direction:
 * @ldim: an #AdgLDim entity
 *
 * Gets the direction where @ldim will extend.
 *
 * Return value: the direction angle, in radians
 */
double
adg_ldim_get_direction (AdgLDim *ldim)
{
  g_return_val_if_fail (ADG_IS_LDIM (ldim), ADG_NAN);

  return ldim->direction;
}

/**
 * adg_ldim_set_direction:
 * @ldim: an #AdgLDim entity
 * @direction: an angle value, in radians
 *
 * Sets the direction angle where to extend @ldim.
 */
void
adg_ldim_set_direction (AdgLDim *ldim,
                        double   direction)
{
  g_return_if_fail (ADG_IS_LDIM (ldim));

  ldim->direction = direction;
  g_object_notify ((GObject *) ldim, "direction");
}
