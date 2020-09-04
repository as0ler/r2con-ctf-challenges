#!/bin/bash
docker stop r2ulette1
docker rm r2ulette1
docker build -t r2ulette1:latest .
docker run -p 4545:4545 -d --restart always -it --name r2ulette1 r2ulette1
