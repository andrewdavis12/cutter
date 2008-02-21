/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 *  Boston, MA  02111-1307  USA
 *
 */

#ifndef __CUT_LOADER_H__
#define __CUT_LOADER_H__

#include <glib-object.h>

#include "cut-test.h"
#include "cut-test-case.h"

G_BEGIN_DECLS

#define CUT_TYPE_LOADER            (cut_loader_get_type ())
#define CUT_LOADER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_LOADER, CutLoader))
#define CUT_LOADER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_LOADER, CutLoaderClass))
#define CUT_IS_LOADER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_LOADER))
#define CUT_IS_LOADER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_LOADER))
#define CUT_LOADER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_LOADER, CutLoaderClass))

typedef struct _CutLoader      CutLoader;
typedef struct _CutLoaderClass CutLoaderClass;

struct _CutLoader
{
    GObject object;
};

struct _CutLoaderClass
{
    GObjectClass parent_class;
};

GType cut_loader_get_type  (void) G_GNUC_CONST;

CutLoader *cut_loader_new (const gchar *soname);
CutTestCase *cut_loader_load_test_case (CutLoader *loader);

G_END_DECLS

#endif /* __CUT_LOADER_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
