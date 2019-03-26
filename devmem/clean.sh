#!/bin/bash

rm *.order
rm *.symvers
rm *.ko
rm *.o
rm *.mod.c
rm .* > /dev/null 2>&1
rm .tmp_versions/ -rf
