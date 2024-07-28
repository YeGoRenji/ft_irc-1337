#!/bin/bash
(echo -e -n "PASS 123\r\nNICK $1\r\nUSER $1 0 * $1\r\nJOIN #d\r\n" ; cat) | nc -C localhost 6969
