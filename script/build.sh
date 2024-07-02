#!/bin/sh -ex

chown -R builder2 /app/
cd /app/ && gear-rpm -ba --target=$ARCH
