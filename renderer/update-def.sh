#!/bin/bash
echo "EXPORTS" > renderer.def
grep -r -a '//export' --include='*.go' . | while read line
do
    fn=`echo $line | awk -F ' ' '{ print $2 }'`
    echo "    $fn" >> renderer.def
done

sed -i.bak '/#line/d' renderer.h
sed -i.bak '/Complex/d' renderer.h
sed -i.bak '/SIZE_TYPE/d' renderer.h
rm -f renderer.h.bak
