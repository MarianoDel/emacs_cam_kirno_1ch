//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### UTILS.H ###############################
//---------------------------------------------
#ifndef _UTILS_H_
#define _UTILS_H_

/**
 * @file utils.h
 * @brief Helper utilities.
 */

#include <stdint.h>

#if HAVE_CONFIG_H
// We're in the test environment
#include <config.h>
#else
#include <machine/endian.h>
#endif

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return the LSB from a 16bit value.
 * @param s The 16bit value to split.
 * @returns The lower 8 bits of the uint16_t.
 */
static inline uint8_t ShortLSB(uint16_t s) {
  return s & 0xff;
}

/**
 * @brief Return the MSB from a 16bit value.
 * @param s The 16bit value to split.
 * @returns The upper 8 bits of the uint16_t.
 */
static inline uint8_t ShortMSB(uint16_t s) {
  return s >> 8;
}

/**
 * @brief Combine two 8-bit values to a 16-bit value.
 * @param upper the MSB,
 * @param lower the LSB,
 * @returns A 16-bit value.
 */
static inline uint16_t JoinShort(uint8_t upper, uint8_t lower) {
  return (upper << 8) + lower;
}

/**
 * @brief Return the MSB from a 32bit value.
 * @param s The 32bit value to split.
 * @returns The first 8 bits of the uint32_t.
 */
static inline uint8_t UInt32Byte0(uint32_t s) {
  return s >> 24;
}

/**
 * @brief Return the second-highest byte from a 32bit value.
 * @param s The 32bit value to split.
 * @returns Bits 9-16 bits of the uint32_t.
 */
static inline uint8_t UInt32Byte1(uint32_t s) {
  return s >> 16;
}

/**
 * @brief Return the second-lowest byte from a 32bit value.
 * @param s The 32bit value to split.
 * @returns Bits 17-24 bits of the uint32_t.
 */
static inline uint8_t UInt32Byte2(uint32_t s) {
  return s >> 8;
}

/**
 * @brief Return the lowest byte from a 32bit value.
 * @param s The 32bit value to split.
 * @returns Bits 25-32 bits of the uint32_t.
 */
static inline uint8_t UInt32Byte3(uint32_t s) {
  return s & 0xff;
}

/**
 * @brief Extract a uint16_t in network-byte order from a memory location.
 * @param ptr A pointer to the memory.
 * @returns A 16-bit value.
 */
static inline uint32_t ExtractUInt16(const uint8_t *ptr) {
  return (ptr[0] << 8) + ptr[1];
}

/**
 * @brief Extract a uint32_t in network-byte order from a memory location.
 * @param ptr A pointer to the memory.
 * @returns A 32-bit value.
 */
static inline uint32_t ExtractUInt32(const uint8_t *ptr) {
  return (ptr[0] << 24) + (ptr[1] << 16) + (ptr[2] << 8) + ptr[3];
}

/**
 * @brief Copy a 16-bit value to a memory location in network-byte order.
 * @param ptr A pointer to the memory.
 * @param value The value to push.
 * @returns A pointer to the next byte after the last one that was copied.
 */
static inline uint8_t* PushUInt16(uint8_t *ptr, uint16_t value) {
  *ptr++ = (value >> 8);
  *ptr++ = value & 0xff;
  return ptr;
}

/**
 * @brief Copy a 32-bit value to a memory location in network-byte order.
 * @param ptr A pointer to the memory.
 * @param value The value to push.
 * @returns A pointer to the next byte after the last one that was copied.
 */
static inline uint8_t* PushUInt32(uint8_t *ptr, uint32_t value) {
  *ptr++ = (value >> 24);
  *ptr++ = (value >> 16);
  *ptr++ = (value >> 8);
  *ptr++ = value & 0xff;
  return ptr;
}

//--- Funciones del modulo utils.c -------------------------------------//
unsigned char StringIsANumber (char * pn, unsigned short * number);
unsigned short GetValue (unsigned char * pn, char delimiter);

#ifdef __cplusplus
}
#endif

#endif    /* _UTILS_H_ */

//--- end of file ---//

