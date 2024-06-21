/* Copyright 2022 Collabora, Ltd
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#include <stdint.h>

#define PDD_LOG_MAX_NUM_CHILDREN 1000

enum pdd_log_type {
    PDD_LOG_TYPE_ARGS,
    PDD_LOG_TYPE_PDATA,
    PDD_LOG_TYPE_CHILD_ARGS_START = 1 * PDD_LOG_MAX_NUM_CHILDREN,
    PDD_LOG_TYPE_CHILD_PDATA_START = 2 * PDD_LOG_MAX_NUM_CHILDREN,
};

struct pdd_log_ops {
    void (*begin)(void *priv, uint32_t tag,
                  const char *entrypoint, uint32_t num_children);
    void (*log_data)(void *priv, uint32_t tag, int type,
                     const void *data, uint32_t size);
    void (*end)(void *priv, uint32_t tag);
};

typedef uint32_t (*pdd_log_register_func)(const struct pdd_log_ops *ops, void *priv);

extern "C" uint32_t pdd_log_register(const struct pdd_log_ops *ops, void *priv);

typedef void (*pdd_log_unregister_func)(uint32_t tag);

extern "C" void pdd_log_unregister(uint32_t tag);
