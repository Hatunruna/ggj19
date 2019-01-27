#!/bin/bash

# To rename for FILE in *; do SUFFIX=`echo $FILE | cut -d_ -f 3`; mv $FILE player_up_$SUFFIX; done

montage raw/west/move/*.png \
        raw/north_west/move/*.png \
        raw/north/move/*.png \
        raw/north_east/move/*.png \
        raw/east/move/*.png \
        raw/south_east/move/*.png \
        raw/south_west/move/*.png \
        raw/west/pause/*.png \
        raw/north_west/pause/*.png \
        raw/north/pause/*.png \
        raw/north_east/pause/*.png \
        raw/east/pause/*.png \
        raw/south_east/pause/*.png \
        raw/south_west/pause/*.png \
        raw/south/*.png \
        raw/north_west/harvest/*.png \
        raw/north_east/harvest/*.png \
        raw/south_east/harvest/*.png \
        raw/south_west/harvest/*.png \
        -background none -tile 22x19 -geometry 256x256+0  png32:player_animations.png

montage raw/death/*.png \
        -background none -tile 22x4 -geometry 256x256+0  png32:player_death.png
