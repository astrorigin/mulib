/*
    Plib
    Copyright (C) 2014 Stanislas Marquis <smarquis@astrorigin.ch>

    This program is free software: you can redistribute it and/or modify it
    under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *  \file p_shell.h
 *  \brief Container for a pointer.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <smarquis@astrorigin.ch>
 */

#ifndef P_SHELL_H
#define P_SHELL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "p_h.h"

/**
 *  \typedef P_SHELLTYPE
 */
typedef enum _p_shelltype P_SHELLTYPE;

/**
 *  \struct _p_shelltype
 */
enum _p_shelltype
{
    P_SHELLTYPE_NULL = 0,
    /* standard types */
    P_SHELLTYPE_CHAR,
    P_SHELLTYPE_UCHAR,
    P_SHELLTYPE_INT8,
    P_SHELLTYPE_UINT8,
    P_SHELLTYPE_INT16,
    P_SHELLTYPE_UINT16,
    P_SHELLTYPE_INT32,
    P_SHELLTYPE_UINT32,
    P_SHELLTYPE_INT64,
    P_SHELLTYPE_UINT64,
    P_SHELLTYPE_DBL,
    P_SHELLTYPE_WCHAR,
    P_SHELLTYPE_SZ,
    P_SHELLTYPE_ID,
    P_SHELLTYPE_BOOL,
    P_SHELLTYPE_CSTR,   /* C string */
    P_SHELLTYPE_PTR,
    /* P types */
    P_SHELLTYPE_BTREE,
    P_SHELLTYPE_DICT,
    P_SHELLTYPE_DLLIST,
    P_SHELLTYPE_SLLIST,
    P_SHELLTYPE_STRING,
    P_SHELLTYPE_VDICT,
    P_SHELLTYPE_VECTOR

};

/**
 *  \typedef p_Shell
 */
typedef struct _p_shell p_Shell;

/**
 *  \struct _p_shell
 */
struct _p_shell
{
    P_SHELLTYPE type;   /**< Shell type */
    P_PTR       p;      /**< Pointer */
    P_VOID      (*freedoer)( P_PTR );  /**< Function for lists and pointers */
};

/**
 *  \brief Allocate for a shell.
 */
P_EXPORT P_BOOL
p_shell_new( p_Shell** sh,
        const P_SHELLTYPE tp );

/**
 *  \brief Deallocate a shell.
 */
P_EXPORT P_VOID
p_shell_delete( p_Shell** sh );

/**
 *  \brief Initialize a shell.
 *
 *  Vectors are special beasts that need to be initialized after function call.
 */
P_EXPORT P_BOOL
p_shell_init( p_Shell* sh,
        const P_SHELLTYPE tp );

/**
 *  \brief Finalize a shell.
 *
 *  C strings and pointers are special beasts that need to be dealloced before
 *  function call, or by setting freedoer.
 */
P_EXPORT P_VOID
p_shell_fini( p_Shell* sh );

/**
 *  \brief Initialize the content of the shell.
 */
P_EXPORT P_BOOL
p_shell_set_type( p_Shell* sh,
        const P_SHELLTYPE tp );

/**
 *  \typedef p_ShellElem
 */
typedef struct _p_shell_elem p_ShellElem;

/**
 *  \struct _p_shell_elem
 */
struct _p_shell_elem
{
    p_ShellElem*    next;
    p_Shell         shell;
};

/**
 *  \brief Allocate for a shell element.
 */
P_EXPORT P_BOOL
p_shellelem_new( p_ShellElem** sh,
        const P_SHELLTYPE tp );

/**
 *  \brief Deallocate a shell element.
 */
P_EXPORT P_VOID
p_shellelem_delete( p_ShellElem** sh );

/**
 *
 */
P_EXPORT P_BOOL
p_shellelem_init( p_ShellElem* sh,
        const P_SHELLTYPE tp );

/**
 *  \brief Initialize a shell element.
 */
P_EXPORT P_VOID
p_shellelem_fini( p_ShellElem* sh );

/**
 *  \brief Finalize a shell element.
 */
P_EXPORT P_VOID
p_shelllist_delete( p_ShellElem* lst );

#ifdef __cplusplus
}
#endif

#endif /* P_SHELL_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
