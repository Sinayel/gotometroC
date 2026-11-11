#!/bin/sh
set -e
cd "$(dirname "$0")"
cp -r ~/eez-projects/affiche-metro/src/ui src/
sed -i 's#lvgl/lvgl\.h#lvgl.h#g' src/ui/*.c src/ui/*.h
sed -i '/LV_PART_CUSTOM1/d' src/ui/screens.c
echo "UI synchronisée"
