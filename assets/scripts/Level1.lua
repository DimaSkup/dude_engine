Level1 = {
    -------------------------------------------------------
    --- Table to define the list of assets
    -------------------------------------------------------
    assets = {
        [0] = { type="texture", id="terrain-texture-day",   file="./assets/tilemaps/jungle.png" },
        [1] = { type="texture", id="terrain-texture-night", file="./assets/tilemaps/jungle-night.png" },
        [2] = { type="texture", id="chopper-texture",       file="./assets/images/chopper-spritesheet.png" },
        [3] = { type="texture", id="tank-texture-big-left", file="./assets/images/tank-big-left.png" },
        [4] = { type="texture", id="radar-texture",         file="./assets/images/radar.png" },
        [5] = { type="texture", id="projectile-texture",    file="./assets/images/bullet-enemy.png" },
        [6] = { type="texture", id="heliport-texture",      file="./assets/images/heliport.png" },
        [7] = { type="font",    id="charriot-font",         file="./assets/fonts/charriot.ttf", fontSize=14 },
    }, 

    -------------------------------------------------------
    --- Table to define the map config variables
    -------------------------------------------------------
    map = {
        textureAssetId = "terrain-texture-night",
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
            layer = 3,
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
                    textureAssetId = "heliport-texture",
                    animated = false
                }
            } -- components
        }, -- start

        [2] = {
            name = "heliport",
            layer = 3,
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

        [1] = {
            name = "tank-enemy-1",
            layer = 2,
            components = {
                transform = {
                    position = {
                        x = 650,
                        y = 405
                    },
                    width = 32,
                    height = 32,
                    scale = 1,
                    rotation = 0
                },
                sprite = {
                    textureAssetId = "tank-texture-big-left",
                    animated = false
                },
                collider = {
                    tag = "ENEMY"
                },
                projectileEmitter = {
                    textureAssetId = "projectile-texture",
                    speed = 70,
                    range = 300,
                    angle = 180,
                    width = 4,
                    height = 4,
                    shouldLoop = true,
                }
            } -- components
        } -- tank-enemy-1

    } -- entities
} -- level 1
