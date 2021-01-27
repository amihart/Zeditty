cat ../src/CZ80LIB.H | sed "/__REPLACE/{
r /dev/stdin
d
}" doc.template > doc.html
export QT_QPA_PLATFORM=offscreen
export QT_QPA_FONTDIR=/usr/share/fonts/truetype/dejavu/
phantomjs --ignore-ssl-errors=true --ssl-protocol=any --ssl-ciphers=ALL --debug=yes convert.js
