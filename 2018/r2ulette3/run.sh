#!/bin/bash
docker stop r2ulette3
docker rm r2ulette3
docker build -t r2ulette3:latest .
docker run --privileged -p 7878:7878 --restart always -d --name r2ulette3 r2ulette3
