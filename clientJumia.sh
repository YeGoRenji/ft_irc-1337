#!/bin/bash
(echo -e -n "PASS 123\r\nNICK $1\r\nUSER $1 0 * $1\r\nJOIN #d\r\n" ; cat) | nc -c "${2:-localhost}" "${3:-6969}"
