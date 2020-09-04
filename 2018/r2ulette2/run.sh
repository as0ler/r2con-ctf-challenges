#!/bin/bash
docker stop r2ulette2
docker rm r2ulette2
docker build -t r2ulette2:latest .
docker run -p 6767:6767 --restart always -d --name r2ulette2 r2ulette2
