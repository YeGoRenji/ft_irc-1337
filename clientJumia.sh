(echo -en "PASS 123\r\n"; sleep 1; echo "NICK $1\r\n"; sleep 1; echo "USER $1 0 * $1\r\n" ; cat) | nc localhost 6969
