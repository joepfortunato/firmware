/*
 * Copyright (c) 2018 Particle Industries, Inc.  All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include "dct_hal.h"
#include "dcd_flash_impl.h"

namespace {

uint32_t calculateCRC(const void* data, size_t len) {
    return Compute_CRC32(reinterpret_cast<const uint8_t*>(data), len, NULL);
}

/**
 * The DCD is called before constructors have executed (from HAL_Core_Config) so we need to manually construct
 * rather than rely upon global construction.
 */
UpdateDCD<InternalFlashStore, 16*1024, 0x000F0000, 0x000F4000, calculateCRC>& dcd()
{
    static UpdateDCD<InternalFlashStore, 16*1024, 0x000F0000, 0x000F4000, calculateCRC> dcd;
    return dcd;
}

} // namespace

const void* dct_read_app_data(uint32_t offset) {
    dct_lock(0);
    const void* ptr = dcd().read(offset);
    dct_unlock(0);
    return ptr;
}

int dct_read_app_data_copy(uint32_t offset, void* ptr, size_t size) {
    int result = -1;
    dct_lock(0);
    const void* p = dcd().read(offset);
    if (ptr && p) {
        memcpy(ptr, p, size);
        result = 0;
    }
    dct_unlock(0);
    return result;
}

const void* dct_read_app_data_lock(uint32_t offset) {
    dct_lock(0);
    return dcd().read(offset);
}

int dct_read_app_data_unlock(uint32_t offset) {
    return dct_unlock(0);
}

int dct_write_app_data(const void* data, uint32_t offset, uint32_t size) {
    dct_lock(1);
    const int result = dcd().write(offset, data, size);
    dct_unlock(1);
    return result;
}
