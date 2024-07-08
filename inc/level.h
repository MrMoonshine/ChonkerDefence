#pragma once

#define LEVEL_NAME_LENGTH 64
#define LEVEL_STYLE_LENGTH 32
#define LEVEL_RESERVED_LENGTH 16

#define LEVEL_BLOCK_PATH                0b0000
#define LEVEL_BLOCK_WATER               0b0001
#define LEVEL_BLOCK_GRASS               0b0010
#define LEVEL_BLOCK_BRIDGE              0b0011
#define LEVEL_BLOCK_DECORATION_LAND     0b0100
#define LEVEL_BLOCK_DECORATION_WATER    0b0101
#define LEVEL_BLOCK_CAT_2               0b0110
#define LEVEL_BLOCK_CAT_1               0b0111

#define LEVEL_IS_LAND(node) (node == LEVEL_BLOCK_GRASS || node == LEVEL_BLOCK_DECORATION_LAND || node == LEVEL_BLOCK_CAT_2 || node == LEVEL_BLOCK_CAT_1)
#define LEVEL_IS_WATER(node) (node == LEVEL_BLOCK_WATER || node == LEVEL_BLOCK_BRIDGE || node == LEVEL_BLOCK_DECORATION_WATER)
#define LEVEL_IS_PATH(node) (node == LEVEL_BLOCK_PATH || node == LEVEL_BLOCK_BRIDGE)

#define LEVEL_IS_SKIRT_REQURED(node) (LEVEL_IS_LAND(node) || node == LEVEL_BLOCK_PATH)

#define LEVEL_TILEMAP_TERRAIN_WIDTH 2
#define LEVEL_TILEMAP_TERRAIN_HEIGHT 3
#define LEVEL_TILEMAP_TERRAIN_FILE_PATTERN "../assets/textures/world/terrain_%s.png"
