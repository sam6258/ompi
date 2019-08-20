/*
 * Copyright (c) 2016-2018 Intel, Inc.  All rights reserved.
 * Copyright (c) 2019      IBM Corporation.  All rights reserved.
 * Copyright (c) 2018      Research Organization for Information Science
 *                         and Technology (RIST). All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 *
 */

#include "orte_config.h"
#include "orte/types.h"
#include "opal/types.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <ctype.h>

#include "opal/util/argv.h"
#include "opal/util/basename.h"
#include "opal/util/opal_environ.h"

#include "orte/runtime/orte_globals.h"
#include "orte/util/name_fns.h"
#include "orte/util/show_help.h"
#include "orte/util/compress.h"
#include "orte/mca/errmgr/errmgr.h"
#include "orte/mca/rmaps/base/base.h"
#include "orte/mca/routed/routed.h"
#include "orte/mca/regx/base/base.h"

#include "regx_compress.h"

static int nidmap_create(opal_pointer_array_t *pool, char **regex);

orte_regx_base_module_t orte_regx_compress_module = {
    .nidmap_create = nidmap_create,
    .nidmap_parse = orte_regx_base_nidmap_parse,
    .extract_node_names = orte_regx_base_extract_node_names,
    .encode_nodemap = orte_regx_base_encode_nodemap,
    .decode_daemon_nodemap = orte_regx_base_decode_daemon_nodemap,
    .generate_ppn = orte_regx_base_generate_ppn,
    .parse_ppn = orte_regx_base_parse_ppn
};

static int nidmap_create(opal_pointer_array_t *pool, char **regex)
{
    char *node;
    int n;
    char *nodenames, *vpids;
    orte_regex_range_t *rng;
    opal_list_item_t *item;
    char **regexargs = NULL, **vpidargs = NULL, *tmp, *tmp2;
    orte_node_t *nptr;
    orte_vpid_t vpid;
    size_t uncmplen, cmplen;

    rng = NULL;
    for (n=0; n < pool->size; n++) {
        if (NULL == (nptr = (orte_node_t*)opal_pointer_array_get_item(pool, n))) {
            continue;
        }
        /* if no daemon has been assigned, then this node is not being used */
        if (NULL == nptr->daemon) {
            vpid = -1;  // indicates no daemon assigned
        } else {
            vpid = nptr->daemon->name.vpid;
        }

        asprintf(&tmp, "%u", vpid);
        opal_argv_append_nosize(&vpidargs, tmp);
        free(tmp);

        node = nptr->name;
        opal_output_verbose(5, orte_regx_base_framework.framework_output,
                            "%s PROCESS NODE <%s>",
                            ORTE_NAME_PRINT(ORTE_PROC_MY_NAME),
                            node);

        /* Don't compress the name - just add it to the list */
        if (NULL != node) {
            /* solitary node */
            opal_argv_append_nosize(&regexargs, node);
        }
    }

    /* assemble final result */
    nodenames = opal_argv_join(regexargs, ',');
    /* cleanup */
    opal_argv_free(regexargs);

    vpids = opal_argv_join(vpidargs, ',');
    /* cleanup */
    opal_argv_free(vpidargs);

    /* now concatenate the results into one string */
    asprintf(&tmp2, "%s@%s", nodenames, vpids);
    free(nodenames);
    free(vpids);
    opal_output_verbose(5, orte_regx_base_framework.framework_output,
                        "%s Compressing : <%s>",
                        ORTE_NAME_PRINT(ORTE_PROC_MY_NAME),
                        tmp2);

    uncmplen = strlen(tmp2) + 1;
    if (uncmplen < ORTE_COMPRESS_LIMIT) {
        if (NULL == (tmp = (char *)malloc(ORTE_COMPRESS_LIMIT))) {
            opal_output(0, "MALLOC FAILED");
            return ORTE_ERROR;
        }
        strncpy(tmp, tmp2, ORTE_COMPRESS_LIMIT);
        uncmplen = ORTE_COMPRESS_LIMIT;
        free(tmp2);
        tmp2 = tmp;
    }

    if (!orte_util_compress_block((uint8_t *)tmp2, uncmplen, (uint8_t **)regex, &cmplen)) {
        opal_output_verbose(5, orte_regx_base_framework.framework_output,
                            "%s Compression failed",
                            ORTE_NAME_PRINT(ORTE_PROC_MY_NAME));
        free(tmp2);
        return ORTE_ERROR;
    }

    free(tmp2);
    return ORTE_SUCCESS;
}

int orte_regx_base_nidmap_parse(char *regex)
{
}
