#!/bin/sh
set -eu
root=$(cd "$(dirname "$0")/.." && pwd)
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

c++ -std=c++11 -I"$root/src" -I"$root/tests" "$root/assets/demo/rb_steps.cpp" -o "$tmp/rb_steps"
cd "$tmp"
./rb_steps dot
for f in step_*.dot; do
	n=$(echo "$f" | sed 's/[^0-9]//g' | sed 's/^0//')
	sed "s/^digraph tree {/&\n\tgraph [label=\"insert $n\", labelloc=t, fontname=\"monospace\", fontsize=18];/" "$f" \
		| dot -Tpng -Gdpi=96 -Gpad=0.3 -o "${f%.dot}.png"
done
size=$(magick step_*.png -format '%w %h\n' info: | awk '{ if ($1 > w) w = $1; if ($2 > h) h = $2 } END { print w "x" h }')
for f in step_*.png; do
	magick "$f" -background white -gravity north -extent "$size" -type TrueColor "PNG24:$f"
done
magick -delay 110 step_0[1-7].png -delay 320 step_08.png -loop 0 +dither -layers Optimize "$root/assets/dot-steps.gif"
