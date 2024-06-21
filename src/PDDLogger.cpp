/* Copyright 2022 Collabora, Ltd
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#include "PDDLogger.hpp"

#include <iostream>

std::mutex PDDLogger::lock;
uint32_t PDDLogger::next_tag = 1;
uint32_t PDDLogger::next_endpoint_tag = 1;
std::vector<PDDLogger::Endpoint> PDDLogger::endpoints;

PDDLogger::PDDLogger(const char *entrypoint, uint32_t num_children)
{
    std::lock_guard<std::mutex> guard(lock);

    tag = next_tag++;

    for (auto e : endpoints)
        e.ops->begin(e.priv, tag, entrypoint, num_children);
}

PDDLogger::~PDDLogger()
{
    std::lock_guard<std::mutex> guard(lock);

    for (auto e : endpoints)
        e.ops->end(e.priv, tag);
}

void
PDDLogger::set_args(const void *data, uint32_t size)
{
    log_data(PDD_LOG_TYPE_ARGS, data, size);
}

void
PDDLogger::set_child_args(uint32_t child, const void *data, uint32_t size)
{
    log_data(PDD_LOG_TYPE_CHILD_ARGS_START + child, data, size);
}

void
PDDLogger::set_pdata(const void *data, uint32_t size)
{
    log_data(PDD_LOG_TYPE_PDATA, data, size);
}

void
PDDLogger::set_child_pdata(uint32_t child, const void *data, uint32_t size)
{
    log_data(PDD_LOG_TYPE_CHILD_PDATA_START + child, data, size);
}

uint32_t
PDDLogger::register_endpoint(const struct pdd_log_ops *ops, void *priv)
{
    std::lock_guard<std::mutex> guard(lock);

    Endpoint e;
    e.tag = next_endpoint_tag++;
    e.ops = ops;
    e.priv = priv;

    endpoints.push_back(e);

    return e.tag;
}

void
PDDLogger::unregister_endpoint(uint32_t tag)
{
    std::lock_guard<std::mutex> guard(lock);

    for (auto it = endpoints.begin(); it != endpoints.end();) {
        if (it->tag == tag) {
            endpoints.erase(it);
            return;
        }
    }

    std::cerr << "WARNING: Attempted to unregister invalid endpoint" << std::endl;
}

void
PDDLogger::log_data(int type, const void *data, uint32_t size)
{
    std::lock_guard<std::mutex> guard(lock);

    for (auto e : endpoints)
        e.ops->log_data(e.priv, tag, type, data, size);
}

extern "C" uint32_t
pdd_log_register(const struct pdd_log_ops *ops, void *priv)
{
    return PDDLogger::register_endpoint(ops, priv);
}

extern "C" void
pdd_log_unregister(uint32_t tag)
{
    return PDDLogger::unregister_endpoint(tag);
}
