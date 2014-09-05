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
 *  \file p_g11n.h
 *  \brief Globalization.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <smarquis@astrorigin.ch>
 */

#ifndef P_G11N_H
#define P_G11N_H

#ifdef __cplusplus
extern "C" {
#endif

#include "p_dict.h"
#include "p_h.h"
#include "p_vdict.h"

/**
 *  \brief Get translation for a single text or term.
 *  \param txt Text to translate.
 *  \return Translation found, or txt if not found.
 */
P_EXPORT P_CHAR*
p_i18n( const P_CHAR* txt );

/**
 *  \brief Get translation for a single term (with context).
 *  \param ctxt Context information.
 *  \param txt Text to translate.
 *  \return Translation found, or txt if not found.
 */
P_EXPORT P_CHAR*
p_i18n_c( const P_CHAR* ctxt,
        const P_CHAR* txt );

/**
 *  \brief Get translation for a single term (with plural alternative).
 *  \param txt Text to translate (singular).
 *  \param plural Text to translate (plural).
 *  \param count Determine which translation to choose.
 *  \return Translation found, or txt (or plural) if not found
 */
P_EXPORT P_CHAR*
p_i18n_p( const P_CHAR* txt,
        const P_CHAR* plural,
        const P_SZ count );

/**
 *
 */
P_EXPORT P_CHAR*
p_i18n_cp( const P_CHAR* ctxt,
        const P_CHAR* txt,
        const P_CHAR* plural,
        const P_SZ count );

/**
 *
 */
typedef struct _p_l10n_translations p_l10n_Translations;

/**
 *
 */
struct _p_l10n_translations
{
    p_Dict*     l10n_dict;
    p_VDict*    l10n_c_vdict;
    p_Dict*     l10n_p_dict_1;
    p_Dict*     l10n_p_dict_2;
    p_VDict*    l10n_cp_vdict_1;
    p_VDict*    l10n_cp_vdict_2;
};

/**
 *
 */
extern p_l10n_Translations*
_p_l10n_t;

/**
 *
 */
P_EXPORT P_BOOL
p_l10n_init( p_l10n_Translations* t );

/**
 *  \brief Finalize a translations struct.
 *  \param t The translations struct.
 *  \param dofree Delete translation strings.
 */
P_EXPORT P_VOID
p_l10n_fini( p_l10n_Translations* t,
        const P_BOOL dofree );

/**
 *  \brief Add a translated element to a translations struct.
 *  \param t Initialized Translations struct.
 *  \param ctxt Context string, or NULL.
 *  \param txt Text string, not NULL.
 *  \param plural Plural string, or NULL.
 *  \param tr1 Text translation, not NULL.
 *  \param tr2 Plural translation, or NULL.
 *  \return 1 on success, 0 if element is duplicated, -1 on error.
 *
 *  Translation strings are not copied.
 */
P_EXPORT P_INT8
p_l10n_load( p_l10n_Translations* t,
        const P_CHAR* ctxt,
        const P_CHAR* txt,
        const P_CHAR* plural,
        const P_CHAR* tr1,
        const P_CHAR* tr2 );

/**
 *  \brief Load a vector of translation strings.
 *  \param t Initialized Translations struct.
 *  \param strings The vector.
 *  \param len Size of vector, nulls included.
 *  \return P_TRUE on success.
 */
P_EXPORT P_BOOL
p_l10n_load_vector( p_l10n_Translations* t,
        const P_CHAR* strings,
        const P_SZ len );

/**
 *  \brief Load a file of translations strings (produced by p_i18n).
 *  \param t Initialized translations struct.
 *  \param f Opened stream.
 *  \return P_TRUE on success.
 *
 *  Incomplete translations are not added to the struct.
 *
 *  \note Translations strings need to be freed.
 */
P_EXPORT P_BOOL
p_l10n_load_file( p_l10n_Translations* t,
        FILE* f );

#ifdef __cplusplus
}
#endif

#endif /* P_I18N_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
