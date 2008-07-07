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


#ifndef __ADG_DIM_STYLE_PRIVATE_H__
#define __ADG_DIM_STYLE_PRIVATE_H__

#include <adg/adg-style.h>


G_BEGIN_DECLS


struct _AdgDimStylePrivate
{
  AdgStyle	*label_style;
  AdgStyle	*tolerance_style;
  AdgStyle	*note_style;
  AdgStyle	*line_style;
  AdgStyle	*arrow_style;
  double	 from_offset;
  double	 to_offset;
  double	 baseline_spacing;
  AdgPair	 quote_offset;
  AdgPair	 tolerance_offset;
  double	 tolerance_spacing;
  AdgPair	 note_offset;
  gchar		*number_format;
  gchar		*number_tag;
};


G_END_DECLS


#endif /* __ADG_DIM_STYLE_PRIVATE_H__ */
