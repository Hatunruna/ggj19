#!/bin/sh

montage rawTiles/tileMap/*.png -background none -tile 7x5 -geometry 128x128+0  png32:TileSet.png
