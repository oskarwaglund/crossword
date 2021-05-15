#!/bin/sh

g++ -I . server/*.cxx common/*.cxx -o serverCw
g++ -I . client/*.cxx common/*.cxx -o clientCw