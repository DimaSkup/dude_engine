Level1 = {
    -------------------------------------------------------
    --- Table to define the list of assets
    -------------------------------------------------------
    assets = {
        [0]  = { type="texture", id = "terrain-texture-day",    file = "./assets/tilemaps/jungle.png" },
        [1]  = { type="texture", id = "terrain-texture-night",  file = "./assets/tilemaps/jungle-night.png" },
        [2]  = { type="texture", id = "chopper-texture",        file = "./assets/images/chopper-spritesheet.png" },
        [3]  = { type="texture", id = "projectile-texture",     file = "./assets/images/bullet-enemy.png" },
        [4]  = { type="texture", id = "obstacles-texture",      file = "./assets/images/obstacles.png" },
        [5]  = { type="texture", id = "truck-left-texture",     file = "./assets/images/truck-left.png" },
        [6]  = { type="texture", id = "truck-right-texture",    file = "./assets/images/truck-right.png" },
        [7]  = { type="texture", id = "truck-down-texture",     file = "./assets/images/truck-down.png" },
        [8]  = { type="texture", id = "tank-big-down-texture",  file = "./assets/images/tank-big-down.png" },
        [9]  = { type="texture", id = "tank-big-left-texture",  file = "./assets/images/tank-big-left.png" },
        [10] = { type="texture", id = "tank-big-right-texture", file = "./assets/images/tank-big-right.png" },
        [11] = { type="texture", id = "tank-small-left-texture", file = "./assets/images/tank-small-left.png" },
        [12] = { type="texture", id = "tank-small-right-texture", file = "./assets/images/tank-small-right.png" },
        [13] = { type="texture", id = "army-group-1-texture",   file = "./assets/images/army-group-1.png" },
        [14] = { type="texture", id = "army-group-2-texture",   file = "./assets/images/army-group-2.png" },
        [15] = { type="texture", id = "army-group-3-texture",   file = "./assets/images/army-group-3.png" },
        [16] = { type="texture", id = "rock-big-1-texture",     file = "./assets/images/rock-big-1.png" },
        [17] = { type="texture", id = "rock-big-2-texture",     file = "./assets/images/rock-big-2.png" },
        [18] = { type="texture", id = "rock-big-3-texture",     file = "./assets/images/rock-big-3.png" },
        [19] = { type="texture", id = "rock-big-4-texture",     file = "./assets/images/rock-big-4.png" },
        [20] = { type="texture", id = "rock-small-1-texture",   file = "./assets/images/rock-small-1.png" },
        [21] = { type="texture", id = "tree-small-1-texture",   file = "./assets/images/tree-small-1.png" },
        [22] = { type="texture", id = "tree-small-2-texture",   file = "./assets/images/tree-small-2.png" },
        [23] = { type="texture", id = "tree-small-3-texture",   file = "./assets/images/tree-small-3.png" },
        [24] = { type="texture", id = "tree-small-4-texture",   file = "./assets/images/tree-small-4.png" },
        [25] = { type="texture", id = "tree-small-5-texture",   file = "./assets/images/tree-small-5.png" },
        [26] = { type="texture", id = "tree-small-6-texture",   file = "./assets/images/tree-small-6.png" },
        [27] = { type="texture", id = "tree-small-7-texture",   file = "./assets/images/tree-small-7.png" },
        [28] = { type="texture", id = "tree-small-8-texture",   file = "./assets/images/tree-small-8.png" },
        [29] = { type="texture", id = "start-texture",          file = "./assets/images/start.png" },
        [30] = { type="texture", id = "heliport-texture",       file = "./assets/images/heliport.png" },
        [31] = { type="texture", id = "bullet-friendly-texture", file = "./assets/images/bullet-friendly.png" },
        [32] = { type="texture", id = "radar-texture",          file = "./assets/images/radar.png" },
        [33] = { type="sound",   id = "blades-sound",           file = "./assets/sounds/blades.wav" },
        [34] = { type="font",    id="charriot-font",         file="./assets/fonts/charriot.ttf", fontSize=14 },
    }, 

    -------------------------------------------------------
    --- Table to define the map config variables
    -------------------------------------------------------
    map = {
        textureAssetId = "terrain-texture-day",
        file = "./assets/tilemaps/jungle.map",
        scale = 2,
        tileSize = 32,
        mapSizeX = 25,
        mapSizeY = 20
    }, 

    -------------------------------------------------------
    --- Table to define entities and their components
    -------------------------------------------------------
    entities = {
        [0] = {
            name = "player",
            layer = 3,
            components = {
                transform = {
                    position = {
                        x = 240,
                        y = 106
                    },
                    velocity = {
                        x = 0,
                        y = 0,
                    },
                    width    = 32,
                    height   = 32,
                    scale    = 1,
                    rotation = 0
                },
                sprite = {
                    textureAssetId = "chopper-texture",
                    animated = true,
                    frameCount = 2,
                    animationSpeed = 90,
                    hasDirections = true,
                    fixed = false
                },
                collider = {
                    tag = "PLAYER"
                },
                input = {
                    keyboard = {
                        up    = "w",
                        left  = "a",
                        down  = "s",
                        right = "d",
                        shoot = "space"
                    }
                }
            } -- components
        }, -- player
        
        [1] = {
            name = "start",
            layer = 2,
            components = {
                transform = {
                    position = {
                        x = 240,
                        y = 115
                    },
                    velocity = {
                        x = 0,
                        y = 0
                    },
                    width = 32,
                    height = 32,
                    scale = 1,
                    rotation = 0
                },
                sprite = {
                    textureAssetId = "start-texture",
                    animated = false
                }
            } -- components
        }, -- start

        [2] = {
            name = "heliport",
            layer = 2,
            components = {
                transform = {
                    position = {
                        x = 1395,
                        y = 495
                    },
                    velocity = {
                        x = 0,
                        y = 0
                    },
                    width = 32,
                    height = 32,
                    scale = 1,
                    rotation = 0
                },
                sprite = {
                    textureAssetId = "heliport-texture",
                    animated = false
                },
                collider = {
                    tag = "LEVEL_COMPLETE"
                }
            } -- components
        }, -- heliport

        [3] = {
            name = "tank1",
            layer = 2,
            components = {
                transform = {
                    position = {
                        x = 650,
                        y = 405
                    },
                    velocity = {
                        x = 5,
                        y = 0
                    },
                    width = 32,
                    height = 32,
                    scale = 1,
                    rotation = 0
                },
                sprite = {
                    textureAssetId = "tank-big-right-texture",
                    animated = false
                },
                collider = {
                    tag = "ENEMY"
                },
                projectileEmitter = {
                    textureAssetId = "projectile-texture",
                    speed = 70,
                    range = 300,
                    angle = 0,
                    width = 4,
                    height = 4,
                    shouldLoop = true,
                }
            } -- components
        }, -- tank1

        [4] = {
            name = "tank2",
            layer = 2,
            components = {
                transform = {
                    position = {
                        x = 660,
                        y = 535
                    },
                    velocity = {
                        x = 0,
                        y = 3
                    },
                    width = 32,
                    height = 32,
                    scale = 1,
                    rotation = 0
                },
                sprite = {
                    textureAssetId = "tank-big-down-texture",
                    animated = false
                },
                collider = {
                    tag = "ENEMY"
                },
                projectileEmitter = {
                    textureAssetId = "projectile-texture",
                    speed = 70,
                    range = 300,
                    angle = 90,
                    width = 4,
                    height = 4,
                    shouldLoop = true,
                }
            } -- components
        } -- tank2


    } -- entities
} -- level 1
