#!/bin/bash

docker build -t bleeding_heart:latest .
docker run -d -p 4444:1337 bleeding_heart
