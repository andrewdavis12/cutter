/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2006-2009 Kouhei Sutou <kou@clear-code.com>
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

#ifndef __CUT_MODULE_IMPL_H__
#define __CUT_MODULE_IMPL_H__

#include <glib-object.h>

G_BEGIN_DECLS

#include "cut-module.h"

typedef GList   *(*CutModuleInitFunc)         (GTypeModule *module);
typedef void     (*CutModuleExitFunc)         (void);
typedef GObject *(*CutModuleInstantiateFunc)  (const gchar *first_property,
                                               va_list      var_args);

#define CUT_MODULE_IMPL_INIT           cut_module_impl_init
#define CUT_MODULE_IMPL_EXIT           cut_module_impl_exit
#define CUT_MODULE_IMPL_INSTANTIATE    cut_module_impl_instantiate

G_MODULE_EXPORT GList   *CUT_MODULE_IMPL_INIT        (GTypeModule  *module);
G_MODULE_EXPORT void     CUT_MODULE_IMPL_EXIT        (void);
G_MODULE_EXPORT GObject *CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property,
                                                      va_list      var_args);

G_END_DECLS

#endif /* __CUT_MODULE_IMPL_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
