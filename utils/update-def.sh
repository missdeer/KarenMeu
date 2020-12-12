#!/bin/bash
echo "EXPORTS" > utils.def
grep -r -a '//export' --include='*.go' . | while read line
do
    fn=`echo $line | awk -F ' ' '{ print $2 }'`
    echo "    $fn" >> utils.def
done

sed -i.bak '/#line/d' utils.h
sed -i.bak '/Complex/d' utils.h
sed -i.bak '/SIZE_TYPE/d' utils.h
rm -f utils.h.bak
