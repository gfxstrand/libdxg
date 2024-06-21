/* Copyright 2022 Collabora, Ltd
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#pragma once

#include "pdd_log_iface.h"

#include <atomic>
#include <mutex>
#include <vector>

class PDDLogger {
public:
    PDDLogger(const char *entrypoint, uint32_t num_children);
    ~PDDLogger();

    void set_args(const void *data, uint32_t size);
    void set_child_args(uint32_t child, const void *data, uint32_t size);

    void set_pdata(const void *data, uint32_t size);
    void set_child_pdata(uint32_t child, const void *data, uint32_t size);

    template<typename T>
    inline void set_args(const T& args)
    {
        set_args(&args, sizeof(args));
    }

    template<typename T>
    inline void set_pdata(const T& args)
    {
        set_pdata(args.pPrivateDriverData, args.PrivateDriverDataSize);
    }

    template<typename T>
    inline void set_args_with_pdata(const T& args)
    {
        set_args(&args, sizeof(args));
        set_pdata(args);
    }

    template<typename T>
    inline void set_child_args(uint32_t child, const T& args)
    {
        set_child_args(child, &args, sizeof(args));
    }

    template<typename T>
    inline void set_child_pdata(uint32_t child, const T& args)
    {
        set_child_pdata(child, args.pPrivateDriverData,
                        args.PrivateDriverDataSize);
    }

    template<typename T>
    inline void set_child_args_with_pdata(uint32_t child, const T& args)
    {
        set_child_args(child, &args, sizeof(args));
        set_child_pdata(child, args);
    }

    static uint32_t register_endpoint(const pdd_log_ops *ops, void *priv);
    static void unregister_endpoint(uint32_t tag);

private:
    void log_data(int type, const void *data, uint32_t size);

    static std::mutex lock;

    static uint32_t next_tag;
    static uint32_t next_endpoint_tag;

    struct Endpoint {
        uint32_t tag;
        const pdd_log_ops *ops;
        void *priv;
    };
    static std::vector<Endpoint> endpoints;

    uint32_t tag;
};
