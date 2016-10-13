#! /bin/sh

VERSION=0.1

SCRIPTS="
highlight.js
lang-js.js
lang-css.js
lang-xml.js
lang-html.js
helpers.js"

BASENAME=`pwd -P`
BASENAME=`basename $BASENAME`
cd ..
rm -rf /tmp/$BASENAME
cp -r $BASENAME /tmp/
cd /tmp/$BASENAME

# echo -n '' > hl-all.js

for i in $SCRIPTS; do
    cat $i >> hl-all.js
done

/tmp/js/js --inline hl-all.js
cd ..
zip -q -r "${BASENAME}-${VERSION}.zip" $BASENAME
