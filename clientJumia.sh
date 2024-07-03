#!/bin/bash
(echo -e -n "PASS 123\r\nNICK $1\r\nUSER $1 0 * $1\r\nJOIN #CH1\r\n" ; cat) | nc -c localhost 6969
