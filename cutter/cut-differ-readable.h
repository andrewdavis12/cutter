/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2009  Kouhei Sutou <kou@clear-code.com>
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __CUT_DIFFER_READABLE_H__
#define __CUT_DIFFER_READABLE_H__

#include <glib-object.h>
#include <cutter/cut-diff.h>

G_BEGIN_DECLS

#define CUT_TYPE_DIFFER_READABLE            (cut_differ_readable_get_type ())
#define CUT_DIFFER_READABLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_DIFFER_READABLE, CutDifferReadable))
#define CUT_DIFFER_READABLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_DIFFER_READABLE, CutDifferReadableClass))
#define CUT_IS_DIFFER_READABLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_DIFFER_READABLE))
#define CUT_IS_DIFFER_READABLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_DIFFER_READABLE))
#define CUT_DIFFER_READABLE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_DIFFER_READABLE, CutDifferReadableClass))

typedef struct _CutDifferReadable         CutDifferReadable;
typedef struct _CutDifferReadableClass    CutDifferReadableClass;

struct _CutDifferReadable
{
    CutDiffer object;
};

struct _CutDifferReadableClass
{
    CutDifferClass parent_class;
};

GType      cut_differ_readable_get_type  (void) G_GNUC_CONST;

CutDiffer *cut_differ_readable_new       (const gchar *from,
                                          const gchar *to);

gboolean   cut_diff_readable_is_interested
                                         (const gchar *diff);
gboolean   cut_diff_readable_need_fold   (const gchar *diff);

G_END_DECLS

#endif /* __CUT_DIFFER_READABLE_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
